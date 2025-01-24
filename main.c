#include <SFML/Graphics.h>
#include <SFML/Audio.h>
#include <SFML/Graphics/Color.h>
#include <SFML/Graphics/Sprite.h>
#include <SFML/Graphics/Text.h>
#include <SFML/Graphics/Types.h>
#include <SFML/System/Vector2.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
//#include <math.h>
#include <pthread.h>
#include "bankerAlgo.h"
#include "positions.h"
#include "trafficLights.h"
#include "car.h"
#include "heavyVehicle.h"
#include "lightVehicle.h"
#include "emergencyVehicle.h"
#include "challan.h"
#include "vehicleQueue.h"
#include "toeTruck.h"

#define WIDTH 800
#define HEIGHT 600
#define CAR_WIDTH 50
#define CAR_HEIGHT 100

#define TITLE "Smart Traffix"
#define MAX_VEHICLES 100
#define MAX_QUEUE_SIZE 20

// Struct for the game state
typedef struct Start {
    Car* vehicles[MAX_VEHICLES];
    int vehicleCount;
    TrafficLights* trafficLights[4];
    Challan challanList[100];
    int challanCount;
    VehicleQueue northQueue;
    VehicleQueue southQueue;
    VehicleQueue eastQueue;
    VehicleQueue westQueue;
    ToeTruck toeTruck;
    BankerState bankerState;
    sfSprite* background;
    sfTexture* bgTexture;
    sfFont* font;
    pthread_mutex_t vehicleMutex;
} Start;

Start game;
static int northCounter = 0, southCounter = 0, eastCounter = 0, westCounter=0;

void openUserPortal(sfRenderWindow* userPortalWindow);
void processPayment(const char* licensePlate, float amount);

int isSafeState(int process, int resource) {
    int work[MAX_RESOURCES];
    int finish[MAX_PROCESSES] = {0};
    
    memcpy(work, game.bankerState.available, sizeof(work));
    
    // Simulate allocation
    work[resource]--;
    game.bankerState.allocation[process][resource]++;
    game.bankerState.need[process][resource]--;
    
    while (1) {
        int found = 0;
        for (int i = 0; i < game.vehicleCount; i++) {
            if (!finish[i]) {
                int j;
                for (j = 0; j < MAX_RESOURCES; j++) {
                    if (game.bankerState.need[i][j] > work[j])
                        break;
                }
                if (j == MAX_RESOURCES) {
                    for (j = 0; j < MAX_RESOURCES; j++)
                        work[j] += game.bankerState.allocation[i][j];
                    finish[i] = 1;
                    found = 1;
                }
            }
        }
        if (!found) break;
    }
    
    // Undo the simulation
    work[resource]++;
    game.bankerState.allocation[process][resource]--;
    game.bankerState.need[process][resource]++;
    
    for (int i = 0; i < game.vehicleCount; i++) {
        if (!finish[i]) return 0;  // Unsafe state
    }
    return 1;  // Safe state
}

void updateBankerState() {
    initializeBankerState(game.bankerState);
    
    for (int i = 0; i < game.vehicleCount; i++) {
        Car* vehicle = game.vehicles[i];
        int direction = vehicle->currentDirection;
        
        game.bankerState.max[i][direction] = 1;
        game.bankerState.need[i][direction] = 1 - game.bankerState.allocation[i][direction];
    }
}

int requestResource(int process, int resource) {
    if (game.bankerState.need[process][resource] == 0) return 0;  // Resource not needed
    if (game.bankerState.available[resource] == 0) return 0;  // Resource not available
    
    if (isSafeState(process, resource)) {
        game.bankerState.available[resource]--;
        game.bankerState.allocation[process][resource]++;
        game.bankerState.need[process][resource]--;
        return 1;  // Resource granted
    }
    
    return 0;  // Resource not granted
}

// Function to calculate the fine amount
float calculateFineAmount(const char* vehicleType, float speed) {
    float fine = 0;
    if (strcmp(vehicleType, "Light Vehicle") == 0) {
        fine = 5000.0;
    } else if (strcmp(vehicleType, "Heavy Vehicle") == 0) {
        fine = 7000.0;
    }
    return fine;
}

// Function to calculate total amount with service charges
float calculateTotalAmount(float fineAmount) {
    return fineAmount + (fineAmount * 0.17);  // 17% service charges
}

// Function to generate the current date as a string
void getCurrentDate(char* buffer) {
    time_t now = time(NULL);
    struct tm* local = localtime(&now);
    strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", local);
}

// Function to calculate due date (3 days after issue date)
void calculateDueDate(char* issueDate, char* dueDate) {
    struct tm tm;
    //strptime(issueDate, "%Y-%m-%d %H:%M:%S", &tm);
    tm.tm_mday += 3;  // Add 3 days
    mktime(&tm);  // Normalize the date
    strftime(dueDate, 20, "%Y-%m-%d %H:%M:%S", &tm);
}

// Function to issue a challan
void issueChallan(const char* vehicleID, const char* vehicleType, float speed, bool* status) {
    if(vehicleID==NULL || vehicleType==NULL) return;
    if (strcmp(vehicleType, "Emergency") == 0) {
        printf("No challan for emergency vehicles.\n");
        return;
    }

    Challan challan;
    challan.challanID = game.challanCount + 1;
    strcpy(challan.vehicleID, vehicleID);
    strcpy(challan.vehicleType, vehicleType);

    // Calculate amounts
    float fineAmount = calculateFineAmount(vehicleType, speed);
    challan.fineAmount = fineAmount;
    challan.totalAmount = calculateTotalAmount(fineAmount);

    getCurrentDate(challan.issueDate);
    calculateDueDate(challan.issueDate, challan.dueDate);

    strcpy(challan.paymentStatus, "Unpaid");

    game.challanList[game.challanCount++] = challan;

    printf("Challan issued:\n");
    printf("Challan ID: %d\n", challan.challanID);
    printf("Vehicle ID: %s\n", challan.vehicleID);
    printf("Vehicle Type: %s\n", challan.vehicleType);
    printf("Fine Amount: %.2f\n", challan.fineAmount);
    printf("Total Amount: %.2f\n", challan.totalAmount);
    printf("Issue Date: %s\n", challan.issueDate);
    printf("Due Date: %s\n", challan.dueDate);
    printf("Payment Status: %s\n", challan.paymentStatus);
    printf("----------------------------------------------------\n");
    *status=true;
}

