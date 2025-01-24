#include <SFML/Graphics.h>
#include "heavyVehicle.h"
#include <stdlib.h>

// Function to generate a random license plate
void HgenerateRandomPlate(char* plate, size_t size) {
    if (size < 8) return;  // Ensure the plate buffer is large enough
    for (int i = 0; i < 3; i++) plate[i] = 'A' + (rand() % 26); // Random letters
    plate[3] = '-';
    for (int i = 0; i < 4; i++) plate[4 + i] = '0' + (rand() % 10); // Random numbers
    plate[7] = '\0'; // Null-terminate the string
}

// Function to initialize a HeavyVehicle structure
void HeavyVehicle_init(HeavyVehicle* hv, int pos) {
    // Initialize the base Car structure
    Car_init(&hv->base, "images/truck_right.png", "Heavy Vehicle", ""); // Default texture and type
    hv->base.speed = 50.0f;
    hv->base.maxSpeed = 40.0f;
    hv->base.x = 550.0f;
    hv->base.y = 90.0f;
    hv->base.pos = pos;

    // Set the appropriate texture and position based on `pos`
    switch (pos) {
        case 1:
            hv->base.texture = sfTexture_createFromFile("images/truck_up.png", NULL);
            break;
        case 2:
            hv->base.texture = sfTexture_createFromFile("images/truck_down.png", NULL);
            break;
        case 3:
            hv->base.texture = sfTexture_createFromFile("images/truck_left.png", NULL);
            break;
        default:
            hv->base.texture = sfTexture_createFromFile("images/truck_right.png", NULL);
            break;
    }

    // Load the texture and set up the sprite
    if (hv->base.texture) {
        hv->base.sprite = sfSprite_create();
        sfSprite_setTexture(hv->base.sprite, hv->base.texture, sfTrue);

        // Center the sprite origin
        sfFloatRect bounds = sfSprite_getLocalBounds(hv->base.sprite);
        sfVector2f origin = {
            bounds.width / 2.0f,   // x-coordinate of origin
            bounds.height / 2.0f   // y-coordinate of origin
        };
    sfSprite_setOrigin(hv->base.sprite, origin);

        // Set initial position and scaling
        sfSprite_setPosition(hv->base.sprite, (sfVector2f){hv->base.x, hv->base.y});
        sfSprite_setScale(hv->base.sprite, (sfVector2f){0.54f, 0.54f});
    }

    // Generate a random license plate
    HgenerateRandomPlate(hv->licensePlate, sizeof(hv->licensePlate));
    strncpy(hv->base.licensePlate, hv->licensePlate, sizeof(hv->base.licensePlate));
}

// Function to destroy a HeavyVehicle structure
void HeavyVehicle_destroy(HeavyVehicle* hv) {
    // Clean up the base Car structure
    Car_destroy(&hv->base);
}