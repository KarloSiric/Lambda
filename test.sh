#!/bin/bash

echo "===================="
echo "Building Half-Life Model Viewer..."
echo "===================="

cd "/Users/karlosiric/Documents/SublimeText Programming/C_Projects/ModelViewer"

# Clean previous build
rm -f modelviewer

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
    -Wall -std=c99 -g 2>&1

if [ $? -eq 0 ]; then
    echo ""
    echo "✅ Build successful!"
    echo ""
    
    # Check if a model file exists to test with
    if [ -f "models/HL1_Original/scientist.mdl" ]; then
        echo "Running with scientist model..."
        ./modelviewer models/HL1_Original/scientist.mdl
    elif [ -f "models/HL1_Original/headcrab.mdl" ]; then
        echo "Running with headcrab model..."
        ./modelviewer models/HL1_Original/headcrab.mdl
    else
        echo "No model file found. Run with:"
        echo "./modelviewer <path_to_mdl_file>"
    fi
else
    echo ""
    echo "❌ Build failed! Check errors above."
fi
