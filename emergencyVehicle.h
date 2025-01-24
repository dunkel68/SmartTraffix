#ifndef EMERGENCY_VEHICLE_H
#define EMERGENCY_VEHICLE_H

#include <SFML/Graphics.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "car.h"

// Define the EmergencyVehicle structure
typedef struct {
    Car base;                 // Base Car structure
    char licensePlate[16];    // License plate for the emergency vehicle
} EmergencyVehicle;

// Function to generate a random license plate
void EgenerateRandomPlate(char* plate, size_t size);

// Function to initialize an EmergencyVehicle structure
void EmergencyVehicle_init(EmergencyVehicle* ev, int pos);

// Function to destroy an EmergencyVehicle structure
void EmergencyVehicle_destroy(EmergencyVehicle* ev);
#endif // EMERGENCY_VEHICLE_H
