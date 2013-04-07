#ifndef COMPASS_H
#define COMPASS_H

#include "ring_avg.h"
#define PI 3.14159265f

enum MAG {XY, Z};

void Compass_Init(void);

/**
* @brief Read LSM303DLHC output register, and calculate the acceleration ACC=(1/SENSITIVITY)* (out_h*256+out_l)/16 (12 bit rappresentation)
* @param pnData: pointer to float buffer where to store data
* @retval None
*/
void Compass_ReadAcc(int* pfData);

/**
  * @brief  calculate the magnetic field Magn.
* @param  pfData: pointer to the data out
  * @retval None
  */
void Compass_ReadMag (int* pfData);
float Compass_AvgAcc(RingAvg *ring);
float Compass_AddAvgAcc(RingAvg *ring, int val);
float Compass_AvgMag(RingAvg *ring, enum MAG mag);
float Compass_AddAvgMag(RingAvg *ring, enum MAG mag, int val);
float Compass_GetHeading();
float Compass_AvgHeading(FloatRingAvg *ring);
float Compass_AddAvgHeading(FloatRingAvg *ring, float val);

#endif