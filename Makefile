# Compiler
CC = gcc

# Compiler Flags
CFLAGS = -Wall -Wextra -g -Iinclude

# Source Files
SRCS = main.c src/graph.c src/queue.c src/manager.c

# Output Executable Name
TARGET = flight_manager.exe

# Default target
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	del $(TARGET)
