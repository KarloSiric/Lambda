#!/bin/bash

# Debug script for the Half-Life Model Viewer
# This will help you test and debug your model viewer

echo "=== Half-Life Model Viewer Debug Tool ==="
echo ""

PROJECT_DIR="/Users/karlosiric/Documents/SublimeText Programming/C_Projects/ModelViewer"
cd "$PROJECT_DIR"

# Check if build directory exists
if [ ! -d "build" ]; then
    echo "❌ Build directory not found. Creating..."
    mkdir build
fi

echo "🔨 Building project..."
if make clean && make; then
    echo "✅ Build successful!"
else
    echo "❌ Build failed! Check compilation errors above."
    exit 1
fi

echo ""
echo "📁 Available MDL files:"
find models/ -name "*.mdl" -type f | head -10

echo ""
echo "🎮 Testing with barney.mdl..."
if [ -f "models/HL1_Original/barney.mdl" ]; then
    echo "✅ Barney model found"
    echo "📊 File size: $(ls -lh models/HL1_Original/barney.mdl | awk '{print $5}')"
else
    echo "❌ Barney model not found in models/HL1_Original/"
    echo "Available models in HL1_Original:"
    ls -la models/HL1_Original/*.mdl | head -5
fi

echo ""
echo "🖥️  System info:"
echo "OpenGL version: $(system_profiler SPDisplaysDataType | grep "OpenGL Engine" | head -1)"
echo "Graphics: $(system_profiler SPDisplaysDataType | grep "Chipset Model" | head -1)"

echo ""
echo "🚀 Running model viewer..."
echo "Controls:"
echo "  - ESC: Exit"
echo "  - TAB: Switch models (when implemented)"
echo "  - SPACE: Wireframe mode (when implemented)"
echo ""
echo "Press Ctrl+C to stop..."

# Run the model viewer
./build/minimal_window
