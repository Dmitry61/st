#include <stdint.h>
#include <math.h>
#include "ring_avg.h"
#include "compass.h"

#define abs(x) (x > 0 ? x: -x)


float vectorAngle(vector vec) {
	return (atan2f(vec.y, vec.x) * 180) / PI;
}

void ringAdd(RingAvg *ring, int val) {
    ring->sum -= ring->data[ring->idx];
    ring->data[ring->idx] = val;
    ring->sum += ring->data[ring->idx];
    ++ring->idx;
    if(ring->idx == BUF_SIZE)
    	ring->idx = 0;
}

void vectorRingAdd(VectorRingAvg *ring, vector val) {
	float avg = vectorAngle(ring->sum);
	float valAng = vectorAngle(val);
	// 20 is our threshold value for preventing false data entering the buffer
	if(abs(avg - valAng) < 20.0) {
		ring->consecutiveFaults = 0;
	}
	
	if(abs(avg - valAng) < 20.0 || ring->consecutiveFaults > 4) {
		ring->sum.x -= ring->data[ring->idx].x;
		ring->sum.y -= ring->data[ring->idx].y;
	    ring->data[ring->idx] = val;
	    ring->sum.x += ring->data[ring->idx].x;
	    ring->sum.y += ring->data[ring->idx].y;
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

void vectorRingInit(VectorRingAvg *ring) {
	for(int i = 0; i < BUF_SIZE; ++i) {
		ring->data[i].x = ring->data[i].y = 0;
	}
	ring->consecutiveFaults = 0;
	ring->sum.x = ring->sum.y = 0;
	ring->idx = 0;
}