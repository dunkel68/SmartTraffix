#ifndef BANKER_ALGO_H
#define BANKER_ALGO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_RESOURCES 4  // Representing the 4 directions
#define MAX_PROCESSES 20 // Maximum number of vehicles we'll consider at intersections

typedef struct {
    int allocation[MAX_PROCESSES][MAX_RESOURCES];
    int max[MAX_PROCESSES][MAX_RESOURCES];
    int available[MAX_RESOURCES];
    int need[MAX_PROCESSES][MAX_RESOURCES];
} BankerState;


void initializeBankerState(BankerState bankerState);
#endif