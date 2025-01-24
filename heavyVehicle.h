#ifndef HEAVY_VEHICLE_H
#define HEAVY_VEHICLE_H

#include <SFML/Graphics.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "car.h"

// Define the HeavyVehicle structure
typedef struct {
    Car base;                 // Base Car structure
    char licensePlate[16];    // License plate for the heavy vehicle
} HeavyVehicle;

// Function to generate a random license plate
void HgenerateRandomPlate(char* plate, size_t size);
// Function to initialize a HeavyVehicle structure
void HeavyVehicle_init(HeavyVehicle* hv, int pos);

// Function to destroy a HeavyVehicle structure
void HeavyVehicle_destroy(HeavyVehicle* hv);

#endif // HEAVY_VEHICLE_H
