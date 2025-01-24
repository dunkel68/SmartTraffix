#ifndef POSITIONS_H
#define POSITIONS_H

#include <stdlib.h>
#include <time.h>

// Structure for positions
typedef struct {
    float x, y;
    int direction; // 1 = UP, 2 = DOWN, 3 = LEFT, 4 = RIGHT
} Positions;

// Function to initialize positions based on direction
Positions initialPositions(int direction);

// Function to initialize positions for heavy vehicles
Positions initialPositionsForHeavyVehicles();

// Function to get exit positions
Positions exitPositions();

// Function to determine turning points based on coordinates
Positions turningPoints(int x, int y);

// Function to switch lanes based on coordinates
Positions switchLane(int x, int y);

#endif // POSITIONS_H
