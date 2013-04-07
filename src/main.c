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
    FloatRingAvg headingRing;
    headingRing.consecutiveFaults = 0;

    USART2_Init();

    Gyro_Init();
    Gyro_Calibrate();
    Compass_Init();

    int angRates[3];
    float heading;
    float headingAvg;
    RingAvg angAvg[3] = {};
    float gyroAvg[3] = {};
    
    char debugBuffer[100];

   ringInit(&angAvg[0]);
   ringInit(&angAvg[1]);
   ringInit(&angAvg[2]);
   floatRingInit(&headingRing);
    while(1) {
        for(int k = 0; k < 10; ++k) {
            /* Read Gyro Angular data */
            Gyro_ReadAngRate(angRates);
            heading = Compass_GetHeading();
            for(int i=0; i<3; ++i) {
                gyroAvg[i] = Gyro_AddAvgAngRate(&angAvg[i], angRates[i]);
            }
            headingAvg = Compass_AddAvgHeading(&headingRing, heading);
            //sprintf(debugBuffer, "heading %9.3f, average %9.3f\r\n", heading, headingAvg);
            //USART2_write(debugBuffer);
        }
        printf("c%9.3f\n", headingAvg);
        printf("g%9.3f\n", gyroAvg[2]);
    }
    
    while(1);
    return 0;
}
