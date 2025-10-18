# Lambda Model Editor - Complete API Reference
## Technical Documentation for Developers

---

<div align="center">

**Version:**  0.2.0-alpha.1

**Last Updated:**  January 16, 2025  

**Author:**. Karlo Siric 

**License:**  Valve SDK License (Non-Commercial)

*Complete line-by-line technical reference documenting every function, structure, macro, and system in the Lambda Half-Life Model Editor codebase.*

</div>

---

## Table of Contents

### **PART I: INTRODUCTION & OVERVIEW**

#### **1. [Introduction](#1-introduction)**
- [1.1 About This Document](#11-about-this-document)
  - [1.1.1 Purpose and Scope](#111-purpose-and-scope)
  - [1.1.2 Target Audience](#112-target-audience)
  - [1.1.3 How to Use This Reference](#113-how-to-use-this-reference)
  - [1.1.4 Document Updates and Maintenance](#114-document-updates-and-maintenance)
- [1.2 Document Conventions](#12-document-conventions)
  - [1.2.1 Code Formatting](#121-code-formatting)
  - [1.2.2 Parameter Notation](#122-parameter-notation)
  - [1.2.3 Cross-References](#123-cross-references)
- [1.3 Prerequisites](#13-prerequisites)
  - [1.3.1 Required Knowledge](#131-required-knowledge)
  - [1.3.2 Required Tools](#132-required-tools)
  - [1.3.3 Recommended Reading](#133-recommended-reading)
- [1.4 Getting Help](#14-getting-help)
  - [1.4.1 Community Resources](#141-community-resources)
  - [1.4.2 Issue Reporting](#142-issue-reporting)
  - [1.4.3 Contributing Guidelines](#143-contributing-guidelines)

#### **2. [Half-Life MDL Format Documentation](#2-half-life-mdl-format-documentation)**
- [2.1 Introduction to MDL Files](#21-introduction-to-mdl-files)
  - [2.1.1 What is an MDL File?](#211-what-is-an-mdl-file)
  - [2.1.2 History and Evolution](#212-history-and-evolution)
  - [2.1.3 MDL vs Other 3D Formats](#213-mdl-vs-other-3d-formats)
  - [2.1.4 Use Cases in Half-Life Engine](#214-use-cases-in-half-life-engine)
- [2.2 File Format Specification](#22-file-format-specification)
  - [2.2.1 File Structure Overview](#221-file-structure-overview)
  - [2.2.2 Binary Layout](#222-binary-layout)
  - [2.2.3 Endianness and Platform Considerations](#223-endianness-and-platform-considerations)
  - [2.2.4 Version Differences](#224-version-differences)
- [2.3 MDL File Components](#23-mdl-file-components)
  - [2.3.1 Main Header (studiohdr_t)](#231-main-header-studiohdr_t)
  - [2.3.2 Bones and Skeleton](#232-bones-and-skeleton)
  - [2.3.3 Body Parts and Sub-models](#233-body-parts-and-sub-models)
  - [2.3.4 Meshes and Geometry](#234-meshes-and-geometry)
  - [2.3.5 Textures and Materials](#235-textures-and-materials)
  - [2.3.6 Animations and Sequences](#236-animations-and-sequences)
  - [2.3.7 Bone Controllers](#237-bone-controllers)
  - [2.3.8 Attachments](#238-attachments)
  - [2.3.9 Hit Boxes](#239-hit-boxes)
  - [2.3.10 Events](#2310-events)
  - [2.3.11 Sequence Groups](#2311-sequence-groups)
- [2.4 Coordinate Systems and Transformations](#24-coordinate-systems-and-transformations)
  - [2.4.1 Half-Life Coordinate System](#241-half-life-coordinate-system)
  - [2.4.2 OpenGL Coordinate System](#242-opengl-coordinate-system)
  - [2.4.3 Coordinate Transformation](#243-coordinate-transformation)
  - [2.4.4 Rotation and Scaling](#244-rotation-and-scaling)
- [2.5 Animation System](#25-animation-system)
  - [2.5.1 Skeletal Animation Overview](#251-skeletal-animation-overview)
  - [2.5.2 Bone Hierarchy](#252-bone-hierarchy)
  - [2.5.3 Animation Sequences](#253-animation-sequences)
  - [2.5.4 Frame Data and Interpolation](#254-frame-data-and-interpolation)
  - [2.5.5 Blending and Transitions](#255-blending-and-transitions)
  - [2.5.6 Animation Events](#256-animation-events)
- [2.6 Texture System](#26-texture-system)
  - [2.6.1 Internal vs External Textures](#261-internal-vs-external-textures)
  - [2.6.2 Texture File Format (.mdl T files)](#262-texture-file-format-mdl-t-files)
  - [2.6.3 Palette-Based Colors](#263-palette-based-colors)
  - [2.6.4 UV Mapping](#264-uv-mapping)
  - [2.6.5 Skin Families](#265-skin-families)
- [2.7 Advanced Features](#27-advanced-features)
  - [2.7.1 Level of Detail (LOD)](#271-level-of-detail-lod)
  - [2.7.2 Bone Controllers](#272-bone-controllers)
  - [2.7.3 Attachments and Muzzle Flashes](#273-attachments-and-muzzle-flashes)
  - [2.7.4 Hit Boxes for Collision](#274-hit-boxes-for-collision)
  - [2.7.5 Bounding Boxes](#275-bounding-boxes)
- [2.8 File Dependencies](#28-file-dependencies)
  - [2.8.1 Required Files](#281-required-files)
  - [2.8.2 Optional Files](#282-optional-files)
  - [2.8.3 Sequence Group Files (.mdl##)](#283-sequence-group-files-mdl)
  - [2.8.4 Texture Files (.mdl T)](#284-texture-files-mdl-t)
- [2.9 Creating and Compiling MDL Files](#29-creating-and-compiling-mdl-files)
  - [2.9.1 Source Formats](#291-source-formats)
  - [2.9.2 QC Script Format](#292-qc-script-format)
  - [2.9.3 StudioMDL Compiler](#293-studiomdl-compiler)
  - [2.9.4 Common Compilation Errors](#294-common-compilation-errors)
- [2.10 Tools and Utilities](#210-tools-and-utilities)
  - [2.10.1 Official Valve Tools](#2101-official-valve-tools)
  - [2.10.2 Third-Party Tools](#2102-third-party-tools)
  - [2.10.3 Decompilers](#2103-decompilers)
  - [2.10.4 Model Viewers](#2104-model-viewers)
- [2.11 Common Issues and Troubleshooting](#211-common-issues-and-troubleshooting)
  - [2.11.1 Corrupted Files](#2111-corrupted-files)
  - [2.11.2 Missing Textures](#2112-missing-textures)
  - [2.11.3 Animation Problems](#2113-animation-problems)
  - [2.11.4 Compatibility Issues](#2114-compatibility-issues)
- [2.12 References and Resources](#212-references-and-resources)
  - [2.12.1 Official Documentation](#2121-official-documentation)
  - [2.12.2 Community Resources](#2122-community-resources)
  - [2.12.3 Research Papers](#2123-research-papers)
  - [2.12.4 Source Code References](#2124-source-code-references)

#### **3. [Codebase Architecture Overview](#3-codebase-architecture-overview)**
- [3.1 Project Structure](#31-project-structure)
  - [3.1.1 Directory Layout](#311-directory-layout)
  - [3.1.2 Module Organization](#312-module-organization)
  - [3.1.3 File Naming Conventions](#313-file-naming-conventions)
- [3.2 Dependency Graph](#32-dependency-graph)
  - [3.2.1 External Dependencies](#321-external-dependencies)
  - [3.2.2 Internal Module Dependencies](#322-internal-module-dependencies)
  - [3.2.3 Build System (CMake)](#323-build-system-cmake)
- [3.3 Design Philosophy](#33-design-philosophy)
  - [3.3.1 Why C Instead of C++](#331-why-c-instead-of-c)
  - [3.3.2 Memory Management Strategy](#332-memory-management-strategy)
  - [3.3.3 Error Handling Approach](#333-error-handling-approach)
  - [3.3.4 Platform Abstraction](#334-platform-abstraction)

#### **4. [Program Execution Flow](#4-program-execution-flow)**
- [4.1 Application Lifecycle](#41-application-lifecycle)
  - [4.1.1 Startup Phase](#411-startup-phase)
  - [4.1.2 Runtime Phase](#412-runtime-phase)
  - [4.1.3 Shutdown Phase](#413-shutdown-phase)
- [4.2 Complete Call Chain from main()](#42-complete-call-chain-from-main)
- [4.3 State Machine Diagram](#43-state-machine-diagram)
- [4.4 Threading Model](#44-threading-model)
- [4.5 Memory Lifecycle](#45-memory-lifecycle)

---

### **PART II: ENTRY POINT & INITIALIZATION**

#### **5. [Entry Point: main.c](#5-entry-point-mainc)**
- [5.1 File Overview](#51-file-overview)
  - [5.1.1 File Location](#511-file-location)
  - [5.1.2 Purpose and Responsibilities](#512-purpose-and-responsibilities)
  - [5.1.3 Dependencies](#513-dependencies)
  - [5.1.4 Global Variables](#514-global-variables)
- [5.2 main() - Application Entry Point](#52-main---application-entry-point)
  - [5.2.1 Function Signature](#521-function-signature)
  - [5.2.2 Purpose](#522-purpose)
  - [5.2.3 Parameters](#523-parameters)
  - [5.2.4 Return Value](#524-return-value)
  - [5.2.5 Complete Source Code](#525-complete-source-code)
  - [5.2.6 Line-by-Line Explanation](#526-line-by-line-explanation)
  - [5.2.7 Execution Flow Diagram](#527-execution-flow-diagram)
  - [5.2.8 Error Handling](#528-error-handling)
  - [5.2.9 Side Effects](#529-side-effects)
  - [5.2.10 Platform-Specific Behavior](#5210-platform-specific-behavior)
  - [5.2.11 Performance Characteristics](#5211-performance-characteristics)
  - [5.2.12 Example Usage Scenarios](#5212-example-usage-scenarios)
  - [5.2.13 Related Functions](#5213-related-functions)
  - [5.2.14 Known Issues](#5214-known-issues)
  - [5.2.15 TODO Items](#5215-todo-items)

---

### **PART III: COMMAND-LINE ARGUMENT SYSTEM**

#### **6. [Argument Parsing (utils/args.h & args.c)](#6-argument-parsing-utilsargsh--argsc)**
- [6.1 Module Overview](#61-module-overview)
- [6.2 Data Structures](#62-data-structures)
  - [6.2.1 dump_level_t Enumeration](#621-dump_level_t-enumeration)
  - [6.2.2 log_detail_t Enumeration](#622-log_detail_t-enumeration)
  - [6.2.3 app_args_t Structure](#623-app_args_t-structure)
- [6.3 Functions](#63-functions)
  - [6.3.1 parse_args()](#631-parse_args)
  - [6.3.2 print_banner()](#632-print_banner)
  - [6.3.3 print_version_info()](#633-print_version_info)
  - [6.3.4 print_usage()](#634-print_usage)

---

### **PART IV: LOGGING SYSTEM**

#### **7. [Logger System (utils/logger.h & logger.c)](#7-logger-system-utilsloggerh--loggerc)**
- [7.1 Module Overview](#71-module-overview)
- [7.2 Architecture](#72-architecture)
  - [7.2.1 Thread-Safety Model](#721-thread-safety-model)
  - [7.2.2 Category System](#722-category-system)
  - [7.2.3 Level Hierarchy](#723-level-hierarchy)
- [7.3 Data Structures](#73-data-structures)
  - [7.3.1 t_log_level Enumeration](#731-t_log_level-enumeration)
  - [7.3.2 t_log_options Structure](#732-t_log_options-structure)
- [7.4 Core Functions](#74-core-functions)
  - [7.4.1 logger_init()](#741-logger_init)
  - [7.4.2 logger_shutdown()](#742-logger_shutdown)
  - [7.4.3 logger_log()](#743-logger_log)
  - [7.4.4 logger_logv()](#744-logger_logv)
- [7.5 Configuration Functions](#75-configuration-functions)
  - [7.5.1 logger_set_global_level()](#751-logger_set_global_level)
  - [7.5.2 logger_get_global_level()](#752-logger_get_global_level)
  - [7.5.3 logger_set_category_level()](#753-logger_set_category_level)
  - [7.5.4 logger_get_category_level()](#754-logger_get_category_level)
  - [7.5.5 logger_set_console_level()](#755-logger_set_console_level)
- [7.6 Utility Functions](#76-utility-functions)
  - [7.6.1 logger_is_tty()](#761-logger_is_tty)
  - [7.6.2 logger_should_log()](#762-logger_should_log)
  - [7.6.3 logger_now_ms()](#763-logger_now_ms)
  - [7.6.4 logger_hexdump()](#764-logger_hexdump)
- [7.7 Macro API](#77-macro-api)
  - [7.7.1 LOG_TRACEF()](#771-log_tracef)
  - [7.7.2 LOG_DEBUGF()](#772-log_debugf)
  - [7.7.3 LOG_INFOF()](#773-log_infof)
  - [7.7.4 LOG_WARNF()](#774-log_warnf)
  - [7.7.5 LOG_ERRORF()](#775-log_errorf)
  - [7.7.6 LOG_FATALF()](#776-log_fatalf)
  - [7.7.7 LOG_HEXDUMP()](#777-log_hexdump)
  - [7.7.8 LOG_CHECK()](#778-log_check)
  - [7.7.9 LOG_TIME_BLOCK()](#779-log_time_block)
- [7.8 Implementation Details](#78-implementation-details)
- [7.9 Category System](#79-category-system)
- [7.10 Performance Considerations](#710-performance-considerations)

---

### **PART V: MDL FILE LOADING & PARSING**

#### **8. [MDL Data Structures (studio.h)](#8-mdl-data-structures-studioh)**
- [8.1 File Overview](#81-file-overview)
- [8.2 Core Header Structures](#82-core-header-structures)
  - [8.2.1 studiohdr_t](#821-studiohdr_t)
  - [8.2.2 studioseqhdr_t](#822-studioseqhdr_t)
- [8.3 Bone Structures](#83-bone-structures)
  - [8.3.1 mstudiobone_t](#831-mstudiobone_t)
  - [8.3.2 mstudiobonecontroller_t](#832-mstudiobonecontroller_t)
- [8.4 Animation Structures](#84-animation-structures)
  - [8.4.1 mstudioseqdesc_t](#841-mstudioseqdesc_t)
  - [8.4.2 mstudioanim_t](#842-mstudioanim_t)
  - [8.4.3 mstudioevent_t](#843-mstudioevent_t)
- [8.5 Geometry Structures](#85-geometry-structures)
  - [8.5.1 mstudiobodyparts_t](#851-mstudiobodyparts_t)
  - [8.5.2 mstudiomodel_t](#852-mstudiomodel_t)
  - [8.5.3 mstudiomesh_t](#853-mstudiomesh_t)
- [8.6 Texture Structures](#86-texture-structures)
  - [8.6.1 mstudiotexture_t](#861-mstudiotexture_t)
- [8.7 Other Structures](#87-other-structures)
  - [8.7.1 mstudioattachment_t](#871-mstudioattachment_t)
  - [8.7.2 mstudiobbox_t](#872-mstudiobbox_t)
- [8.8 Type Definitions](#88-type-definitions)

#### **9. [MDL Loader (mdl/mdl_loader.h & mdl_loader.c)](#9-mdl-loader-mdlmdl_loaderh--mdl_loaderc)**
- [9.1 Module Overview](#91-module-overview)
- [9.2 Data Structures](#92-data-structures)
  - [9.2.1 mdl_seqgroup_blob_t](#921-mdl_seqgroup_blob_t)
  - [9.2.2 mdl_model_t](#922-mdl_model_t)
  - [9.2.3 mdl_result_t](#923-mdl_result_t)
- [9.3 High-Level Functions](#93-high-level-functions)
  - [9.3.1 create_mdl_model()](#931-create_mdl_model)
  - [9.3.2 free_model()](#932-free_model)
  - [9.3.3 load_model_with_textures()](#933-load_model_with_textures)
  - [9.3.4 load_sequence_groups()](#934-load_sequence_groups)
  - [9.3.5 free_sequences_groups()](#935-free_sequences_groups)
- [9.4 File Operations](#94-file-operations)
  - [9.4.1 read_mdl_file()](#941-read_mdl_file)
  - [9.4.2 parse_mdl_h()](#942-parse_mdl_h)
  - [9.4.3 validate_mdl_magic()](#943-validate_mdl_magic)
  - [9.4.4 validate_mdl_version()](#944-validate_mdl_version)
- [9.5 Parsing Functions](#95-parsing-functions)
  - [9.5.1 parse_bone_hierarchy()](#951-parse_bone_hierarchy)
  - [9.5.2 parse_animation_sequences()](#952-parse_animation_sequences)
  - [9.5.3 parse_mesh_data()](#953-parse_mesh_data)
  - [9.5.4 parse_vertex_data()](#954-parse_vertex_data)
  - [9.5.5 parse_triangle_commands_fixed()](#955-parse_triangle_commands_fixed)
  - [9.5.6 extract_triangles_with_uvs()](#956-extract_triangles_with_uvs)
- [9.6 Texture Functions](#96-texture-functions)
  - [9.6.1 extract_texture_rgb()](#961-extract_texture_rgb)
  - [9.6.2 generate_texture_filename()](#962-generate_texture_filename)
- [9.7 Utility Functions](#97-utility-functions)
  - [9.7.1 transform_vertices_to_opengl()](#971-transform_vertices_to_opengl)
  - [9.7.2 get_model_by_bodypart()](#972-get_model_by_bodypart)
- [9.8 Debug Functions](#98-debug-functions)

#### **10. [MDL Report (mdl/mdl_report.c)](#10-mdl-report-mdlmdl_reportc)**
- [10.1 Module Overview](#101-module-overview)
- [10.2 Functions](#102-functions)

#### **11. [MDL Info (mdl/mdl_info.c)](#11-mdl-info-mdlmdl_infoc)**
- [11.1 Module Overview](#111-module-overview)
- [11.2 Functions](#112-functions)

---

### **PART VI: RENDERING SYSTEM**

#### **12. [Platform OpenGL (graphics/gl_platform.h)](#12-platform-opengl-graphicsgl_platformh)**
- [12.1 File Overview](#121-file-overview)
- [12.2 Platform Detection](#122-platform-detection)
- [12.3 Include Strategy](#123-include-strategy)
- [12.4 Helper Macros](#124-helper-macros)

#### **13. [Renderer (graphics/renderer.h & renderer.c)](#13-renderer-graphicsrendererh--rendererc)**
- [13.1 Module Overview](#131-module-overview)
- [13.2 Global Variables](#132-global-variables)
- [13.3 Constants](#133-constants)
- [13.4 Initialization](#134-initialization)
  - [13.4.1 init_renderer()](#1341-init_renderer)
  - [13.4.2 cleanup_renderer()](#1342-cleanup_renderer)
- [13.5 Main Loop](#135-main-loop)
  - [13.5.1 render_loop()](#1351-render_loop)
  - [13.5.2 should_close_window()](#1352-should_close_window)
- [13.6 Rendering Functions](#136-rendering-functions)
  - [13.6.1 render_model()](#1361-render_model)
  - [13.6.2 clear_screen()](#1362-clear_screen)
  - [13.6.3 set_model_data()](#1363-set_model_data)
  - [13.6.4 set_wireframe_mode()](#1364-set_wireframe_mode)
  - [13.6.5 set_current_texture()](#1365-set_current_texture)
- [13.7 Internal Helpers](#137-internal-helpers)
  - [13.7.1 UpdateBonesForCurrentFrame()](#1371-updatebonesforcurrentframe)
  - [13.7.2 ProcessModelForRendering()](#1372-processmodelforrendering)
  - [13.7.3 AddVertexToBuffer()](#1373-addvertextobuffer)
- [13.8 Shader Management](#138-shader-management)
- [13.9 GLFW Callbacks](#139-glfw-callbacks)

#### **14. [Camera (graphics/camera.h & camera.c)](#14-camera-graphicscamerah--camerac)**
- [14.1 Module Overview](#141-module-overview)
- [14.2 Data Structures](#142-data-structures)
- [14.3 Functions](#143-functions)

#### **15. [Textures (graphics/textures.h & textures.c)](#15-textures-graphicstexturesh--texturesc)**
- [15.1 Module Overview](#151-module-overview)
- [15.2 Functions](#152-functions)

---

### **PART VII: ANIMATION SYSTEM**

#### **16. [Animations (mdl/mdl_animations.c)](#16-animations-mdlmdl_animationsc)**
- [16.1 Module Overview](#161-module-overview)
- [16.2 Functions](#162-functions)

#### **17. [Bone System (mdl/bone_system.c)](#17-bone-system-mdlbone_systemc)**
- [17.1 Module Overview](#171-module-overview)
- [17.2 Functions](#172-functions)

#### **18. [Body Part Manager (mdl/bodypart_manager.c)](#18-body-part-manager-mdlbodypart_managerc)**
- [18.1 Module Overview](#181-module-overview)
- [18.2 Functions](#182-functions)

---

### **PART VIII: UTILITY SYSTEMS**

#### **19. [Utilities (utils/utils.h & utils.c)](#19-utilities-utilsutilsh--utilsc)**
- [19.1 Module Overview](#191-module-overview)
- [19.2 Functions](#192-functions)

#### **20. [MDL Messages (utils/mdl_messages.h & mdl_messages.c)](#20-mdl-messages-utilsmdl_messagesh--mdl_messagesc)**
- [20.1 Module Overview](#201-module-overview)
- [20.2 Error Codes](#202-error-codes)

---

### **PART IX: APPENDICES**

#### **[Appendix A: Complete Function Index](#appendix-a-complete-function-index)**

#### **[Appendix B: Error Code Reference](#appendix-b-error-code-reference)**

#### **[Appendix C: Platform-Specific Notes](#appendix-c-platform-specific-notes)**

#### **[Appendix D: Build System](#appendix-d-build-system)**

#### **[Appendix E: Memory Management](#appendix-e-memory-management)**

#### **[Appendix F: Performance Profiling](#appendix-f-performance-profiling)**

#### **[Appendix G: Glossary](#appendix-g-glossary)**

#### **[Appendix H: Version History](#appendix-h-version-history)**

---

<div align="center">
**End of Table of Contents**

*Complete documentation sections begin below...*

</div>

---

---

## 1. Introduction

### 1.1 About This Document

#### 1.1.1 Purpose and Scope

**Project Origins**

The Lambda Model Editor began as a personal exploration into game engine architecture and low-level C programming. Originally conceived as a Minecraft clone for learning 3D graphics fundamentals, the project shifted direction after discovering Valve's released GoldSrc SDK. The opportunity to study and recreate a proven, shipped game engine's model system proved more valuable than building yet another voxel renderer from scratch.

Half-Life's .mdl format offered an ideal learning target: complex enough to be interesting, documented enough to be approachable, and old enough that reverse-engineering it serves preservation rather than piracy. The format demonstrates sophisticated optimization techniques from an era when memory was measured in megabytes and every byte mattered.

**The Learning Journey**

What started as experimentation with file parsing became a deep dive into C's raw, unfiltered relationship with hardware. No garbage collection, no runtime safety nets—just manual memory management and direct system calls. This directness, while initially intimidating, reveals exactly how computers work beneath the abstractions modern languages provide.

The scope grew beyond initial expectations. What seemed like a straightforward file viewer revealed layers of complexity: skeletal animation systems, texture coordinate transformations, bone hierarchy calculations, and OpenGL rendering pipelines. Each solved problem uncovered three more. The project evolved from a weekend learning exercise into a multi-month recreation of core engine subsystems.

**Why This Became More Than a Learning Project**

As the codebase expanded, so did the realization that this work mirrors what game engines actually do. Loading models, calculating bone transformations, rendering textured geometry, playing back animations—these aren't just modding tool features. They're fundamental game engine operations. Building this tool meant rebuilding pieces of Half-Life's renderer, just with modern OpenGL instead of 1998-era software rendering.

This intersection of game preservation, engine archaeology, and practical systems programming turned a personal project into something worth documenting and sharing. The result is a focused tool for a specific job: understanding and manipulating Half-Life's proprietary model format through direct implementation rather than abstraction layers.

**Current Project Scope**

This tool is NOT:

- A complete game engine
- A full modding suite with GUI
- A replacement for Valve's official tools
- Production-ready software (alpha stage)

This tool IS:

- A working .mdl file parser and validator
- An OpenGL-based model renderer with animation playback
- An educational codebase demonstrating file format engineering
- A foundation for future model editing capabilities

**What This Project Actually Does**

Current capabilities:

- Parses binary .mdl files including texture and sequence group sub-files
- Validates file integrity and reports format violations
- Renders 3D geometry with proper texture mapping and UV coordinates
- Plays skeletal animations using the original bone system
- Transforms Half-Life's coordinate system to OpenGL conventions
- Exports detailed technical reports about model structure

Planned capabilities:

- Model manipulation (vertex editing, bone adjustments)
- Texture replacement and skin swapping
- Animation retargeting and blending
- Export to modern formats (glTF, FBX, OBJ)
- QC script generation for studiomdl recompilation

**Why Pure C?**

This project uses C (C99 standard) exclusively for deliberate reasons:

**Educational clarity**: C forces explicit thinking about memory layout, pointer arithmetic, and data structure design. Every allocation is visible. Every transformation is manual. This transparency makes the code readable as a learning resource.

**Historical accuracy**: The original GoldSrc engine was derived from the Quake original engine written completely in C, GoldSrc was a heavily modified version of that written in C++ format, but this only meant that using C shouldn't pose any problems with getting the project to run. Matching that paradigm helps understand the original design decisions and constraints.

**Portability**: C code compiles everywhere. Windows, macOS, Linux—same source, minimal platform-specific code. Only GLFW, GLEW and OpenGL as external dependencies.

**Direct control**: No hidden allocations, no runtime overhead, no language magic. When you call `malloc()`, you know exactly what happens. When you free memory, it's gone immediately.

The codebase prioritizes readability and educational value over aggressive optimization. This is teaching code that happens to work in production, not production code that happens to be readable.

**Documentation Scope**

This reference documents:

- Every public function (200+ functions with full specifications)
- Complete .mdl binary format specification
- Architecture decisions and design trade-offs
- Memory management patterns and lifecycle tracking
- Platform-specific considerations and workarounds
- OpenGL rendering pipeline implementation

This documentation assumes you have a reason to care about these details. You're either contributing code, learning from it, or researching the .mdl format itself.

#### 1.1.2 Target Audience

**Who This Documentation Serves**

This documentation wasn't originally planned. The project began as personal code with personal notes. As the codebase grew and became potentially useful to others, proper documentation became necessary. This reference now serves multiple distinct audiences with different needs.

**Primary Audiences**

**Contributors and Maintainers**

Developers who want to extend this tool or fix bugs. You need to understand the existing architecture before making changes. You should be comfortable with:

- C programming (pointers, manual memory management, struct layout)
- Basic 3D mathematics (vectors, matrices, transformations)
- OpenGL fundamentals (vertex buffers, shaders, texture mapping)
- Version control and build systems (Git, CMake)

Start with Section 3 (Architecture Overview) and Section 4 (Execution Flow) to understand how the pieces fit together. Then dive into specific module sections as needed.

**Students and Self-Learners**

Programmers using this project as a case study in practical C development and 3D graphics. You might be:

- Learning C beyond basic tutorials
- Studying file format engineering
- Understanding how 3D rendering actually works
- Exploring game engine architecture

You should have intermediate C knowledge (comfortable with malloc/free and pointers) and willingness to read unfamiliar code. Start with Section 2 (MDL Format) to understand what the code accomplishes, then work through Section 5-11 to see the implementation.

**Half-Life Modders and Researchers**

Modders who need deep technical knowledge of the .mdl format, or researchers documenting how 1990s game technology worked. You don't necessarily care about this specific implementation—you care about understanding the format itself.

Read Section 2 (complete format specification) and Section 8 (data structures). The rest is optional unless you're debugging a corrupt model or building your own tools.

**Secondary Audiences**

**General Programmers**

Developers curious about practical systems programming or how model viewers and editors work. You have programming experience but not necessarily in graphics or low-level C.

Focus on Section 13 (Renderer) for OpenGL usage patterns and Section 9 (MDL Loader) for binary file parsing techniques. These sections demonstrate practical solutions to common problems.

**Game Preservation Advocates**

People interested in how older game technology worked and how to preserve it. You care about the historical context and design constraints that shaped these formats.

Section 2.1-2.2 covers format history and evolution. Section 2.9-2.10 documents the original Valve toolchain. The implementation details are less relevant unless you're building preservation tools.

**What You Need to Know**

**For contributors:**

- Solid C fundamentals (not just syntax—memory models and undefined behavior)
- Understanding of 3D coordinate systems and transformations
- OpenGL basics (you should know what a VAO is)
- Debugging skills (gdb/lldb or Visual Studio debugger)

**For students:**

- Intermediate C (you should understand why `char *ptr` and `char ptr[]` differ)
- Basic 3D math (you don't need calculus, but understand what a dot product does)
- Patience to read existing code before writing new code
- Access to a debugger and willingness to use it

**For modders:**

- Familiarity with existing Half-Life modding workflows
- Basic 3D modeling concepts (vertices, UVs, bones)
- Hex editor experience helpful for format investigation
- No programming experience required for format documentation

**What You Don't Need**

You do NOT need:

- Advanced mathematics (linear algebra beyond basic vector/matrix operations)
- Prior game engine experience (Unity, Unreal, Godot)
- Knowledge of Half-Life's source code or Quake engine architecture
- Windows-specific development experience (code is cross-platform)
- Assembly language or low-level reverse engineering skills

**Why This Matters**

The original GoldSrc engineers worked under severe constraints: limited memory, slow CPUs, no GPU vertex processing. The .mdl format reflects ingenious solutions to these constraints—packed data structures, lookup tables, animation compression. Understanding these techniques provides insight into fundamental optimization principles that remain relevant today, even though the specific problems have changed.

This documentation exists because that knowledge shouldn't be lost. Whether you're extending this tool, building your own, or just satisfying curiosity about how games worked in 1998, the information is here.

**A Note on Tone**

This documentation is technical but not academic. It explains not just what the code does, but why it does it that way. It admits limitations and mistakes. It assumes you're intelligent enough to understand complex topics without hand-holding, but doesn't assume you already know everything.

If something is unclear, that's a documentation bug and you should feel free to file an issue.

#### 1.1.3 How to Use This Reference

**Document Structure**

This reference follows a deliberate learning progression rather than alphabetical ordering. Each major section builds on previous concepts:

**Part I: Introduction and Context** (Sections 1-4)
 Explains what this project is, why it exists, and how the codebase is organized. Section 4 maps the complete execution flow from program start to shutdown.

**Part II-III: Initialization** (Sections 5-7)
 Walks through program startup: main() entry point, command-line argument parsing, and logging system initialization. Shows proper C initialization patterns.

**Part IV: MDL Format and Parsing** (Sections 8-11)
 Core content. Section 8 documents the original Valve data structures. Section 9 explains how we parse binary .mdl files. Section 10-11 cover reporting and validation.

**Part V: Rendering System** (Sections 12-15)
 How parsed data becomes pixels on screen. OpenGL setup, shader management, camera system, and texture handling.

**Part VI: Animation System** (Sections 16-18)
 Skeletal animation implementation: bone transformations, sequence playback, and body part management.

**Part VII: Utilities** (Sections 19-20)
 Supporting systems: file I/O helpers, error handling, and message formatting.

**Part VIII: Appendices**
 Quick reference: complete function index, error code listing, platform-specific notes, and build instructions.

**Reading Strategies by Purpose**

**If you are contributing code:**

First read Section 3 to understand module boundaries and architectural decisions. Then read Section 4 to see how the entire program flows from initialization to shutdown. After understanding the overall structure, jump to the specific module sections relevant to your changes. Use Appendix A for quick function lookups when you need to find where something is implemented.

**If you are learning C and graphics programming:**

Start with Section 2 to understand what problem the code is solving. The .mdl format itself is complex, and understanding it first makes the parsing code make sense. Then read Section 5 to see complete program initialization from main() forward. After that, deep dive into Section 9 which demonstrates practical binary file parsing techniques. Finally, study Section 13 to see how OpenGL is used in practice for model rendering. Work alongside the actual source code as you read.

**If you are researching the .mdl format:**

Read Section 2.2 through 2.6 for complete file format specification. Reference Section 8 for the original Valve data structure definitions. Check Section 9.5 to see how individual fields are parsed and interpreted. The rendering and animation sections are less relevant unless you are debugging visual issues or trying to understand how the format was intended to be used.

**If you are building your own tool:**

Study Section 2 thoroughly to understand every aspect of the format. Examine Section 9.4 to see the file loading strategy and what order operations must happen. Reference Section 9.5 carefully because it documents edge cases and format quirks discovered during implementation. Check Section 2.11 for common pitfalls and how to handle malformed files.

**Function Documentation Format**

Each function section follows this structure when fully documented:

Purpose: One-sentence or multiple sentence , depending on the function,  description goal of what the function accomplishes

Signature: Complete C function signature with return type and parameter types

Parameters: Each parameter listed with its type, purpose, and any constraints or valid value ranges

Return Value: What the function returns, including specific meaning of different return values

Description: Detailed explanation of the function's behavior, side effects, and implementation notes

Example: Typical usage demonstrating the function in context

Error Handling: What error conditions exist and how they are reported to the caller

See Also: Related functions and relevant documentation sections

Not every function receives this level of documentation. Simple accessor functions and obvious utilities are documented more concisely.

**When Code and Documentation Conflict**

The actual source code is authoritative. If this documentation contradicts the implementation, the documentation contains an error and should be reported as a bug.

Version synchronization: This documentation version 0.2.0-alpha.1 matches the code version it documents. Reading documentation from version 0.2.0 while working with code from version 0.3.0 will result in discrepancies.

Each new release will contain an updated code and this documentation will follow that update, in case if it doesnt that simply means that the documentation contains a bug and it should be reported as a bug.

**Cross-Referencing**

Functions reference related functions in their See Also sections. Lifecycle pairs always reference each other. Data structures reference the functions that operate on them. Sections reference prerequisite knowledge where necessary.

**Using This for Self-Study**

Read this documentation alongside the actual source code. Open the files in your editor while reading the documentation sections. Build the project following Appendix D and get it running on your machine. Make small experimental changes to understand how the pieces fit together. Use a debugger to step through the functions described here and watch how data flows through the program.

**Limitations of This Documentation**

This reference documents public APIs and their contracts, data structure layouts and field meanings, major design decisions and their rationale, and platform-specific considerations where they exist.

This reference does not document internal implementation details that may change without notice, temporary debugging code or experimental features, performance micro-optimizations that do not affect behavior, or obvious code where the implementation is self-explanatory.

**Staying Current**

Check Appendix H for version history showing what changed between releases. For questions not answered in this documentation, consult source code comments particularly in header files, examine git commit messages for context on specific changes, review GitHub issues for known problems and ongoing discussions, or check community resources listed in Section 1.4.

#### 1.1.4 Document Updates and Maintenance

**Update Policy**

This documentation follows the project release cycle. Major releases receive complete documentation reviews. Minor releases update sections for new features and API changes. Patch releases correct errors and clarify existing content.

The version number at the top of this document matches the project release it documents. Documentation version 0.2.0-alpha.1 corresponds to code version 0.2.0-alpha.1. Using mismatched versions will result in inaccuracies.

**What Triggers Updates**

Documentation updates occur when:

- Public APIs change signature or behavior
- New modules or major features are added
- Data structures gain or lose fields
- Command-line arguments change
- File format support expands

Internal refactoring that does not affect public interfaces may not trigger documentation updates.

**Staying Current**

To work with current documentation, use the version bundled with each release. The documentation file is versioned alongside the code in the repository.

Check Appendix H for a changelog listing what sections changed between versions. This helps identify what documentation updates affect your use case.

**Reporting Documentation Errors**

If documentation conflicts with actual code behavior, file an issue on GitHub with the section number, what the documentation states, and what the code actually does. The code is authoritative when conflicts exist.

### 1.2 Document Conventions

#### 1.2.1 Code Formatting

Code appears in monospace font using standard C syntax highlighting. Function names include parentheses to distinguish them from variables. Structure and type names use the project's naming convention with _t suffix for types.

Inline references appear as: function_name(), MACRO_NAME, struct_name_t, variable_name

Multi-line code blocks appear indented with complete context. Comments explain non-obvious behavior. Line numbers reference actual source file locations when discussing specific implementations.

#### 1.2.2 Symbols and Status Indicators

This documentation uses minimal status indicators:

**Stable**: API unlikely to change before 1.0 release
**Experimental**: Subject to change based on testing
**Deprecated**: Being replaced, migration path provided
**Incomplete**: Declared but not fully implemented

These appear only when relevant to warn about API stability.

#### 1.2.3 Parameter Notation

Function parameters are documented with:

**Name**: Parameter identifier as it appears in code
**Type**: C type including pointer depth and const qualifiers
**Purpose**: What the parameter represents
**Constraints**: Valid ranges, NULL handling, ownership semantics

Input parameters are distinguished from output parameters. Functions that modify parameters through pointers note this explicitly.

#### 1.2.4 Cross-References

Section references use the format: Section X.Y or Section X.Y.Z

Related functions are listed in See Also subsections within each function's documentation. Prerequisites are noted at the beginning of complex sections.

Appendices provide alternate access paths: function index by name, error codes by number, and platform-specific notes by operating system.

### 1.3 Prerequisites

#### 1.3.1 Required Knowledge

**For reading and understanding this documentation:**

C programming language fundamentals including pointer arithmetic, manual memory management, structure layout, and function pointers. You should understand what undefined behavior means and why it matters.

Binary file formats and byte ordering. Ability to read hex dumps and understand how data structures map to bytes on disk.

Basic 3D mathematics: vectors, matrices, coordinate transformations. You do not need advanced linear algebra, but should understand what a transformation matrix does.

**For contributing to the codebase:**

All of the above plus practical experience with:

OpenGL programming including vertex buffers, shaders, and texture management. You should have written at least one working OpenGL application.

Build systems, specifically CMake. Ability to modify build configurations and add new source files.

Version control with Git including branches, commits, and pull requests.

Debugging tools: gdb, lldb, or Visual Studio debugger. Ability to set breakpoints, inspect memory, and trace execution.

#### 1.3.2 Required Tools

**To build this project:**

C compiler supporting C99 standard: GCC 7.0 or newer, Clang 6.0 or newer, MSVC 2017 or newer

CMake 3.10 or newer for build configuration

Git for cloning the repository and version control

**Compiler Requirements by Platform:**

**Linux:**

- GCC 7.0 or newer, or Clang 6.0 or newer
- C99 standard support required
- Tested on Arch Linux with kernel 6.17.3

**macOS:**

- Clang (included with Xcode Command Line Tools)
- C99 standard support required
- Tested on macOS Sequoia 15.0.1 (Tahoe kernel 26.0.1)
- Xcode Command Line Tools: install via `xcode-select --install`

**Windows:**

- Visual Studio 2017 or newer with C/C++ development tools
- Alternative: MinGW-w64 with GCC 7.0 or newer
- C99 standard support required

**Required Runtime Dependencies:**

**GLFW 3.3 or newer** (all platforms)

- Windowing, input handling, and OpenGL context creation
- Install via package manager or build from source

**OpenGL 3.3 Core Profile or newer** (all platforms)

- Requires graphics drivers supporting OpenGL 3.3
- Verify with: `glxinfo | grep "OpenGL version"` (Linux) or equivalent, although this doesnt work on macOS without installing XQuartz so Mac doesn't do GLX. So the best thing to do is have it be displayed inside the app or using the following command  ` system_profiler SPDisplaysDataType | egrep "Chipset Model|Vendor|VRAM|Metal"` .

**GLEW (OpenGL Extension Wrangler)** - PLATFORM SPECIFIC:

- **Required on Linux**: Must be installed via package manager
  - Arch/Manjaro: `sudo pacman -S glew`
  - Ubuntu/Debian: `sudo apt install libglew-dev`
  - Fedora: `sudo dnf install glew-devel`
- **Not required on macOS**: OpenGL extension loading handled by system frameworks
- **Required on Windows**: Must be installed or built from source

**Platform-Specific Development Packages:**

**Linux:**

- X11 development headers: `libx11-dev` or `libX11-devel`
- OpenGL development headers: `mesa-libGL-devel` or equivalent
- GLEW development headers: `glew` or `libglew-dev`
- GLFW development headers: `glfw` or `libglfw3-dev`

**macOS:**

- Xcode Command Line Tools (provides OpenGL frameworks)
- GLFW via Homebrew: `brew install glfw`
- No GLEW required - OpenGL extension loading uses native macOS frameworks

**Windows:**

- Visual Studio with Desktop Development with C++ workload
- GLFW: download pre-compiled binaries or build from source
- GLEW: download pre-compiled binaries or build from source
- OpenGL drivers: install latest GPU drivers from manufacturer

**Development tools:**

Text editor or IDE with C language support
 Debugger:

- Linux: gdb (install via package manager)
- macOS: lldb (included with Xcode Command Line Tools)
- Windows: Visual Studio debugger or gdb via MinGW

Hex editor for examining binary .mdl files (optional but recommended):

- Linux: hexdump (preinstalled), ghex, or bless
- macOS: Hex Fiend or hexdump (preinstalled)
- Windows: HxD or any hex editor

**Verified Build Configurations:**

This project has been successfully built and tested on:

- Arch Linux, kernel 6.17.3, GCC 11.2, GLFW 3.3.8, GLEW 2.2.0
- macOS Sequoia 15.0.1 (Tahoe kernel 26.0.1), Clang 14.0, GLFW 3.3.8, no GLEW
- Windows OS platform has not yet been tested as off this, current version so that won't be included in this documentation until the testing is confirmed and fully working.

Other configurations may work but have not been explicitly tested.

#### 1.3.3 Recommended Reading

**For understanding the .mdl format:**

Valve Developer Community wiki articles on GoldSrc model format
 Original Half-Life SDK documentation (included with SDK)
 Quake modding documentation (GoldSrc is derived from Quake engine)

**For C programming:**

"The C Programming Language" by Kernighan and Ritchie
 "Expert C Programming" by Peter van der Linden
 "C Interfaces and Implementations" by David Hanson

**For 3D graphics:**

"3D Math Primer for Graphics and Game Development - 2nd Edition" by Fletcher Dunn & Ian Parberry
"OpenGL Programming Guide" (Red Book) chapters on vertex buffers and transformations
 "Real-Time Rendering" by Akenine-Möller for general 3D graphics concepts
 "Mathematics for 3D Game Programming" by Eric Lengyel for transformation mathematics

**For understanding game engine architecture:**

"Game Engine Architecture" by Jason Gregory
 Original Quake and Quake II source code releases
 GoldSrc SDK source code (publicly released by Valve)

None of these are strictly required, but they provide valuable context for understanding design decisions in this codebase.

### 1.4 Getting Help

#### 1.4.1 Community Resources

**Primary Reference Implementations:**

Sam Vanheer's HalfLifeModelViewer2: https://github.com/SamVanheer/HalfLifeModelViewer2

- Primary reference for this project's development
- Complete working implementation of .mdl format handling
- Demonstrates proper parsing and rendering techniques

Official Valve GoldSrc SDK: https://github.com/ValveSoftware/halflife

- Authoritative source for engine structures and behavior
- Original studiomdl compiler source code
- Reference implementations of model loading and rendering

**Project Resources:**

GitHub repository: https://github.com/KarloSiric/Lambda

- Source code and releases
- Issue tracker for bugs and features
- Documentation and wiki

**Half-Life Modding Community:**

Valve Developer Community wiki: https://developer.valvesoftware.com/wiki/Half-Life

- GoldSrc engine documentation
- MDL format specifications and structure definitions
- Modding tutorials and technical guides
- Essential reading for understanding the format

**MDL Format Learning Resources:**

Valve Developer Community - Studio Model Format:
https://developer.valvesoftware.com/wiki/MDL_(GoldSrc)

Valve Developer Community - Studiomdl:
https://developer.valvesoftware.com/wiki/StudioMDL_(Source)

Valve Developer Community - Quake Model Format(Quake mdl):
https://developer.valvesoftware.com/wiki/MDL_(Quake)

#### 1.4.2 Issue Reporting

**Before Filing an Issue:**

Search existing issues to avoid duplicates
Verify the problem exists in the latest release
Attempt to reproduce with a minimal test case

**Required Information for Bug Reports:**

Operating system and version
Compiler and version used to build
Complete error message or crash log
Steps to reproduce the issue
Expected behavior versus actual behavior

If the issue involves a specific model file, include the filename and source if possible. For crashes, include a stack trace from your debugger.

**For Documentation Issues:**

Section number where error appears
What the documentation states
What the code actually does
Suggested correction if you have one

#### 1.4.3 Contributing Guidelines

**Code Contributions:**

Fork the repository and create a feature branch
Follow existing code style and naming conventions
Keep changes focused on a single issue or feature
Test your changes before submitting
Update relevant documentation for API changes
Submit pull request with clear description

**Code Style:**

Follow C99 standard
Use descriptive variable names
Comment non-obvious logic
Keep functions focused and reasonably sized
Avoid platform-specific code unless necessary

**Documentation Contributions:**

Corrections and clarifications are welcome
Keep existing structure and formatting
Technical accuracy is more important than perfect prose
Add examples where they improve understanding

**What Makes a Good Contribution:**

Solves a real problem
Explains why the change is needed
Does not break existing functionality
Includes appropriate error handling

**What to Discuss First:**

Large architectural changes
Adding new dependencies
Breaking changes to public APIs
Major refactoring

Open an issue to discuss the approach before investing significant time in implementation. This avoids wasted effort if the approach does not align with project goals.

---

## 2. Half-Life MDL Format Documentation

### 2.1 Introduction to MDL Files

####  2.1.1 What is an MDL File?

An MDL file is Half-Life's binary model format containing 3D geometry, skeletal animation data, textures, and metadata. The format stores everything needed to render and animate a character or object in the GoldSrc engine.

MDL files use skeletal animation rather than the vertex animation found in Quake. This allows smoother movement, more detailed models, and smaller file sizes through animation compression.

**File Structure:**

The format stores data in a binary structure with offset pointers. The file begins with a header (studiohdr_t) containing:

- Magic number "IDST" identifying the file as a studio model
- Version number (10 for GoldSrc)
- Model name (64 characters)
- Eye position, bounding boxes
- Counts and offsets for bones, sequences, textures, body parts

After the header come data blocks accessed via offset pointers. Blocks can appear in any order. This flexible layout allows features to be omitted when unused.

**Core Components:**

Bones: Skeletal structure defining the model's hierarchy. Each bone has a name, parent index, position, and rotation data.

Sequences: Animation data storing bone positions across frames. Uses delta compression - only moving bones store keyframes.

Body Parts: Groups of sub-models allowing model variations. Examples: different heads, weapons, or equipment.

Meshes: Geometry stored as triangle strips with vertex positions, normals, and texture coordinates.

Textures: Either embedded 8-bit indexed bitmaps or references to external texture files.

Attachments: Points for attaching effects, weapons, or other models.

Hit Boxes: Bounding volumes for collision and damage detection.

**Technical Specifications:**

Binary format, little-endian throughout 32-bit integers, 32-bit IEEE 754 floats C struct alignment with natural padding Maximum limits (from studio.h):

- 128 bones per model (MAXSTUDIOBONES)
- 2048 sequences (MAXSTUDIOSEQUENCES)
- 100 textures (MAXSTUDIOSKINS)
- 32 body parts (MAXSTUDIOBODYPARTS)
- 2048 vertices per mesh (MAXSTUDIOVERTS)
- 20000 triangles (MAXSTUDIOTRIANGLES)

**File Size and Optimization:**

MDL files are compact by design, optimized for the hardware constraints of 1998: 32-64MB of system RAM, slow CD-ROM drives, and no GPU vertex processing. Optimization techniques include:

Delta-compressed animation data: Only bones that actually move in a sequence store keyframe data. Bones that remain static store no animation data, saving significant space.

8-bit indexed textures: 256-color palettes reduce texture memory to one-third the size of 24-bit RGB textures.

Triangle strips: Vertices shared between adjacent triangles are stored once rather than duplicated, reducing vertex data by approximately 50%.

Compact data structures: Careful struct packing and alignment minimize wasted bytes.

Typical file sizes range from 20KB for simple weapon models to 200KB for detailed character models with multiple animations and textures. This compactness allowed dozens of models to coexist in memory during gameplay.

#### 2.1.2 History and Evolution

**Development Context:**

Valve licensed the Quake engine in 1996 to develop Half-Life. Quake's model format used vertex animation where each frame stored complete vertex positions. This approach consumed excessive memory and produced visible vertex wobbling.

Ken Birdwell, Valve engineer #1, designed a skeletal animation system for Half-Life. According to Birdwell: "Our skeletal animation system is new, though it was heavily influenced by the existing model rendering code."

The skeletal approach stores a bone hierarchy and animates bones rather than vertices. Vertices attach to bones and move with them. This reduces animation data by 10-100x compared to vertex animation.

**Quake MDL vs Half-Life MDL:**

Despite sharing the .mdl extension, the formats are incompatible:

Quake MDL: Vertex animation, single texture, 8-bit vertex precision, 162-entry normal lookup table, single mesh only.

Half-Life MDL: Skeletal animation, multiple textures, floating-point vertices, explicit normals, multiple meshes, body part system, attachments, hit boxes.

The formats share only the three-letter extension. Internal structure is completely different.

**Format Stability:**

Version 10 shipped with Half-Life in November 1998 and never changed. All GoldSrc games use version 10. This stability allowed community tools to work across all releases.

Valve released the Half-Life SDK in 2001 including studio.h with complete structure definitions and studiomdl compiler source code. This enabled the modding community to create custom models and reverse-engineer the format.

**Source Engine Transition:**

Source engine (Half-Life 2, 2004) uses a completely redesigned MDL format with versions 44-49. Despite sharing the .mdl extension and "IDST" magic number, Source MDL files are not compatible with GoldSrc. Source MDLs require additional files (.vtx, .vvd, .phy, .ani) while GoldSrc MDLs are self-contained or use simple external texture/sequence files.

#### 2.1.3 MDL vs Other 3D Formats

**Proprietary vs Open Formats:**

MDL is a closed, game-specific format. Unlike open formats (FBX, glTF, OBJ), MDL is documented only through SDK source code and community reverse engineering. No official format specification document exists.

**Comparison to Modern Formats:**

**glTF (GL Transmission Format):**

- JSON-based with binary buffers
- Supports skeletal animation, morph targets, PBR materials
- Designed for web and real-time applications
- Open specification, widely supported

MDL stores everything in binary with pointer offsets. No text representation exists.

**FBX (Filmbox):**

- Autodesk proprietary format
- Supports complex scenes, multiple objects, cameras, lights
- Binary or ASCII encoding
- Industry standard for DCC tool interchange

MDL stores single models only. No scene graph or camera data.

**OBJ (Wavefront):**

- Text-based geometry format
- No animation support
- Simple vertex/face/normal/UV data
- Widely supported, human-readable

MDL includes skeletal animation data. Cannot be represented in OBJ.

**Quake Formats (MD2/MD3):**

- Vertex animation (MD2/MD3)
- Multiple meshes and textures (MD3)
- Better vertex precision than Quake 1 MDL

MDL uses skeletal animation for smoother motion and better compression.

**Advantages of MDL:**

Compact file size through delta compression 
Self-contained format (geometry, animation, textures in one file) 
Optimized for real-time rendering on 1998 hardware 
 Proven format used in hundreds of shipped games

**Disadvantages of MDL:**

No modern tooling support 
Closed format with no official specification 
Limited to 128 bones, 100 textures, other 1998-era constraints 
Cannot represent modern PBR materials or advanced features 
Difficult to edit without decompiling to SMD

**Half-Life Release (1998):**

The first shipping version of Half-Life's MDL format appeared in November 1998. This version established the binary structure that remains unchanged:

Header magic number "IDST" and version 10 Skeletal bone hierarchy with parent-child relationships
 Animation sequences with delta-compressed keyframes Texture skins with 8-bit indexed color palettes Body parts enabling model variations (different heads, equipment, etc.) Attachment points for weapons and effects Hit boxes for location-based damage Animation events triggering sounds and particles

**Version Stability:**

The format version number has remained 10 across all GoldSrc releases. Despite engine updates, SDK releases, and game patches spanning from 1998 to present day, Valve never changed the binary format or incremented the version. This stability allowed community tools and mods to work across all Half-Life versions without compatibility issues.

Version 10 is the only GoldSrc version. Source engine (Half-Life 2 and later) uses versions 44-49 with the same "IDST" magic number but completely different structure. Despite sharing the .mdl extension, GoldSrc and Source MDL files are not compatible.

**Studio Model Compiler Evolution:**

Valve's studiomdl compiler evolved while maintaining output format compatibility:

Original studiomdl (1998): Compiled SMD source files and BMP textures into MDL format Half-Life SDK release (2001): Source code publicly released Sven Co-op enhanced compiler: Added support for tiling UVs and larger texture sizes Community improvements: Better error messages and validation

The compiler reads QC (Quake C) script files specifying source geometry, animations, textures, and compilation options. Despite the name, QC scripts are text-based and unrelated to Quake's QuakeC programming language.

**Relationship to Other Formats:**

Quake MDL (QMDL): Predecessor using vertex animation. Completely incompatible despite shared extension.

Quake II MD2: Similar vertex animation format. Not compatible with Half-Life.

Quake III MD3: More advanced vertex animation with better precision. Still incompatible.

Source Engine MDL: Completely redesigned format for Half-Life 2. Shares .mdl extension and "IDST" header but has entirely different structure. Not compatible with GoldSrc despite the shared file extension.

The GoldSrc MDL format is unique to Half-Life, Counter-Strike 1.6, Team Fortress Classic, Day of Defeat, and other GoldSrc engine games. It is not used in Source engine games.

#### 2.1.4 Use Cases in Half-Life GoldSrc Engine

The GoldSrc engine uses MDL files for all animated entities. The format serves different purposes depending on entity type.

**Player and NPC Characters:**

Full skeletal hierarchies with 20-50 bones. Human characters typically use 30-40 bones. Complex creatures like the alien grunt use 45+ bones.

80-100+ animation sequences for movement, combat, reactions, death animations. The human grunt model includes over 80 sequences.

Hit boxes defining damage zones. Headshots deal different damage than body shots. Most characters use 6-10 hit boxes for head, torso, limbs.

Attachments for eyes (view position), muzzle flash positions, weapon attachment points.

**First-Person Weapon Models:**

Simplified skeletons with 5-15 bones. The crowbar uses 2 bones. The MP5 uses 12 bones for magazine, bolt, and trigger animations.

Synchronized animation sequences for draw, holster, fire, reload, idle. Animations trigger at specific game code events.

Attachment points for muzzle flash, shell ejection, projectile spawn.

Lower polygon counts than world models (300-800 polygons) since these render close to camera constantly.

**World Models:**

Third-person representations of items and objects. The shotgun world model, health packs, ammo boxes.

Lower polygon counts than first-person models (200-500 polygons).

Simpler animations or static poses.

Bounding boxes for physics simulation and item pickup detection.

**Monsters and Creatures:**

Custom bone hierarchies matching creature anatomy. The headcrab uses 29 bones. The tentacle boss uses 25 bones for its segmented body, so despite being a larger mob it uses in fact less bones than some other smaller models in the game.

Complex behavioral animations. The sequence group animations include idle variations, walking, attacking, flinching, multiple death animation showcases each with a different style of death.

**Engine Loading Process:**

When a map loads, GoldSrc:

1. Reads the main .mdl file into memory
2. Parses the studiohdr_t header
3. Loads external texture files (.mdl + "T" suffix) if textures are external
4. Loads sequence group files (.mdl + "01", ".mdl02", etc.) if animations are externalized
5. Validates magic number and version
6. Constructs bone hierarchy in memory
7. Uploads textures to video memory
8. Builds triangle strip render lists

Models remain cached until the map changes. The engine can cache dozens of models simultaneously due to MDL's compact file size.



### 2.2 File Format Specification

#### 2.2.1 File Structure Overview

MDL files use a binary format with a header-based structure. The file begins with a main header containing metadata and pointer offsets to various data blocks located elsewhere in the file.

The header appears at file offset zero and contains counts and offsets for every data type in the model. Following the header are data blocks storing bones, animations, textures, meshes, and other components. These blocks can appear in any order within the file.

**Reading the File:**

Loading an MDL file follows a consistent pattern. First, read the main header structure from offset zero. The header contains pairs of fields for each data type: a count field specifying how many items exist, and an offset field pointing to where that data begins in the file.

To access any data block, check the corresponding count field in the header. If the count is zero, that data type is not present. If the count is non-zero, seek to the offset specified by the matching offset field and read the appropriate number of structures.

For example, to read bone data, check the numbones field in the header. If numbones is 30, seek to the file position specified by boneindex and read 30 bone structures. Each bone structure has a fixed size, so reading count multiplied by structure size gives the complete data block.

**Data Block Organization:**

The file contains these primary data blocks:

Bone data defines the skeletal hierarchy with parent-child relationships between bones.

Bone controller data specifies which bones can be manipulated at runtime for effects like mouth movement or eye tracking.

Hit box data defines collision volumes attached to bones for damage detection.

Sequence data stores animation information including frame counts, playback speed, and animation events.

Sequence group data references external files containing additional animations.

Texture data includes texture metadata and optionally the texture pixel data itself.

Skin family data defines texture variations allowing different appearances without duplicating geometry.

Body part data groups sub-models for selectable model variations like different heads or equipment.

Attachment data specifies points on the model where other entities or effects can attach.

Transition data defines allowed transitions between animation sequences.

**Flexible Layout:**

The offset-based structure provides flexibility. Data blocks can appear in any order within the file. Unused features can be completely omitted by setting their count to zero. This design allows the format to efficiently store models with varying complexity without wasting space on unused features.

The offset values are always relative to the beginning of the file, not relative to the header or any other structure. This simplifies file loading since all offsets use the same reference point.

#### 2.2.2 Binary Layout

The MDL file stores all data in binary format using little-endian byte order. Understanding the exact byte layout is essential for parsing the format correctly.

**File Header Layout:**

The file begins immediately with the studiohdr_t structure at byte offset zero. The first four bytes contain the magic number "IDST" which identifies the file as a studio model. The next four bytes store the version number as a 32-bit integer.

Following the magic number and version, the header continues with the model name stored as a 64-byte character array. This name field is null-terminated, meaning the actual name can be shorter than 64 characters with the remaining bytes set to zero.

After the name field, the header stores the file length as a 32-bit integer indicating the total size of the file in bytes. This value allows validation that the complete file was read successfully.

The header continues with position and bounding box data stored as vec3_t types. Each vec3_t consists of three 32-bit floating point values representing x, y, and z coordinates. The eye position, minimum extent, maximum extent, bounding box minimum, and bounding box maximum each occupy 12 bytes.

Following the spatial data, the remainder of the header consists of integer pairs for each data type. Each pair contains a count field followed by an offset field, both 32-bit integers. The count specifies how many items of that type exist, and the offset specifies the byte position where that data begins.

**Data Block Layout:**

Each data block consists of an array of fixed-size structures. The structures are tightly packed with no padding between array elements beyond what C struct alignment requires naturally.

For bone data, the block contains an array of mstudiobone_t structures. Each structure is 112 bytes: 32 bytes for the name, 4 bytes for the parent index, 4 bytes for flags, 24 bytes for bone controller indices, 24 bytes for default values, and 24 bytes for scale values.

For texture data, each mstudiotexture_t structure occupies 80 bytes: 64 bytes for the name, 4 bytes for flags, 4 bytes for width, 4 bytes for height, and 4 bytes for the texture data offset.

Mesh data uses triangle strip commands stored as 16-bit signed integers. The strip format encodes multiple triangles efficiently by sharing vertices between adjacent triangles. A positive value indicates the start of a new strip with that many vertices. A negative value indicates a triangle fan. A zero value terminates the strip list.

**Alignment Requirements:**

The format follows standard C struct alignment rules for x86 architecture. Integer and float fields align to 4-byte boundaries. Character arrays have no alignment requirement and pack tightly. Vector types align their first element to a 4-byte boundary.

Structures may contain padding bytes inserted by the compiler to maintain proper alignment. For example, if a structure ends with a single byte field, the compiler may add three padding bytes so the next structure in an array starts on a 4-byte boundary.

When reading the file, the loader must account for this padding. Simply reading sizeof(structure) bytes for each element handles padding correctly since the size calculation includes any compiler-inserted padding.

**Offset Calculation:**

All offset values in the header and data structures are absolute offsets from the beginning of the file. To access data at a given offset, seek to that byte position from the start of the file.

Some structures contain offset fields pointing to subsidiary data. For example, a sequence structure contains an animindex field pointing to animation data for that sequence. These offsets are also absolute file offsets, not relative to the sequence structure.

When an offset field has a value of zero, it typically indicates that data does not exist rather than pointing to the start of the file. Always check count fields before using offset fields to avoid reading nonexistent data.



#### 2.2.3 Endianness and Platform Considerations

The MDL format was designed for x86 Windows systems and makes several platform assumptions that affect portability.

**Byte Order:**

All multi-byte numeric values use little-endian byte order. In little-endian format, the least significant byte appears first in memory. For example, the integer value 305419896 (0x12345678 in hexadecimal) stores in memory as the byte sequence 0x78, 0x56, 0x34, 0x12.

The magic number "IDST" demonstrates this byte order. In ASCII, these characters have values I=0x49, D=0x44, S=0x53, T=0x54. When stored as a little-endian 32-bit integer, the byte sequence in the file is 0x49, 0x44, 0x53, 0x54, which reads as the integer value 0x54534449.

Floating-point values use IEEE 754 single-precision format with little-endian byte order. The 32-bit float representation stores the sign bit, exponent, and mantissa in a specific layout. When writing or reading floats, the four bytes appear in memory in little-endian order.

**Type Sizes:**

The format assumes specific sizes for primitive types matching the x86 architecture. The char type is 8 bits or one byte. The short type is 16 bits or two bytes. The int type is 32 bits or four bytes. The float type is 32 bits or four bytes using IEEE 754 format.

Vector types use three consecutive floats for x, y, and z components. A vec3_t occupies 12 bytes total with no padding between components.

Character arrays store null-terminated strings. The name fields in various structures allocate fixed-size buffers. Actual strings may be shorter with unused bytes set to zero.

**Platform Portability:**

The format is not portable to big-endian systems without conversion. Loading an MDL file on a big-endian system requires byte swapping all multi-byte values. Each 16-bit and 32-bit integer must have its bytes reversed. Each 32-bit float must have its bytes reversed while maintaining IEEE 754 format validity.

The format also assumes 32-bit integers and pointers, though offset fields store as integers rather than actual pointers. The format was designed before 64-bit systems became common.

No provisions exist for cross-platform compatibility. The format expects Windows x86 architecture with its specific alignment rules and type sizes. Loading on other platforms requires careful attention to these assumptions.

**Struct Alignment:**

The C compiler inserts padding bytes to align structure fields according to their type requirements. On x86, integers and floats align to 4-byte boundaries. This means if a structure has a 32-bit integer followed by a single byte, the compiler inserts three padding bytes before the next field.

The MDL format relies on the compiler's natural struct layout. Different compilers or compilation settings that alter struct packing would break compatibility. The format expects default alignment with no pragma pack directives.

When implementing a loader, use the same struct definitions and compilation settings as the original Valve SDK code to ensure binary compatibility. Alternatively, manually calculate field positions accounting for alignment rules.

#### 2.2.4 Version Differences

The MDL format version number indicates which engine and feature set the file uses. Understanding version differences is critical for loader compatibility.

**GoldSrc Version 10:**

Version 10 is the only version used by the GoldSrc engine. This version appeared in Half-Life's November 1998 release and never changed. All GoldSrc-based games including Counter-Strike 1.6, Team Fortress Classic, Day of Defeat, and Half-Life expansions use version 10.

The version 10 format includes skeletal animation, multiple textures, body parts, attachments, hit boxes, bone controllers, and animation events. All features documented in this specification apply to version 10.

Version 10 files use the magic number "IDST" and version number 10 stored as a 32-bit integer with value 0x0000000A in little-endian format.

**Source Engine Versions:**

The Source engine introduced with Half-Life 2 in 2004 uses completely different MDL versions numbered 44 through 49. These versions share the "IDST" magic number but have entirely different header structures and data layouts.

Source MDL files are not compatible with GoldSrc. They use different structure definitions, include new features like flexes and vertex animation, and split data across multiple files including VTX, VVD, PHY, and ANI files.

Attempting to load a Source MDL file with a GoldSrc loader will fail. The structure sizes and offsets do not match, causing incorrect data interpretation. Always check the version number after validating the magic number.

**Version Detection:**

A robust loader should validate both magic number and version. Read the first four bytes and verify they equal "IDST" in ASCII or 0x54534449 as a little-endian integer. Then read the next four bytes as a 32-bit integer and check the version.

If the version equals 10, proceed with GoldSrc parsing. If the version is 44 or higher, the file is a Source engine model and requires completely different parsing logic. Any other version number indicates an unknown or corrupted file.

**No Sub-versions:**

Version 10 has no sub-versions or revisions. The format remained completely stable from 1998 through all GoldSrc updates. Valve never incremented the version or modified the binary structure.

This stability allowed community tools developed for early Half-Life releases to work with all subsequent GoldSrc games. Mods created in 1999 still load correctly in current GoldSrc builds.

**Forward Compatibility:**

The GoldSrc format has no forward compatibility mechanism. Later versions of the format are completely incompatible. A GoldSrc loader cannot load Source models, and a Source loader cannot load GoldSrc models despite the shared file extension.

The lack of version evolution reflects the format's age. Modern formats typically include versioning schemes allowing new features while maintaining backward compatibility. The MDL format predates these practices.



### 2.3 MDL File Components

#### 2.3.1 Main Header (studiohdr_t)

The main header contains all essential metadata about the model and serves as the entry point for accessing every data block in the file. This structure appears at file offset zero and must be read first before accessing any other data.

**Header Structure:**

The header begins with the magic number and version fields for format identification. Following these identification fields, the header stores the model's internal name as a 64-byte null-terminated string. The file length field contains the total size of the MDL file in bytes, allowing validation that the entire file loaded correctly.

Spatial data follows the identification and size fields. The eye position defines where the camera should be located for first-person view models. The minimum and maximum extents define the overall size of the model geometry. The bounding box minimum and maximum define a volume used for visibility culling and collision detection.

The flags field contains bit flags controlling various model properties and rendering behaviors. These flags affect how the engine handles the model during rendering and gameplay.

The remainder of the header consists of count and offset pairs for each data type the format supports. Each pair contains a count field specifying how many items of that type exist, followed by an offset field indicating where in the file that data begins. If a count is zero, that feature is not present in the model and the corresponding offset value should be ignored.

**Data Type Organization:**

The header provides access to bones through the numbones and boneindex fields. Bones form the skeletal hierarchy that drives all animation in the model.

Bone controllers are accessed via numbonecontrollers and bonecontrollerindex. These allow runtime manipulation of specific bones for effects like mouth movement or eye tracking.

Hit boxes use numhitboxes and hitboxindex to define collision volumes attached to bones for damage detection and hit registration.

Sequences are accessed through numseq and seqindex. Each sequence represents an animation clip with its frame data and metadata.

Sequence groups use numseqgroups and seqgroupindex to reference external files containing additional animations, allowing large animation sets without loading everything into memory simultaneously.

Textures use numtextures, textureindex, and texturedataindex. The texture count and initial offset point to texture metadata structures, while the data index points to the actual pixel data if textures are embedded.

Skin references use numskinref, numskinfamilies, and skinindex. The skin system allows different texture variations without duplicating geometry.

Body parts use numbodyparts and bodypartindex to group sub-models for selectable model variations.

Attachments use numattachments and attachmentindex to define points where other entities or effects can attach to the model.

Transitions use numtransitions and transitionindex to define allowed animation transitions for smooth blending between sequences.

**Unused Fields:**

Several fields in the header exist but are unused in GoldSrc. The soundtable, soundindex, soundgroups, and soundgroupindex fields were part of an abandoned sound system design. These fields should be ignored when reading files and set to zero when writing files.

#### 2.3.2 Bones and Skeleton

Bones define the skeletal structure that animates the model. Each bone stores its name, parent relationship, and transformation data. Together, bones form a hierarchy tree with one or more root bones.

**Bone Structure:**

Each bone contains a 32-byte name field storing a null-terminated string identifier. This name allows animation files to match bones between the model and animation data.

The parent field stores an integer index pointing to the parent bone in the bone array. Root bones have a parent value of negative one, indicating they have no parent. All other bones reference their parent by array index.

The flags field contains bit flags controlling bone behavior. The specific flag meanings are defined in the studio.h header file.

The bonecontroller array contains six integer indices corresponding to the six degrees of freedom for a bone: translation in x, y, and z, plus rotation around x, y, and z. Each value is either an index into the bone controller array or negative one indicating no controller affects that degree of freedom.

The value array contains six float values providing default transformation data for the bone. The first three values specify position, and the last three specify rotation angles.

The scale array contains six float values used as scaling factors when applying animation data to the bone.

**Hierarchy Formation:**

Bones form a tree structure through their parent relationships. To find the root bones, search for all bones with parent set to negative one. Each bone can have multiple children but only one parent.

Transforming a bone requires walking up the hierarchy chain. Start with the bone's local transformation, multiply by its parent's transformation, and continue multiplying by ancestor transformations until reaching a root bone. The final result is the bone's position and orientation in world space.

Most character models use a single root bone at the pelvis or hips with the spine, legs, and other body parts as children. Weapon models often use a root bone at the grip with barrel, stock, and magazine as children.

#### 2.3.3 Body Parts and Sub-models

Body parts enable model variations by grouping alternative sub-models. Each body part contains one or more sub-models representing different options for that part. Only one sub-model per body part can be active at a time.

**Body Part Structure:**

Each body part has a 64-byte name field identifying the part. Common names include head, torso, legs, or weapon.

The nummodels field specifies how many sub-model variations exist for this body part. Each variation represents a different visual option.

The base field is used in calculations to determine which sub-model should be displayed based on a body value. The engine uses this value combined with the body value to select the appropriate sub-model.

The modelindex field contains an offset pointing to the array of sub-model structures for this body part.

**Sub-model Structure:**

Each sub-model has its own 64-byte name field. This name typically describes the variation, such as "head_gasmask" or "weapon_mp5".

The type field exists but is unused in GoldSrc models.

The boundingradius field stores a float value used for culling calculations.

The nummesh and meshindex fields provide access to the mesh array for this sub-model. Each sub-model can contain multiple meshes, with each mesh using a different texture.

The numverts, vertinfoindex, and vertindex fields provide access to vertex position data. Vertex positions are stored as three-component floating point vectors.

The numnorms, norminfoindex, and normindex fields provide access to vertex normal data. Normals are also stored as three-component vectors.

The numgroups and groupindex fields exist but are unused.

**Body Value Calculation:**

The engine calculates which sub-models to display using a body value. This integer value encodes the selection for all body parts simultaneously. The engine decodes this value using the base field from each body part to determine the active sub-model index.

This system allows characters to have multiple customizable parts with different options for each part while storing all variations in a single model file.

#### 2.3.4 Meshes and Geometry

Meshes store the actual triangle geometry that makes up the visible model. Each sub-model contains one or more meshes, with each mesh typically corresponding to a different texture or material.

**Mesh Structure:**

The numtris field specifies how many triangles exist in this mesh. However, the triangle data is stored as triangle strips rather than individual triangles, so this count represents the expanded triangle count after processing the strip commands.

The triindex field contains an offset pointing to the triangle strip command data. These commands encode the geometry in a space-efficient format.

The skinref field stores an index into the skin reference table, which maps to a texture index. This indirection allows skin families to swap textures without modifying the mesh data.

The numnorms field and normindex field relate to per-triangle normal data but are unused in the final GoldSrc implementation.

**Triangle Strip Format:**

Triangle strips reduce memory usage by sharing vertices between adjacent triangles. A strip begins with a vertex count, then lists that many vertices. Each vertex after the first two forms a triangle with the previous two vertices.

The strip commands are stored as 16-bit signed integers. A positive value indicates the start of a new strip and specifies how many vertices follow. A negative value indicates a triangle fan. A zero value marks the end of all strips for this mesh.

After each strip command indicating vertex count, that many vertex entries follow. Each entry consists of a vertex index referencing the position array, a normal index referencing the normal array, texture coordinate s, and texture coordinate t.

The vertex and normal indices are 16-bit integers. The texture coordinates are 16-bit integers that must be divided by the texture width and height to get normalized coordinates between zero and one.

**Vertex Data Organization:**

Vertex positions are stored in a separate array referenced by the sub-model's vertindex offset. Each position is a vec3_t consisting of three 32-bit floats for x, y, and z coordinates.

Vertex normals are similarly stored in a separate array referenced by normindex. Each normal is also a vec3_t with x, y, and z components.

The vertex info array contains byte values indicating which bone each vertex is attached to. This allows the engine to transform vertices based on bone animations.

#### 2.3.5 Textures and Materials

Textures can be embedded directly in the MDL file or stored in separate external files. The texture structures define texture properties, dimensions, and either contain pixel data or reference external files.

**Texture Structure:**

Each texture has a 64-byte name field. For internal textures, this name identifies the texture within the file. For external textures, this name specifies the filename to load.

The flags field contains bit flags controlling texture rendering behavior. Common flags include chrome for environment-mapped reflection, additive for additive blending, masked for alpha testing, and flatshade for ignoring lighting.

The width and height fields specify the texture dimensions in pixels. Half-Life textures typically use power-of-two dimensions up to 512x512 pixels, though some engines support larger sizes.

The index field contains an offset pointing to the texture data. For internal textures, this points to the palette and pixel data within the MDL file. For external textures, this field is unused and the texture loads from a separate file.

**Internal Texture Format:**

Internal textures use 8-bit indexed color. The texture data begins with a 256-entry palette where each entry contains three bytes for red, green, and blue values. Following the palette are width times height bytes of pixel data, where each byte is an index into the palette.

This indexed format was chosen for memory efficiency in 1998 when texture memory was severely limited. Converting indexed pixels to RGB at runtime requires looking up each pixel's palette entry and using those RGB values.

**External Texture Format:**

External textures are stored in files with the same base name as the MDL file but with a "T" appended before the extension. For example, if the model is named "player.mdl", the texture file would be "playerT.mdl".

The external texture file format is identical to internal texture format but stored in a separate file. This separation allows texture updates without recompiling the entire model.

**Texture Flags:**

Chrome textures use environment mapping to simulate reflective metal surfaces. The engine generates texture coordinates based on the viewing angle to create the illusion of reflection.

Additive textures blend with the background using additive blending. Black pixels become transparent, and brighter pixels add light. This effect is used for glows, muzzle flashes, and other light-emitting effects.

Masked textures use the last color in the palette as transparent. Any pixel with an index of 255 becomes fully transparent, while all other pixels are fully opaque. This provides simple binary transparency.

Flatshade textures ignore lighting calculations and render at full brightness regardless of light conditions. This flag is useful for self-illuminated surfaces.

#### 2.3.6 Animations and Sequences

Sequences define animation clips that play back over time. Each sequence contains frame data for animating bones, along with metadata controlling playback behavior and triggering events.

**Sequence Structure:**

The label field contains a 32-byte name identifying the sequence. Common names include idle, walk, run, attack, reload, and death.

The fps field specifies frames per second for playback. This determines how fast the animation plays. Typical values range from 10 to 30 fps.

The flags field contains bit flags controlling sequence behavior such as looping.

The activity and actweight fields classify the sequence by gameplay purpose, allowing the game code to select appropriate animations for character states.

The numevents and eventindex fields provide access to animation events that trigger at specific frames. Events can play sounds, spawn particle effects, or notify game code of important animation moments.

The numframes field specifies how many frames of animation exist in this sequence. Frame numbers start at zero and go up to numframes minus one.

The numpivots and pivotindex fields exist but are unused in GoldSrc.

The motiontype and motionbone fields control motion extraction, where movement encoded in the animation is transferred to the entity's world position.

The linearmovement field specifies how far the entity should move per frame when motion extraction is active.

The bbmin and bbmax fields define a bounding box that encompasses the model throughout this animation.

The numblends field specifies how many blend variations exist. Blending allows smooth transitions between similar animations or player-controlled aiming.

The animindex field contains an offset pointing to the actual animation data for this sequence.

The blendtype, blendstart, and blendend fields control animation blending behavior for sequences that support it.

The seqgroup field specifies which sequence group contains the animation data. Zero indicates the main file, while other values indicate external sequence files.

The entrynode, exitnode, and nodeflags fields control automatic transitions between sequences.

The nextseq field specifies which sequence should play after this one completes, enabling automatic sequence chains.

**Animation Data Format:**

Animation data uses delta compression to minimize size. For each bone in each frame, the animation stores only position and rotation values that differ from the default. Bones that don't move in a sequence store no data.

The compressed format stores values as 16-bit integers rather than 32-bit floats, using the scale values from the bone structure to convert back to float coordinates during playback.

#### 2.3.7 Bone Controllers

Bone controllers provide runtime control over specific bones or bone properties. These allow the game code to adjust bone positions dynamically without defining new animation sequences.

**Bone Controller Structure:**

The bone field specifies which bone this controller affects. A value of negative one indicates the controller modifies mouth movement for speech rather than a specific bone.

The type field specifies what the controller modulates: X translation, Y translation, Z translation, X rotation, Y rotation, or Z rotation.

The start and end fields define the range of values the controller can produce. Input values between 0 and 255 map linearly to output values between start and end.

The rest field specifies the controller value when at rest or neutral position.

The index field identifies which controller this is. Values 0 through 3 are general-purpose controllers that game code can set. Value 4 is reserved for mouth movement during speech.

**Controller Usage:**

Game code sets controller values as bytes from 0 to 255. The engine maps these values to the start-to-end range and applies the result to the specified bone's degree of freedom.

Mouth controllers allow characters to move their jaw when speaking without requiring separate animation sequences for every phoneme.

Eye controllers can aim the character's eyes independently of the head bone, making characters appear to track moving objects.

#### 2.3.8 Attachments

Attachments define named points fixed to bones that follow those bones through animation. Other entities or effects can attach to these points.

**Attachment Structure:**

The name field contains a 32-byte identifier for the attachment. Common names include muzzle for weapon flash points, eyes for view position, and hand for weapon placement.

The type field exists but is unused.

The bone field specifies which bone this attachment follows. As the bone transforms during animation, the attachment point transforms with it.

The org field contains a vec3_t offset from the bone's origin. This offset is in the bone's local coordinate space.

The vectors array contains three vec3_t values defining the attachment's local coordinate axes. These vectors specify the attachment's orientation relative to the bone.

**Attachment Usage:**

First-person weapon models use attachments to specify where muzzle flashes appear when firing. The engine spawns a temporary sprite at the attachment point.

Character models use attachments to specify where the view camera should be positioned for first-person view.

Weapon attachment points on character hands allow the engine to position weapon models correctly when held.

#### 2.3.9 Hit Boxes

Hit boxes define collision volumes attached to bones for damage detection. When a projectile hits the model, the engine checks which hit box was struck and applies damage accordingly.

**Hit Box Structure:**

The bone field specifies which bone this hit box follows. The box transforms with the bone during animation, ensuring proper hit detection regardless of the character's pose.

The group field categorizes the hit box for damage multiplier purposes. Common groups include head for headshots, chest for torso hits, and legs for leg hits. Different groups can apply different damage multipliers.

The bbmin and bbmax fields define the box extents relative to the bone's origin. These vec3_t values specify the minimum and maximum corners of an axis-aligned bounding box.

**Hit Box Usage:**

The engine checks hit boxes when calculating bullet impacts, explosion damage, and melee attacks. A hit on a head group hit box typically deals more damage than a hit on a leg group hit box.

Character models typically use between six and twelve hit boxes covering the head, torso, upper arms, lower arms, upper legs, and lower legs.

#### 2.3.10 Events

Animation events trigger game code execution at specific frames during animation playback. These synchronize sounds, particle effects, and gameplay mechanics with animation timing.

**Event Structure:**

The frame field specifies when the event triggers. Frame numbers correspond to the sequence's frame timeline, with zero being the first frame.

The event field contains an integer code identifying the event type. The engine's game code interprets these codes to determine what action to perform.

The type field exists but is unused in GoldSrc.

The options field contains a 64-byte string providing parameters for the event. For sound events, this might contain the sound filename. For effect events, this might specify effect parameters.

**Common Event Types:**

Footstep events play walking or running sounds synchronized with foot placement in the animation. The options field specifies which footstep sound to play.

Muzzle flash events trigger weapon firing effects at the exact frame when the weapon discharges in the animation.

Shell ejection events spawn shell casings from the weapon at the appropriate animation frame.

Sound events play arbitrary sounds at specific animation moments, such as weapon reloading sounds or character vocalizations.

**Event Timing:**

The engine checks events every frame during sequence playback. When the current frame number equals or passes an event's frame number, the engine triggers that event exactly once. Events do not retrigger if the animation loops unless the frame counter wraps back to the beginning.

#### 2.3.11 Sequence Groups

Sequence groups allow animations to be stored in external files rather than embedding everything in the main MDL file. This enables large animation sets without loading all data into memory simultaneously.

**Sequence Group Structure:**

The label field contains a 32-byte name identifying the group. This name is primarily for documentation and debugging purposes.

The name field contains a 64-byte filename for the external file containing this group's animation data. The filename follows the pattern of the base model name plus a two-digit group number.

**External Sequence Files:**

External sequence files use the naming convention modelname.mdl## where ## is a two-digit number. For example, if the main model is player.mdl, external groups would be player.mdl01, player.mdl02, and so on.

Each external file contains a studioseqhdr_t header followed by animation data for sequences assigned to that group. The file format mirrors the main MDL structure but contains only animation data.

**Sequence Group Assignment:**

Each sequence specifies which group contains its animation data via the seqgroup field. A value of zero indicates the sequence data is in the main MDL file. Other values indicate external group files.

The engine loads external sequence files on demand when a sequence from that group begins playback. This lazy loading reduces memory usage for models with many animations.



### 2.4 Coordinate Systems and Transformations

#### 2.4.1 Half-Life Coordinate System

Half-Life uses a left-handed coordinate system inherited from the Quake engine. Understanding this coordinate system is essential for correctly interpreting and transforming model data.

The X axis points forward or east. Positive X values move forward in the direction the model faces.

The Y axis points left or north. Positive Y values move to the left when facing the positive X direction.

The Z axis points up. Positive Z values move vertically upward.

This forms a left-handed system where if you point your left hand's fingers along the positive X axis and curl them toward the positive Y axis, your thumb points in the positive Z direction.

Rotations follow the left-hand rule. Positive rotation values rotate clockwise when looking down the axis of rotation. For example, a positive rotation around the Z axis rotates from the positive X axis toward the positive Y axis.

#### 2.4.2 OpenGL Coordinate System

OpenGL uses a right-handed coordinate system by default, which differs from Half-Life's left-handed system. Converting between these systems requires careful coordinate transformation.

In OpenGL's default configuration, the X axis points right, the Y axis points up, and the Z axis points toward the viewer. This orientation is standard for 3D graphics but incompatible with Half-Life's coordinate system.

The right-handed system means if you point your right hand's fingers along the positive X axis and curl them toward the positive Y axis, your thumb points in the positive Z direction.

Rotations in a right-handed system follow the right-hand rule. Positive rotations are counter-clockwise when looking down the axis of rotation.

#### 2.4.3 Coordinate Transformation

Converting from Half-Life's coordinate system to OpenGL requires swapping and negating certain axes. The specific transformation depends on the desired OpenGL camera orientation.

A common transformation swaps the Y and Z axes and negates the new Z axis. This makes Half-Life's up axis become OpenGL's up axis while adjusting the forward direction appropriately.

The transformation can be expressed as a matrix multiplication where the transformation matrix converts Half-Life coordinates to OpenGL coordinates. Applying this matrix to all vertex positions and normal vectors ensures correct rendering.

Bone transformations must also account for the coordinate system change. When calculating bone positions and rotations, apply the coordinate transformation after computing the final bone matrix but before rendering.

#### 2.4.4 Rotation and Scaling

Rotations in Half-Life use Euler angles stored as three floating-point values representing rotation around the X, Y, and Z axes. The rotation order affects the final orientation and must be applied consistently.

Scaling transformations multiply position values by scale factors. The bone scale array contains scaling factors used when applying animation data. These factors convert compressed integer values back to floating-point coordinates.

When combining transformations, the order matters. Typically, transformations apply in the order: scale, rotate, translate. This ensures scaling affects the model's shape, rotation affects orientation, and translation positions the result in space.



### 2.5 Animation System

#### 2.5.1 Skeletal Animation Overview

Half-Life uses skeletal animation where bones deform the mesh. This differs from vertex animation where each frame stores complete vertex positions. Skeletal animation provides smoother motion with smaller file sizes.

The skeleton consists of bones arranged in a hierarchy. Each bone has a parent except for root bones. Transforming a bone affects all its children.

Animation data stores bone positions and rotations for each frame. The engine interpolates between frames to create smooth motion at any playback speed.

Vertices attach to bones through the vertex info array. Each vertex specifies which bone controls its position. When the bone moves during animation, the attached vertices move accordingly.

#### 2.5.2 Bone Hierarchy

The bone hierarchy forms a tree structure starting from one or more root bones. Root bones have no parent and serve as the top of the hierarchy.

Each non-root bone specifies its parent through the parent index. This creates parent-child relationships where transforming a parent automatically affects all children.

Calculating a bone's world-space position requires walking up the hierarchy. Start with the bone's local transformation, multiply by the parent's transformation, and continue multiplying up to the root. The accumulated transformation gives the bone's final position and orientation.

This hierarchical system allows natural motion. When a shoulder bone rotates, the entire arm rotates with it because the elbow and wrist bones are children of the shoulder.


#### 2.5.3 Animation Sequences

Sequences define complete animation clips with metadata controlling playback behavior. Each sequence specifies frame count, playback speed, events, and blending parameters.

Sequences can loop continuously for idle animations or play once for actions like attacks. The flags field controls looping behavior.

The frame count determines animation duration. At 30 frames per second, a 90-frame sequence lasts three seconds.

Multiple sequences can share frames through the sequence group system, reducing memory usage when similar animations differ only slightly.

#### 2.5.4 Frame Data and Interpolation

Frame data stores bone transformations at specific points in time. The format uses delta compression where only bones that move store data.

Each frame contains position and rotation values for moving bones. Static bones store no frame data, saving significant space.

The engine interpolates between frames during playback. If the current time falls between frame 10 and frame 11, the engine calculates interpolated bone positions based on how far between those frames the current time is.

Linear interpolation provides smooth motion. The interpolated value equals the first frame value plus the difference between frames multiplied by the interpolation factor.

#### 2.5.5 Blending and Transitions

Some sequences support blending where two animations mix to create intermediate poses. Aiming animations often use blending to smoothly transition between aiming up and aiming down.

The blend type specifies which parameter controls blending. Common blend types include up-down for vertical aiming and left-right for horizontal aiming.

Blend values range from the blend start to blend end. The engine selects and mixes appropriate animation frames based on the current blend value.

Transitions between sequences can be automatic or manual. The entry node and exit node fields define allowed transitions, ensuring animations connect smoothly without jarring jumps.

#### 2.5.6 Animation Events

Events synchronize animation with gameplay mechanics. Common events include playing sounds, spawning effects, and triggering gameplay code.

Events specify the frame number when they trigger. The engine checks events each frame and fires any events whose frame number has been reached.

Event parameters in the options string provide flexibility. A footstep event might specify different sounds for different surfaces.

Events fire once per playback unless the sequence loops back past the event frame. Looping animations retrigger events on each loop.



### 2.6 Texture System

#### 2.6.1 Internal vs External Textures

Textures can be embedded in the MDL file or stored externally. Internal textures increase file size but ensure all data is in one file. External textures reduce the main file size and allow texture updates without recompiling the model.

The texture structure's index field determines texture location. Non-zero values point to internal texture data. External textures rely on the filename in the name field.

Internal textures are common for simple models with few textures. External textures are preferred for complex models or when artists need to update textures frequently.

#### 2.6.2 Texture File Format (.mdl T files)

External texture files append a T to the model's base filename before the extension. The player.mdl model uses playerT.mdl for textures.

External texture files contain the same data as internal textures: a palette followed by indexed pixel data. The format is identical whether textures are internal or external.

Multiple models can share external texture files by referencing the same filename. This reduces disk space when several models use identical textures.

#### 2.6.3 Palette-Based Colors

Half-Life textures use 8-bit indexed color with 256-color palettes. Each pixel stores a palette index rather than direct RGB values.

The palette contains 256 entries with three bytes each for red, green, and blue. Total palette size is 768 bytes.

Pixel data follows the palette. Each pixel is one byte indexing into the palette. To get a pixel's color, look up the palette entry at that index.

This format was chosen for memory efficiency. In 1998, video cards had limited texture memory. Indexed color uses one-third the memory of 24-bit RGB.

#### 2.6.4 UV Mapping

UV coordinates specify how textures map onto geometry. Each vertex in a triangle strip includes s and t texture coordinates.

Coordinates are stored as 16-bit integers requiring division by texture width and height to normalize. An s value of 256 on a 512-wide texture becomes 0.5 after division.

Original Half-Life studiomdl clamped UV coordinates to the 0-1 range, preventing texture tiling. Later compilers like Sven Co-op's version support tiling by allowing coordinates outside this range.

UV coordinates can be negative or greater than one if the compiler supports tiling. The texture repeats when coordinates exceed the 0-1 bounds.

#### 2.6.5 Skin Families

Skin families allow multiple texture variations without duplicating geometry. Each family specifies a different texture for each mesh.

The skin reference table maps mesh skinref values to actual texture indices. Different families use different mappings, changing which textures appear on which meshes.

The table is stored as a two-dimensional array with numskinfamilies rows and numskinref columns. Each entry is a 16-bit texture index.

Changing skin families at runtime instantly swaps all textures to the selected family's mapping. This enables character customization and team colors without additional models.

### 2.7 Advanced Features

#### 2.7.1 Level of Detail (LOD)

GoldSrc MDL format does not natively support level of detail systems. Models use the same geometry regardless of distance from camera.

Third-party engines extending GoldSrc sometimes add LOD support, but the base format lacks this feature. Source engine added proper LOD support with multiple geometry versions per model.

#### 2.7.2 Bone Controllers

Bone controllers provide runtime control over specific bones. Covered in detail in Section 2.3.7.

Controllers allow game code to adjust bone positions dynamically without creating new animation sequences. Mouth movement during speech is the most common use case.

#### 2.7.3 Attachments and Muzzle Flashes

Attachments define points following bones through animation. Covered in detail in Section 2.3.8.

Muzzle flash attachments specify where weapon firing effects appear. The engine creates temporary sprites at attachment positions when weapons fire.

#### 2.7.4 Hit Boxes for Collision

Hit boxes enable accurate hit detection for damage calculations. Covered in detail in Section 2.3.9.

Boxes follow bones during animation, ensuring hits register correctly regardless of character pose. Different box groups apply different damage multipliers.

#### 2.7.5 Bounding Boxes

Bounding boxes define volumes enclosing the model for culling and collision. The main header contains overall bounding box minimum and maximum extents.

Each sequence also has its own bounding box encompassing the model throughout that animation. This allows per-animation culling optimization.

Bounding boxes are axis-aligned and specified by minimum and maximum corner coordinates. The engine uses these for visibility culling and rough collision detection.

### 2.8 File Dependenices

#### 2.8.1 Required Files

The main MDL file is always required. This file contains the header and may contain all other data.

If the model uses external textures, the texture file with T suffix is required. Without it, textures will not display.

If the model uses external sequence groups, those numbered files are required to play animations in those groups.

#### 2.8.2 Optional Files

External texture files are optional if textures are embedded internally.

External sequence group files are optional if all sequences are in the main file.

#### 2.8.3 Sequence Group Files (.mdl##)

Sequence group files follow the naming pattern basename.mdl## where ## is a two-digit number from 01 to 16.

Each file contains a studioseqhdr_t header and animation data for sequences assigned to that group.

The engine loads these files on demand when playing sequences from that group.

#### 2.8.4 Texture Files (.mdl T)

Texture files use the naming pattern basenameT.mdl where basename matches the main model filename.

These files contain palette and pixel data for all external textures in the model.

### 2.9 Creating and Compiling MDL Files

#### 2.9.1 Source Formats

SMD files are the source format for model geometry and animations. These are text files containing vertex data, skeleton definitions, and animation frames.

BMP files provide textures. Must be 8-bit indexed color with resolution multiples of 8, preferably 16.

QC files are scripts directing the studiomdl compiler. These specify which SMD files to use, texture settings, sequence definitions, and compilation options.

#### 2.9.2 QC Script Format

QC files use a simple command-based syntax. Each line contains a command followed by parameters.

Common commands include modelname for output file, body or model for geometry, sequence for animations, and texture commands for material properties.

Commands are case-insensitive. Comments begin with double slashes.

#### 2.9.3 StudioMDL Compiler

StudioMDL is Valve's official compiler converting SMD and BMP files into MDL format.

The compiler reads the QC script, loads referenced files, and outputs the binary MDL file plus any external texture or sequence files.

Compile by running studiomdl with the QC filename as an argument.

#### 2.9.4 Common Compilation Errors

Exceeding vertex or triangle limits causes compilation failure. Split meshes into multiple SMD files if limits are exceeded.

Incorrect texture format or size causes errors. Textures must be 8-bit BMP with dimensions that are multiples of 8.

Mismatched bone names between reference and animation SMD files cause bone assignment errors.

Missing or misspelled filenames in the QC cause file not found errors.

### 2.10 Tools and Utillities

#### 2.10.1 Official Valve Tools

StudioMDL compiles models from source files.

Half-Life SDK includes studio.h with structure definitions and sample code.

#### 2.10.2 Third-Party Tools

Jed's Half-Life Model Viewer displays and edits MDL files.

Crowbar decompiles MDL files back to SMD format.

MilkShape 3D imports and exports SMD files.

Blender with Source Tools plugin supports MDL workflow.

#### 2.10.3 Decompilers

MDLDec and similar tools extract SMD files and textures from compiled MDL files.

Decompilation allows editing existing models without original source files.

Decompiled models may require QC adjustments before recompiling correctly.

####  2.10.4 Model Viewers

HLMV (Half-Life Model Viewer) displays models with animation playback, texture viewing, and basic editing.

Allows viewing bone structure, sequences, hit boxes, and attachments.

Supports model recompilation and texture flag editing.

### 2.11 Common Issues and Troubleshooting

#### 2.11.1 Corrupted Files

Invalid magic number indicates file is not an MDL or is corrupted. Verify file integrity.

Version mismatch causes loading failure. Ensure using GoldSrc-compatible version 10.

Truncated files have incorrect length. File may not have downloaded or copied completely.

#### 2.11.2 Missing Textures

Pink and black checkerboard pattern indicates texture not found.

Verify external texture file exists if model uses external textures.

Check texture name in model matches actual texture name.

#### 2.11.3 Animation Problems

Jerky animation may indicate incorrect FPS setting or missing frames.

Bones not animating suggests bone controller issues or incorrect bone names.

Animation playing too fast or slow indicates FPS value is incorrect.

#### 2.11.4 Compatibility Issues

GoldSrc and Source MDL files are incompatible despite sharing file extension.

Always check version number to ensure correct format.

Modified engines may have different limits or features than vanilla GoldSrc.

### 2.12 References and Resources

#### 2.12.1 Official Documentation

Valve Developer Community wiki MDL format pages

Half-Life SDK studio.h header file

Official GoldSrc SDK documentation

#### 2.12.2 Community Resources

The303's GoldSrc modelling tutorials

TWHL tutorials and documentation

Sam Vanheer's HalfLifeModelViewer2 source code

#### 2.12.3 Research Papers

None specific to MDL format. Format was reverse-engineered by community.

#### 2.12.4 Source Code References

ValveSoftware/halflife GitHub repository

HalfLifeModelViewer2 by Sam Vanheer

Various community decompilers and tools
