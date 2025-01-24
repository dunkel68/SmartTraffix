#include <SFML/Graphics.h>
#include "trafficLights.h"
#include <stdlib.h>

// Function to initialize a TrafficLights structure
void TrafficLights_init(TrafficLights* light, int state) {
    // Initialize the position
    //light->x = 300;
    //light->y = 475;
    light->currentState = state;

    // Load the appropriate texture based on the initial state
    light->texture = sfTexture_createFromFile(
        (state == 0 ? "images/red_light.png" :
         state == 2 ? "images/yellow_light.png" :
                      "images/green_light.png"),
        NULL);

    // Create and set up the sprite
    if (light->texture) {
        light->sprite = sfSprite_create();
        sfSprite_setTexture(light->sprite, light->texture, sfTrue);

        // Center the sprite origin
        sfFloatRect bounds = sfSprite_getLocalBounds(light->sprite);
        sfVector2f origin = {
            bounds.width / 2.0f,   // x-coordinate of origin
            bounds.height / 2.0f   // y-coordinate of origin
        };
        sfSprite_setOrigin(light->sprite, origin);

        // Set the position and scale
        sfSprite_setPosition(light->sprite, (sfVector2f){light->x, light->y});
        sfSprite_setScale(light->sprite, (sfVector2f){0.34f, 0.34f});
    }
}

// Function to change the traffic light state
int TrafficLights_changeLight(int currentState) {
    // Cycle through the states: 0 (red) -> 1 (yellow) -> 2 (green) -> 0
    return (currentState == 0) ? 1 : (currentState == 1) ? 2 : 0;
}

// Function to update the texture based on the current state
void TrafficLights_updateTexture(TrafficLights* light) {
    if (light->texture) {
        sfTexture_destroy(light->texture);
    }

    light->texture = sfTexture_createFromFile(
        (light->currentState == 0 ? "images/red_light.png" :
         light->currentState == 2 ? "images/yellow_light.png" :
                                    "images/green_light.png"),
        NULL);

    if (light->sprite && light->texture) {
        sfSprite_setTexture(light->sprite, light->texture, sfTrue);
    }
}

// Function to destroy a TrafficLights structure
void TrafficLights_destroy(TrafficLights* light) {
    if (light->sprite) {
        sfSprite_destroy(light->sprite);
        light->sprite = NULL;
    }

    if (light->texture) {
        sfTexture_destroy(light->texture);
        light->texture = NULL;
    }
}