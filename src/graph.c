#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/graph.h"

static char *trim_newline(char *s) {
    if (!s) return s;
    s[strcspn(s, "\r\n")] = 0;
    return s;
}

Graph* createGraphFromFile(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error: could not open %s\n", filename);
        return NULL;
    }

    int count = 0;
    char buffer[MAX_CITY_NAME];
    while (fgets(buffer, sizeof(buffer), fp)) {
        trim_newline(buffer);
        if (buffer[0] == '\0') continue; // skip blank lines
        count++;
    }

    if (count == 0) {
        fclose(fp);
        fprintf(stderr, "Error: no cities found in %s\n", filename);
        return NULL;
    }

    rewind(fp);

    Graph* g = malloc(sizeof(Graph));
    if (!g) { fclose(fp); return NULL; }
    g->cityCount = count;
    g->cities = calloc(count, sizeof(City));
    if (!g->cities) { free(g); fclose(fp); return NULL; }

    int i = 0;
    while (fgets(buffer, sizeof(buffer), fp)) {
        trim_newline(buffer);
        if (buffer[0] == '\0') continue;
        strncpy(g->cities[i].name, buffer, MAX_CITY_NAME - 1);
        g->cities[i].name[MAX_CITY_NAME - 1] = '\0';
        i++;
    }

    /* allocate adjacency matrix */
    g->adj = malloc(count * sizeof(int*));
    if (!g->adj) { free(g->cities); free(g); fclose(fp); return NULL; }
    for (int r = 0; r < count; ++r) {
        g->adj[r] = calloc(count, sizeof(int));
        if (!g->adj[r]) {
            for (int k = 0; k < r; ++k) free(g->adj[k]);
            free(g->adj);
            free(g->cities);
            free(g);
            fclose(fp);
            return NULL;
        }
    }

    fclose(fp);
    return g;
}

void freeGraph(Graph* g) {
    if (!g) return;
    if (g->adj) {
        for (int i = 0; i < g->cityCount; ++i) {
            free(g->adj[i]);
        }
        free(g->adj);
    }
    free(g->cities);
    free(g);
}

int findCityIndex(const Graph* g, const char* name) {
    if (!g || !name) return -1;
    for (int i = 0; i < g->cityCount; ++i) {
        if (strcmp(g->cities[i].name, name) == 0) return i;
    }
    return -1;
}

int addFlightRoute(Graph* g, const char* from, const char* to) {
    if (!g || !from || !to) return -1;
    int u = findCityIndex(g, from);
    int v = findCityIndex(g, to);
    if (u < 0 || v < 0) return -1;
    if (u == v) return -1; // no self-loops
    if (g->adj[u][v]) return -1; /* duplicate */
    g->adj[u][v] = 1;
    return 0;
}

int removeFlightRoute(Graph* g, const char* from, const char* to) {
    if (!g || !from || !to) return -1;
    int u = findCityIndex(g, from);
    int v = findCityIndex(g, to);
    if (u < 0 || v < 0) return -1;
    if (!g->adj[u][v]) return -1;
    g->adj[u][v] = 0;
    return 0;
}

void displayNetwork(const Graph* g) {
    if (!g) return;
    printf("\n--- Flight Network ---\n");
    for (int i = 0; i < g->cityCount; ++i) {
        printf("%s -> ", g->cities[i].name);
        int first = 1;
        for (int j = 0; j < g->cityCount; ++j) {
            if (g->adj[i][j]) {
                if (!first) printf(", ");
                printf("%s", g->cities[j].name);
                first = 0;
            }
        }
        printf("\n");
    }
}

/* BFS connectivity: print all reachable cities from start (excluding start) */
void bfsConnectivity(const Graph* g, const char* start) {
    if (!g || !start) return;
    int s = findCityIndex(g, start);
    if (s < 0) {
        printf("City '%s' not found.\n", start);
        return;
    }

    int n = g->cityCount;
    int *visited = calloc(n, sizeof(int));
    if (!visited) return;

    int *queue = malloc(n * sizeof(int));
    if (!queue) { free(visited); return; }

    int head = 0, tail = 0;
    visited[s] = 1;
    queue[tail++] = s;

    printf("Reachable from %s:\n", start);
    int any = 0;
    while (head < tail) {
        int u = queue[head++];
        for (int v = 0; v < n; ++v) {
            if (g->adj[u][v] && !visited[v]) {
                visited[v] = 1;
                queue[tail++] = v;
                printf(" - %s\n", g->cities[v].name);
                any = 1;
            }
        }
    }

    if (!any) printf(" (none)\n");

    free(visited);
    free(queue);
}

/* Find one path using BFS; returns 1 if found, 0 otherwise.
   If found, *outPath will point to malloc'd array of length *outLen (caller frees). */
int findFlightPath(const Graph* g, const char* start, const char* end, int** outPath, int* outLen) {
    if (!g || !start || !end || !outPath || !outLen) return 0;
    int s = findCityIndex(g, start);
    int t = findCityIndex(g, end);
    if (s < 0 || t < 0) return 0;

    int n = g->cityCount;
    int *visited = calloc(n, sizeof(int));
    int *parent = malloc(n * sizeof(int));
    int *queue = malloc(n * sizeof(int));
    if (!visited || !parent || !queue) {
        free(visited); free(parent); free(queue);
        return 0;
    }
    for (int i = 0; i < n; ++i) parent[i] = -1;

    int head = 0, tail = 0;
    visited[s] = 1;
    queue[tail++] = s;
    int found = 0;

    while (head < tail) {
        int u = queue[head++];
        if (u == t) { found = 1; break; }
        for (int v = 0; v < n; ++v) {
            if (g->adj[u][v] && !visited[v]) {
                visited[v] = 1;
                parent[v] = u;
                queue[tail++] = v;
                if (v == t) { found = 1; break; }
            }
        }
        if (found) break;
    }

    if (!found) {
        free(visited); free(parent); free(queue);
        return 0;
    }

    /* reconstruct path */
    int cur = t;
    int pathLen = 0;
    while (cur != -1) { pathLen++; cur = parent[cur]; }

    int *path = malloc(pathLen * sizeof(int));
    if (!path) { free(visited); free(parent); free(queue); return 0; }
    cur = t;
    for (int i = pathLen - 1; i >= 0; --i) {
        path[i] = cur;
        cur = parent[cur];
    }

    *outPath = path;
    *outLen = pathLen;

    free(visited);
    free(parent);
    free(queue);
    return 1;
}
