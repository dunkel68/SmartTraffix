#include <SFML/Graphics.h>
#include "positions.h"
#include <stdlib.h>

// Function to initialize positions based on direction
Positions initialPositions(int direction) {
    srand(time(0));
    Positions pos[8] = {
        {470, 770, 0}, {425, 770, 0}, {375, 30, 0}, {330, 30, 0},
        {30, 475, 0},  {30, 425, 0},  {770, 375, 0}, {770, 330, 0}
    };

    int random = rand() % 2;

    if (direction == 2) { // North
        if (random) {
            pos[0].direction = 1;
            return pos[0];
        } else {
            pos[1].direction = 1;
            return pos[1];
        }
    } else if (direction == 3) { // South
        if (random) {
            pos[2].direction = 2;
            return pos[2];
        } else {
            pos[3].direction = 2;
            return pos[3];
        }
    } else if (direction == 0) { // West
        if (random) {
            pos[6].direction = 3;
            return pos[6];
        } else {
            pos[7].direction = 3;
            return pos[7];
        }
    } else { // East
        if (random) {
            pos[4].direction = 4;
            return pos[4];
        } else {
            pos[5].direction = 4;
            return pos[5];
        }
    }
}

// Function to initialize positions for heavy vehicles
Positions initialPositionsForHeavyVehicles() {

    srand(time(0));
    Positions pos[8] = {
        {470, 770, 0}, {425, 770, 0}, {375, 30, 0}, {330, 30, 0},
        {30, 475, 0},  {30, 425, 0},  {770, 375, 0}, {770, 330, 0}
    };

    int random = rand() % 8;

    if (random == 0 || random == 1) {
        pos[random].direction = 1; // UP
    } else if (random == 2 || random == 3) {
        pos[random].direction = 2; // DOWN
    } else if (random == 6 || random == 7) {
        pos[random].direction = 3; // LEFT
    } else {
        pos[random].direction = 4; // RIGHT
    }

    return pos[random];
}

// Function to get exit positions
Positions exitPositions() {
    Positions pos[8] = {
        {375, 770, 0}, {330, 770, 0}, {470, 30, 0}, {425, 30, 0},
        {30, 375, 0},  {30, 330, 0},  {770, 475, 0}, {770, 425, 0}
    };

    return pos[0]; // Returns the first exit position
}

// Function to determine turning points based on coordinates
Positions turningPoints(int x, int y) {
    Positions pos[4] = {
        {470, 510, 0}, {330, 300, 0}, {510, 330, 0}, {300, 475, 0}
    };

    if (x == (int)pos[0].x && y == (int)pos[0].y) {
        pos[0].direction = 4; // RIGHT
        return pos[0];
    } else if (x == (int)pos[1].x && y == (int)pos[1].y) {
        pos[1].direction = 3; // LEFT
        return pos[1];
    } else if (x == (int)pos[2].x && y == (int)pos[2].y) {
        pos[2].direction = 1; // UP
        return pos[2];
    } else {
        pos[3].direction = 2; // DOWN
        return pos[3];
    }
}

// Function to switch lanes based on coordinates
Positions switchLane(int x, int y) {
    Positions pos[4] = {
        {425, 510, 0}, {375, 300, 0}, {510, 375, 0}, {300, 425, 0}
    };

    if (x == (int)pos[0].x && y == (int)pos[0].y) {
        return pos[0];
    } else if (x == (int)pos[1].x && y == (int)pos[1].y) {
        return pos[1];
    } else if (x == (int)pos[2].x && y == (int)pos[2].y) {
        return pos[2];
    } else {
        return pos[3];
    }
}