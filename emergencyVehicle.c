#include <SFML/Graphics.h>
#include "emergencyVehicle.h"
#include <stdlib.h>

// Function to generate a random license plate
void EgenerateRandomPlate(char* plate, size_t size) {
    if (size < 8) return;  // Ensure the plate buffer is large enough
    for (int i = 0; i < 3; i++) plate[i] = 'A' + (rand() % 26); // Random letters
    plate[3] = '-';
    for (int i = 0; i < 4; i++) plate[4 + i] = '0' + (rand() % 10); // Random numbers
    plate[7] = '\0'; // Null-terminate the string
}

// Function to initialize an EmergencyVehicle structure
void EmergencyVehicle_init(EmergencyVehicle* ev, int pos) {
    // Initialize the base Car structure
    Car_init(&ev->base, "images/emg_car_right.png", "Emergency Vehicle", ""); // Default texture and type
    ev->base.speed = 50.0f;
    ev->base.maxSpeed = 80.0f;
    ev->base.x = 550.0f;
    ev->base.y = 90.0f;
    ev->base.pos = pos;

    // Set the appropriate texture and position based on `pos`
    switch (pos) {
        case 1:
            ev->base.texture = sfTexture_createFromFile("images/emg_car_up.png", NULL);
            break;
        case 2:
            ev->base.texture = sfTexture_createFromFile("images/emg_car_down.png", NULL);
            break;
        case 3:
            ev->base.texture = sfTexture_createFromFile("images/emg_car_left.png", NULL);
            break;
        default:
            ev->base.texture = sfTexture_createFromFile("images/emg_car_right.png", NULL);
            break;
    }

    // Load the texture and set up the sprite
    if (ev->base.texture) {
        ev->base.sprite = sfSprite_create();
        sfSprite_setTexture(ev->base.sprite, ev->base.texture, sfTrue);

        // Center the sprite origin
        sfFloatRect bounds = sfSprite_getLocalBounds(ev->base.sprite);
        sfVector2f origin = {
            bounds.width / 2.0f,   // x-coordinate of origin
            bounds.height / 2.0f   // y-coordinate of origin
        };
        sfSprite_setOrigin(ev->base.sprite, origin);

        // Set initial position and scaling
        sfSprite_setPosition(ev->base.sprite, (sfVector2f){ev->base.x, ev->base.y});

        // Apply scaling based on position
        switch (pos) {
            case 1:
                sfSprite_setScale(ev->base.sprite, (sfVector2f){0.24f, 0.24f});
                break;
            case 2:
                sfSprite_setScale(ev->base.sprite, (sfVector2f){0.34f, 0.34f});
                break;
            case 3:
                sfSprite_setScale(ev->base.sprite, (sfVector2f){0.24f, 0.24f});
                break;
            default:
                sfSprite_setScale(ev->base.sprite, (sfVector2f){0.44f, 0.44f});
                break;
        }
    }

    // Generate a random license plate
    EgenerateRandomPlate(ev->licensePlate, sizeof(ev->licensePlate));
    strncpy(ev->base.licensePlate, ev->licensePlate, sizeof(ev->base.licensePlate));
}

// Function to destroy an EmergencyVehicle structure
void EmergencyVehicle_destroy(EmergencyVehicle* ev) {
    // Clean up the base Car structure
    Car_destroy(&ev->base);
}