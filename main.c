#include <stdio.h>
#include <stdlib.h>
#include "include/graph.h"
#include "include/queue.h"
#include "include/manager.h"

int main(void) {
    const char* cityFile = "cities.txt";
    Graph* g = createGraphFromFile(cityFile);
    if (!g) return 1;

    Queue q;
    initQueue(&q);

    runCLI(g, &q);

    freeQueue(&q);
    freeGraph(g);
    return 0;
}