void checkforChallan(){
    for(int i=0; i<MAX_VEHICLES; i++){
        if(game.vehicles[i]==NULL) continue;{
            if(game.vehicles[i]->speed>game.vehicles[i]->maxSpeed){
                //issueChallan(game.vehicles[i]->licensePlate, game.vehicles[i]->vehicleType, game.vehicles[i]->speed);
            }
            else{
                continue;
            }
        }
    }
}

// Function to handle payment alert
void handlePaymentAlert(int challanID) {
    for (int i = 0; i < game.challanCount; i++) {
        if (game.challanList[i].challanID == challanID) {
            strcpy(game.challanList[i].paymentStatus, "Paid");

            // Notify the system
            printf("Payment received for Challan ID: %d\n", challanID);
            break;
        }
    }
}

// Checking if there are any emergency vehicles in the given direction
int isEmergencyVehicleInDirection(int direction) {
    for (int i = 0; i < MAX_VEHICLES; ++i) {
        if (game.vehicles[i] != NULL && (strcmp(game.vehicles[i]->vehicleType, "Emergency Vehicle") == 0) && game.vehicles[i]->currentDirection == direction) {
            return 1;  // Emergency vehicle detected
        }
    }
    return 0;
}

// Function to handle emergency vehicle priority
void checkEmergencyVehiclesAndAdjustLights() {
    for (int i = 0; i < 4; ++i) {
        if (isEmergencyVehicleInDirection(i)) {
            // Giving priority to this direction, keeping green light on for longer duration
            if(i==0){
                game.trafficLights[i]->currentState = 1; // green
                game.trafficLights[1]->currentState = 0; // red
            }
            else if(i==1){
                game.trafficLights[i]->currentState = 1;
                game.trafficLights[0]->currentState = 0;
            }
            else if(i==2){
                game.trafficLights[i]->currentState = 1;
                game.trafficLights[3]->currentState = 0;
            }
            else {
                game.trafficLights[i]->currentState = 1;
                game.trafficLights[2]->currentState = 0;
            }
        }
    }
}

// Traffic light management function
void manageTrafficLights() {
    int currentGreenIndex = 0;
    while (1) {
        sleep(10);  // 10 seconds per cycle
        for (int i = 0; i < 4; ++i) {
            // Reset all lights to red
            game.trafficLights[i]->currentState = 0;
            TrafficLights_updateTexture(game.trafficLights[i]);
        }

        if(currentGreenIndex==0){
            game.trafficLights[currentGreenIndex]->currentState = 1; // Green
            game.trafficLights[1]->currentState = 1; // Opposite direction of green is also green
            TrafficLights_updateTexture(game.trafficLights[currentGreenIndex]);
            TrafficLights_updateTexture(game.trafficLights[1]);
        }
        else if(currentGreenIndex==1){
            game.trafficLights[currentGreenIndex]->currentState = 1; // Green
            game.trafficLights[0]->currentState = 1; // Opposite direction of green is also green
            TrafficLights_updateTexture(game.trafficLights[currentGreenIndex]);
            TrafficLights_updateTexture(game.trafficLights[0]);
        }
        else if(currentGreenIndex==2){
            game.trafficLights[currentGreenIndex]->currentState = 1; // Green
            game.trafficLights[3]->currentState = 1; // Opposite direction of green is also green
            TrafficLights_updateTexture(game.trafficLights[currentGreenIndex]);
            TrafficLights_updateTexture(game.trafficLights[3]);
        }
        else {
            game.trafficLights[currentGreenIndex]->currentState = 1; // Green
            game.trafficLights[2]->currentState = 1; // Opposite direction of green is also green
            TrafficLights_updateTexture(game.trafficLights[currentGreenIndex]);
            TrafficLights_updateTexture(game.trafficLights[2]);
        }
        // Change Light (anticlockwise)
        currentGreenIndex = (currentGreenIndex + 1) % 4;

        checkEmergencyVehiclesAndAdjustLights();
        //sfSleep(sfSeconds(1));
    }
}


