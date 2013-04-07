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
    printf("Starting code\n");
    RingAvg accAvg[3] = {}, magAvg[3] = {};
    Gyro_Init();
    Gyro_Calibrate();
    Compass_Init();

    int angRates[3];
    RingAvg angAvg[3] = {};
    float gyroAvg[3] = {};
    
    while(1) { 
        /* Read Gyro Angular data */
        Gyro_ReadAngRate(angRates);
        for(int i=0; i<3; ++i) {
            gyroAvg[i] = Gyro_AddAvgAngRate(&angAvg[i], angRates[i]);
        }
        printf("c%9.3f\n", Compass_GetHeading(accAvg, magAvg));
        printf("g%9.3f\n", gyroAvg[2]);
    }
    
    while(1);
    return 0;
}
