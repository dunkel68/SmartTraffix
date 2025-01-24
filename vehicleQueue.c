#include "vehicleQueue.h"

void initQueue(VehicleQueue* queue) {
    queue->front = 0;
    queue->rear = -1;
    queue->size = 0;
}

int isQueueFull(VehicleQueue* queue) {
    return queue->size == MAX_QUEUE_SIZE;
}

int isQueueEmpty(VehicleQueue* queue) {
    return queue->size == 0;
}

void enqueue(VehicleQueue* queue, Car* vehicle) {
    if (isQueueFull(queue)) {
        printf("Queue is full. Cannot add more vehicles.\n");
        return;
    }
    queue->rear = (queue->rear + 1) % MAX_QUEUE_SIZE;
    queue->vehicles[queue->rear] = vehicle;
    queue->size++;
}

Car* dequeue(VehicleQueue* queue) {
    if (isQueueEmpty(queue)) {
        return NULL;
    }
    Car* vehicle = queue->vehicles[queue->front];
    queue->front = (queue->front + 1) % MAX_QUEUE_SIZE;
    queue->size--;
    return vehicle;
}

void enqueueEmergency(VehicleQueue* queue, Car* vehicle) {
    if (isQueueFull(queue)) {
        printf("Queue is full. Cannot add emergency vehicle.\n");
        return;
    }
    // Shift all vehicles one position back
    for (int i = queue->size; i > 0; i--) {
        int index = (queue->front + i) % MAX_QUEUE_SIZE;
        int prevIndex = (queue->front + i - 1) % MAX_QUEUE_SIZE;
        queue->vehicles[index] = queue->vehicles[prevIndex];
    }
    // Add emergency vehicle at the front
    queue->vehicles[queue->front] = vehicle;
    queue->rear = (queue->rear + 1) % MAX_QUEUE_SIZE;
    queue->size++;
}