void spawnVehicle(float deltaTime) {
    srand(time(0));
    static float northTimer = 0, southTimer = 0, eastTimer = 0, westTimer = 0;
    static float emergencyNorthTimer = 0, emergencySouthTimer = 0, emergencyEastTimer = 0, emergencyWestTimer = 0;
    static float heavyVehicleTimer = 0;

    static float gameTime = 0;
    gameTime += deltaTime;
    VehicleQueue* targetQueue = NULL;

    bool isPeakHour = (gameTime >= 7 * 3600 && gameTime <= 9 * 3600); // Peak hour: 7 AM to 9 AM

    northTimer += deltaTime;
    emergencyNorthTimer += deltaTime;

    if (northTimer >= 1.0f && northCounter<10) {
        int randomType = (emergencyNorthTimer >= 15.0f && (rand() % 100) < 20) ? 2 : 0; // 20% chance for emergency vehicle
        Positions spawnPos = initialPositions(3);
        Car* newVehicle;

        if (randomType == 0) {
            newVehicle = malloc(sizeof(LightVehicle));
            if (newVehicle) {
                LightVehicle_init((LightVehicle*)newVehicle, spawnPos.direction);  // Initialize LightVehicle
            }
        } else if (randomType == 1) {
            newVehicle = malloc(sizeof(HeavyVehicle));
            if (newVehicle) {
                HeavyVehicle_init((HeavyVehicle*)newVehicle, spawnPos.direction);  // Initialize HeavyVehicle
            }
        } else {
            newVehicle = malloc(sizeof(EmergencyVehicle));
            if (newVehicle) {
                EmergencyVehicle_init((EmergencyVehicle*)newVehicle, spawnPos.direction);  // Initialize EmergencyVehicle
            }
        }
        if (!newVehicle) {
            fprintf(stderr, "Failed to allocate memory for car\n");
            return;
        }
        targetQueue = &game.northQueue;

        if (newVehicle) {
            sfSprite_setPosition(newVehicle->sprite, (sfVector2f){spawnPos.x, spawnPos.y});
            newVehicle->moveDirection = (sfVector2f){0, 1};  // Moving South
            newVehicle->speed = rand() % 30 + 30;

            pthread_mutex_lock(&game.vehicleMutex);
            if (game.vehicleCount < MAX_VEHICLES) {
                game.vehicles[game.vehicleCount++] = newVehicle;
                northCounter++;
            } else {
                sfSprite_destroy(newVehicle->sprite);
                free(newVehicle);
                printf("Vehicle list is full. Unable to add new vehicle.\n");
            }
            // Check if the vehicle has an active challan
            if (newVehicle->challanStatus) {
                printf("Vehicle with active challan not allowed to enter. Waiting for payment.\n");
                sfSprite_destroy(newVehicle->sprite);
                free(newVehicle);
            } else if (strcmp(newVehicle->vehicleType, "Emergency Vehicle") == 0) {
                enqueueEmergency(targetQueue, newVehicle);
            } else {
                enqueue(targetQueue, newVehicle);
            }
            pthread_mutex_unlock(&game.vehicleMutex);
        }

        northTimer -= 1.0f;
        emergencyNorthTimer = (emergencyNorthTimer >= 15.0f) ? 0 : emergencyNorthTimer;
    }


    // South spawning logic (1 regular vehicle every 2 seconds, 5% chance of emergency vehicle)
    southTimer += deltaTime;
    emergencySouthTimer += deltaTime;

    if (southTimer >= 2.0f && southCounter < 10) {
        int randomType = (rand() % 100) < 5 ? 2 : 0; // 5% chance for emergency vehicle
        Positions spawnPos = initialPositions(2);
        Car* newVehicle;

        if (randomType == 0) {
            newVehicle = malloc(sizeof(LightVehicle));
            if (newVehicle) {
                LightVehicle_init((LightVehicle*)newVehicle, spawnPos.direction);  // Initialize LightVehicle
            }
        } else if (randomType == 1) {
            newVehicle = malloc(sizeof(HeavyVehicle));
            if (newVehicle) {
                HeavyVehicle_init((HeavyVehicle*)newVehicle, spawnPos.direction);  // Initialize HeavyVehicle
            }
        } else {
            newVehicle = malloc(sizeof(EmergencyVehicle));
            if (newVehicle) {
                EmergencyVehicle_init((EmergencyVehicle*)newVehicle, spawnPos.direction);  // Initialize EmergencyVehicle
            }
        }
        if (!newVehicle) {
            fprintf(stderr, "Failed to allocate memory for car\n");
            return;
        }
        targetQueue = &game.southQueue;

        if (newVehicle) {
            sfSprite_setPosition(newVehicle->sprite, (sfVector2f){spawnPos.x, spawnPos.y});
            newVehicle->moveDirection = (sfVector2f){0, -1};  // Moving North
            newVehicle->speed = rand() % 30 + 30;

            pthread_mutex_lock(&game.vehicleMutex);
            if (game.vehicleCount < MAX_VEHICLES) {
                game.vehicles[game.vehicleCount++] = newVehicle;
                southCounter++;
            } else {
                sfSprite_destroy(newVehicle->sprite);
                free(newVehicle);
                printf("Vehicle list is full. Unable to add new vehicle.\n");
            }
            // Check if the vehicle has an active challan
            if (newVehicle->challanStatus) {
                printf("Vehicle with active challan not allowed to enter. Waiting for payment.\n");
                sfSprite_destroy(newVehicle->sprite);
                free(newVehicle);
            } else if (strcmp(newVehicle->vehicleType, "Emergency Vehicle") == 0) {
                enqueueEmergency(targetQueue, newVehicle);
            } else {
                enqueue(targetQueue, newVehicle);
            }
            pthread_mutex_unlock(&game.vehicleMutex);
        }

        southTimer -= 2.0f;
    }

    eastTimer += deltaTime;
    emergencyEastTimer += deltaTime;

    if (eastTimer >= 1.5f && eastCounter < 10) {
        int randomType = (emergencyEastTimer >= 20.0f && (rand() % 100) < 10) ? 2 : 0; // 10% chance for emergency vehicle
        Positions spawnPos = initialPositions(0);
        Car* newVehicle;

        if (randomType == 0) {
            newVehicle = malloc(sizeof(LightVehicle));
            if (newVehicle) {
                LightVehicle_init((LightVehicle*)newVehicle, spawnPos.direction);  // Initialize LightVehicle
            }
        } else if (randomType == 1) {
            newVehicle = malloc(sizeof(HeavyVehicle));
            if (newVehicle) {
                HeavyVehicle_init((HeavyVehicle*)newVehicle, spawnPos.direction);  // Initialize HeavyVehicle
            }
        } else {
            newVehicle = malloc(sizeof(EmergencyVehicle));
            if (newVehicle) {
                EmergencyVehicle_init((EmergencyVehicle*)newVehicle, spawnPos.direction);  // Initialize EmergencyVehicle
            }
        }
        if (!newVehicle) {
            fprintf(stderr, "Failed to allocate memory for car\n");
            return;
        }
        targetQueue = &game.eastQueue;

        if (newVehicle) {
            sfSprite_setPosition(newVehicle->sprite, (sfVector2f){spawnPos.x, spawnPos.y});
            newVehicle->moveDirection = (sfVector2f){-1, 0};  // Moving West
            newVehicle->speed = rand() % 30 + 30;

            pthread_mutex_lock(&game.vehicleMutex);
            if (game.vehicleCount < MAX_VEHICLES) {
                game.vehicles[game.vehicleCount++] = newVehicle;
                eastCounter++;
            } else {
                sfSprite_destroy(newVehicle->sprite);
                free(newVehicle);
                printf("Vehicle list is full. Unable to add new vehicle.\n");
            }
            // Check if the vehicle has an active challan
            if (newVehicle->challanStatus) {
                printf("Vehicle with active challan not allowed to enter. Waiting for payment.\n");
                sfSprite_destroy(newVehicle->sprite);
                free(newVehicle);
            } else if (strcmp(newVehicle->vehicleType, "Emergency Vehicle") == 0) {
                enqueueEmergency(targetQueue, newVehicle);
            } else {
                enqueue(targetQueue, newVehicle);
            }
            pthread_mutex_unlock(&game.vehicleMutex);
        }

        eastTimer -= 1.5f;
        emergencyEastTimer = (emergencyEastTimer >= 20.0f) ? 0 : emergencyEastTimer;
    }


    westTimer += deltaTime;
    emergencyWestTimer += deltaTime;

    if (westTimer >= 2.0f && westCounter < 10) {
        int randomType = (rand() % 100) < 30 ? 2 : 0; // 30% chance for emergency vehicle
        Positions spawnPos = initialPositions(1);
        Car* newVehicle;

        if (randomType == 0) {
            newVehicle = malloc(sizeof(LightVehicle));
            if (newVehicle) {
                LightVehicle_init((LightVehicle*)newVehicle, spawnPos.direction);  // Initialize LightVehicle
            }
        } else if (randomType == 1) {
            newVehicle = malloc(sizeof(HeavyVehicle));
            if (newVehicle) {
                HeavyVehicle_init((HeavyVehicle*)newVehicle, spawnPos.direction);  // Initialize HeavyVehicle
            }
        } else {
            newVehicle = malloc(sizeof(EmergencyVehicle));
            if (newVehicle) {
                EmergencyVehicle_init((EmergencyVehicle*)newVehicle, spawnPos.direction);  // Initialize EmergencyVehicle
            }
        }
        if (!newVehicle) {
            fprintf(stderr, "Failed to allocate memory for car\n");
            return;
        }
        targetQueue = &game.westQueue;

        if (newVehicle) {
            sfSprite_setPosition(newVehicle->sprite, (sfVector2f){spawnPos.x, spawnPos.y});
            newVehicle->moveDirection = (sfVector2f){1, 0};  // Moving East
            newVehicle->speed = rand() % 30 + 30;

            pthread_mutex_lock(&game.vehicleMutex);
            if (game.vehicleCount < MAX_VEHICLES) {
                game.vehicles[game.vehicleCount++] = newVehicle;
                westCounter++;
            } else {
                sfSprite_destroy(newVehicle->sprite);
                free(newVehicle);
                printf("Vehicle list is full. Unable to add new vehicle.\n");
            }
            
            if (newVehicle->challanStatus) {
                printf("Vehicle with active challan not allowed to enter. Waiting for payment.\n");
                sfSprite_destroy(newVehicle->sprite);
                free(newVehicle);
            } else if (strcmp(newVehicle->vehicleType, "Emergency Vehicle") == 0) {
                enqueueEmergency(targetQueue, newVehicle);
            } else {
                enqueue(targetQueue, newVehicle);
            }
            pthread_mutex_unlock(&game.vehicleMutex);
        }

        westTimer -= 2.0f;
    }

    // Heavy vehicle spawning (if not peak hour)
    heavyVehicleTimer += deltaTime;
    if (heavyVehicleTimer >= 15.0f) {
        if (!isPeakHour) {
            Positions spawnPos = initialPositionsForHeavyVehicles();
            Car* heavyVehicle = malloc(sizeof(HeavyVehicle));
            if (heavyVehicle) {
                HeavyVehicle_init((HeavyVehicle*)heavyVehicle, spawnPos.direction);  // Initialize HeavyVehicle
            }

            sfSprite_setPosition(heavyVehicle->sprite, (sfVector2f){spawnPos.x, spawnPos.y});
            heavyVehicle->moveDirection = (sfVector2f){0, 1};  // Moving in a specific direction depending on side
            heavyVehicle->speed = rand() % 30 + 30;

            //pthread_mutex_lock(&game.vehicleMutex);
            game.vehicles[game.vehicleCount++] = heavyVehicle;
            //pthread_mutex_unlock(&game.vehicleMutex);
        }

        heavyVehicleTimer = 0;
    }
}

