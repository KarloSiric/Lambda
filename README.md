# 3D Model Viewer

A cross-platform 3D model viewer built with OpenGL as a learning project to understand graphics programming and 3D file formats.

## Project Overview

This model viewer is designed as a general-purpose 3D model visualization tool, with initial focus on Half-Life MDL files. The project serves as an educational exploration into:

- OpenGL graphics programming
- 3D file format parsing and binary data handling
- Real-time 3D rendering techniques
- Cross-platform development with C and OpenGL

## Current Features

- Basic OBJ file loading and rendering
- OpenGL 4.1 Core Profile support (native macOS implementation)
- Cross-platform compatible (macOS primary, Linux/Windows supported)
- Matrix transformations using cglm
- Shader-based rendering pipeline
- Half-Life MDL file format support (in development)

## Technical Notes

- **OpenGL Version:** Built with OpenGL 4.1 Core Profile (last version officially supported on macOS)
- **Platform Support:** Primary development on macOS, designed to be cross-platform
- **Dependencies:** Uses native OpenGL on macOS (no GLEW), GLEW required for Windows/Linux
- **Graphics:** Modern shader-based rendering pipeline despite OpenGL 4.1 limitation

## Planned Features

- MDL file parsing and rendering
- Animation playback support
- Texture loading and display
- Model export functionality
- Multi-format support (OBJ, MDL, potentially M2, MPQ)

## Legal Notice & Copyright

**Important:** This software does not include any copyrighted game assets, models, textures, or other proprietary content. 

- You must legally own the games to use their model files
- This tool is for educational and personal use only
- Game assets remain property of their respective copyright holders (Valve Corporation, Blizzard Entertainment, etc.)
- The source code of this viewer is provided under open source license, but does not grant rights to any game assets

## Requirements

### macOS
- OpenGL 4.1 support (native, no GLEW required)
- GLFW3 and cglm libraries
- C99 compatible compiler

### Other Platforms
- OpenGL 4.1+ support
- GLFW3, GLEW (for Windows/Linux), and cglm libraries
- C99 compatible compiler

## Installation

### macOS
```bash
brew install glfw cglm
make
```

### Linux/Windows
```bash
# Install glfw, glew, cglm via your package manager
# Example for Ubuntu:
sudo apt-get install libglfw3-dev libglew-dev libcglm-dev
make
```

2. Clone and build:
```bash
git clone [your-repo-url]
cd 3d-model-viewer
make
```

3. Run with a model file:
```bash
./build/minimal_window
```

## Using with Half-Life Models

To view Half-Life MDL files (when implemented):

1. **Legally obtain the files:** Purchase and install Half-Life via Steam
2. **Locate models:** Find them in your Steam installation directory
3. **Copy to project:** Place model files in the `models/` directory
4. **Run viewer:** `./build/model_viewer models/barney.mdl`

**Note:** Model files are not included in this repository due to copyright restrictions.

## Project Structure

```
├── src/           # Source code
├── shaders/       # OpenGL shaders
├── models/        # Your model files (not in repo)
├── build/         # Compiled output
└── Makefile       # Build configuration
```

## Development Progress

This is an active learning project. Current development focuses on:
- Understanding binary file formats
- Implementing MDL format parsing
- Building a robust rendering pipeline
- Creating educational documentation

## Contributing

This is primarily a learning project, but contributions, suggestions, and educational discussions are welcome!

## License

[Add your chosen license here]

## Acknowledgments

Built as part of learning graphics programming and 3D file formats. Special thanks to the game development and modding communities for their documentation and resources.
