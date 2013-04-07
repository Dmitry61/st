#ifndef RING_AVG_H
#define RING_AVG_H

#define BUF_SIZE 100
#define sqr(x) (x*x)

typedef struct {
    int data[BUF_SIZE];
    int sum;
    int idx;
} RingAvg;

void ringAdd(RingAvg *ring, int val);

#endif
