# Makefile for the FT60X Demo Application

# --- Variables ---
# The compiler to use
CC = gcc

# Standard C flags, including warnings and debugging symbols
CFLAGS = -std=c11 -Wall -Wextra -g

# Name of the final executable
TARGET = FT60X_DemoApp

# All source files in the project
# Note: FTD3XX_HotPlug.c and FTD3XX_LoopbackTransfers.c are not included
# based on your previous 'ls' output.
SRCS = FT60X_DemoApp.c \
       FTD3XX_ChipConfiguration.c \
       FTD3XX_Descriptors.c \
       FTD3XX_GPIO.c \
       FTD3XX_OpenClose.c \
	   FTD3XX_LoopbackTransfers.c \
	   FTD3XX_NotificationData.c \
	   FTD3XX_AbortRec.c \
	   FTD3XX_Streaming.c

# Convert source file names to object file names
OBJS = $(SRCS:.c=.o)

# Libraries to link against.
# We are specifying that the linker should look in the current directory
# for libraries (`-L.`) and then link with the `ftd3xx` library (`-lftd3xx`).
# -pthread and -lrt are for multithreading and real-time extensions.
# IMPORTANT: The libraries must come AFTER the object files in the link command.
# Library search paths (common)

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
    LDFLAGS = -L. -lftd3xx -pthread -lrt
endif

ifeq ($(UNAME_S),Darwin)
    LDFLAGS = -L. -L/usr/local/lib -lftd3xx -pthread
endif


# --- Build Rules ---

# The default rule. This will build the TARGET executable from the object files.
# The command explicitly places the object files before the libraries.
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

# Rule to compile a single source file into an object file.
# The `$<` is an automatic variable for the first prerequisite (the .c file).
# The `$@` is an automatic variable for the target (the .o file).
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# --- Cleanup Rule ---
# This rule is for cleaning up the compiled files.
# It removes the executable and all object files.
.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS)


