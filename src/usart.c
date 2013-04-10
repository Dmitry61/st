#include "usart.h"
#include "stm32f30x.h"
#include <string.h>
#include "usb_pwr.h"
#include "hw_config.h"

#undef errno
extern int errno;

#define RINGBUF_SIZE (1<<RINGBUF_SIZE_BITS)
volatile uint8_t USART1_ringbuf[RINGBUF_SIZE];
volatile uint32_t USART1_readidx = 0;
volatile uint32_t USART1_writeidx = 0;

void USART1_Init(void) {
    /* enable usart clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

  
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
  
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_7);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_7);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_7);
  
    USART_InitTypeDef USART_InitStructure;
    USART_StructInit(&USART_InitStructure);
    USART_InitStructure.USART_BaudRate = 115200;
    USART_Init(USART1, &USART_InitStructure);
  
    USART_Cmd(USART1, ENABLE);
  
    NVIC_EnableIRQ(USART1_IRQn);
}

void USART1_IRQHandler(void) {
    if(USART_GetITStatus(USART1, USART_FLAG_TXE) == RESET) {
        if (USART1_writeidx - USART1_readidx == 0) {
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
            return;
        }
        USART_SendData(USART1, USART1_ringbuf[(USART1_readidx++) & (RINGBUF_SIZE-1)]);
    }
}

#define MIN(a, b) ((a)<(b)?(a):(b))

void USART1_putc(char ch) {
    while (1) {
        uint32_t capacity = RINGBUF_SIZE - (USART1_writeidx - USART1_readidx);
        if (capacity > 0) break;
    }
    USART1_ringbuf[(USART1_writeidx++) & (RINGBUF_SIZE-1)] = ch;
}

void USART1_write(const char *str, int len) {
    uint32_t i = 0;
    while (i < len) {
        uint32_t writeidx = USART1_writeidx & (RINGBUF_SIZE-1);
        uint32_t len_to_end = RINGBUF_SIZE - writeidx;
        uint32_t capacity = RINGBUF_SIZE - (USART1_writeidx - USART1_readidx);
        uint32_t max_len = MIN(len_to_end, capacity);
        if (max_len == 0) continue;

        uint32_t this_len = MIN(max_len, len - i);
        // this cast should be safe...
        memcpy((void *) USART1_ringbuf + writeidx, str + i, this_len);
        USART1_writeidx += this_len;
        i += this_len;

        USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    }
}

void USART1_print(const char *str) {
    uint32_t len = strlen(str);
    USART1_write(str, len);
}

void USART1_directputc(const char ch) {
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, ch);
}

void USART1_directprint(const char *str) {
    uint32_t i = 0;
    uint32_t len = strlen(str);
    while (i < len) {
        USART1_directputc(str[i++]);
    }
}

void USART1_flush(void) {
    while (USART1_readidx != USART1_writeidx);
}

char USART1_getc(void) {
    while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
    return (char)USART_ReceiveData(USART1);
}

void USART2_Init(void) {
    /* enable usart clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

  
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
  
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_7);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_7);
  
    USART_InitTypeDef USART_InitStructure;
    USART_StructInit(&USART_InitStructure);
    USART_InitStructure.USART_BaudRate = 115200;
    USART_Init(USART2, &USART_InitStructure);
  
    USART_Cmd(USART2, ENABLE);
}

void USART2_write(char *str) {
    int idx = 0;
    while (str[idx]) {
        while(!USART_GetFlagStatus(USART2, USART_FLAG_TXE));
        USART_SendData(USART2, str[idx]);
        ++idx;
    }
}


extern __IO uint32_t bDeviceState;
extern __IO uint8_t Receive_Buffer[64];
__IO uint32_t packet_sent = 1;
__IO uint32_t packet_receive = 1;

void USB_UART_read(uint8_t *buf, int len) {
    if (bDeviceState == CONFIGURED) {
        CDC_Receive_DATA();
        while(!packet_receive);
        for(int i = 0; i < len; ++i)
            *buf++ = Receive_Buffer[i];
    }
}

void USB_UART_write(uint8_t *buf, int len) {
    if (bDeviceState == CONFIGURED) {
        while(!packet_sent);
        CDC_Send_DATA(buf, len);
    }
}