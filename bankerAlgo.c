#include "bankerAlgo.h"

void initializeBankerState(BankerState bankerState) {
    memset(bankerState.allocation, 0, sizeof(bankerState.allocation));
    memset(bankerState.max, 0, sizeof(bankerState.max));
    memset(bankerState.need, 0, sizeof(bankerState.need));
    
    // Initialize available resources (1 for each direction)
    for (int i = 0; i < MAX_RESOURCES; i++) {
        bankerState.available[i] = 1;
    }
}
