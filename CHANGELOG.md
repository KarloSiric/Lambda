# Changelog

All notable changes to **Lambda** - Half-Life Studio Model Viewer and Editor - are documented in this file.

This project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html) and follows the [Keep a Changelog](https://keepachangelog.com/en/1.0.0/) format.

**Repository**: [https://github.com/KarloSiric/Lambda](https://github.com/KarloSiric/Lambda)

---

## Table of Contents

- [Unreleased](#unreleased)
- [0.7.0](#070---2026-03-16)
- [0.6.0](#060---2026-02-15)
- [0.5.0](#050---2026-01-20)
- [0.4.0](#040---2025-11-22)
- [0.3.0](#030---2025-11-07)
- [0.2.0-alpha.1](#020-alpha1---2025-10-15)
- [0.1.1-alpha.1](#011-alpha1---2025-10-15)
- [0.1.0-alpha.1](#010-alpha1---2025-10-10)
- [Development History](#development-history-2025)

---

## [Unreleased]

### Planned Features
- Hitbox visualization with color-coded hit groups (head, chest, arms, legs)
- Bone skeleton wireframe rendering with joint spheres
- Attachment point 3D gizmos
- Normal vector visualization
- Screenshot tool with transparent background support
- QC file generation (model decompiler)
- Texture batch export (PNG/TGA)
- Animation timeline with event markers
- Model comparison side-by-side view
- Windows platform build and installer
- Linux AppImage distribution

### Under Consideration
- Animation blending preview
- Bone controller UI sliders
- Plugin system architecture
- OBJ/FBX/SMD export
- StudioMDL compiler integration

---

## [0.7.0] - 2026-03-16

**Milestone**: Inspector Panel System and Visual Gizmos

This release introduces a comprehensive inspector panel system with multiple specialized tabs, rotation gizmos for visual orientation feedback, and a compass display showing current camera orientation.

### Added

#### Inspector Panel System
- **Multi-Row Tab Layout**
  - Three-row tabbed inspector interface
  - Organized by functionality: Info, Animation, Display
  - Collapsible sections within each panel
  - Smooth tab switching with state preservation

- **Model Info Panel** (`ModelInfoPanel.cpp`)
  - Model name and file path display
  - Vertex, triangle, and mesh counts
  - Bone count and hierarchy depth
  - Sequence and bodypart counts
  - Texture count and memory usage
  - Attachment and hitbox counts
  - Bounding box dimensions (min/max/clipping hull)
  - Eye position coordinates
  - Model flags display

- **Sequences Panel** (`SequencesPanel.cpp`)
  - Complete sequence list with search/filter
  - Sequence properties display (FPS, frame count, flags)
  - Activity ID and weight information
  - Blend type and range display
  - Motion type and bone information
  - Event count per sequence
  - Double-click to play functionality
  - Current sequence highlighting

- **Textures Panel** (`TexturesPanel.cpp`)
  - Texture grid view with thumbnails
  - Full-size texture preview on selection
  - Texture dimensions and flag display
  - Chrome, fullbright, additive, masked indicators
  - Texture index and offset information
  - Click to view in TextureViewWidget

- **Bodyparts Panel** (`BodypartsPanel.cpp`)
  - Hierarchical bodypart/submodel tree view
  - Radio button submodel selection
  - Vertex and triangle counts per submodel
  - Mesh count display
  - Visual indication of current selection
  - Reset to defaults button

- **Bones Panel** (`BonesPanel.cpp`)
  - Complete bone hierarchy tree view
  - Parent-child relationship visualization
  - Bone index and parent index display
  - Bone flags information
  - Controller assignments
  - Default position/rotation values
  - Scale values per axis

- **Bone Controllers Panel** (`BoneControllersPanel.cpp`)
  - List of all bone controllers
  - Controller type display (X/Y/Z rotation, mouth)
  - Affected bone indication
  - Value range (start/end)
  - Rest position value
  - Controller index

- **Attachments Panel** (`AttachmentsPanel.cpp`)
  - Complete attachment list
  - Parent bone assignment
  - Local position coordinates (X, Y, Z)
  - Orientation vectors display
  - Attachment type information

- **Model Display Panel** (`ModelDisplayPanel.cpp`)
  - Render mode toggles (Textured, Wireframe, Points)
  - Visualization toggles (Bones, Hitboxes, Attachments, Normals)
  - Environment toggles (Grid, Axes, Ground)
  - Lighting enable/disable
  - Background color picker

- **Lighting Panel** (`LightingPanel.cpp`)
  - Light position controls (X, Y, Z sliders)
  - Light color picker (RGB)
  - Ambient intensity slider
  - Diffuse intensity control
  - Specular highlights toggle
  - Flat shading option

- **Browser Panel** (`BrowserPanel.cpp`)
  - File system navigation
  - Recent files quick access
  - Favorite folders bookmarks
  - MDL file filtering
  - Double-click to load model

#### Visual Gizmos
- **Rotation Gizmos** (`r_gizmo.c`)
  - 3D rotation ring display
  - Real-time rotation feedback
  - X/Y/Z axis color coding (Red/Green/Blue)
  - Shader-based rendering (`gizmo.vert`, `gizmo.frag`)
  - Configurable size and opacity

- **Compass Orientation** (`r_compass.c`)
  - On-screen compass in viewport corner
  - Shows current view direction
  - X/Y/Z axis indicators
  - Updates with camera rotation
  - Shader-based rendering (`compass.vert`, `compass.frag`)

#### Camera Improvements
- WASD movement responsiveness enhancement
- Smoother camera transition interpolation
- Model movement offset corrections
- Raycasting camera foundation (`188b693`)
- Better orbital camera behavior
- Camera reset improvements

### Changed
- Inspector layout reorganized from single panel to multi-tab rows
- Toolbar icon organization refined
- Status bar information density increased
- Dark theme color adjustments for better contrast
- Model alignment on grid improved (`0f120765`)

### Fixed
- Camera movement transition offset issues (`2bafc8e`)
- Model movement synchronization with camera (`ee5d855`)
- Status bar label registration (three labels created but not registered) (`d1a6496`)
- Widget parent-child relationship hierarchy (`e1184c6`)
- Window close behavior causing crashes (`e1184c6`)
- Inspector panel layout issues with varying content sizes
- Tab switching state preservation

### Technical Details
- New files: `InspectorPanel.cpp/h`, all panel implementations
- New files: `r_gizmo.c/h`, `r_compass.c/h`
- New shaders: `gizmo.vert/frag`, `compass.vert/frag`
- Inspector base class with virtual refresh mechanism
- Model viewport connection to all inspector panels
- ~15 new source files added

---

## [0.6.0] - 2026-02-15

**Milestone**: Toolbar Icon System and Resource Management

This release introduces a complete custom icon set for the toolbar and establishes the Qt resource management system.

### Added

#### Toolbar Icon System
- **Custom Icon Set** (60+ icons)
  - Render mode icons: wireframe, textured, points
  - Visualization icons: bones, hitboxes, attachments, normals
  - Environment icons: grid, axes, ground
  - Animation icons: play, pause, stop, loop
  - Frame navigation: previous frame, next frame
  - Camera controls: reset, zoom views (XY, XZ, ZY)
  - File operations: open folder, save, new
  - Panel toggles: console, inspector
  - Miscellaneous: screenshot, help, checkmark, sound toggle

- **Icon Factory** (`IconFactory.cpp/h`)
  - Centralized icon loading and caching
  - Support for multiple icon sizes
  - Fallback icon handling
  - Icon state variants (normal, disabled, active)

- **Resource System**
  - Qt Resource Collection (`resources.qrc`)
  - Organized `resources/icons/` directory
  - PNG format with transparency
  - Multiple resolution variants for HiDPI

#### Toolbar Factory Updates
- `ToolbarFactory.cpp` updated with icon assignments
- Icon-based action creation
- Tooltip integration with icons
- Separator organization between icon groups

### Changed
- All toolbar actions now use custom icons instead of text
- Improved icon sizing for consistency (16x16, 24x24, 32x32)
- Better visual feedback for toggle button states
- Toolbar spacing and padding adjustments

### Fixed
- Icon display on Retina/HiDPI screens
- Toolbar layout spacing inconsistencies
- Build system linkage errors with Qt resources (`91ac2d4`)
- Resource file compilation order

### Technical Details
- New files: `IconFactory.cpp/h`
- Modified: `ToolbarFactory.cpp`, `resources.qrc`
- Icon directory: `resources/icons/`
- ~60 PNG icon files added
- CMake resource compilation with `qt_add_resources()`

---

## [0.5.0] - 2026-01-20

**Milestone**: Qt6 GUI Editor Foundation

This major release introduces LambdaEditor, a complete Qt6-based graphical user interface. The project now builds two executables: Lambda (CLI) and LambdaEditor (GUI).

### Added

#### Qt6 Editor Application
- **Main Window** (`MainWindow.cpp/h`)
  - Professional dock-based layout
  - Multi-tab model viewing support
  - Central OpenGL viewport
  - Resizable dock panels
  - Window state persistence

- **Model Viewport** (`ModelViewport.cpp/h`)
  - QOpenGLWidget integration
  - Full C backend rendering in Qt context
  - Mouse interaction (rotation, zoom, pan)
  - Keyboard input forwarding
  - Multi-viewport support with independent state
  - Instance-based rendering (`r_qt_instance_t`)

- **Console Widget** (`ConsoleWidget.cpp/h`)
  - Styled text output panel
  - Color-coded message types
  - Auto-scroll to latest messages
  - Read-only QTextEdit implementation
  - Scrollbar styling
  - Maximum line limit with cleanup
  - Timestamp display option

- **Log Widget** (`LogWidget.cpp/h`)
  - Detailed logging panel
  - Log level filtering (TRACE, DEBUG, INFO, WARN, ERROR, FATAL)
  - Category filtering
  - Search/filter functionality
  - Log file path display
  - Auto-scroll toggle
  - Clear log button
  - Export to file option

- **Status Bar Widget** (`StatusBarWidget.cpp/h`)
  - Real-time FPS counter with averaging
  - CPU usage percentage display
  - GPU renderer and vendor information
  - RAM usage monitoring
  - Grid state indicator (on/off, size)
  - Camera position display (X, Y, Z, distance)
  - Current sequence name and frame
  - Model vertex/triangle counts
  - Styled section separators

- **Theme Manager** (`ThemeManager.cpp/h`)
  - Centralized theme management
  - Dark theme implementation
  - Color palette definitions
  - Font configuration (family, sizes)
  - Widget-specific styling
  - Stylesheet generation

- **Menu System** (`MenuFactory.cpp/h`)
  - **File Menu**: New, Open Model, Open Recent, Close, Exit
  - **Edit Menu**: Preferences, Reset Settings
  - **View Menu**: Toggle Console, Toggle Log, Toggle Inspector, Fullscreen
  - **Model Menu**: Model Info, Bodyparts, Sequences
  - **Render Menu**: Textured, Wireframe, Points, Lighting
  - **Tools Menu**: Screenshot, Export Textures
  - **Debug Menu**: Show Bones, Show Hitboxes, Show Normals
  - **Help Menu**: Documentation, Keyboard Shortcuts, About, About Qt
  - Recent files submenu with history

- **Toolbar System** (`ToolbarFactory.cpp/h`)
  - **Main Toolbar**: File operations, common actions
  - **Animation Toolbar**: Play, Pause, Stop, Loop, Frame stepping
  - **View Toolbar**: Render modes, visualization toggles
  - **Camera Toolbar**: Reset, preset views
  - Toolbar visibility toggles
  - Movable/dockable toolbars

- **Bridge Layer**
  - **Console Bridge** (`ConsoleBridge.cpp/h`)
    - C printf/fprintf capture to Qt console
    - Message formatting and routing
    - Thread-safe message queue
  - **Logger Bridge** (`LoggerBridge.cpp/h`)
    - C logger integration with Qt log widget
    - Log level translation
    - Category preservation

- **Editor Configuration** (`EditorConfig.h`)
  - User preferences structure
  - UI configuration (fonts, colors, visibility)
  - Viewport settings (background, grid, FOV, clip planes)
  - Render configuration (VSync, MSAA, default modes)
  - Animation settings (speed, looping, interpolation)
  - Recent files management
  - JSON serialization support

- **Flow Layout** (`FlowLayout.cpp/h`)
  - Custom Qt layout for flowing widgets
  - Used in toolbar and panel layouts
  - Responsive to window resizing

- **Texture View Widget** (`TextureViewWidget.cpp/h`)
  - Full-size texture display
  - Zoom and pan controls
  - Texture information overlay
  - Export functionality

#### Rendering Integration
- `r_qt_create_instance()` - Create per-viewport render instance
- `r_qt_destroy_instance()` - Cleanup viewport instance
- `r_qt_set_model_data()` - Load model into viewport
- `r_qt_render_with_matrices()` - Render with Qt-provided matrices
- `r_qt_set_animation_state()` - Control animation per-viewport
- `r_qt_set_lighting_enabled()` - Per-viewport lighting control
- `r_qt_set_wireframe_overlay()` - Wireframe mode toggle
- Instance-based state management for multi-tab support

#### New Shaders for Qt
- `axes.vert/frag` - Coordinate axes rendering
- `ground.frag` - Ground plane with grid

### Changed
- CMakeLists.txt restructured for dual-target build
- Source organization with `src/editor/` directory
- Header include paths updated
- Build system detects Qt6 availability
- Conditional compilation of GUI components

### Technical Details
- **Qt6 Components**: Core, Widgets, Gui, OpenGL, OpenGLWidgets
- **Qt Build Features**: AUTOMOC, AUTORCC, AUTOUIC
- **New Entry Point**: `src/editor/main.cpp`
- **New Directories**: `editor/`, `editor/panels/`, `editor/widgets/`, `editor/bridge/`, `editor/menus/`, `editor/toolbars/`, `editor/theme/`
- ~25 new C++ source files
- CMake minimum version: 3.15
- Qt6 minimum version: 6.2

### Build Targets
| Target | Description |
|--------|-------------|
| Lambda | CLI model viewer (C) |
| LambdaEditor | Qt6 GUI editor (C++) |

---

## [0.4.0] - 2025-11-22

**Milestone**: Audio System, Skin Families, and Logger Integration

This release adds event-driven audio playback for animation sounds, complete skin family support, and comprehensive logger integration throughout the audio and animation subsystems.

### Added

#### Audio System (`mdl_audio.c/h`)
- **Audio Engine**
  - Complete implementation using miniaudio library
  - Cross-platform audio backend (macOS, Linux, Windows)
  - WAV file format support
  - Mono and stereo playback
  - Sample rate conversion

- **Animation Event Sound Playback**
  - Event-driven sound triggering (event codes 1000+)
  - Frame-accurate sound synchronization
  - Automatic sound stopping on sequence change
  - Event option parsing for sound file paths

- **Sound Search System**
  - Multiple search path support (MAX_SEARCH_PATHS: 10)
  - Automatic sound directory detection from model path
  - Valve directory fallback (valve_hd to valve)
  - Case-insensitive file extension handling
  - Parent directory traversal for sound folder detection

- **Sound Variants**
  - Numbered sound file support (sound1.wav, sound2.wav)
  - Random variant selection
  - Fallback to base sound name

- **Audio Controls**
  - Audio toggle (M key)
  - Per-model audio state
  - Global audio enable/disable

#### Skin Family System
- **Complete Implementation**
  - Skin reference table lookup
  - Multiple skin family support
  - Per-mesh texture assignment based on family
  - Family index validation

- **Controls**
  - Next skin family (UP arrow)
  - Previous skin family (DOWN arrow)
  - Current skin display (B key shows info)
  - Skin family count display

- **Renderer Integration**
  - `next_skin_family()` / `prev_skin_family()`
  - `get_current_skin_family()` / `get_num_skin_families()`
  - Texture rebinding on family change

#### Enhanced Bodypart Controls
- Bodypart cycling ([ and ] keys)
- Submodel cycling within bodypart (- and = keys)
- Reset all bodyparts (BACKSPACE key)
- Bodypart info display (B key)
- Body value calculation from selections

#### Logger Integration
- **Audio Logging**
  - Audio engine initialization messages
  - Sound playback notifications
  - Missing sound warnings
  - Path search debugging
  - Error reporting

- **Animation Logging**
  - Sequence change notifications
  - Event processing messages
  - Frame boundary detection
  - Sequence group errors

- **Log Categories**
  - `audio` - All audio subsystem messages
  - `animation` - Animation and event messages
  - Clean filtering with `--quiet` flag

### Changed
- Input handler extended with skin and audio controls
- Animation system refactored for event processing
- Texture binding updated for skin family support
- Debug output converted to structured logging

### Fixed
- **Audio Fallback**
  - Missing sounds for valve_hd models now found via fallback
  - Case sensitivity issues with .WAV/.wav extensions
  - Directory separator handling (/ vs \)

- **Sound Path Detection**
  - Filesystem root detection edge cases
  - Path construction with proper separators
  - Search depth limiting

- **Event Processing**
  - Sentence events (non-.wav) no longer cause errors
  - Event spam on looping animations eliminated
  - Frame crossing detection improved
  - Boundary event triggering fixed

### Performance
- Printf blocking I/O removed from audio hot path
- Verbose messages moved to DEBUG level
- Console spam reduced during playback
- Logger buffering for better performance

### Technical Details
- **New Dependency**: miniaudio (header-only, bundled in `external/`)
- **New Files**: `mdl_audio.c/h`
- **Modified Files**: `input_handler.c`, `r_draw.c/h`, `mdl_animations.c`
- **Version Header**: Added `HLMV_HAS_AUDIO` flag
- **Audio Architecture**:
  - miniaudio device initialization
  - Callback-based audio streaming
  - Sound handle management
  - Event-to-sound mapping

### Known Issues
- Audio supports WAV format only (no MP3/OGG)
- Sound directory detection limited to 10 parent levels
- Mono playback only (no 3D positional audio)

---

## [0.3.0] - 2025-11-07

**Milestone**: Math Library Completion and Performance Optimization

This release introduces a complete centralized math library wrapping CGLM, fixes critical animation bugs, and dramatically improves performance by removing excessive logging from hot paths.

### Added

#### Math Library System (`math/`)
- **Matrix Operations** (`math_matrix.c/h`)
  - `Math_Mat4_Rotate()` - Rotation around arbitrary axis
  - `Math_Mat4_LookAt()` - View matrix generation
  - `Math_Mat4_Perspective()` - Perspective projection
  - `Math_Mat4_Ortho()` - Orthographic projection
  - `Math_Mat4_Identity()` - Identity matrix
  - `Math_Mat4_Multiply()` - Matrix multiplication
  - `Math_Mat4_Translate()` - Translation matrix
  - `Math_Mat4_Scale()` - Scale matrix

- **Vector Operations** (`math_vector.c/h`)
  - `Math_Vec3Copy()`, `Math_Vec4Copy()`
  - `Math_Vec3Add()`, `Math_Vec3Sub()`
  - `Math_Vec3Scale()`, `Math_Vec3Normalize()`
  - `Math_Vec3Dot()`, `Math_Vec3Cross()`
  - `Math_Vec3Length()`, `Math_Vec3Distance()`

- **Quaternion Operations** (`math_quaternion.c/h`)
  - `Math_QuaternionSlerp()` - Spherical interpolation
  - `Math_QuaternionMatrix3x4()` - Convert to matrix
  - `Math_QuaternionFromAngles()` - Euler to quaternion
  - `Math_QuaternionNormalize()`
  - `Math_QuaternionMultiply()`

- **Angle Utilities** (`math_angles.c/h`)
  - `Math_AngleVectors()` - Angles to direction vectors
  - `Math_VectorAngles()` - Direction to angles
  - `Math_AngleMod()` - Angle normalization
  - Degree/radian conversions

- **Math Utilities** (`math_utils.c/h`)
  - `Math_Clamp()`, `Math_Lerp()`
  - `Math_Min()`, `Math_Max()`
  - `Math_Abs()`, `Math_Sign()`

### Changed

#### Renderer Refactoring
- All `glm_*` calls replaced with `Math_*` equivalents
- `r_draw.c`: 7 OpenGL operations updated
  - `glm_vec3_copy()` -> `Math_Vec3Copy()`
  - `glm_mat4_identity()` -> `Math_Mat4_Identity()`
  - `glm_rotate()` -> `Math_Mat4_Rotate()`
  - `glm_lookat()` -> `Math_Mat4_LookAt()`
  - `glm_perspective()` -> `Math_Mat4_Perspective()`
- `r_camera.c`: Updated to use Math library
- Centralized linear algebra through single API

#### Logging Standards
- ERROR: Critical failures only
- WARN: Important non-fatal issues
- INFO: High-level operation summaries
- DEBUG: Detailed state (removed from hot paths)
- TRACE: Ultra-verbose (removed from frame loops)

### Fixed

#### Animation System
- **Quaternion Interpolation**
  - Fixed SLERP implementation for smooth blending
  - Corrected `Math_QuaternionMatrix3x4()` output format
  - Fixed matrix row/column order
- **Bone Transformations**
  - Hierarchy traversal order corrected
  - Parent-child transformation chain fixed
  - Animation artifacts eliminated
- **Matrix Conversion**
  - 3x4 to 4x4 matrix conversion fixed
  - Proper bone matrix composition

### Performance

#### Logging Optimization
- **Removed from Hot Paths**
  - All `fflush(stdout)` calls eliminated
  - ~150 per-bone log statements removed from `SetUpBones()`
  - Per-frame TRACE messages removed from `render_loop()`
  - Frame count logging removed (was every 60 frames)
  - `ProcessModelForRendering()` logging cleaned

- **Metrics**
  - `SetUpBones()`: 74 lines reduced to 47 lines (36% reduction)
  - For 128-bone models: ~150 log calls eliminated per setup
  - 5 `fflush()` blocking calls removed per bone setup
  - 10+ TRACE messages removed per render frame
  - Estimated 30-50% CPU reduction from logging removal
  - Log file size dramatically reduced

### Technical Details
- **Modified Files**:
  - `math_matrix.c/h` - 3 new function declarations + implementations
  - `r_draw.c` - Complete Math library refactoring
  - `r_camera.c` - Camera transformation updates
  - `mdl_bones.c` - Massive logging cleanup
  - `mdl_animations.c` - Quaternion bug fixes
- **Architecture**: Math library as single abstraction over CGLM
- **Code Coverage**: 100% of renderer, camera, bone code uses Math library
- **Naming Convention**: `Math_<Type>_<Operation>()`

### Compatibility
- Tested: macOS Tahoe 26.0.1
- Tested: Arch Linux Kernel 6.17.3
- CMake 3.15+
- C11 standard compliance

### Known Issues
- Unused variable warnings (non-critical)
- CGLM const qualifier warnings (cosmetic)
- GNU extension warnings for `##__VA_ARGS__`

---

## [0.2.0-alpha.1] - 2025-10-15

**Milestone**: Linux Platform Support

This release adds Linux platform support with proper OpenGL initialization handling for different platforms.

### Added

#### Cross-Platform Support
- **Linux Platform** (Arch Linux tested)
  - GLEW initialization on Linux
  - X11 window system support
  - Platform-specific OpenGL loading

- **Platform Header** (`r_gl_platform.h`)
  - OS detection macros
  - Architecture detection
  - Conditional OpenGL header includes
  - GLEW vs native GL/gl3.h handling

### Fixed
- **Segmentation Faults**
  - GLEW initialization on Linux (was causing crashes)
  - OpenGL function pointer loading
  - Context creation order

- **Build System**
  - CMakeLists.txt Linux dependencies
  - GLEW library detection
  - Include path configuration

### Changed
- GLEW conditionally enabled (Linux/Windows only)
- macOS uses native OpenGL framework
- Platform detection in `init_renderer()`

### Technical Details
- **New File**: `src/r/r_gl_platform.h`
- **Modified**: `CMakeLists.txt`, `r_draw.c`
- **Platform Macros**: `__APPLE__`, `__linux__`, `_WIN32`
- **Tested Platforms**:
  - macOS Tahoe 26.0.1 (Apple Silicon, Intel)
  - Arch Linux Kernel 6.17.3

---

## [0.1.1-alpha.1] - 2025-10-15

**Milestone**: Logging System Refinement

Minor release focusing on logging verbosity improvements.

### Fixed
- **Texture Logging Spam**
  - Texture creation logs moved from DEBUG to TRACE
  - Log file size reduced (11MB+ to manageable)
  - `-vv` now shows useful debug info without texture spam

### Changed
- **Verbosity Levels**
  - `-v`: Standard verbose output
  - `-vv`: Debug information (no texture spam)
  - `--trace`: Full texture creation logging
- Better separation between debug levels

### Technical Details
- **Modified**: `src/r/r_textures.c` (line 299)
- Changed `LOG_DEBUGF` to `LOG_TRACEF` for texture success messages
- Build number: 2

---

## [0.1.0-alpha.1] - 2025-10-10

**Milestone**: Initial Public Release

First alpha release of Lambda with complete Half-Life MDL viewing capabilities.

### Added

#### Core MDL Loading (`mdl/`)
- **File Loading** (`mdl_loader.c/h`)
  - MDL file reading and validation
  - Magic number verification (IDST)
  - Version checking (v10 for HL1)
  - Memory-mapped file loading
  - Error handling and reporting

- **Model Structure Parsing**
  - `studiohdr_t` main header parsing
  - `mstudiobone_t` bone array
  - `mstudiobonecontroller_t` controllers
  - `mstudiohitbox_t` hitbox data
  - `mstudioseqdesc_t` sequences
  - `mstudioseqgroup_t` sequence groups
  - `mstudiotexture_t` texture info
  - `mstudiobodyparts_t` bodyparts
  - `mstudiomodel_t` submodels
  - `mstudiomesh_t` mesh data
  - `mstudioattachment_t` attachments

- **External File Support**
  - Companion texture files (modelT.mdl)
  - Sequence groups (model01.mdl, model02.mdl, etc.)
  - Automatic companion file detection

- **Model Information** (`mdl_info.c/h`)
  - `print_model_info()` - Summary output
  - `print_texture_info()` - Texture details
  - `print_bodypart_info()` - Bodypart structure
  - `print_bone_info()` - Bone hierarchy
  - `print_sequence_info()` - Animation list

#### Rendering System (`r/`)
- **Renderer** (`r_draw.c/h`)
  - OpenGL 3.3+ core profile
  - VAO/VBO mesh rendering
  - Shader program management
  - Texture binding and sampling
  - Wireframe mode (F key)
  - Point rendering mode (P key)
  - Depth testing and face culling
  - Background color (dark gray)

- **Shader System** (`shaders/shader.c/h`)
  - Vertex shader compilation
  - Fragment shader compilation
  - Program linking
  - Uniform location caching
  - Shader file loading

- **Shaders** (`shaders/`)
  - `basic.vert/frag` - Simple rendering
  - `textured.vert/frag` - Textured model rendering
  - `debug.vert` - Debug visualization
  - `grid.vert/frag` - Ground grid

- **Textures** (`r_textures.c/h`)
  - 8-bit indexed color conversion
  - Palette extraction (256 colors)
  - RGB texture generation
  - OpenGL texture upload
  - Texture flag handling (chrome, fullbright, masked)

- **Camera** (`r_camera.c/h`)
  - Orbital camera mode
  - Distance-based zoom
  - Pitch and yaw rotation
  - Look-at matrix generation
  - Perspective projection
  - Mouse drag rotation
  - Keyboard rotation (WASD)
  - Zoom (Q/E, scroll wheel)
  - Reset (R key)

- **Grid** (`r_grid.c/h`)
  - Ground plane rendering
  - Grid line drawing
  - Configurable grid size

#### Animation System (`mdl/`)
- **Animation Playback** (`mdl_animations.c/h`)
  - Frame interpolation
  - FPS-based timing
  - Bone transformation calculation
  - Quaternion blending
  - Root motion handling

- **Bones** (`mdl_bones.c/h`)
  - Hierarchy traversal
  - Parent-relative transforms
  - World space conversion
  - T-pose calculation
  - Animation pose blending

- **Sequences** (`mdl_sequences.c/h`)
  - Sequence loading
  - External sequence group support
  - Sequence metadata access
  - Activity information

- **Controls**
  - Sequence cycling (LEFT/RIGHT arrows)
  - Play/pause toggle (SPACE)
  - Speed control (UP/DOWN arrows)
  - Frame reset (0 key)
  - Looping toggle (L key)
  - Animation info (I key)

#### Input System (`input/`)
- **Input Handler** (`input_handler.c/h`)
  - Keyboard state tracking
  - Key press vs held detection
  - Mouse button tracking
  - Mouse position delta
  - Scroll wheel input

- **Input Manager** (`input.c/h`)
  - GLFW callback registration
  - Event processing
  - Input state queries

#### Utilities (`util/`)
- **Logger** (`util_logger.c/h`)
  - Log levels: TRACE, DEBUG, INFO, WARN, ERROR, FATAL
  - Category-based logging
  - File output support
  - Console output with colors
  - Timestamp formatting
  - Configurable verbosity

- **Console** (`util_console.c/h`)
  - Colored terminal output
  - Success/info/warn/error prefixes
  - Banner and version display

- **Arguments** (`util_args.c/h`)
  - Command-line parsing
  - Flag handling: `--help`, `--version`
  - `--dump`: Basic model dump
  - `--dump-ex`: Extended dump
  - `--dump-only`: Dump and exit
  - `--quiet`: Suppress output
  - `-v`, `-vv`: Verbosity levels

- **Messages** (`util_messages.c/h`)
  - User-friendly error messages
  - Formatted output helpers

#### Application Core (`cl/`)
- **App Lifecycle** (`cl_app.c/h`)
  - Application state management
  - Main loop control
  - Shutdown handling

- **Initialization** (`cl_app_init.c/h`)
  - GLFW initialization
  - Window creation
  - OpenGL context setup
  - Renderer initialization
  - Model loading

- **Configuration** (`cl_app_config.c/h`)
  - Application settings
  - Default values

#### Documentation
- README.md with build instructions
- CHANGELOG.md (this file)
- ROADMAP.md development plan
- LICENSE (MIT)

### Technical Details
- **Language**: C11 standard
- **Build System**: CMake 3.15+
- **Dependencies**:
  - GLFW 3.x (windowing)
  - OpenGL 3.3+ (rendering)
  - CGLM (math, header-only)
- **Architectures**: x86_64, ARM64
- **Compiler Support**: Clang, GCC, MSVC

### Performance
- Model loading: < 100ms typical
- Rendering: 60+ FPS on integrated graphics
- Memory: ~20-50 MB per model
- Bone limit: 128 bones
- Efficient sequence group handling

### Compatibility
- **Supported Formats**:
  - Half-Life 1 Studio Models (IDST v10)
  - Texture companion files (T.mdl)
  - Sequence groups (01.mdl, 02.mdl)
- **Tested Models**:
  - Half-Life 1 original models
  - Counter-Strike 1.6 models
  - Custom community models

### Known Limitations
- No graphical user interface (CLI only)
- No model editing capabilities
- No export functionality
- No hitbox/bone visualization
- No animation blending
- No undo/redo system

---

## Development History (2025)

### Project Genesis (January - February 2025)

The Lambda project began as an effort to create a modern, cross-platform Half-Life model viewer using official Valve SDK structures.

#### Initial Setup
- Repository initialization with MIT license (`37300ee`)
- Project structure: `src/`, `shaders/`, `docs/`
- CMake build system configuration
- GLFW window management integration
- First OpenGL window rendered (`eaf9a8b`)

#### MDL Format Research
- Studied Valve SDK `studio.h` header
- Documented MDL file structure
- Implemented file reading with validation (`0905b56`)
- First successful header parsing (`98b95c5`)

### MDL Parser Development (February - March 2025)

#### Structure Implementation
- `studiohdr_t` main header (`c73f07e`)
- Bone hierarchy parsing (`4af4ae1`)
- Bodypart and submodel parsing (`c4f2864`, `15ec561`)
- Texture structure parsing (`0236ed9`)
- Animation sequence parsing (`debe76e`)

#### Vertex Data Extraction
- Triangle command parsing (`e6e32ac`)
- Vertex coordinate extraction (`9e4d64c`)
- UV coordinate mapping
- Normal vector extraction

#### Major Milestones
- Complete model data extraction (`b47e764`)
- All vertices visible in debugger
- Ready for rendering phase

### Rendering Pipeline (March - April 2025)

#### OpenGL Implementation
- Shader loading and compilation (`c373e92`)
- VAO/VBO setup for model data (`b97348c`)
- First triangle rendering (`bd58cf9`)
- Textured rendering implementation

#### Texture System
- Palette extraction from MDL
- RGB conversion from indexed color
- OpenGL texture upload
- Texture coordinate mapping

#### Camera System
- Orbital camera implementation
- Mouse drag rotation
- Keyboard controls (WASD, QE)
- Zoom functionality

### Animation System (April - May 2025)

#### Bone Transformations
- Bone hierarchy traversal
- Parent-child matrix chain
- Animation frame interpolation
- Quaternion rotation blending

#### Playback Controls
- Sequence selection
- Play/pause functionality
- Speed control
- Looping toggle

### Polish and Debugging (May - October 2025)

#### Bug Fixes
- Triangle indices correction (`5adfd4f`)
- Mesh rendering issues (`141215b`)
- Animation coordinate system
- Model positioning

#### Performance
- Logging optimization
- Hot path cleanup
- Memory management

### Alpha Release (October 2025)

- v0.1.0-alpha.1: Initial public release
- v0.1.1-alpha.1: Logging improvements
- v0.2.0-alpha.1: Linux support
- v0.3.0: Math library and optimization
- v0.4.0: Audio system

---

## Version History Summary

| Version | Date | Build | Commits | Highlights |
|---------|------|-------|---------|------------|
| 0.7.0 | 2026-03-16 | 16 | 588 | Inspector panels, rotation gizmos, compass |
| 0.6.0 | 2026-02-15 | 14 | ~500 | Toolbar icons, resource system |
| 0.5.0 | 2026-01-20 | 12 | ~450 | Qt6 GUI editor, console, log widget, status bar |
| 0.4.0 | 2025-11-22 | 8 | ~350 | Audio system, skin families, logger integration |
| 0.3.0 | 2025-11-07 | 6 | ~300 | Math library, performance optimization |
| 0.2.0-alpha.1 | 2025-10-15 | 4 | ~250 | Linux platform support |
| 0.1.1-alpha.1 | 2025-10-15 | 2 | ~240 | Logging improvements |
| 0.1.0-alpha.1 | 2025-10-10 | 1 | ~230 | Initial release |

---

## Upgrade Guide

### Upgrading to 0.7.0
- No breaking changes from 0.6.0
- Inspector panel replaces simple property display
- New keyboard shortcuts for panel toggles available

### Upgrading to 0.5.0+
- **New Executable**: LambdaEditor (Qt6 GUI)
- Original Lambda CLI still available and unchanged
- Qt6 6.2+ required for GUI builds
- To build CLI only: `cmake -DQt6_DIR="" ..`

### Upgrading to 0.4.0
- **New Dependency**: miniaudio (bundled, header-only)
- Audio enabled by default
- Press M to toggle audio on/off
- New controls: UP/DOWN for skin families

### Upgrading to 0.3.0
- Math library now required (bundled)
- No API changes for external code
- Performance significantly improved

### Upgrading to 0.2.0
- **Linux Users**: Install GLEW
  - Arch: `sudo pacman -S glew`
  - Debian/Ubuntu: `sudo apt install libglew-dev`
- macOS users: No changes required

---

## Contributors

**Author and Maintainer**
- Karlo Siric ([@KarloSiric](https://github.com/KarloSiric))

---

## Acknowledgments

### Technical Reference
- **Sam Vanheer** - Half-Life Asset Manager (HLAM) provided invaluable reference for MDL format understanding, UI patterns, and implementation approaches
- **Valve Corporation** - Half-Life SDK and official studio.h structures
- **Id Software** - Id Technology foundation

### Inspiration
- Hammer Editor / WorldCraft
- TrenchBroom
- GtkRadiant / NetRadiant
- J.A.C.K. (Jackhammer)
- Milkshape 3D

### Libraries
- GLFW - Window and input management
- GLEW - OpenGL extension loading
- CGLM - Mathematics library
- Qt6 - GUI framework
- miniaudio - Audio playback

### Community
- Half-Life Modding Community
- TWHL (The Whole Half-Life)
- Sven Co-op Team

---

## Links

- **Repository**: [https://github.com/KarloSiric/Lambda](https://github.com/KarloSiric/Lambda)
- **Issues**: [https://github.com/KarloSiric/Lambda/issues](https://github.com/KarloSiric/Lambda/issues)
- **Roadmap**: [ROADMAP.md](ROADMAP.md)
- **License**: [LICENSE](LICENSE) (MIT)

---

*Lambda - Professional Half-Life Studio Model Viewer and Editor*
