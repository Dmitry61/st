#ifndef RING_AVG_H
#define RING_AVG_H

#define BUF_SIZE 100
#define sqr(x) (x*x)

typedef struct {
    int data[BUF_SIZE];
    int sum;
    int idx;
    int consecutiveFaults;
} RingAvg;

void ringAdd(RingAvg *ring, int val);

typedef struct  {
	float data[BUF_SIZE];
	float sum;
	int idx;
	int consecutiveFaults;
} FloatRingAvg;
void ringInit(RingAvg *ring);
void floatRingInit(FloatRingAvg *ring);
void floatRingAdd(FloatRingAvg *ring, float val);

#endif
