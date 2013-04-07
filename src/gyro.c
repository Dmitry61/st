#include "gyro.h"
#include "stm32f30x.h"
#include "stm32f3_discovery_l3gd20.h"
#include <math.h>

#define L3G_Sensitivity_250dps     (float)   114.285f         /*!< gyroscope sensitivity with 250 dps full scale [LSB/dps] */
#define L3G_Sensitivity_500dps     (float)    57.1429f        /*!< gyroscope sensitivity with 500 dps full scale [LSB/dps] */
#define L3G_Sensitivity_2000dps    (float)    14.285f	      /*!< gyroscope sensitivity with 2000 dps full scale [LSB/dps] */

static int Gyro_offset[3];

void Gyro_Init(void)
{
  L3GD20_InitTypeDef L3GD20_InitStructure;
  L3GD20_FilterConfigTypeDef L3GD20_FilterStructure;
  
  /* Configure Mems L3GD20 */
  L3GD20_InitStructure.Power_Mode = L3GD20_MODE_ACTIVE;
  L3GD20_InitStructure.Output_DataRate = L3GD20_OUTPUT_DATARATE_4;
  L3GD20_InitStructure.Axes_Enable = L3GD20_AXES_ENABLE;
  L3GD20_InitStructure.Band_Width = L3GD20_BANDWIDTH_4;
  L3GD20_InitStructure.BlockData_Update = L3GD20_BlockDataUpdate_Continous;
  L3GD20_InitStructure.Endianness = L3GD20_BLE_LSB;
  L3GD20_InitStructure.Full_Scale = L3GD20_FULLSCALE_500; 
  L3GD20_Init(&L3GD20_InitStructure);
   
  L3GD20_FilterStructure.HighPassFilter_Mode_Selection =L3GD20_HPM_NORMAL_MODE_RES;
  L3GD20_FilterStructure.HighPassFilter_CutOff_Frequency = L3GD20_HPFCF_0;
  L3GD20_FilterConfig(&L3GD20_FilterStructure) ;
  
  L3GD20_FilterCmd(L3GD20_HIGHPASSFILTER_ENABLE);
}

/**
  * @brief  Calculate the angular Data rate Gyroscope.
  * @param  pfData : Data out pointer
  * @retval None
  */
static void Gyro_readRaw(int *pfData) {
  uint8_t tmpbuffer[6] ={0};
  int i = 0;

  uint8_t STATG = 0;
  // waits until new data is avialable
  while(!(STATG & 0x08)) {
    L3GD20_Read(&STATG,L3GD20_STATUS_REG_ADDR,1);
  }
  
  L3GD20_Read(tmpbuffer,L3GD20_OUT_X_L_ADDR,6);
  
  
  for(i=0; i<3; i++)
    pfData[i]=(int16_t)(((uint16_t)tmpbuffer[2*i+1] << 8) + tmpbuffer[2*i]);
}
void Gyro_ReadAngRate (int* pfData)
{
  Gyro_readRaw(pfData);
  pfData[0] -= Gyro_offset[0];
  pfData[1] -= Gyro_offset[1];
  pfData[2] -= Gyro_offset[2];
}

float Gyro_AvgAngRate(RingAvg *ring) {
  float sensitivity = 0;
  sensitivity=L3G_Sensitivity_500dps;

  return (float)ring->sum / ((float)BUF_SIZE * sensitivity);
}

float Gyro_AddAvgAngRate(RingAvg *ring, int val) {
  ringAdd(ring, val);
  return Gyro_AvgAngRate(ring);
}

void Gyro_Calibrate() {
  int gyro[3];

  for(int i = 0; i<128; ++i) {
    Gyro_readRaw(gyro);
    Gyro_offset[0] += gyro[0];
    Gyro_offset[1] += gyro[1];
    Gyro_offset[2] += gyro[2];
  }
  Gyro_offset[0] /= 128;
  Gyro_offset[1] /= 128;
  Gyro_offset[2] /= 128;
}