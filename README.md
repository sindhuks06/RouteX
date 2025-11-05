# Flight Network (C) - Multi-file Project

## Overview
Simple Flight Connection Manager in C. Uses:
- Directed graph (adjacency list with linked lists) to model routes.
- Queue for flight records.
- BFS for connectivity and pathfinding.

## Files
- `include/graph.h`, `src/graph.c` — graph data structures & algorithms
- `include/queue.h`, `src/queue.c` — queue for flight records
- `include/manager.h`, `src/manager.c` — CLI & coordination
- `main.c` — entry point
- `Makefile` — build script (MinGW / Windows)
- `cities.txt` — initial city list (one city per line)

## Build (Windows with MinGW)
Open terminal in project folder:
