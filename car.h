#ifndef CAR_H
#define CAR_H

#include <SFML/Graphics.h>
#include <SFML/System/Vector2.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

// Define the Car structure
typedef struct {
    float x, y;                     // Position of the car
    int currentDirection;           // 0: East, 1: West, 2: South, 3: North
    bool hasChangedLane;            // Tracks if the car has changed lane
    bool hasChangedDirection;       // Tracks if the car has changed direction
    char licensePlate[16];          // License plate (assuming a max length of 15 characters)
    float speed;                    // Current speed of the car
    float maxSpeed;                 // Maximum speed of the car
    sfVector2f moveDirection;       // Direction vector for movement
    int pos;                        // Position index or identifier
    bool challanStatus;             // Whether the car has been fined (challaned)
    sfSprite* sprite;               // Pointer to the SFML sprite object
    sfTexture* texture;             // Pointer to the SFML texture object
    sfVector2f position;
    char vehicleType[32];           // Vehicle type (e.g., "LightVehicle", "HeavyVehicle")
    bool isMoving;                  // Flag to track if the car is moving
    bool isPriority;
    bool isBrokenDown;
} Car;

// Function to initialize a Car structure
void Car_init(Car* car, const char* textureFile, const char* vehicleType, const char* licensePlate);
// General move function for cars
void Car_move(Car* car, float deltaTime);

// Function to free resources associated with a Car structure
void Car_destroy(Car* car);

#endif // CAR_H
