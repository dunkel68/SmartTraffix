#ifndef TRAFFICLIGHTS_H
#define TRAFFICLIGHTS_H

#include <SFML/Graphics.h>
#include <stdlib.h>

// Define the TrafficLights structure
typedef struct {
    sfTexture* texture;      // Texture for the traffic light
    sfSprite* sprite;        // Sprite for the traffic light
    float x, y;                // Position of the traffic light
    int currentState;        // Current state (0 = red, 1 = yellow, 2 = green)
} TrafficLights;

// Function to initialize a TrafficLights structure
void TrafficLights_init(TrafficLights* light, int state);

void TrafficLights_updateTexture(TrafficLights* light);

// Function to destroy a TrafficLights structure
void TrafficLights_destroy(TrafficLights* light);

#endif // TRAFFICLIGHTS_H
