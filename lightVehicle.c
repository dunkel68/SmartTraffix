#include <SFML/Graphics.h>
#include "lightVehicle.h"
#include <stdlib.h>

// Function to generate a random license plate
void LgenerateRandomPlate(char* plate, size_t size) {
    if (size < 8) return;  // Ensure the plate buffer is large enough
    for (int i = 0; i < 3; i++) plate[i] = 'A' + (rand() % 26); // Random letters
    plate[3] = '-';
    for (int i = 0; i < 4; i++) plate[4 + i] = '0' + (rand() % 10); // Random numbers
    plate[7] = '\0'; // Null-terminate the string
}

// Function to initialize a LightVehicle structure
void LightVehicle_init(LightVehicle* lv, int pos) {
    // Initialize the base Car structure
    Car_init(&lv->base, "images/red_car_right.png", "Light Vehicle", ""); // Default texture and type
    lv->base.speed = 50.0f;
    lv->base.maxSpeed = 60.0f;
    lv->base.x = 300.0f;
    lv->base.y = 475.0f;
    lv->base.pos = pos;

    // Set the appropriate texture and position based on `pos`
    switch (pos) {
        case 1:
            lv->base.texture = sfTexture_createFromFile("images/red_car_up.png", NULL);
            break;
        case 2:
            lv->base.texture = sfTexture_createFromFile("images/red_car_down.png", NULL);
            break;
        case 3:
            lv->base.texture = sfTexture_createFromFile("images/red_car_left.png", NULL);
            break;
        default:
            lv->base.texture = sfTexture_createFromFile("images/red_car_right.png", NULL);
            break;
    }

    if (!lv->base.texture) {
        printf("Error loading texture for position %d\n", pos);
        return;
    }

    // Load the texture and set up the sprite
    lv->base.sprite = sfSprite_create();
    sfSprite_setTexture(lv->base.sprite, lv->base.texture, sfTrue);

    // Center the sprite origin (with explicit float casting)
    sfFloatRect bounds = sfSprite_getLocalBounds(lv->base.sprite);
    sfVector2f origin = {
        bounds.width / 2.0f,   // x-coordinate of origin
        bounds.height / 2.0f   // y-coordinate of origin
    };
    sfSprite_setOrigin(lv->base.sprite, origin);

    // Set initial position and scaling
    sfSprite_setPosition(lv->base.sprite, (sfVector2f){lv->base.x, lv->base.y});
    sfSprite_setScale(lv->base.sprite, (sfVector2f){0.2f, 0.2f});

    // Generate a random license plate
    LgenerateRandomPlate(lv->licensePlate, sizeof(lv->licensePlate));
    strncpy(lv->base.licensePlate, lv->licensePlate, sizeof(lv->base.licensePlate));
}


// Function to destroy a LightVehicle structure
void LightVehicle_destroy(LightVehicle* lv) {
    // Clean up the base Car structure
    Car_destroy(&lv->base);
}