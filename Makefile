# Professional Makefile for Half-Life Model Viewer
# We'll write this together to handle all the build complexity

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
DEBUG_FLAGS = -g -DDEBUG

# We'll add the actual build rules as we write code
# This keeps it clean and professional

.PHONY: all debug release clean run

all:
	@echo "🔨 Building Half-Life Model Viewer..."
	@echo "Use 'make debug' or 'make release'"

debug:
	@echo "🐛 Debug build - coming soon!"

release:
	@echo "🚀 Release build - coming soon!"

clean:
	@echo "🧹 Cleaning build files..."
	
run:
	@echo "▶️  Running viewer - coming soon!"
