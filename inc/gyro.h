#ifndef GYRO_H
#define GYRO_H

#include "ring_avg.h"

void Gyro_Init(void);
void Gyro_ReadAngRate(int* pfData);
float Gyro_AvgAngRate(RingAvg *ring);
float Gyro_AddAvgAngRate(RingAvg *rate, int value);

#endif
