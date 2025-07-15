# Simple Makefile for our model viewer project

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -std=c99 -g

# Package names for pkg-config
PACKAGES = glfw3 glew

# Get compiler flags from pkg-config
CFLAGS += $(shell pkg-config --cflags $(PACKAGES))

# Get linker flags from pkg-config
LDFLAGS = $(shell pkg-config --libs $(PACKAGES))

# Add OpenGL framework (macOS specific)
LDFLAGS += -framework OpenGL

# Directories
SRC_DIR = src
BUILD_DIR = build

# Source files (in src directory)
SOURCES = $(SRC_DIR)/*.c

# Output executable name
TARGET = $(BUILD_DIR)/minimal_window

# Default target (what happens when you just type 'make')
all: $(BUILD_DIR) $(TARGET)

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# How to build the executable
$(TARGET): $(SOURCES) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

# Clean up compiled files
clean:
	rm -rf $(BUILD_DIR)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Show what pkg-config found (useful for debugging)
debug:
	@echo "CFLAGS: $(CFLAGS)"
	@echo "LDFLAGS: $(LDFLAGS)"
	@echo "Packages: $(PACKAGES)"

# Tell make these aren't files
.PHONY: all clean run debug
