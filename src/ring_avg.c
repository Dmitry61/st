#include <stdint.h>
#include <math.h>
#include "ring_avg.h"

void ringAdd(RingAvg *ring, int val) {
    ring->sum -= ring->data[ring->idx];
    ring->data[ring->idx] = val;
    ring->sum += ring->data[ring->idx];
    ++ring->idx;
    if(ring->idx == BUF_SIZE)
    	ring->idx = 0;
}