void checkAndMoveQueue(VehicleQueue* queue, int direction) {
    if (isQueueEmpty(queue)) return;

    // Check if lanes are available
    int availableLanes = 2;
    for (int i = 0; i < game.vehicleCount; i++) {
        if(!game.vehicles[i]) continue;
        if (game.vehicles[i]->currentDirection == direction) {
            availableLanes--;
        }
    }

    while (availableLanes > 0 && !isQueueEmpty(queue)) {
        Car* vehicle = dequeue(queue);
        if (vehicle) {
            pthread_mutex_lock(&game.vehicleMutex);
            if (game.vehicleCount < MAX_VEHICLES) {
                game.vehicles[game.vehicleCount++] = vehicle;
                availableLanes--;
            } else {
                enqueue(queue, vehicle); // Put it back in the queue if game is full
                break;
            }
            pthread_mutex_unlock(&game.vehicleMutex);
        }
    }
}

float calculateDistance(sfVector2f pos1, sfVector2f pos2) {
    float dx = pos1.x - pos2.x;
    float dy = pos1.y - pos2.y;
    return sqrt(dx*dx + dy*dy);
}

// Function for calling a toe truck
void callToeTruck(Car* brokenVehicle) {
    if(brokenVehicle == NULL || brokenVehicle->sprite == NULL) return;

    printf("Toe truck called for vehicle at position (%.2f, %.2f)\n",
           sfSprite_getPosition(brokenVehicle->sprite).x,
           sfSprite_getPosition(brokenVehicle->sprite).y);

    //pthread_mutex_lock(&game.vehicleMutex);

    // Create a new toe truck
    Car* toeTruck = malloc(sizeof("Toe Truck"));
    if (!toeTruck) {
        printf("Failed to allocate memory for toe truck\n");
        pthread_mutex_unlock(&game.vehicleMutex);
        return;
    }

    // Initialize the toe truck
    initToeTruck((ToeTruck*)toeTruck, brokenVehicle->pos);

    // Get broken vehicle's position
    sfVector2f brokenVehiclePos = sfSprite_getPosition(brokenVehicle->sprite);
    
    // Set toe truck position
    toeTruck->x = brokenVehiclePos.x;
    toeTruck->y = brokenVehiclePos.y;
    sfSprite_setPosition(toeTruck->sprite, brokenVehiclePos);

    // Remove the broken vehicle from the game
    for (int i = 0; i < game.vehicleCount; i++) {
        if(!game.vehicles[i]) continue;
        if (game.vehicles[i] == brokenVehicle) {
            //Car_destroy(brokenVehicle);
            //sfSprite_destroy(brokenVehicle->sprite);
            //free(brokenVehicle);
            // Shift vehicles
            // for (int j = i; j < game.vehicleCount - 1; j++) {
            //     game.vehicles[j] = game.vehicles[j + 1];
            // }
            // game.vehicleCount--;
            break;
        }
    }

    //Add the toe truck to the game's vehicle list CAREFULLY
    // if (game.vehicleCount < MAX_VEHICLES) {
    //     // Create a special wrapper Car for the toe truck
    //     Car* toeTruckAsCar = malloc(sizeof(ToeTruck));
    //     if (toeTruckAsCar && toeTruckAsCar->sprite) {
    //         // Copy essential toe truck properties to the Car structure
    //         toeTruckAsCar->sprite = toeTruck->sprite;
    //         toeTruckAsCar->texture = toeTruck->texture;
    //         strcpy(toeTruckAsCar->vehicleType, "Toe Truck");
    //         toeTruckAsCar->isMoving = true;
            
    //         game.vehicles[game.vehicleCount++] = toeTruckAsCar;
    //     } else {
    //         printf("Failed to create toe truck car wrapper\n");
    //         sfSprite_destroy(toeTruck->sprite);
    //         free(toeTruck);
    //     }
    // } else {
    //     printf("Vehicle list is full. Unable to add toe truck.\n");
    //     sfSprite_destroy(toeTruck->sprite);
    //     free(toeTruck);
    // }

    //pthread_mutex_unlock(&game.vehicleMutex);

    printf("Toe truck has arrived and removed the broken vehicle\n");
    Car_destroy(toeTruck);
}

