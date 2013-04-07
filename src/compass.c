#include "compass.h"
#include "stm32f3_discovery_lsm303dlhc.h"
#include <math.h>

#define LSM_Acc_Sensitivity_2g     (float)     1.0f            /*!< accelerometer sensitivity with 2 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_4g     (float)     0.5f            /*!< accelerometer sensitivity with 4 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_8g     (float)     0.25f           /*!< accelerometer sensitivity with 8 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_16g    (float)     0.0834f         /*!< accelerometer sensitivity with 12 g full scale [LSB/mg] */


void Compass_Init(void)
{
  LSM303DLHCMag_InitTypeDef LSM303DLHC_InitStructure;
  LSM303DLHCAcc_InitTypeDef LSM303DLHCAcc_InitStructure;
  LSM303DLHCAcc_FilterConfigTypeDef LSM303DLHCFilter_InitStructure;
  
  /* Configure MEMS magnetometer main parameters: temp, working mode, full Scale and Data rate */
  LSM303DLHC_InitStructure.Temperature_Sensor = LSM303DLHC_TEMPSENSOR_DISABLE;
  LSM303DLHC_InitStructure.MagOutput_DataRate = LSM303DLHC_ODR_220_HZ;
  LSM303DLHC_InitStructure.MagFull_Scale = LSM303DLHC_FS_8_1_GA;
  LSM303DLHC_InitStructure.Working_Mode = LSM303DLHC_CONTINUOS_CONVERSION;
  LSM303DLHC_MagInit(&LSM303DLHC_InitStructure);
  
   /* Fill the accelerometer structure */
  LSM303DLHCAcc_InitStructure.Power_Mode = LSM303DLHC_NORMAL_MODE;
  LSM303DLHCAcc_InitStructure.AccOutput_DataRate = LSM303DLHC_ODR_1344_HZ;
  LSM303DLHCAcc_InitStructure.Axes_Enable= LSM303DLHC_AXES_ENABLE;
  LSM303DLHCAcc_InitStructure.AccFull_Scale = LSM303DLHC_FULLSCALE_2G;
  LSM303DLHCAcc_InitStructure.BlockData_Update = LSM303DLHC_BlockUpdate_Continous;
  LSM303DLHCAcc_InitStructure.Endianness=LSM303DLHC_BLE_LSB;
  LSM303DLHCAcc_InitStructure.High_Resolution=LSM303DLHC_HR_ENABLE;
  /* Configure the accelerometer main parameters */
  LSM303DLHC_AccInit(&LSM303DLHCAcc_InitStructure);
  
  /* Fill the accelerometer LPF structure */
  LSM303DLHCFilter_InitStructure.HighPassFilter_Mode_Selection =LSM303DLHC_HPM_NORMAL_MODE;
  LSM303DLHCFilter_InitStructure.HighPassFilter_CutOff_Frequency = LSM303DLHC_HPFCF_16;
  LSM303DLHCFilter_InitStructure.HighPassFilter_AOI1 = LSM303DLHC_HPF_AOI1_DISABLE;
  LSM303DLHCFilter_InitStructure.HighPassFilter_AOI2 = LSM303DLHC_HPF_AOI2_DISABLE;

  /* Configure the accelerometer LPF main parameters */
  LSM303DLHC_AccFilterConfig(&LSM303DLHCFilter_InitStructure);
}

/**
* @brief Read LSM303DLHC output register, and calculate the acceleration ACC=(1/SENSITIVITY)* (out_h*256+out_l)/16 (12 bit rappresentation)
* @param pnData: pointer to float buffer where to store data
* @retval None
*/
void Compass_ReadAcc(int* pfData)
{
  int16_t pnRawData[3];
  uint8_t ctrlx[2];
  uint8_t buffer[6], cDivider;
  uint8_t i = 0;
  
  uint8_t STATA = 0;
  // waits until new data is avialable
  while(!(STATA & 0x08)) {
    LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_STATUS_REG_A, &STATA, 1);
  }

  /* Read the register content */
  LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_CTRL_REG4_A, ctrlx,2);
  LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_OUT_X_L_A, buffer, 6);

  if(ctrlx[1]&0x40)
    cDivider=64;
