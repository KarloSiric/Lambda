# Professional Makefile for Half-Life Model Viewer - WORKING VERSION
# Fixed to properly build the corrected codebase

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -I./src
DEBUG_FLAGS = -g -DDEBUG
LDFLAGS = -framework OpenGL -lglfw -lm

# macOS specific OpenGL
ifeq ($(shell uname), Darwin)
    CFLAGS += -DGL_SILENCE_DEPRECATION
endif

# Source files
SOURCES = src/main.c \
          src/mdl/mdl_loader.c \
          src/mdl/mdl_info.c \
          src/mdl/mdl_report.c \
          src/mdl/mdl_animations.c \
          src/mdl/bodypart_manager.c \
          src/mdl/bone_system.c \
          src/graphics/renderer.c \
          src/graphics/camera.c \
          src/graphics/textures.c \
          src/utils/logger.c \
          src/utils/mdl_messages.c \
          src/utils/utils.c \
          src/utils/args.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Target executable
TARGET = modelviewer

.PHONY: all debug release clean run test

all: release

debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)
	@echo "🐛 Debug build complete!"

release: $(TARGET)
	@echo "🚀 Release build complete!"

$(TARGET): $(OBJECTS)
	@echo "🔗 Linking $(TARGET)..."
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	@echo "🔨 Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "🧹 Cleaning build files..."
	rm -f $(OBJECTS) $(TARGET)

test: $(TARGET)
	@echo "🧪 Running test build..."
	@echo "Built successfully! Now test with: ./$(TARGET) path/to/model.mdl"

run: $(TARGET)
	@echo "▶️  To run: ./$(TARGET) path/to/model.mdl"

# Dependencies
src/main.o: src/main.c src/main.h src/mdl/mdl_loader.h src/mdl/mdl_info.h src/studio.h src/graphics/renderer.h
src/mdl/mdl_loader.o: src/mdl/mdl_loader.c src/mdl/mdl_loader.h src/studio.h
src/mdl/mdl_info.o: src/mdl/mdl_info.c src/mdl/mdl_info.h src/mdl/mdl_loader.h src/studio.h
src/graphics/renderer.o: src/graphics/renderer.c src/graphics/renderer.h src/studio.h

help:
	@echo "Half-Life Model Viewer Build System"
	@echo ""
	@echo "Commands:"
	@echo "  make debug    - Build with debug symbols"
	@echo "  make release  - Build optimized version"
	@echo "  make clean    - Remove build files"
	@echo "  make test     - Build and test"
	@echo "  make run      - Show run instructions"
