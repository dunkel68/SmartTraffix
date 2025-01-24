#ifndef CHALLAN_H
#define CHALLAN_H

#include <SFML/Graphics.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "car.h"

typedef struct {
    int challanID;
    char vehicleID[20];
    char vehicleType[20];
    float fineAmount;
    float totalAmount;
    char issueDate[20];
    char dueDate[20];
    char paymentStatus[20];  // "Unpaid", "Paid", or "Overdue"
} Challan;


void initialize(Challan* challan);

void generate_challan();

#endif // ECHALLAN_H
