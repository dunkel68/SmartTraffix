#include <SFML/Graphics.h>
#include "car.h"
#include <math.h>
#include <stdlib.h>

// Function to initialize a Car structure
void Car_init(Car* car, const char* textureFile, const char* vehicleType, const char* licensePlate) {
    if (!car || !textureFile || !vehicleType || !licensePlate) {
        fprintf(stderr, "Invalid input to Car_init\n");
        return;
    }
    car->x = 0;
    car->y = 0;
    car->currentDirection = 0;
    car->hasChangedLane = false;
    car->hasChangedDirection = false;
    strncpy(car->licensePlate, licensePlate, sizeof(car->licensePlate) - 1);
    car->licensePlate[sizeof(car->licensePlate) - 1] = '\0'; // Ensure null termination
    car->speed = 0.0f;
    car->maxSpeed = 0.0f;
    car->moveDirection = (sfVector2f){0, 0};
    car->pos = 0;
    car->challanStatus = false;
    car->isMoving = true;

    // Load the texture and sprite
    car->texture = sfTexture_createFromFile(textureFile, NULL);
    if (!car->texture) {
        fprintf(stderr, "Failed to load texture: %s\n", textureFile);
        return;
    }

    car->sprite = sfSprite_create();
    if (!car->sprite) {
        fprintf(stderr, "Failed to create sprite\n");
        sfTexture_destroy(car->texture);
        return;
    }

    sfSprite_setTexture(car->sprite, car->texture, sfTrue);
    strncpy(car->vehicleType, vehicleType, sizeof(car->vehicleType) - 1);
    car->vehicleType[sizeof(car->vehicleType) - 1] = '\0'; // Ensure null termination
}

// General move function for cars
void Car_move(Car* car, float deltaTime) {
    // Update the position based on speed and direction
    if (!car || !car->sprite) {
        fprintf(stderr, "Invalid car or sprite in Car_move\n");
        return;
    }
    //car->x += car->speed * deltaTime;
    car->x += car->speed * deltaTime * cos(car->currentDirection);
    car->y += car->speed * deltaTime * sin(car->currentDirection);
    sfSprite_setPosition(car->sprite, (sfVector2f){car->x, car->y});
}

// Function to free resources associated with a Car structure
void Car_destroy(Car* car) {
    if (!car) return;

    if (car->sprite) {
        sfSprite_destroy(car->sprite);
        car->sprite = NULL;
    }
    if (car->texture) {
        sfTexture_destroy(car->texture);
        car->texture = NULL;
    }
    return;
}
