# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased]


## [0.2.0-alpha.1] - 2025-10-15

### Fixed:
- **Dependencies**
  - Added proper dependency for Linux(Tested for Arch Linux Kernel 6.17.3 version)
  - Improved the CMakeList.txt to contain proper dependencies and initializaton
  - Added a so called gl_platform header that takes care of different OS and architecture dependencies 
  - Made the GLEW be initialized on Linux and kept it off for macOS that was caussing seg faults on Linux
  - Added proper GLEW and GL/gl3 dependency difference detection to make seg fault free
  - Currently fully working and stable for both macOS and Linux (tested on macOS Tahoe 26.0.1 and Arch Linux Kernel 6.17.3)
  
### Changed
- **Dependenices**
  - Added a new header called gl_platform which takes care of all the necessary dependencies
  - Improved the GLEW and GL/gl3 header dependency initialization to prevent seg fault crashes
  - Improved the core functionality for both macOS and Linux to make it cross platform and working for now

### Technical Details
- Modified and added a new file called gl_platform.h -> new core cross platform handling file
- Added in init_renderer support for additional checking and initialization of GLEW for Windows/Linux

---

### Planned Features
- Qt GUI framework integration
- Model export capabilities (OBJ, FBX, SMD)
- Model editing tools
- QC compiler integration
- Advanced rendering modes
- Texture import/export

---

## [0.1.1-alpha.1] - 2025-10-15

### Fixed
- **Logging System**
  - Moved texture creation logs from DEBUG to TRACE level
  - Prevents texture spam in verbose mode (`-vv`)
  - Texture creation now only shown with `--trace` flag
  - Reduces log file size significantly (from 11MB to manageable size)
  - Improves readability of verbose logging output

### Changed
- **Debug Verbosity**
  - `-vv` (verbose) now shows useful debug info without texture spam
  - `--trace` flag remains for ultra-detailed logging including all texture operations
  - Better separation between debugging levels

### Technical Details
- Modified `src/graphics/textures.c` line 299
- Changed `LOG_DEBUGF` to `LOG_TRACEF` for texture creation success messages
- Build number incremented from 1 to 2
- No API changes, fully backward compatible

---

## [0.1.0-alpha.1] - 2025-01-15

### Added
- **Core Functionality**
  - Initial Half-Life MDL file loader
  - Studio model format parser (IDST v10)
  - Complete bone hierarchy system
  - Animation sequence playback
  - Sequence group loading (external .mdl files)
  - Texture loading from companion T.mdl files
  - Bodypart management system

- **Rendering**
  - OpenGL 3.3+ rendering pipeline
  - Basic textured model rendering
  - Wireframe mode toggle (F key)
  - Points mode toggle (P key)
  - Camera system (orbit, zoom, pan)
  - Mouse drag controls
  - Keyboard camera controls (WASD, QE, R for reset)

- **Animation**
  - Animation playback with proper bone transformations
  - Sequence selection (LEFT/RIGHT arrow keys)
  - Play/pause toggle (SPACE key)
  - Animation speed control (UP/DOWN keys)
  - Looping toggle (L key)
  - Frame reset (0 key)
  - Animation info display (I key)
  - Frame interpolation for smooth playback

- **Model Analysis**
  - `--dump` flag: Basic model structure dump
    - Header information
    - Bone hierarchy
    - Sequences list
    - Bodyparts summary
    - Texture information
  - `--dump-ex` flag: Extended structure dump
    - Complete raw header data
    - Detailed offset information
    - Memory addresses
    - All model internals
  - `--dump-only` flag: Dump and exit (no viewer window)
  - `--quiet` flag: Suppress non-error output

- **Command-Line Interface**
  - Help system (`--help`, `-h`)
  - Version information
  - Flexible argument parsing
  - Error reporting
  - Usage examples

- **Developer Tools**
  - Comprehensive logging system
  - Debug output categories
  - Error message framework
  - Model validation

- **Documentation**
  - README with build instructions
  - Development roadmap
  - Semantic versioning system
  - Changelog file (this file)

### Technical Details
- **Platform Support**: macOS, Linux, Windows
- **Build System**: CMake 3.15+
- **Dependencies**: 
  - GLFW 3.x (windowing and input)
  - OpenGL 3.3+ (rendering)
  - CGLM (linear algebra)
- **Language**: C11 standard
- **Code Structure**:
  ```
  src/
  ├── main.c              # Entry point
  ├── studio.h            # Format definitions
  ├── graphics/           # Rendering system
  ├── mdl/                # Model loading
  └── utils/              # Utilities
  ```

### Known Issues
- No graphical user interface (command-line only)
- No model editing capabilities
- No export functionality
- Limited render modes (no PBR, no shadows)
- No undo/redo system
- Animation blending not implemented
- No plugin system

### Performance
- Loads typical HL1 models in < 100ms
- Renders at 60+ FPS on integrated graphics
- Memory usage: ~20-50 MB per model
- Supports models with up to 128 bones
- Handles sequence groups efficiently

### Compatibility
- **Supported Formats**:
  - Half-Life 1 Studio Models (IDST v10)
  - Texture files (T.mdl companion files)
  - Sequence groups (01.mdl, 02.mdl, etc.)
- **Tested Models**:
  - Half-Life 1 original models
  - Counter-Strike 1.6 models
  - Custom community models

### Build Information
- Build number: 1
- Compiler: Clang/GCC/MSVC
- Target architectures: x86_64, ARM64
- Debug symbols included in debug builds

---

## Version History Summary

| Version | Release Date | Milestone |
|---------|-------------|-----------|  
| 0.1.1-alpha.1 | 2025-10-15 | Logging improvements |
| 0.1.0-alpha.1 | 2025-01-15 | Initial release |

---

## How to Read This Changelog

### Categories
- **Added**: New features
- **Changed**: Changes to existing functionality
- **Deprecated**: Features that will be removed
- **Removed**: Features that have been removed
- **Fixed**: Bug fixes
- **Security**: Security vulnerability fixes

### Semantic Versioning
- **Major** (x.0.0): Breaking changes
- **Minor** (0.x.0): New features (backward compatible)
- **Patch** (0.0.x): Bug fixes (backward compatible)
- **Pre-release**: alpha, beta, rc
