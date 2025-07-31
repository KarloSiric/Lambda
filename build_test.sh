#!/bin/bash
cd "/Users/karlosiric/Documents/SublimeText Programming/C_Projects/ModelViewer"
echo "Building project..."
make clean
make 2>&1
echo "Build complete."