else
    cDivider=16;

  /* check in the control register4 the data alignment*/
  if(!(ctrlx[0] & 0x40) || (ctrlx[1] & 0x40)) /* Little Endian Mode or FIFO mode */
{
    for(i=0; i<3; i++)
    {
      pnRawData[i]=((int16_t)((uint16_t)buffer[2*i+1] << 8) + buffer[2*i])/cDivider;
  }
}
  else /* Big Endian Mode */
{
    for(i=0; i<3; i++)
      pnRawData[i]=((int16_t)((uint16_t)buffer[2*i] << 8) + buffer[2*i+1])/cDivider;
}
  /* Read the register content */
LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_CTRL_REG4_A, ctrlx,2);

  /* Obtain the mg value for the three axis */
for(i=0; i<3; i++)
{
    // pfData[i]=(float)pnRawData[i]/LSM_Acc_Sensitivity;
    pfData[i]=pnRawData[i];
}

}

/**
  * @brief  calculate the magnetic field Magn.
* @param  pfData: pointer to the data out
  * @retval None
  */
  void Compass_ReadMag(int* pfData)
  {
      static uint8_t buffer[6] = {0};
      uint8_t STATM = 0;
      uint8_t i =0;

      // waits until new data is avialable
      while(!(STATM & 0x01)) {
        LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_SR_REG_M, &STATM, 1);
      }

      LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_X_H_M, buffer, 1);
      LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_X_L_M, buffer+1, 1);
      LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_Y_H_M, buffer+2, 1);
      LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_Y_L_M, buffer+3, 1);
      LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_Z_H_M, buffer+4, 1);
      LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_Z_L_M, buffer+5, 1);

      for(i=0; i<2; i++)
      {
        pfData[i]=(int16_t)(((uint16_t)buffer[2*i] << 8) + buffer[2*i+1])*1000;
    }
    pfData[2]=(int16_t)(((uint16_t)buffer[4] << 8) + buffer[5])*1000;
}

float Compass_AvgAcc(RingAvg *ring) {
  float LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_2g;
  uint8_t ctrlx[2];
  /* Read the register content */
  LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_CTRL_REG4_A, ctrlx,2);

  if(ctrlx[1]&0x40)
  {
    /* FIFO mode */
    LSM_Acc_Sensitivity = 0.25;
}
else
{
    /* normal mode */
    /* switch the sensitivity value set in the CRTL4*/
    switch(ctrlx[0] & 0x30)
    {
        case LSM303DLHC_FULLSCALE_2G:
        LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_2g;
        break;
        case LSM303DLHC_FULLSCALE_4G:
        LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_4g;
        break;
        case LSM303DLHC_FULLSCALE_8G:
        LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_8g;
        break;
        case LSM303DLHC_FULLSCALE_16G:
        LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_16g;
        break;
    }
}
return ring->sum / (float)(BUF_SIZE * LSM_Acc_Sensitivity);
}

float Compass_AddAvgAcc(RingAvg *ring, int val) {
  ringAdd(ring, val);
  return Compass_AvgAcc(ring);
}

float Compass_AvgMag(RingAvg *ring, enum MAG mag) {
  uint16_t Magn_Sensitivity_XY = 0, Magn_Sensitivity_Z = 0;
  uint8_t CTRLB = 0;

  LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_CRB_REG_M, &CTRLB, 1);

 /* Switch the sensitivity set in the CRTLB*/
  switch(CTRLB & 0xE0)
  {
      case LSM303DLHC_FS_1_3_GA:
      Magn_Sensitivity_XY = LSM303DLHC_M_SENSITIVITY_XY_1_3Ga;
      Magn_Sensitivity_Z = LSM303DLHC_M_SENSITIVITY_Z_1_3Ga;
      break;
      case LSM303DLHC_FS_1_9_GA:
      Magn_Sensitivity_XY = LSM303DLHC_M_SENSITIVITY_XY_1_9Ga;
      Magn_Sensitivity_Z = LSM303DLHC_M_SENSITIVITY_Z_1_9Ga;
      break;
      case LSM303DLHC_FS_2_5_GA:
      Magn_Sensitivity_XY = LSM303DLHC_M_SENSITIVITY_XY_2_5Ga;
      Magn_Sensitivity_Z = LSM303DLHC_M_SENSITIVITY_Z_2_5Ga;
      break;
      case LSM303DLHC_FS_4_0_GA:
      Magn_Sensitivity_XY = LSM303DLHC_M_SENSITIVITY_XY_4Ga;
      Magn_Sensitivity_Z = LSM303DLHC_M_SENSITIVITY_Z_4Ga;
      break;
      case LSM303DLHC_FS_4_7_GA:
      Magn_Sensitivity_XY = LSM303DLHC_M_SENSITIVITY_XY_4_7Ga;
      Magn_Sensitivity_Z = LSM303DLHC_M_SENSITIVITY_Z_4_7Ga;
      break;
      case LSM303DLHC_FS_5_6_GA:
      Magn_Sensitivity_XY = LSM303DLHC_M_SENSITIVITY_XY_5_6Ga;
      Magn_Sensitivity_Z = LSM303DLHC_M_SENSITIVITY_Z_5_6Ga;
      break;
      case LSM303DLHC_FS_8_1_GA:
      Magn_Sensitivity_XY = LSM303DLHC_M_SENSITIVITY_XY_8_1Ga;
      Magn_Sensitivity_Z = LSM303DLHC_M_SENSITIVITY_Z_8_1Ga;
      break;
  }
  
  if(mag == XY)
    return ring->sum / (float)(BUF_SIZE * Magn_Sensitivity_XY);
