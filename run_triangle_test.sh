#!/bin/bash

echo "=== TESTING TRIANGLE RENDERING ==="
echo "Building updated project..."

cd "/Users/karlosiric/Documents/SublimeText Programming/C_Projects/ModelViewer"

# Clean and build
make clean
echo "Cleaned project"

make
if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    echo ""
    echo "=== RUNNING TRIANGLE RENDERER ==="
    echo "Press ESC or close window to exit"
    echo "You should see a rotating 3D scientist model with triangulated surfaces!"
    echo ""
    
    # Run the program
    ./build/minimal_window
    
    echo ""
    echo "=== Test Complete ==="
else
    echo "❌ Build failed!"
    echo "Check compiler errors above"
fi
