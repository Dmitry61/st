#ifndef RING_AVG_H
#define RING_AVG_H

#define BUF_SIZE 50
#define sqr(x) (x*x)
typedef struct {
	int x;
	int y;
} vector;

typedef struct {
    int data[BUF_SIZE];
    int sum;
    int idx;
    int consecutiveFaults;
} RingAvg;

void ringAdd(RingAvg *ring, int val);

typedef struct  {
	vector data[BUF_SIZE];
	vector sum;
	int idx;
	int consecutiveFaults;
} VectorRingAvg;
void ringInit(RingAvg *ring);
void vectorRingInit(VectorRingAvg *ring);
void vectorRingAdd(VectorRingAvg *ring, vector val);
float vectorAngle(vector vec);

#endif
