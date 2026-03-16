# Lambda Model Editor - API Reference

Complete reference for all public headers, types, and functions.

**Version:** 0.7.0
**Last Updated:** March 2026

---

## Table of Contents

- [1. Core Headers](#1-core-headers)
  - [studio.h - MDL Format Structures](#studioh---mdl-format-structures)
  - [platform.h - Platform Detection](#platformh---platform-detection)
  - [version.h - Build Information](#versionh---build-information)
- [2. Application Layer (cl/)](#2-application-layer-cl)
  - [cl_app.h - Application Lifecycle](#cl_apph---application-lifecycle)
  - [cl_app_init.h - Initialization](#cl_app_inith---initialization)
  - [cl_app_config.h - Configuration](#cl_app_configh---configuration)
- [3. Model System (mdl/)](#3-model-system-mdl)
  - [mdl_loader.h - Model Loading](#mdl_loaderh---model-loading)
  - [mdl_animations.h - Animation System](#mdl_animationsh---animation-system)
  - [mdl_bones.h - Bone Transformations](#mdl_bonesh---bone-transformations)
  - [mdl_textures.h - Texture Management](#mdl_texturesh---texture-management)
  - [mdl_sequences.h - Sequence Data](#mdl_sequencesh---sequence-data)
  - [mdl_bodypart.h - Body Part System](#mdl_bodyparth---body-part-system)
  - [mdl_hitboxes.h - Hitbox Data](#mdl_hitboxesh---hitbox-data)
  - [mdl_attachments.h - Attachment Points](#mdl_attachmentsh---attachment-points)
  - [mdl_bonecontrollers.h - Bone Controllers](#mdl_bonecontrollersh---bone-controllers)
  - [mdl_bounds.h - Bounding Boxes](#mdl_boundsh---bounding-boxes)
  - [mdl_audio.h - Sound Events](#mdl_audioh---sound-events)
  - [mdl_info.h - Model Information](#mdl_infoh---model-information)
  - [mdl_report.h - Debug Reporting](#mdl_reporth---debug-reporting)
  - [mdl_stats.h - Statistics](#mdl_statsh---statistics)
- [4. Rendering System (r/)](#4-rendering-system-r)
  - [r_draw.h - Main Renderer](#r_drawh---main-renderer)
  - [r_camera.h - Camera System](#r_camerah---camera-system)
  - [r_textures.h - OpenGL Textures](#r_texturesh---opengl-textures)
  - [r_grid.h - Grid Rendering](#r_gridh---grid-rendering)
  - [r_compass.h - Compass Widget](#r_compassh---compass-widget)
  - [r_gizmo.h - Transform Gizmos](#r_gizmoh---transform-gizmos)
  - [r_gl_platform.h - OpenGL Abstraction](#r_gl_platformh---opengl-abstraction)
- [5. Math Library (math/)](#5-math-library-math)
  - [math_types.h - Type Definitions](#math_typesh---type-definitions)
  - [math_vector.h - Vector Operations](#math_vectorh---vector-operations)
  - [math_matrix.h - Matrix Operations](#math_matrixh---matrix-operations)
  - [math_quaternion.h - Quaternion Math](#math_quaternionh---quaternion-math)
  - [math_angles.h - Angle Conversions](#math_anglesh---angle-conversions)
  - [math_utils.h - Math Utilities](#math_utilsh---math-utilities)
- [6. Input System (input/)](#6-input-system-input)
  - [input.h - Input Handling](#inputh---input-handling)
  - [input_types.h - Input Enumerations](#input_typesh---input-enumerations)
  - [input_handler.h - Event Processing](#input_handlerh---event-processing)
- [7. Utilities (util/)](#7-utilities-util)
  - [util_logger.h - Logging System](#util_loggerh---logging-system)
  - [util_args.h - Argument Parsing](#util_argsh---argument-parsing)
  - [util_console.h - Console Interface](#util_consoleh---console-interface)
  - [util_utils.h - General Utilities](#util_utilsh---general-utilities)
  - [util_messages.h - Message Definitions](#util_messagesh---message-definitions)
  - [util_logger_categories.h - Log Categories](#util_logger_categoriesh---log-categories)
- [8. Shader System (shaders/)](#8-shader-system-shaders)
  - [shader.h - Shader Management](#shaderh---shader-management)
- [9. Editor GUI (editor/)](#9-editor-gui-editor)
  - [MainWindow.h - Main Application Window](#mainwindowh---main-application-window)
  - [EditorConfig.h - Editor Configuration](#editorcofnigh---editor-configuration)
  - [Panels](#panels)
  - [Widgets](#widgets)
  - [Bridge Classes](#bridge-classes)
- [10. Third-Party Libraries](#10-third-party-libraries)

---

## 1. Core Headers

### studio.h - MDL Format Structures

**Location:** `src/studio.h`

Official Valve Half-Life Studio Model format definitions. Based on Valve SDK headers (Version 10).

#### Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `STUDIO_VERSION` | 10 | GoldSrc MDL version |
| `IDSTUDIOHEADER` | "IDST" | Magic number for MDL files |
| `IDSEQGRPHEADER` | "IDSQ" | Magic number for sequence groups |
| `MAXSTUDIOBONES` | 128 | Maximum bones per model |
| `MAXSTUDIOSEQUENCES` | 2048 | Maximum animation sequences |
| `MAXSTUDIOSKINS` | 100 | Maximum textures |
| `MAXSTUDIOBODYPARTS` | 32 | Maximum body parts |
| `MAXSTUDIOVERTS` | 2048 | Maximum vertices per mesh |
| `MAXSTUDIOTRIANGLES` | 20000 | Maximum triangles |

#### Types

```c
typedef float vec3_t[3];
typedef float vec4_t[4];
typedef unsigned char byte;
```

#### Structures

| Structure | Description |
|-----------|-------------|
| `studiohdr_t` | Main MDL file header (117 bytes) |
| `studioseqhdr_t` | Sequence group file header |
| `mstudiobodyparts_t` | Body part definition |
| `mstudiomodel_t` | Sub-model within a body part |
| `mstudiomesh_t` | Mesh geometry data |
| `mstudiotrivert_t` | Triangle vertex (position, normal, UV) |
| `mstudiotexture_t` | Texture definition |
| `mstudiobone_t` | Bone in skeleton hierarchy |
| `mstudiobonecontroller_t` | Bone controller for procedural animation |
| `mstudiobbox_t` | Hit box for collision |
| `mstudioseqdesc_t` | Animation sequence descriptor |
| `mstudioevent_t` | Animation event (sound, effect triggers) |
| `mstudioanim_t` | Animation data offsets |
| `mstudioanimvalue_t` | Compressed animation keyframe |
| `mstudiopivot_t` | Pivot point |
| `mstudioattachment_t` | Attachment point (weapons, effects) |
| `mstudioseqgroup_t` | External sequence group reference |

#### Texture Flags

| Flag | Value | Description |
|------|-------|-------------|
| `STUDIO_NF_FLATSHADE` | 0x0001 | Flat shading |
| `STUDIO_NF_CHROME` | 0x0002 | Chrome/environment mapping |
| `STUDIO_NF_FULLBRIGHT` | 0x0004 | Ignore lighting |
| `STUDIO_NF_NOMIPS` | 0x0008 | No mipmaps |
| `STUDIO_NF_ALPHA` | 0x0010 | Alpha transparency |
| `STUDIO_NF_ADDITIVE` | 0x0020 | Additive blending |
| `STUDIO_NF_MASKED` | 0x0040 | Masked transparency (index 255) |

---

### platform.h - Platform Detection

**Location:** `src/platform.h`

Platform-specific macros and includes.

#### Macros

| Macro | Description |
|-------|-------------|
| `PLATFORM_MACOS` | Defined on macOS |
| `PLATFORM_LINUX` | Defined on Linux |
| `PLATFORM_WINDOWS` | Defined on Windows |

---

### version.h - Build Information

**Location:** `src/version.h` (Generated by CMake)

Build-time version information.

#### Macros

| Macro | Description |
|-------|-------------|
| `HLMV_VERSION_MAJOR` | Major version number |
| `HLMV_VERSION_MINOR` | Minor version number |
| `HLMV_VERSION_PATCH` | Patch version number |
| `HLMV_VERSION_STRING` | Full version string (e.g., "0.7.0") |
| `HLMV_BUILD_DATE` | Compilation date |
| `HLMV_BUILD_TIME` | Compilation time |
| `HLMV_GIT_COMMIT` | Git commit hash |
| `HLMV_GIT_BRANCH` | Git branch name |

---

## 2. Application Layer (cl/)

### cl_app.h - Application Lifecycle

**Location:** `src/cl/cl_app.h`

Main application state and lifecycle management.

#### Types

```c
typedef struct app_state_t {
    bool running;
    bool initialized;
    GLFWwindow *window;
    int window_width, window_height;
    bool fullscreen;
    app_args_t args;
    mdl_model_t *model;
    // ... camera, animation, rendering state
} app_state_t;
```

#### Functions

| Function | Description |
|----------|-------------|
| `int app_init(app_args_t *args)` | Initialize application with command-line args |
| `int app_run(void)` | Enter main loop |
| `void app_shutdown(void)` | Clean up and exit |
| `app_state_t *app_get_state(void)` | Get global application state |

---

### cl_app_init.h - Initialization

**Location:** `src/cl/cl_app_init.h`

Subsystem initialization functions.

#### Return Codes

| Constant | Value | Description |
|----------|-------|-------------|
| `APP_INIT_SUCCESS` | 0 | Initialization successful |
| `APP_INIT_EXIT_SUCCESS` | 1 | Early exit (--help, --version) |
| `APP_INIT_ERROR` | -1 | Initialization failed |

---

### cl_app_config.h - Configuration

**Location:** `src/cl/cl_app_config.h`

Application configuration constants and defaults.

---

## 3. Model System (mdl/)

### mdl_loader.h - Model Loading

**Location:** `src/mdl/mdl_loader.h`

Core MDL file loading and parsing.

#### Types

```c
typedef struct mdl_seqgroup_blob_t {
    studioseqhdr_t *sequence_header;
    unsigned char *data;
    size_t size;
} mdl_seqgroup_blob_t;

typedef struct mdl_model_t {
    char name[64];
    studiohdr_t *header;
    unsigned char *data;
    studiohdr_t *texture_header;
    unsigned char *texture_data;
    mdl_seqgroup_blob_t *seqgroups;
    int num_seqgroups;
} mdl_model_t;
```

#### Functions

| Function | Description |
|----------|-------------|
| `mdl_model_t *create_mdl_model(void)` | Allocate empty model structure |
| `bool load_model_with_textures(const char *path, mdl_model_t *model)` | Load MDL with textures |
| `void free_model(mdl_model_t *model)` | Free model and all resources |
| `bool validate_mdl_magic(const studiohdr_t *hdr)` | Check "IDST" magic number |
| `bool validate_mdl_version(const studiohdr_t *hdr)` | Check version == 10 |
| `bool parse_mdl_h(const char *path, studiohdr_t **out_hdr, unsigned char **out_data)` | Parse header |
| `void extract_triangles_with_uvs(...)` | Extract renderable geometry |
| `void transform_vertices_to_opengl(...)` | Convert HL coords to OpenGL |

---

### mdl_animations.h - Animation System

**Location:** `src/mdl/mdl_animations.h`

Skeletal animation playback.

#### Types

```c
typedef struct mdl_animation_state_t {
    int current_sequence;
    float current_frame;
    float previous_frame;
    bool is_looping;
} mdl_animation_state_t;
```

#### Functions

| Function | Description |
|----------|-------------|
| `void mdl_animation_init(mdl_animation_state_t *state)` | Initialize animation state |
| `void mdl_animation_set_sequence(state, model, seq_index)` | Set active sequence |
| `void mdl_animation_update(state, model, delta_time)` | Advance animation |
| `void mdl_animation_calculate_bones(state, model, transforms)` | Compute bone matrices |
| `void mdl_animation_process_events(state, model, callback)` | Handle animation events |

---

### mdl_bones.h - Bone Transformations

**Location:** `src/mdl/mdl_bones.h`

Bone hierarchy and vertex skinning.

#### Globals

```c
extern mat3x4_t g_bonetransformations[MAXSTUDIOBONES];
```

#### Functions

| Function | Description |
|----------|-------------|
| `void SetUpBones(model, frame, sequence, controller_values)` | Calculate all bone matrices |
| `void TransformVertices(model, in_verts, out_verts, count)` | Apply bone transforms to vertices |
| `void TransformNormalByBone(bone_idx, in_normal, out_normal)` | Transform normal by bone |

---

### r_textures.h - Texture Management

**Location:** `src/r/r_textures.h`

OpenGL texture loading from MDL data.

#### Types

```c
typedef struct mdl_gl_texture_t {
    GLuint gl_id;
    int width, height;
    char name[64];
    int flags;
} mdl_gl_texture_t;

typedef struct mdl_texture_set_t {
    mdl_gl_texture_t *textures;
    int count;
} mdl_texture_set_t;
```

#### Functions

| Function | Description |
|----------|-------------|
| `studiohdr_t *mdl_pick_texture_header(model)` | Get header containing textures |
| `bool mdl_load_textures(model, texture_set)` | Load all textures to OpenGL |
| `void mdl_free_texture(texture_set)` | Free OpenGL textures |
| `void mdl_pal8_to_rgba(indexed, palette, rgba, pixel_count)` | Convert 8-bit to RGBA |

---

### mdl_sequences.h - Sequence Data

**Location:** `src/mdl/mdl_sequences.h`

Animation sequence information extraction.

---

### mdl_bodypart.h - Body Part System

**Location:** `src/mdl/mdl_bodypart.h`

Body part and sub-model selection.

---

### mdl_hitboxes.h - Hitbox Data

**Location:** `src/mdl/mdl_hitboxes.h`

Collision hitbox extraction and rendering.

---

### mdl_attachments.h - Attachment Points

**Location:** `src/mdl/mdl_attachments.h`

Attachment point (muzzle flash, effects) handling.

---

### mdl_bonecontrollers.h - Bone Controllers

**Location:** `src/mdl/mdl_bonecontrollers.h`

Procedural bone animation (mouth, eyes, etc.).

---

### mdl_bounds.h - Bounding Boxes

**Location:** `src/mdl/mdl_bounds.h`

Model bounding box calculations.

---

### mdl_audio.h - Sound Events

**Location:** `src/mdl/mdl_audio.h`

Animation sound event playback using miniaudio.

---

### mdl_info.h - Model Information

**Location:** `src/mdl/mdl_info.h`

Model metadata extraction.

---

### mdl_report.h - Debug Reporting

**Location:** `src/mdl/mdl_report.h`

Debug output for model data.

---

### mdl_stats.h - Statistics

**Location:** `src/mdl/mdl_stats.h`

Model statistics (vertex counts, memory usage).

---

## 4. Rendering System (r/)

### r_draw.h - Main Renderer

**Location:** `src/r/r_draw.h`

Core OpenGL rendering system.

#### Types

```c
typedef struct DrawRange {
    GLuint tex;
    int first;
    int count;
    int flags;
} DrawRange;

typedef struct r_qt_instance_t {
    GLuint vbo, vao, ebo;
    GLuint shader_program;
    studiohdr_t *header;
    mdl_gl_texture_t *textures;
    mat3x4_t bone_transformations[MAXSTUDIOBONES];
    mdl_animation_state_t anim_state;
    bool animation_enabled;
    DrawRange *ranges;
    int num_ranges;
    // ... uniforms, lighting state
} r_qt_instance_t;
```

#### Core Functions

| Function | Description |
|----------|-------------|
| `bool init_renderer(void)` | Initialize OpenGL context |
| `void cleanup_renderer(void)` | Free renderer resources |
| `void render_loop(void)` | Main render loop (CLI) |
| `void render_model(void)` | Render current model |
| `bool should_close_window(void)` | Check for window close |

#### Model Functions

| Function | Description |
|----------|-------------|
| `void set_model_data(studiohdr_t*, data, tex_hdr, tex_data, seqgroups, num_seq)` | Set model for rendering |
| `void ProcessModelForRendering(void)` | Process geometry to GPU buffers |
| `void UpdateBonesForCurrentFrame(void)` | Update bone matrices |

#### Rendering Control

| Function | Description |
|----------|-------------|
| `void set_wireframe_mode(bool)` | Enable/disable wireframe |
| `void set_current_texture(int)` | Set active texture |
| `void next_skin_family(void)` | Cycle to next skin |
| `void prev_skin_family(void)` | Cycle to previous skin |

#### Qt Integration Functions

| Function | Description |
|----------|-------------|
| `r_qt_instance_t *r_qt_create_instance(void)` | Create renderer instance |
| `void r_qt_destroy_instance(r_qt_instance_t*)` | Destroy instance |
| `void r_qt_set_model_data(instance, ...)` | Set model data |
| `void r_qt_render_with_matrices(instance, view, proj)` | Render with matrices |
| `void r_qt_set_animation_enabled(instance, bool)` | Toggle animation |
| `void r_qt_set_lighting_enabled(instance, bool)` | Toggle lighting |

---

### r_camera.h - Camera System

**Location:** `src/r/r_camera.h`

Orbit camera for model viewing.

#### Types

```c
typedef struct r_camera_t {
    vec3 position;
    vec3 angles_deg;
    vec3 target;
    float distance;
    float movement_speed;
    float rotation_speed;
    float zoom_speed;
    vec3 forward, right, up;
    mat4 view_matrix;
    bool dirty;
} r_camera_t;
```

#### Functions

| Function | Description |
|----------|-------------|
| `void Camera_Init(r_camera_t *cam)` | Initialize camera defaults |
| `void Camera_Rotate(cam, delta_yaw, delta_pitch)` | Rotate around target |
| `void Camera_Zoom(cam, delta)` | Zoom in/out |
| `void Camera_UpdateTransforms(cam)` | Recalculate matrices |
| `mat4 *Camera_GetViewMatrix(cam)` | Get view matrix |

---

### r_grid.h - Grid Rendering

**Location:** `src/r/r_grid.h`

Ground plane grid rendering.

---

### r_compass.h - Compass Widget

**Location:** `src/r/r_compass.h`

3D orientation compass overlay.

---

### r_gizmo.h - Transform Gizmos

**Location:** `src/r/r_gizmo.h`

Transform manipulation gizmos.

---

### r_gl_platform.h - OpenGL Abstraction

**Location:** `src/r/r_gl_platform.h`

Platform-specific OpenGL includes.

```c
#ifdef PLATFORM_MACOS
    #include <OpenGL/gl3.h>
#else
    #include <GL/glew.h>
#endif
```

---

## 5. Math Library (math/)

### math_types.h - Type Definitions

**Location:** `src/math/math_types.h`

Mathematical type definitions using cglm.

```c
typedef float mat3x4_t[3][4];  // 3x4 transformation matrix
// vec3, vec4, mat4 from cglm
```

---

### math_vector.h - Vector Operations

**Location:** `src/math/math_vector.h`

3D vector mathematics.

#### Functions

| Function | Description |
|----------|-------------|
| `void Math_Vec3Copy(src, dst)` | Copy vector |
| `void Math_Vec3Add(a, b, out)` | Vector addition |
| `void Math_Vec3Sub(a, b, out)` | Vector subtraction |
| `void Math_Vec3Scale(v, scale, out)` | Scalar multiplication |
| `float Math_Vec3Dot(a, b)` | Dot product |
| `void Math_Vec3Cross(a, b, out)` | Cross product |
| `float Math_Vec3Length(v)` | Vector length |
| `float Math_Vec3LengthSquared(v)` | Squared length |
| `void Math_Vec3Normalize(v, out)` | Normalize to unit |
| `bool Math_Vec3Compare(a, b)` | Equality check |
| `void Math_Vec3Transform(v, matrix, out)` | Transform by 3x4 matrix |
| `void Math_Vec3TransformMat4(v, matrix, out)` | Transform by 4x4 matrix |

---

### math_matrix.h - Matrix Operations

**Location:** `src/math/math_matrix.h`

Matrix mathematics for 3x4 and 4x4 matrices.

#### Functions

| Function | Description |
|----------|-------------|
| `void Math_Mat3x4_Identity(m)` | Set to identity |
| `void Math_Mat3x4_Copy(src, dst)` | Copy matrix |
| `void Math_Mat3x4_ConcatTransforms(a, b, out)` | Multiply 3x4 matrices |
| `void Math_Mat3x4_FromQuaternionPosition(q, pos, out)` | Build from quat + pos |
| `void Math_Mat4_Identity(m)` | Set 4x4 to identity |
| `void Math_Mat4_Multiply(a, b, out)` | Multiply 4x4 matrices |
| `void Math_Mat3x4_ToMat4(in, out)` | Convert 3x4 to 4x4 |
| `void Math_Mat4_LookAt(eye, center, up, out)` | View matrix |
| `void Math_Mat4_Perspective(fov, aspect, near, far, out)` | Projection matrix |

---

### math_quaternion.h - Quaternion Math

**Location:** `src/math/math_quaternion.h`

Quaternion operations for rotations.

---

### math_angles.h - Angle Conversions

**Location:** `src/math/math_angles.h`

Euler angles and conversions.

---

### math_utils.h - Math Utilities

**Location:** `src/math/math_utils.h`

General math utilities.

#### Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `MATH_PI` | 3.14159... | Pi |
| `MATH_DEG2RAD` | PI/180 | Degrees to radians |
| `MATH_RAD2DEG` | 180/PI | Radians to degrees |
| `MATH_EPSILON` | 0.0001 | Float comparison epsilon |

#### Functions

| Function | Description |
|----------|-------------|
| `float Math_Clamp(val, min, max)` | Clamp to range |
| `float Math_Min(a, b)` | Minimum |
| `float Math_Max(a, b)` | Maximum |
| `float Math_Lerp(a, b, t)` | Linear interpolation |
| `bool Math_FloatEqual(a, b)` | Float equality |

---

## 6. Input System (input/)

### input.h - Input Handling

**Location:** `src/input/input.h`

Keyboard and mouse input handling via GLFW.

#### Functions

| Function | Description |
|----------|-------------|
| `void Input_Init(GLFWwindow *window)` | Register GLFW callbacks |
| `void Input_Update(void)` | Update input state each frame |
| `void Input_Shutdown(void)` | Cleanup |
| `bool Input_IsKeyPressed(int key)` | Key just pressed this frame |
| `bool Input_IsKeyHeld(int key)` | Key currently held |
| `bool Input_IsKeyReleased(int key)` | Key just released this frame |
| `void Input_GetMousePosition(float *x, float *y)` | Get cursor position |
| `void Input_GetMouseDelta(float *dx, float *dy)` | Get cursor movement |
| `bool Input_IsMouseButtonPressed(int button)` | Mouse button pressed |
| `bool Input_IsMouseButtonHeld(int button)` | Mouse button held |
| `float Input_GetScrollDelta(void)` | Get scroll wheel delta |

---

### input_types.h - Input Enumerations

**Location:** `src/input/input_types.h`

Input type definitions and key mappings.

---

### input_handler.h - Event Processing

**Location:** `src/input/input_handler.h`

Game-specific input processing.

---

## 7. Utilities (util/)

### util_logger.h - Logging System

**Location:** `src/util/util_logger.h`

Multi-level logging with file and console output.

#### Log Levels

| Level | Value | Description |
|-------|-------|-------------|
| `LOG_TRACE` | 0 | Verbose tracing |
| `LOG_DEBUG` | 1 | Debug information |
| `LOG_INFO` | 2 | General information |
| `LOG_WARN` | 3 | Warnings |
| `LOG_ERROR` | 4 | Errors |
| `LOG_FATAL` | 5 | Fatal errors |

#### Types

```c
typedef struct log_options_t {
    const char *file_path;
    size_t max_bytes;
    int max_files;
    bool use_colors;
    bool json_lines;
    t_log_level console_level;
} t_log_options;
```

#### Functions

| Function | Description |
|----------|-------------|
| `bool logger_init(t_log_options *options)` | Initialize logger |
| `void logger_shutdown(void)` | Close log files |
| `void logger_set_global_level(level)` | Set minimum log level |
| `void logger_log(level, category, file, line, msg)` | Log message |
| `void logger_logf(level, category, file, line, fmt, ...)` | Formatted log |
| `void logger_hexdump(level, category, data, size)` | Hex dump |
| `bool logger_should_log(level, category)` | Check if would log |

#### Convenience Macros

```c
LOG_TRACE(category, msg)
LOG_DEBUG(category, msg)
LOG_INFO(category, msg)
LOG_WARN(category, msg)
LOG_ERROR(category, msg)
LOG_FATAL(category, msg)

LOG_TRACEF(category, fmt, ...)
LOG_DEBUGF(category, fmt, ...)
LOG_INFOF(category, fmt, ...)
LOG_WARNF(category, fmt, ...)
LOG_ERRORF(category, fmt, ...)
LOG_FATALF(category, fmt, ...)
```

---

### util_args.h - Argument Parsing

**Location:** `src/util/util_args.h`

Command-line argument parsing.

#### Types

```c
typedef enum {
    DUMP_NONE,
    DUMP_BASIC,
    DUMP_EXTENDED
} dump_level_t;

typedef struct app_args_t {
    const char *model_path;
    dump_level_t dump_level;
    bool verbose;
    bool help;
    bool version;
    const char *log_file;
    // ...
} app_args_t;
```

#### Functions

| Function | Description |
|----------|-------------|
| `int parse_args(int argc, char **argv, app_args_t *args)` | Parse arguments |
| `void print_usage(const char *program_name)` | Print help |
| `void print_version_info(void)` | Print version |
| `void print_banner(void)` | Print startup banner |

---

### util_console.h - Console Interface

**Location:** `src/util/util_console.h`

Console/terminal utilities.

---

### util_utils.h - General Utilities

**Location:** `src/util/util_utils.h`

Miscellaneous utility functions.

---

### util_messages.h - Message Definitions

**Location:** `src/util/util_messages.h`

User-facing message strings.

---

### util_logger_categories.h - Log Categories

**Location:** `src/util/util_logger_categories.h`

Log category definitions.

```c
#define LOG_CAT_GENERAL  0
#define LOG_CAT_LOADER   1
#define LOG_CAT_RENDER   2
#define LOG_CAT_ANIM     3
#define LOG_CAT_INPUT    4
#define LOG_CAT_AUDIO    5
```

---

## 8. Shader System (shaders/)

### shader.h - Shader Management

**Location:** `src/shaders/shader.h`

GLSL shader loading and compilation.

---

## 9. Editor GUI (editor/)

Qt6-based graphical editor (C++).

### MainWindow.h - Main Application Window

**Location:** `src/editor/MainWindow.h`

Main editor window with docking panels.

---

### EditorConfig.h - Editor Configuration

**Location:** `src/editor/EditorConfig.h`

User preferences and settings.

---

### Panels

| Header | Description |
|--------|-------------|
| `InspectorPanel.h` | Base class for inspector panels |
| `ModelInfoPanel.h` | Model metadata display |
| `SequencesPanel.h` | Animation sequence browser |
| `TexturesPanel.h` | Texture viewer |
| `BonesPanel.h` | Bone hierarchy viewer |
| `BodypartsPanel.h` | Body part selection |
| `AttachmentsPanel.h` | Attachment point editor |
| `HitboxesPanel.h` | Hitbox editor |
| `BoneControllersPanel.h` | Bone controller sliders |
| `LightingPanel.h` | Lighting controls |
| `ModelDisplayPanel.h` | Render settings |
| `BrowserPanel.h` | File browser |

---

### Widgets

| Header | Description |
|--------|-------------|
| `ModelViewport.h` | OpenGL viewport widget |
| `ConsoleWidget.h` | Log console display |
| `LogWidget.h` | Log output widget |
| `StatusBarWidget.h` | Status bar |
| `TextureViewWidget.h` | Texture preview |
| `FlowLayout.h` | Flow layout manager |

---

### Bridge Classes

| Header | Description |
|--------|-------------|
| `ConsoleBridge.h` | C logger to Qt console bridge |
| `LoggerBridge.h` | Logger Qt integration |

---

## 10. Third-Party Libraries

### GLFW

Window management and OpenGL context creation.
- Website: https://www.glfw.org/
- Documentation: https://www.glfw.org/docs/latest/

### GLEW

OpenGL extension loading (Linux/Windows).
- Website: http://glew.sourceforge.net/

### cglm

High-performance math library (SIMD-optimized).
- Website: https://github.com/recp/cglm
- Documentation: https://cglm.readthedocs.io/

### miniaudio

Single-file audio library.
- Website: https://miniaud.io/
- Documentation: https://miniaud.io/docs/

### Qt6

Cross-platform GUI framework (Editor only).
- Website: https://www.qt.io/
- Documentation: https://doc.qt.io/qt-6/

---

## See Also

- [TECHNICAL_GUIDE.md](TECHNICAL_GUIDE.md) - Comprehensive developer documentation
- [README.md](../README.md) - Project overview
- [CHANGELOG.md](../CHANGELOG.md) - Version history
