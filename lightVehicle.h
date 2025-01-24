#ifndef LIGHT_VEHICLE_H
#define LIGHT_VEHICLE_H

#include <SFML/Graphics.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "car.h"

// Define the LightVehicle structure
typedef struct {
    Car base;                 // Base Car structure (manual inheritance in C)
    char licensePlate[16];    // License plate for the light vehicle
} LightVehicle;

void LgenerateRandomPlate(char* plate, size_t size);
void LightVehicle_init(LightVehicle* lv, int pos);
void LightVehicle_destroy(LightVehicle* lv);

#endif // LIGHT_VEHICLE_H
