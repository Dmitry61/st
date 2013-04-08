#include "stm32f30x.h"
#include "stm32f3_discovery.h"
#include "stm32_it.h"
#include "hw_config.h"
#include "usb_init.h"
#include "usart.h"
#include "gyro.h"
#include "compass.h"
#include "ring_avg.h"
#include <stdio.h>

#define LEDS 8
Led_TypeDef leds[] = {LED3, LED5, LED7, LED9, LED10, LED8, LED6, LED4};
RCC_ClocksTypeDef RCC_Clocks;

__IO uint32_t USBConnectTimeOut =100;
__IO uint8_t DataReady = 0;
__IO uint32_t UserButtonPressed = 0;
__IO uint8_t PrevXferComplete = 1;

int main() {
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);
    Set_System();
    Set_USBClock();
    USB_Interrupts_Config();
    USB_Init();
    while(1);
}
