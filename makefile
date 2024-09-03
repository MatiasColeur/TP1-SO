# Compiler and flags
CC = gcc
CFLAGS = -Wall -g -v 

# Common source files, excluding app.c, view.c, and slave.c
COMMON_SRCS = $(filter-out app.c view.c slave.c, $(wildcard *.c))

# Targets
TARGETS = app view slave

# Default target
all: $(TARGETS)

# Rule for each target
$(TARGETS): %: %.c $(COMMON_SRCS)
	@echo "Generating $@ binary"
	$(CC) $(CFLAGS) $^ -o $@

# Clean up
clean:
	rm -f $(TARGETS)

.PHONY: all clean

