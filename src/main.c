#include "stm32f30x.h"
#include "stm32f3_discovery.h"
#include "stm32f30x_it.h"
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

    for(int i=0; i<LEDS; ++i) {
        STM_EVAL_LEDInit(leds[i]);
        STM_EVAL_LEDOff(leds[i]);
    }

    USART1_Init();
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    RingAvg accAvg[3] = {}, magAvg[3] = {};
    Gyro_Init();
    Compass_Init();

    int angRate[3];
    RingAvg angAvg[3] = {};
    float angInfo[3] = {};
    
    while(1) { 
        /* Read Gyro Angular data */
        Gyro_ReadAngRate(angRate);
        for(int i=0; i<3; ++i) {
            angInfo[i] = Gyro_AddAvgAngRate(&angAvg[i], angRate[i]);
        }

        printf("g, % 9.3f, % 9.3f, % 9.3f, ", angInfo[0], angInfo[1], angInfo[2]);
        printf("c: % 9.3f\r\n", Compass_GetHeading(accAvg, magAvg));
    }
    
    while(1);
    return 0;
}
