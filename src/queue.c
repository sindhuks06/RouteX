#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/queue.h"

void initQueue(Queue* q) {
    if (!q) return;
    q->front = q->rear = NULL;
    q->size = 0;
}

int enqueue(Queue* q, const FlightRecord* r) {
    if (!q || !r) return -1;
    QueueNode* node = malloc(sizeof(QueueNode));
    if (!node) return -1;
    node->next = NULL;
    node->data = *r; // struct copy
    if (!q->rear) {
        q->front = q->rear = node;
    } else {
        q->rear->next = node;
        q->rear = node;
    }
    q->size++;
    return 0;
}

int dequeue(Queue* q, FlightRecord* out) {
    if (!q || !q->front) return -1;
    QueueNode* node = q->front;
    if (out) *out = node->data;
    q->front = node->next;
    if (!q->front) q->rear = NULL;
    free(node);
    q->size--;
    return 0;
}

void displayQueue(const Queue* q) {
    if (!q) return;
    printf("\n--- Flight Record Queue (size=%zu) ---\n", q->size);
    QueueNode* cur = q->front;
    if (!cur) { printf(" (empty)\n"); return; }
    while (cur) {
        printf("Flight %s: %s -> %s (delay %d)\n",
               cur->data.flightID, cur->data.departure, cur->data.arrival, cur->data.delayMinutes);
        cur = cur->next;
    }
}

void freeQueue(Queue* q) {
    if (!q) return;
    FlightRecord tmp;
    while (dequeue(q, &tmp) == 0) { /* discard */ }
}