else if(mag == Z)
    return ring->sum / (float)(BUF_SIZE * Magn_Sensitivity_Z);
}

float Compass_AddAvgMag(RingAvg *ring, enum MAG mag, int val) {
  ringAdd(ring, val);
  return Compass_AvgMag(ring, mag);
}

float Compass_GetHeading(RingAvg accAvg[], RingAvg magAvg[]) {
    float fNormAcc,fSinRoll,fCosRoll,fSinPitch,fCosPitch, RollAng, PitchAng;
    float fTiltedX,fTiltedY;

    int acc[3];
    int mag[3];

    float magInfo[3] = {}, accInfo[3] = {};

    Compass_ReadAcc(acc);
    Compass_ReadMag(mag);

    for(int i=0; i<3; ++i) {
        accInfo[i] = Compass_AddAvgAcc(&accAvg[i], acc[i]);
        if(i <= 1) {
            enum MAG mag_dir = XY;
            magInfo[i] = Compass_AddAvgMag(&magAvg[i], mag_dir, mag[i]);
        }
        else {
            enum MAG mag_dir = Z;
            magInfo[i] = Compass_AddAvgMag(&magAvg[i], mag_dir, mag[i]);
        }
    }

    for(int i=0;i<3;i++)
        accInfo[i] /= 100.0f;
    fNormAcc = sqrt(sqr(accInfo[0])+sqr(accInfo[1])+sqr(accInfo[2]));

    fSinRoll = -accInfo[1]/fNormAcc;
    fCosRoll = sqrt(1.0-sqr(fSinRoll));
    fSinPitch = accInfo[0]/fNormAcc;
    fCosPitch = sqrt(1.0-sqr(fSinPitch));
    if ( fSinRoll >0) {
        if (fCosRoll>0) {
            RollAng = acos(fCosRoll)*180/PI;
        } else {
            RollAng = acos(fCosRoll)*180/PI + 180;
        }
    } else {
        if (fCosRoll>0) {
            RollAng = acos(fCosRoll)*180/PI + 360;
        } else {
            RollAng = acos(fCosRoll)*180/PI + 180;
        }
    }

    if ( fSinPitch >0) {
        if (fCosPitch>0) {
            PitchAng = acos(fCosPitch)*180/PI;
        } else {
            PitchAng = acos(fCosPitch)*180/PI + 180;
        }
    } else {
        if (fCosPitch>0) {
            PitchAng = acos(fCosPitch)*180/PI + 360;
        } else {
            PitchAng = acos(fCosPitch)*180/PI + 180;
        }
    }

    if (RollAng >=360) {
        RollAng = RollAng - 360;
    }

    if (PitchAng >=360) {
        PitchAng = PitchAng - 360;
    }

    fTiltedX = magInfo[0]*fCosPitch+magInfo[2]*fSinPitch;
    fTiltedY = magInfo[0]*fSinRoll*fSinPitch+magInfo[1]*fCosRoll-magInfo[1]*fSinRoll*fCosPitch;
    float heading = atan2f((float)fTiltedY,(float)fTiltedX)*180/PI;
    return heading;
}