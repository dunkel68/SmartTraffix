#ifndef TOE_TRUCK_H
#define TOE_TRUCK_H

#include "car.h"
#include <SFML/Graphics.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

typedef struct {
    Car base;
    bool isActive;
    sfVector2f target;
} ToeTruck;

void initToeTruck(ToeTruck *t, int pos);
void updateToeTruck(ToeTruck towTruck, float deltaTime);



#endif