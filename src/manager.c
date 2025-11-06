#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/manager.h"

static void read_line(char* buf, size_t n) {
    if (!fgets(buf, (int)n, stdin)) { buf[0] = '\0'; return; }
    buf[strcspn(buf, "\r\n")] = 0;
}

void runCLI(Graph* g, Queue* q) {
    if (!g || !q) return;
    char choiceBuf[16];
    char from[MAX_CITY_NAME], to[MAX_CITY_NAME];
    char tmp[128];

    while (1) {
        printf("\n--- MENU ---\n");
        printf("1. Add flight route\n");
        printf("2. Remove flight route\n");
        printf("3. Display network\n");
        printf("4. Check connectivity (BFS)\n");
        printf("5. Find flight path (start -> end)\n");
        printf("6. Enqueue flight record\n");
        printf("7. Dequeue (remove) flight record\n");
        printf("8. Remove flight by ID (expired/cancelled)\n");
        printf("9. Display queue\n");
        printf("10. Display flights by departure\n");
        printf("11. Track delays through graph path\n");
        printf("12. Exit\n");
        printf("Enter choice: ");
        read_line(choiceBuf, sizeof(choiceBuf));
        int choice = atoi(choiceBuf);

        if (choice == 1) {
            printf("From: "); read_line(from, sizeof(from));
            printf("To:   "); read_line(to, sizeof(to));
            if (addFlightRoute(g, from, to) == 0)
                printf("Route added: %s -> %s\n", from, to);
            else
                printf("Failed to add route (exists/invalid).\n");
        }
        else if (choice == 2) {
            printf("From: "); read_line(from, sizeof(from));
            printf("To:   "); read_line(to, sizeof(to));
            if (removeFlightRoute(g, from, to) == 0)
                printf("Route removed: %s -> %s\n", from, to);
            else
                printf("Failed to remove route (not found).\n");
        }
        else if (choice == 3) {
            displayNetwork(g);
        }
        else if (choice == 4) {
            printf("Start city: ");
            read_line(from, sizeof(from));
            bfsConnectivity(g, from);
        }
        else if (choice == 5) {
            printf("Start city: "); read_line(from, sizeof(from));
            printf("End city:   "); read_line(to, sizeof(to));
            int *path = NULL;
            int len = 0;
            if (findFlightPath(g, from, to, &path, &len)) {
                printf("Path (%d): ", len);
                for (int i = 0; i < len; ++i) {
                    printf("%s", g->cities[path[i]].name);
                    if (i + 1 < len) printf(" -> ");
                }
                printf("\n");
                free(path);
            } else {
                printf("No path found from %s to %s\n", from, to);
            }
        }
        else if (choice == 6) {
            FlightRecord fr = {0};
            printf("Flight ID: "); read_line(tmp, sizeof(tmp)); strncpy(fr.flightID, tmp, FLIGHT_ID_LEN-1);
            printf("Departure: "); read_line(tmp, sizeof(tmp)); strncpy(fr.departure, tmp, MAX_CITY_NAME-1);
            printf("Arrival:   "); read_line(tmp, sizeof(tmp)); strncpy(fr.arrival, tmp, MAX_CITY_NAME-1);
            printf("Delay (min): "); read_line(tmp, sizeof(tmp)); fr.delayMinutes = atoi(tmp);

            /* basic validation: departure and arrival must be known cities */
            if (findCityIndex(g, fr.departure) < 0 || findCityIndex(g, fr.arrival) < 0) {
                printf("Invalid departure or arrival city (not in list).\n");
            } else {
                if (enqueue(q, &fr) == 0) printf("Flight record enqueued.\n");
                else printf("Failed to enqueue (memory).\n");
            }
        }
        else if (choice == 7) {
            FlightRecord out;
            if (dequeue(q, &out) == 0) {
                printf("Dequeued flight %s: %s -> %s (delay %d)\n",
                       out.flightID, out.departure, out.arrival, out.delayMinutes);
            } else {
                printf("Queue empty.\n");
            }
        }
        else if (choice == 8) {
            printf("Flight ID to remove: ");
            read_line(tmp, sizeof(tmp));
            if (removeFlightByID(q, tmp) == 0) printf("Removed flight %s from queue.\n", tmp);
            else printf("Flight ID %s not found in queue.\n", tmp);
        }
        else if (choice == 9) {
            displayQueue(q);
        }
        else if (choice == 10) {
            printf("Departure city: "); read_line(tmp, sizeof(tmp));
            displayFlightsByDeparture(q, tmp);
        }
        else if (choice == 11) {
            /* Find path and summarize delays for the path's legs from the queue */
            printf("Start city: "); read_line(from, sizeof(from));
            printf("End city:   "); read_line(to, sizeof(to));
            int *path = NULL; int len = 0;
            if (!findFlightPath(g, from, to, &path, &len)) {
                printf("No path found from %s to %s\n", from, to);
            } else {
                printf("Path (%d): ", len);
                for (int i = 0; i < len; ++i) {
                    printf("%s", g->cities[path[i]].name);
                    if (i + 1 < len) printf(" -> ");
                }
                printf("\n\nDelay summary along path:\n");

                int totalDelay = 0;
                for (int i = 0; i + 1 < len; ++i) {
                    const char* legFrom = g->cities[path[i]].name;
                    const char* legTo = g->cities[path[i+1]].name;
                    int legDelaySum = 0;
                    int legCount = 0;
                    QueueNode* cur = q->front;
                    while (cur) {
                        if (strcmp(cur->data.departure, legFrom) == 0 && strcmp(cur->data.arrival, legTo) == 0) {
                            legDelaySum += cur->data.delayMinutes;
                            legCount++;
                        }
                        cur = cur->next;
                    }
                    printf("  %s -> %s : %d flights, total delay %d min\n", legFrom, legTo, legCount, legDelaySum);
                    totalDelay += legDelaySum;
                }
                printf("Total accumulated delay along path: %d minutes\n", totalDelay);
                free(path);
            }
        }
        else if (choice == 12) {
            printf("Exiting...\n");
            break;
        }
        else {
            printf("Invalid choice. Please try again.\n");
        }
    }
}
