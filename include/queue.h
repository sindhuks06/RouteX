#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>
#include "graph.h"

#define FLIGHT_ID_LEN 16

typedef struct FlightRecord {
    char flightID[FLIGHT_ID_LEN];
    char departure[MAX_CITY_NAME];
    char arrival[MAX_CITY_NAME];
    int delayMinutes;
} FlightRecord;

typedef struct QueueNode {
    FlightRecord data;
    struct QueueNode* next;
} QueueNode;

typedef struct Queue {
    QueueNode* front;
    QueueNode* rear;
    size_t size;
} Queue;

/* Queue functions */
void initQueue(Queue* q);
int enqueue(Queue* q, const FlightRecord* r);
int dequeue(Queue* q, FlightRecord* out);
void displayQueue(const Queue* q);
void freeQueue(Queue* q);

#endif