void moveVehicles(float deltaTime) {
    checkAndMoveQueue(&game.northQueue, 3);
    checkAndMoveQueue(&game.southQueue, 2);
    checkAndMoveQueue(&game.eastQueue, 0);
    checkAndMoveQueue(&game.westQueue, 1);
    
    pthread_mutex_lock(&game.vehicleMutex);  // Lock the vehicle list while moving vehicles


    for (int i = 0; i < game.vehicleCount; i++) {
        if(game.vehicles[i]==NULL || game.vehicles[i]->sprite == NULL) continue;

        int overspeeding=rand()%11;  // for overspeeding
        Car* vehicle = game.vehicles[i];
        sfVector2f currentPosition = sfSprite_getPosition(vehicle->sprite);
        bool slowDown = false;
        vehicle->isMoving=true;

        if(vehicle->sprite==NULL) continue;

        // Simulate random breakdowns for light vehicles
        if (strcmp(vehicle->vehicleType, "Light Vehicle") == 0 && (rand() % 1000 < 5)) { // 0.5% chance of breakdown
            // vehicle->isBrokenDown = true;
            // callToeTruck(vehicle);
            // currentPosition.x=-30;
            // currentPosition.y=-30;
            // sfSprite_setPosition(vehicle->sprite, currentPosition);
            continue;

            //updateToeTruck(game.toeTruck, deltaTime);
        }

        if(vehicle->speed>vehicle->maxSpeed && !vehicle->challanStatus){
            issueChallan(vehicle->licensePlate, vehicle->vehicleType, vehicle->speed, &vehicle->challanStatus);
        }

        if (strcmp(vehicle->vehicleType, "Emergency Vehicle") == 0) {
            vehicle->isPriority = true;
        }

        // Don't move broken-down vehicles
        if (vehicle->isBrokenDown) {
            vehicle->isMoving = false;
            continue;
        }

        

        // Tolerance value for comparison
        const float tolerance = 0.5f;

        // Checking if there are cars ahead in the same lane
        for (int j = 0; j < game.vehicleCount; j++) {
            if (i == j) continue;

            Car* otherVehicle = game.vehicles[j];
            if(!vehicle->sprite || !otherVehicle->sprite) continue;
            if((vehicle==NULL) || (otherVehicle==NULL)) continue;
            sfVector2f otherPosition = sfSprite_getPosition(otherVehicle->sprite);
            //if(otherVehicle->isBrokenDown) continue;
            // Handle broken-down vehicles
                if (otherVehicle->isBrokenDown && fabsf(currentPosition.y - otherPosition.y) < 100) {
                    slowDown = true;
                    continue;
                    // Implement lane change logic here if possible
                    /*if(vehicle->currentDirection==2){
                        sfVector2f movement = {50, 0};
                        sfSprite_move(vehicle->sprite, movement);
                        vehicle->hasChangedLane = true;
                    }
                    else if(vehicle->currentDirection==3){
                        sfVector2f movement = {-50, 0};
                        sfSprite_move(vehicle->sprite, movement);
                        vehicle->hasChangedLane = true;
                    }
                    else if(vehicle->currentDirection==1){
                        sfVector2f movement = {0, 50};
                        sfSprite_move(vehicle->sprite, movement);
                        vehicle->hasChangedLane = true;
                    }
                    else if(vehicle->currentDirection==0){
                        sfVector2f movement = {0, -50};
                        sfSprite_move(vehicle->sprite, movement);
                        vehicle->hasChangedLane = true;
                    }*/
                }

            // Moving East
            if (vehicle->currentDirection == 0 && 
                fabsf(currentPosition.y - otherPosition.y) <= tolerance && 
                currentPosition.x < otherPosition.x) {
                // Checking distance between the two vehicles
                if (otherPosition.x - currentPosition.x < 50) { // Threshold distance
                    slowDown = true;
                }
            }
            // Moving West
            else if (vehicle->currentDirection == 1 && 
                     fabsf(currentPosition.y - otherPosition.y) <= tolerance && 
                     currentPosition.x > otherPosition.x) {
                if (currentPosition.x - otherPosition.x < 50) {
                    slowDown = true;
                }
            }
            // Moving South
            else if (vehicle->currentDirection == 2 && 
                     fabsf(currentPosition.x - otherPosition.x) <= tolerance && 
                     currentPosition.y < otherPosition.y) {
                if (otherPosition.y - currentPosition.y < 50) {
                    slowDown = true;
                }
            }
            // Moving North
            else if (vehicle->currentDirection == 3 && 
                     fabsf(currentPosition.x - otherPosition.x) <= tolerance && 
                     currentPosition.y > otherPosition.y) {
                if (currentPosition.y - otherPosition.y < 50) {
                    slowDown = true;
                }
            }
        }

        // Speed adjustment
        if (slowDown) {
            vehicle->speed = fmaxf(vehicle->speed - 20 * deltaTime, 5.0f);
        } else if (vehicle->challanStatus) {
            vehicle->speed = fmaxf(vehicle->speed - 10 * deltaTime, vehicle->maxSpeed);
        } else if ((vehicle->speed < vehicle->maxSpeed) && (overspeeding<=5)) {
            vehicle->speed += 20 * deltaTime;
        } else if(overspeeding>5){
            vehicle->speed += 20 * deltaTime;
        }

        // Setting initial direction if not already set
        if (!vehicle->hasChangedDirection) {
            if (currentPosition.x < 100) {
                vehicle->currentDirection = 0;  // Moving East
            }
            else if (currentPosition.x > 600) {
                vehicle->currentDirection = 1;  // Moving West
            }
            else if (currentPosition.y < 100) {
                vehicle->currentDirection = 2;  // Moving South
            }
            else if (currentPosition.y > 600) {
                vehicle->currentDirection = 3;  // Moving North
            }
        }

        // Turning point checks
        if (fabsf(currentPosition.x - 470) < 10 && fabsf(currentPosition.y - 460) < 10 && !vehicle->hasChangedDirection) {
            switch(vehicle->currentDirection) {
                case 0:
                    if(game.trafficLights[1]->currentState==0){
                        vehicle->isMoving=true;
                        vehicle->currentDirection = 2;
                        sfSprite_setRotation(vehicle->sprite, 90);
                    }
                    else{
                        vehicle->isMoving=false;
                    }
                    break;
                case 1:
                    if(game.trafficLights[0]->currentState==0){
                        vehicle->isMoving=true;
                        vehicle->currentDirection = 3;
                        sfSprite_setRotation(vehicle->sprite, 90);
                    }
                    else{
                        vehicle->isMoving=false;}
                    break;
                case 2:
                    if(game.trafficLights[2]->currentState==0){
                        vehicle->isMoving=true;
                        vehicle->isMoving=false;
                        vehicle->currentDirection = 1;
                        sfSprite_setRotation(vehicle->sprite, 90);
                    }
                    break;
                case 3:
                    if(game.trafficLights[3]->currentState==0){
                        vehicle->isMoving=true;
                        vehicle->currentDirection = 0;
                        sfSprite_setRotation(vehicle->sprite, 90);
                    }
                    else{
                        vehicle->isMoving=false;}
                    break;
            }
            vehicle->hasChangedDirection = true;
        }
        else if (fabsf(currentPosition.x - 330) < 10 && fabsf(currentPosition.y - 340) < 10 && !vehicle->hasChangedDirection) {
            switch(vehicle->currentDirection) {
                case 0:
                    if(game.trafficLights[1]->currentState==0){
                        vehicle->isMoving=true;
                        vehicle->currentDirection = 2;
                        sfSprite_setRotation(vehicle->sprite, 90);
                    }
                    else{
                        vehicle->isMoving=false;
                    }
                    break;
                case 1:
                    if(game.trafficLights[0]->currentState==0){
                        vehicle->isMoving=true;
                        vehicle->currentDirection = 3;
                        sfSprite_setRotation(vehicle->sprite, 90);
                    }
                    else{
                        vehicle->isMoving=false;
                    }
                    break;
                case 2:
                    if(game.trafficLights[2]->currentState==0){
                        vehicle->currentDirection = 1;
                        sfSprite_setRotation(vehicle->sprite, 90);
                    }
                    else{
                        vehicle->isMoving=false;
                    }
                    break;
                case 3:
                    if(game.trafficLights[3]->currentState==0){
                        vehicle->isMoving=true;
                        vehicle->currentDirection = 0;
                        sfSprite_setRotation(vehicle->sprite, 90);
                    }
                    else{
                        vehicle->isMoving=false;
                    }
                    break;
            }
            vehicle->hasChangedDirection = true;
        }
        else if (fabsf(currentPosition.x - 460) < 10 && fabsf(currentPosition.y - 330) < 10 && !vehicle->hasChangedDirection) {
            if(game.trafficLights[0]->currentState==0){
                vehicle->isMoving=true;
                vehicle->currentDirection = 3;
                sfSprite_setRotation(vehicle->sprite, 90);
            }
            else{
                vehicle->isMoving=false;
            }
            vehicle->hasChangedDirection = true;
        }
        else if (fabsf(currentPosition.x - 340) < 10 && fabsf(currentPosition.y - 475) < 10 && !vehicle->hasChangedDirection) {
            if(game.trafficLights[1]->currentState==0){
                vehicle->isMoving=true;
                vehicle->currentDirection = 2;
                sfSprite_setRotation(vehicle->sprite, 90);
            }
            else{
                vehicle->isMoving=false;
            }
            vehicle->hasChangedDirection = true;
        }

        // Lane switching points
        if (fabsf(currentPosition.x - 425) < 10 && fabsf(currentPosition.y - 510) < 10 && !vehicle->hasChangedLane) {
            sfVector2f movement = {50, 0};
            sfSprite_move(vehicle->sprite, movement);
            vehicle->hasChangedLane = true;
        }
        else if (fabsf(currentPosition.x - 375) < 10 && fabsf(currentPosition.y - 300) < 10 && !vehicle->hasChangedLane) {
            sfVector2f movement = {-50, 0};
            sfSprite_move(vehicle->sprite, movement);
            vehicle->hasChangedLane = true;
        }
        else if (fabsf(currentPosition.x - 510) < 10 && fabsf(currentPosition.y - 375) < 10 && !vehicle->hasChangedLane) {
            sfVector2f movement = {0, -50};
            sfSprite_move(vehicle->sprite, movement);
            vehicle->hasChangedLane = true;
        }
        else if (fabsf(currentPosition.x - 300) < 10 && fabsf(currentPosition.y - 425) < 10 && !vehicle->hasChangedLane) {
            sfVector2f movement = {0, 50};
            sfSprite_move(vehicle->sprite, movement);
            vehicle->hasChangedLane = true;
        }

        // Move based on current direction
        sfVector2f movement = {0, 0};
        switch(vehicle->currentDirection) {
            case 0: // Moving East
                if(vehicle->isMoving){
                    movement.x = vehicle->speed * deltaTime;
                }
                else{
                    //movement.x = 0;
                    movement.x = vehicle->speed * deltaTime;
                }
                break;
            case 1: // Moving West
                if(vehicle->isMoving){
                    movement.x = -vehicle->speed * deltaTime;
                }
                else{
                    //movement.x = 0;
                    movement.x = -vehicle->speed * deltaTime;
                }
                break;
            case 2: // Moving South
                if(vehicle->isMoving){
                    movement.y = vehicle->speed * deltaTime;
                }
                else{
                    //movement.y = 0;
                    movement.y = vehicle->speed * deltaTime;
                }
                break;
            case 3: // Moving North
                if(vehicle->isMoving){
                    movement.y = -vehicle->speed * deltaTime;
                }
                else{
                    //movement.y = 0;
                    movement.y = -vehicle->speed * deltaTime;
                }
                break;
        }
        if(!vehicle->sprite) continue;
        sfSprite_move(vehicle->sprite, movement);

        // Updating current position
        currentPosition = sfSprite_getPosition(vehicle->sprite);

        // Removing vehicles which are out of bounds
        if (currentPosition.x < -50 || currentPosition.x > 850 ||
            currentPosition.y < -50 || currentPosition.y > 850) {
            Car_destroy(vehicle);
            //sfSprite_destroy(vehicle->sprite);
            free(vehicle);

            for (int j = i; j < game.vehicleCount - 1; j++) {
                game.vehicles[j] = game.vehicles[j + 1];
            }
            game.vehicles[i] = game.vehicles[game.vehicleCount - 1];
            game.vehicles[game.vehicleCount - 1] = NULL;
            game.vehicleCount--;
            i--;
            if(currentPosition.x < -50){
                eastCounter--;
            }
            else if(currentPosition.x > 850){
                westCounter--;
            }
            else if(currentPosition.y < -50){
                southCounter--;
            }
            else{
                northCounter--;
            }
        }
        if(overspeeding) {
            //issueChallan(vehicle->licensePlate, vehicle->vehicleType, vehicle->speed);
        }
    }
    
    //checkforChallan();
    pthread_mutex_unlock(&game.vehicleMutex);  // Unlock the vehicle list
}

