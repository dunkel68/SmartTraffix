#include "toeTruck.h"
#include "car.h"
#include <SFML/Graphics/Sprite.h>
#include <SFML/Graphics/Texture.h>
#include <SFML/Graphics/Types.h>
#include <math.h>
#include <stdio.h>


void initToeTruck(ToeTruck* toeTruck, int pos) {
    // Initialize the base Car structure
    Car_init(&toeTruck->base, "images/truck_right.png", "Toe Truck", ""); // Default texture and type
    toeTruck->base.speed = 50.0f;
    toeTruck->base.maxSpeed = 40.0f;
    toeTruck->base.x = 550.0f;
    toeTruck->base.y = 90.0f;
    toeTruck->base.pos = pos;

    // Set the appropriate texture and position based on `pos`
    switch (pos) {
        case 1:
            toeTruck->base.texture = sfTexture_createFromFile("images/truck_up.png", NULL);
            break;
        case 2:
            toeTruck->base.texture = sfTexture_createFromFile("images/truck_down.png", NULL);
            break;
        case 3:
            toeTruck->base.texture = sfTexture_createFromFile("images/truck_left.png", NULL);
            break;
        default:
            toeTruck->base.texture = sfTexture_createFromFile("images/truck_right.png", NULL);
            break;
    }

    // Load the texture and set up the sprite
    if (toeTruck->base.texture) {
        toeTruck->base.sprite = sfSprite_create();
        sfSprite_setTexture(toeTruck->base.sprite, toeTruck->base.texture, sfTrue);

        // Center the sprite origin
        sfFloatRect bounds = sfSprite_getLocalBounds(toeTruck->base.sprite);
        sfVector2f origin = {
            bounds.width / 2.0f,   // x-coordinate of origin
            bounds.height / 2.0f   // y-coordinate of origin
        };
    sfSprite_setOrigin(toeTruck->base.sprite, origin);

        // Set initial position and scaling
        sfSprite_setPosition(toeTruck->base.sprite, (sfVector2f){toeTruck->base.x, toeTruck->base.y});
        sfSprite_setScale(toeTruck->base.sprite, (sfVector2f){0.54f, 0.54f});
    }

}

// Add this function to update the toe truck's position
void updateToeTruck(ToeTruck toeTruck, float deltaTime) {
    if (toeTruck.isActive) {
        sfVector2f direction;
        direction.x = toeTruck.target.x - toeTruck.base.position.x;
        direction.y = toeTruck.target.y - toeTruck.base.position.y;
        
        float length = sqrt(direction.x * direction.x + direction.y * direction.y);
        
        if (length > 1.0f) {
            direction.x /= length;
            direction.y /= length;
            
            toeTruck.base.position.x += direction.x * toeTruck.base.speed * deltaTime;
            toeTruck.base.position.y += direction.y * toeTruck.base.speed * deltaTime;
            
            sfSprite_setPosition(toeTruck.base.sprite, toeTruck.base.position);
        } else {
            // Toe truck has reached the broken vehicle
            toeTruck.isActive = false;
            
        }
    }
}