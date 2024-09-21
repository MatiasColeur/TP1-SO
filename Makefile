# Compiler to use
CC = gcc

# Compiler flags
CFLAGS = -Wall -g -O2

# Source files
SRCS_APP = app.c slaveMonitorCDT.c shmCDT.c heap.c beSafe.c
SRCS_SLAVE = slave.c heap.c beSafe.c
SRCS_VIEW = View.c shmCDT.c heap.c beSafe.c

# Executable names
TARGET1 = app
TARGET2 = slave
TARGET3 = view

# Default target (compiles all programs)
all: $(TARGET1) $(TARGET2) $(TARGET3)

# Rule to link object files to create the first executable
$(TARGET1):
	$(CC) $(CFLAGS) $(SRCS_APP) -o $(TARGET1)

# Rule to link object files to create the second executable
$(TARGET2):
	$(CC) $(CFLAGS) $(SRCS_SLAVE) -o $(TARGET2)

# Rule to link object files to create the third executable
$(TARGET3):
	$(CC) $(CFLAGS) $(SRCS_VIEW) -o $(TARGET3)

# Clean up executables
clean:
	rm -f $(TARGET1) $(TARGET2) $(TARGET3)

# Phony targets to avoid conflicts with files named 'clean' or 'all'
.PHONY: all clean