// Main loop function
void startTraffic() {
    game.vehicleCount = 0;
    game.challanCount = 0;
    srand(time(NULL));
    sfRenderWindow* window = sfRenderWindow_create((sfVideoMode){800, 800, 32}, TITLE, sfResize | sfClose, NULL);
    sfClock* clock = sfClock_create();

    // Load background and font
    game.bgTexture = sfTexture_createFromFile("images/bg.jpg", NULL);
    game.background = sfSprite_create();
    sfSprite_setTexture(game.background, game.bgTexture, sfTrue);

    game.font = sfFont_createFromFile("fonts/arialceb.ttf");

    // Initialize traffic lights
    for (int i = 0; i < 4; ++i) {
        game.trafficLights[i] = malloc(sizeof(TrafficLights));
        if (game.trafficLights[i]) {
            int initialState = 0;
            if (i == 0) {
                game.trafficLights[i]->x = 290;
                game.trafficLights[i]->y = 260;
            } else if (i == 1) {
                game.trafficLights[i]->x = 510;
                game.trafficLights[i]->y = 500;
            } else if (i == 2) {
                game.trafficLights[i]->x = 290;
                game.trafficLights[i]->y = 500;
            } else {
                game.trafficLights[i]->x = 510;
                game.trafficLights[i]->y = 260;
            }
            TrafficLights_init(game.trafficLights[i], initialState);
        }
    }
    sfRenderWindow* userPortalWindow = NULL;

    // Create threads
    pthread_t spawnThread, moveThread, trafficLightThread;
    pthread_create(&spawnThread, NULL, (void*)spawnVehicle, NULL);
    pthread_create(&moveThread, NULL, (void*)moveVehicles, NULL);
    pthread_create(&trafficLightThread, NULL, (void*)manageTrafficLights, NULL); // Traffic light control

    // displaying vehicle details
    sfRectangleShape* infoBox = sfRectangleShape_create();
    sfRectangleShape_setSize(infoBox, (sfVector2f){200, 80});
    sfRectangleShape_setFillColor(infoBox, sfColor_fromRGBA(50, 50, 50, 200));
    sfRectangleShape_setOutlineColor(infoBox, sfWhite);
    sfRectangleShape_setOutlineThickness(infoBox, 2);

    sfText* infoText = sfText_create();
    sfText_setFont(infoText, game.font);
    sfText_setCharacterSize(infoText, 14);
    sfText_setFillColor(infoText, sfWhite);

    while (sfRenderWindow_isOpen(window)) {
        float deltaTime = sfClock_getElapsedTime(clock).microseconds / 1000000.0f;
        sfClock_restart(clock);

        sfEvent event;
        while (sfRenderWindow_pollEvent(window, &event)) {
            if (event.type == sfEvtClosed) {
                sfRenderWindow_close(window);
            }
        }

        // Get mouse position
        sfVector2i mousePos = sfMouse_getPositionRenderWindow(window);
        sfVector2f mouseWorldPos = (sfVector2f){mousePos.x, mousePos.y};

        spawnVehicle(deltaTime);
        moveVehicles(deltaTime);

        // Clear the screen for drawing
        sfRenderWindow_clear(window, sfBlack);
        sfRenderWindow_drawSprite(window, game.background, NULL);

        //sfEvent eventUserPortal;
        if (sfKeyboard_isKeyPressed(sfKeyU)) {
            if (userPortalWindow == NULL || !sfRenderWindow_isOpen(userPortalWindow)) {
                userPortalWindow = sfRenderWindow_create((sfVideoMode){400, 300, 32}, "User Portal", sfClose, NULL);
                openUserPortal(userPortalWindow);
            }
        }


        // Draw vehicles
        Car* hoveredVehicle = NULL;
        pthread_mutex_lock(&game.vehicleMutex);
        for (int i = 0; i < MAX_VEHICLES; ++i) {
            if (game.vehicles[i] != NULL) {
                sfFloatRect bounds = sfSprite_getGlobalBounds(game.vehicles[i]->sprite);
                sfRenderWindow_drawSprite(window, game.vehicles[i]->sprite, NULL);

                // Check if mouse hovers over this vehicle
                if (sfFloatRect_contains(&bounds, mouseWorldPos.x, mouseWorldPos.y)) {
                    hoveredVehicle = game.vehicles[i];
                }
            }
        }
        pthread_mutex_unlock(&game.vehicleMutex);

        // Draw traffic lights
        for (int i = 0; i < 4; ++i) {
            sfRenderWindow_drawSprite(window, game.trafficLights[i]->sprite, NULL);
        }

        if (game.toeTruck.isActive) {
            sfRenderWindow_drawSprite(window, game.toeTruck.base.sprite, NULL);
        }

        // Display vehicle details if a vehicle is hovered
        if (hoveredVehicle) {
            sfRectangleShape_setPosition(infoBox, (sfVector2f){mouseWorldPos.x + 10, mouseWorldPos.y + 10});
            sfText_setPosition(infoText, (sfVector2f){mouseWorldPos.x + 20, mouseWorldPos.y + 20});
            char details[256];
            snprintf(details, sizeof(details), "Vehicle ID: %s\nType: %s\nSpeed: %.2f km/h", hoveredVehicle->licensePlate, hoveredVehicle->vehicleType, hoveredVehicle->speed);
            sfText_setString(infoText, details);

            sfRenderWindow_drawRectangleShape(window, infoBox, NULL);
            sfRenderWindow_drawText(window, infoText, NULL);
        }

        if (userPortalWindow != NULL && sfRenderWindow_isOpen(userPortalWindow)) {
            sfEvent userPortalEvent;
            while (sfRenderWindow_pollEvent(userPortalWindow, &userPortalEvent)) {
                if (userPortalEvent.type == sfEvtClosed) {
                    sfRenderWindow_close(userPortalWindow);
                    sfRenderWindow_destroy(userPortalWindow);
                    userPortalWindow = NULL;
                }
            }
        }

        sfRenderWindow_display(window);
    }

    // Clean up threads and resources
    pthread_join(spawnThread, NULL);
    pthread_join(moveThread, NULL);
    pthread_join(trafficLightThread, NULL); // Join traffic light control thread
    sfRectangleShape_destroy(infoBox);
    sfText_destroy(infoText);
    sfRenderWindow_destroy(window);
    sfClock_destroy(clock);
}

