#!/bin/bash

echo "Building Half-Life Model Viewer..."
cd "/Users/karlosiric/Documents/SublimeText Programming/C_Projects/ModelViewer"

# Compile with all source files
gcc -o modelviewer \
    src/main.c \
    src/mdl/mdl_loader.c \
    src/mdl/mdl_info.c \
    src/graphics/renderer.c \
    -I/opt/homebrew/include \
    -L/opt/homebrew/lib \
    -lglfw \
    -framework OpenGL \
    -framework Cocoa \
    -framework IOKit \
    -framework CoreVideo \
    -Wall -Wextra -std=c99 -g

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Run with: ./modelviewer <path_to_mdl_file>"
else
    echo "Build failed!"
fi
