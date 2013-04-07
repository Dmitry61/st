#include <stdint.h>
#include <math.h>
#include "ring_avg.h"

#define abs(x) (x > 0 ? x: -x)

void ringAdd(RingAvg *ring, int val) {
    ring->sum -= ring->data[ring->idx];
    ring->data[ring->idx] = val;
    ring->sum += ring->data[ring->idx];
    ++ring->idx;
    if(ring->idx == BUF_SIZE)
    	ring->idx = 0;
}

void floatRingAdd(FloatRingAvg *ring, float val) {
	float avg = ring->sum / (float)BUF_SIZE;
	// 20 is our threshold value for preventing false data entering the buffer
	if(abs(avg - val) < 20.0) {
		ring->consecutiveFaults = 0;
	}
	if(abs(avg - val) < 20.0 || ring->consecutiveFaults > 4) {
		ring->sum -= ring->data[ring->idx];
	    ring->data[ring->idx] = val;
	    ring->sum += ring->data[ring->idx];
	    ++ring->idx;
	    if(ring->idx == BUF_SIZE)
	    	ring->idx = 0;
	} else {
		++ring->consecutiveFaults;
	}
}

void ringInit(RingAvg *ring) {
	ring->sum = 0;
	ring->idx = 0;
}

void floatRingInit(FloatRingAvg *ring) {
	for(int i = 0; i < BUF_SIZE; ++i) {
		ring->data[i] = 0.0f;
	}
	ring->consecutiveFaults = 0;
	ring->sum = 0;
	ring->idx = 0;
}