void openUserPortal(sfRenderWindow* userPortalWindow) {
    sfFont* font = sfFont_createFromFile("fonts/arialceb.ttf");
    if (!font) {
        printf("Failed to load font\n");
        return;
    }

    // title and instruction texts
    sfText* titleText = sfText_create();
    sfText_setString(titleText, "User Portal");
    sfText_setFont(titleText, font);
    sfText_setCharacterSize(titleText, 24);
    sfText_setPosition(titleText, (sfVector2f){10, 10});

    sfText* instructionText = sfText_create();
    sfText_setFillColor(instructionText, sfBlack);
    sfText_setString(instructionText, "Press 'Enter' to process payment. Use 'Tab' to switch fields.");
    sfText_setFont(instructionText, font);
    sfText_setCharacterSize(instructionText, 18);
    sfText_setPosition(instructionText, (sfVector2f){10, 50});

    // labels for input fields
    sfText* licensePlateLabel = sfText_create();
    sfText_setFillColor(licensePlateLabel, sfBlack);
    sfText_setString(licensePlateLabel, "Enter License Plate:");
    sfText_setFont(licensePlateLabel, font);
    sfText_setCharacterSize(licensePlateLabel, 18);
    sfText_setPosition(licensePlateLabel, (sfVector2f){10, 100});

    sfText* amountLabel = sfText_create();
    sfText_setFillColor(amountLabel, sfBlack);
    sfText_setString(amountLabel, "Enter Amount:");
    sfText_setFont(amountLabel, font);
    sfText_setCharacterSize(amountLabel, 18);
    sfText_setPosition(amountLabel, (sfVector2f){10, 180});

    // Input fields
    char licensePlate[20] = "";
    char amountStr[20] = "";
    bool inputLicensePlate = true; // Flag to track input focus

    sfText* licensePlateText = sfText_create();
    sfText_setFillColor(licensePlateText, sfBlack);
    sfText_setFont(licensePlateText, font);
    sfText_setCharacterSize(licensePlateText, 18);
    sfText_setPosition(licensePlateText, (sfVector2f){10, 130}); // Below "Enter License Plate:"

    sfText* amountText = sfText_create();
    sfText_setFillColor(amountText, sfBlack);
    sfText_setFont(amountText, font);
    sfText_setCharacterSize(amountText, 18);
    sfText_setPosition(amountText, (sfVector2f){10, 210}); // Below "Enter Amount:"

    while (sfRenderWindow_isOpen(userPortalWindow)) {
        sfEvent event;
        while (sfRenderWindow_pollEvent(userPortalWindow, &event)) {
            if (event.type == sfEvtClosed) {
                sfRenderWindow_close(userPortalWindow);
            } else if (event.type == sfEvtTextEntered) {
                if (inputLicensePlate) {
                    if (event.text.unicode < 128 && event.text.unicode != 8) { // Add characters
                        strncat(licensePlate, (char*)&event.text.unicode, 1);
                    } else if (event.text.unicode == 8 && strlen(licensePlate) > 0) { // Handle backspace
                        licensePlate[strlen(licensePlate) - 1] = '\0';
                    }
                } else {
                    if (event.text.unicode < 128 && event.text.unicode != 8) { // Add characters
                        strncat(amountStr, (char*)&event.text.unicode, 1);
                    } else if (event.text.unicode == 8 && strlen(amountStr) > 0) { // Handle backspace
                        amountStr[strlen(amountStr) - 1] = '\0';
                    }
                }
            } else if (event.type == sfEvtKeyPressed) {
                if (event.key.code == sfKeyEnter) {
                    // Convert amount string to float and process payment
                    float amount = atof(amountStr);
                    printf("Processing payment for %s: %.2f\n", licensePlate, amount);
                    processPayment(licensePlate,amount);
                    strcpy(licensePlate, "");
                    strcpy(amountStr, "");
                    inputLicensePlate = true;
                } else if (event.key.code == sfKeyTab) {
                    // Toggle between license plate and amount inputs
                    inputLicensePlate = !inputLicensePlate;
                }
            }
        }

        sfText_setFillColor(licensePlateText, inputLicensePlate ? sfBlue : sfBlack);
        sfText_setFillColor(amountText, inputLicensePlate ? sfBlack : sfBlue);

        // Render User Portal
        sfRenderWindow_clear(userPortalWindow, sfWhite);

        // Update the text objects with current input
        sfText_setString(licensePlateText, licensePlate);
        sfText_setString(amountText, amountStr);

        // Draw all elements
        sfRenderWindow_drawText(userPortalWindow, titleText, NULL);
        sfRenderWindow_drawText(userPortalWindow, instructionText, NULL);
        sfRenderWindow_drawText(userPortalWindow, licensePlateLabel, NULL);
        sfRenderWindow_drawText(userPortalWindow, licensePlateText, NULL);
        sfRenderWindow_drawText(userPortalWindow, amountLabel, NULL);
        sfRenderWindow_drawText(userPortalWindow, amountText, NULL);

        sfRenderWindow_display(userPortalWindow);
    }

    // Cleanup
    sfText_destroy(titleText);
    sfText_destroy(instructionText);
    sfText_destroy(licensePlateLabel);
    sfText_destroy(amountLabel);
    sfText_destroy(licensePlateText);
    sfText_destroy(amountText);
    sfFont_destroy(font);
    sfRenderWindow_destroy(userPortalWindow);
}




void processPayment(const char* licensePlate, float amount) {
    int challanID = -1;
    float fineAmount = 0.0f;

    for (int i = 0; i < game.challanCount; i++) {
        if (strcmp(game.challanList[i].vehicleID, licensePlate) == 0) {
            challanID = game.challanList[i].challanID;
            fineAmount = game.challanList[i].fineAmount;
            break;
        }
    }

    if (challanID != -1) {
        if (amount >= fineAmount) {
            // Processing payment
            printf("Processing payment of %.2f for license plate %s\n", amount, licensePlate);
            handlePaymentAlert(challanID); // Update the payment status
            printf("Payment processed successfully.\n");
            for (int i = 0; i < game.challanCount; i++) {
                if (game.vehicles[i]->licensePlate == licensePlate) {
                    game.vehicles[i]->challanStatus=1;
                    break;
                }
            }
        } else {
            printf("Insufficient amount. The fine is %.2f.\n", fineAmount);
        }
    } else {
        printf("No challan found for license plate %s\n", licensePlate);
    }
}


int main() {
    startTraffic();
    return 0;
}
