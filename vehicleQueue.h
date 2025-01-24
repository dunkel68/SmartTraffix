#ifndef VEHICLE_QUEUE_H
#define VEHICLE_QUEUE_H

#include "car.h"
#include <stdlib.h>
#include <stdbool.h>

#define MAX_QUEUE_SIZE 20

typedef struct {
    Car* vehicles[MAX_QUEUE_SIZE];
    int front;
    int rear;
    int size;
} VehicleQueue;

void initQueue(VehicleQueue* queue);

int isQueueFull(VehicleQueue* queue);
int isQueueEmpty(VehicleQueue* queue);

void enqueue(VehicleQueue* queue, Car* vehicle);
Car* dequeue(VehicleQueue* queue);
void enqueueEmergency(VehicleQueue* queue, Car* vehicle);

#endif