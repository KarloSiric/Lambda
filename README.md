# Half-Life Model Viewer
## Professional MDL Viewer using Official Valve SDK

A modern Half-Life 1 model viewer built with official Valve structures and proper software architecture.

## Features (Planned)
- ✅ Official Valve studio.h structures  
- 🔄 Professional MDL file loading
- 🔄 OpenGL rendering with proper triangle parsing
- 🔄 Texture support
- 🔄 Camera controls
- 🔄 Model information display

## Project Structure
```
src/
├── core/           # Core engine systems
├── graphics/       # Rendering and OpenGL code  
├── mdl/           # MDL format handling
├── ui/            # User interface
└── utils/         # Utility functions
```

## Build Instructions
```bash
make debug      # Debug build
make release    # Release build  
make clean      # Clean build files
make run        # Build and run
```

## Dependencies
- OpenGL 3.3+
- GLFW3 (window management)
- GLAD (OpenGL loading)
- CMath (built-in)
