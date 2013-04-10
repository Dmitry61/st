#ifndef USART_H
#define USART_H

#include "platform_config.h"

#define RINGBUF_SIZE_BITS 7

void USART1_Init(void);
void USART1_IRQHandler(void);
void USART1_putc(char ch);
void USART1_write(const char *str, int len);
void USART1_print(const char *str);
void USART1_directputc(const char ch);
void USART1_directprint(const char *str);
void USART1_flush(void);
char USART1_getc(void);
void USART2_Init(void);
void USART2_write(char *str);
void USB_UART_read(uint8_t *buf, int len);
void USB_UART_write(uint8_t *buf, int len);

#endif
