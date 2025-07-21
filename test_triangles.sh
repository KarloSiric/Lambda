#!/bin/bash

echo "=== Testing Triangle Extraction ==="
echo "Building project..."

cd "/Users/karlosiric/Documents/SublimeText Programming/C_Projects/ModelViewer"

# Clean and build
make clean
make

if [ $? -eq 0 ]; then
    echo "Build successful! Running to test triangle extraction..."
    echo "=========================================="
    # Run for just a moment to see the triangle data, then close
    timeout 3s ./build/minimal_window || echo "Program output captured"
else
    echo "Build failed!"
fi
