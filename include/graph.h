#ifndef GRAPH_H
#define GRAPH_H

#include <stddef.h>

#define MAX_CITY_NAME 64

typedef struct Node {
    int destIndex;
    struct Node* next;
} Node;

typedef struct City {
    char name[MAX_CITY_NAME];
    Node* head;
} City;

typedef struct Graph {
    City* cities;
    int cityCount;
} Graph;

/* Graph functions */
Graph* createGraphFromFile(const char* filename);
void freeGraph(Graph* g);
int findCityIndex(const Graph* g, const char* name);
int addFlightRoute(Graph* g, const char* from, const char* to);
int removeFlightRoute(Graph* g, const char* from, const char* to);
void displayNetwork(const Graph* g);

/* BFS / Pathfinding */
void bfsConnectivity(const Graph* g, const char* start);
int findFlightPath(const Graph* g, const char* start, const char* end, int** outPath, int* outLen);

#endif
