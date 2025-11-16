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

---

## 3. Codebase Architecture Overview

### 3.1 Project Structure

#### 3.1.1 Directory Layout

The Lambda Model Viewer codebase is organized into a modular structure with clear separation of concerns:

```
ModelViewer/
├── build/                             # CMake build artifacts (generated)
├── docs/                              # Documentation
│   ├── API_REFERENCE.md               # Complete API documentation (this file)
│   ├── DEVELOPMENT_INFO.md            # Development notes and guides
│   └── texture_format.txt             # MDL texture format specification
├── shaders/                           # OpenGL GLSL shaders
│   ├── basic.vert/.frag               # Basic unlit shaders
│   ├── textured.vert/.frag            # Textured model shaders with lighting
│   └── debug.vert                     # Debug visualization shader
├── src/                               # All source code
│   ├── cl/                            # Client application layer
│   │   ├── cl_app.c/h                 # Main application state management
│   │   ├── cl_app_config.c/h          # Configuration management
│   │   └── cl_app_init.c/h            # Application initialization
│   ├── input/                         # Input handling system
│   │   ├── input.c/h                  # Low-level input state tracking
│   │   ├── input_handler.c/h          # High-level input event handlers
│   │   └── input_types.h              # Input-related type definitions
│   ├── math/                          # Math library (wrapper over CGLM)
│   │   ├── math_angles.c/h            # Euler angle conversions
│   │   ├── math_matrix.c/h            # Matrix operations (3x4, 4x4)
│   │   ├── math_quaternion.c/h        # Quaternion operations & SLERP
│   │   ├── math_types.h               # Type definitions (vec3, mat4, quat)
│   │   ├── math_utils.c/h             # Utility functions (clamp, lerp, etc.)
│   │   └── math_vector.c/h            # Vector operations
│   ├── mdl/                           # MDL file loading and manipulation
│   │   ├── mdl_animations.c/h         # Animation playback & interpolation
│   │   ├── mdl_bodypart.c/h           # Bodypart selection management
│   │   ├── mdl_bones.c/h              # Bone system & transformations
│   │   ├── mdl_info.c/h               # Model information extraction
│   │   ├── mdl_loader.c/h             # MDL file parsing & loading
│   │   └── mdl_report.c/h             # Model data dumping/reporting
│   ├── r/                             # Renderer subsystem
│   │   ├── r_camera.c/h               # Camera system (orbit, zoom)
│   │   ├── r_draw.c/h                 # Main rendering pipeline
│   │   ├── r_gl_platform.h            # OpenGL platform abstraction
│   │   └── r_textures.c/h             # Texture loading & management
│   ├── shaders/                       # Shader compilation utilities
│   │   └── shader.c/h                 # Shader loading & compilation
│   ├── util/                          # Utility systems
│   │   ├── util_args.c/h              # Command-line argument parsing
│   │   ├── util_logger.c/h            # Logging system
│   │   ├── util_logger_categories.h   # Log category definitions
│   │   ├── util_messages.c/h          # User-facing messages
│   │   └── util_utils.c/h             # Miscellaneous utilities
│   ├── main.c                         # Application entry point
│   ├── platform.h                     # Platform detection macros
│   ├── studio.h                       # Half-Life MDL format structures
│   └── version.h                      # Version information
├── CMakeLists.txt                     # CMake build configuration
├── CHANGELOG.md                       # Version history and changes
├── LICENSE                            # Valve SDK Non-Commercial License
└── README.md                          # Project overview and build instructions
```

**Key Observations:**
- **Modular design**: Each subsystem is contained in its own directory
- **Clear naming**: Prefixes indicate module ownership (cl_, r_, mdl_, util_, math_)
- **Separation of concerns**: Rendering, model loading, math, and utilities are independent
- **Header/implementation pairs**: Each .c file has corresponding .h for public API

#### 3.1.2 Module Organization

The codebase is organized into 7 primary modules:

**1. Client Layer (cl/)**
- **Purpose**: High-level application state and lifecycle management
- **Responsibilities**: App initialization, configuration, main loop coordination
- **Key files**: `cl_app.c`, `cl_app_init.c`, `cl_app_config.c`

**2. Input System (input/)**
- **Purpose**: Handle keyboard, mouse, and window events
- **Responsibilities**: Raw input capture, event dispatching, camera/animation controls
- **Key files**: `input.c`, `input_handler.c`

**3. Math Library (math/)**
- **Purpose**: Centralized linear algebra operations
- **Responsibilities**: Vector, matrix, quaternion math; angle conversions; interpolation
- **Design**: Wrapper layer over CGLM for consistent API
- **Key files**: `math_matrix.c`, `math_quaternion.c`, `math_vector.c`, `math_angles.c`

**4. MDL Subsystem (mdl/)**
- **Purpose**: Load, parse, and manipulate Half-Life MDL files
- **Responsibilities**: File I/O, binary parsing, animation playback, bone transformations
- **Key files**: `mdl_loader.c`, `mdl_animations.c`, `mdl_bones.c`, `mdl_bodypart.c`

**5. Renderer (r/)**
- **Purpose**: OpenGL-based 3D rendering
- **Responsibilities**: Vertex buffer management, shader management, texture loading, camera
- **Key files**: `r_draw.c`, `r_textures.c`, `r_camera.c`

**6. Utilities (util/)**
- **Purpose**: Cross-cutting concerns
- **Responsibilities**: Logging, command-line parsing, error messages
- **Key files**: `util_logger.c`, `util_args.c`, `util_messages.c`

**7. Shaders (shaders/)**
- **Purpose**: Shader compilation and management
- **Responsibilities**: Load .vert/.frag files, compile, link shader programs
- **Key files**: `shader.c`

**Data Flow:**
```
main.c
  └─> util_args (parse CLI)
  └─> util_logger (init logging)
  └─> cl_app_init (init application)
      └─> r_draw (init renderer)
          └─> r_textures (init OpenGL)
      └─> mdl_loader (load model file)
          └─> mdl_bones (setup skeleton)
          └─> mdl_animations (init animation system)
      └─> input (init input handlers)
  └─> cl_app (main loop)
      └─> input (process events)
      └─> mdl_animations (update animation)
      └─> r_draw (render frame)
```

#### 3.1.3 File Naming Conventions

**Module Prefixes:**
- `cl_*` - Client application layer
- `r_*` - Renderer subsystem
- `mdl_*` - MDL file handling
- `math_*` - Math library
- `util_*` - Utilities
- `input_*` - Input system

**Common Suffixes:**
- `*_init.c` - Initialization functions
- `*_config.c` - Configuration management
- `*_types.h` - Type definitions only (no functions)
- `*_platform.h` - Platform-specific abstractions

**Special Files:**
- `studio.h` - Valve's official MDL format definitions (not our code)
- `platform.h` - Platform detection macros
- `version.h` - Build version information
- `main.c` - Entry point (no prefix)

### 3.2 Dependency Graph

#### 3.2.1 External Dependencies

Lambda Model Viewer relies on the following third-party libraries:

**1. GLFW 3.x**
- **Purpose**: Cross-platform window creation and input handling
- **Why chosen**: Industry standard, excellent OpenGL integration, active development
- **Usage**: Window management, keyboard/mouse input, OpenGL context creation
- **License**: zlib/libpng (permissive)
- **Platform support**: macOS, Linux, Windows

**2. OpenGL 3.3+ / 4.1 Core Profile**
- **Purpose**: Hardware-accelerated 3D rendering
- **Why chosen**: Mature, cross-platform, no alternative for low-level graphics
- **macOS**: Limited to OpenGL 4.1 (Apple deprecated OpenGL)
- **Linux/Windows**: OpenGL 4.5+ typically available
- **Features used**: VBOs, VAOs, GLSL shaders, textures, depth testing

**3. GLEW (Linux/Windows only)**
- **Purpose**: OpenGL extension loading
- **Why chosen**: Required on Windows/Linux to access modern OpenGL functions
- **macOS**: Not needed (native OpenGL framework provides function pointers)
- **License**: BSD-style (permissive)

**4. CGLM**
- **Purpose**: C-based linear algebra library
- **Why chosen**: Pure C (no C++), column-major like OpenGL, optimized SIMD support
- **Usage**: Matrix/vector operations (internally - wrapped by our Math library)
- **License**: MIT (permissive)
- **Note**: Application code does NOT call CGLM directly - only through Math library

**Dependency Summary Table:**
| Library | Version | Required On | Purpose | Wrapped? |
|---------|---------|-------------|---------|----------|
| GLFW    | 3.3+    | All platforms | Windowing/Input | No (used directly) |
| OpenGL  | 3.3+/4.1 | All platforms | Rendering | No (used directly) |
| GLEW    | Latest  | Linux/Windows | GL Extension Loading | No (platform-specific) |
| CGLM    | Latest  | All platforms | Linear Algebra | **Yes** (via Math library) |

#### 3.2.2 Internal Module Dependencies

**Dependency Hierarchy (least dependent → most dependent):**

**Level 0 (No dependencies):**
- `studio.h` - Pure data structure definitions
- `platform.h` - Preprocessor macros only
- `version.h` - Compile-time constants
- `math_types.h` - Type definitions only

**Level 1 (Depends only on Level 0):**
- `util_logger.h` - Depends on platform macros
- `math_utils.h` - Depends on math_types.h
- `r_gl_platform.h` - Depends on platform.h

**Level 2 (Depends on Level 0-1):**
- `math_vector.c` - Depends on math_types.h
- `math_matrix.c` - Depends on math_types.h, math_vector.h
- `math_quaternion.c` - Depends on math_types.h, math_vector.h
- `math_angles.c` - Depends on math_quaternion.h, math_matrix.h
- `util_args.c` - Depends on util_logger.h
- `util_messages.c` - Depends on util_logger.h

**Level 3 (Depends on Level 0-2):**
- `mdl_loader.c` - Depends on studio.h, util_logger.h
- `r_textures.c` - Depends on r_gl_platform.h, util_logger.h, mdl_loader.h
- `r_camera.c` - Depends on math library
- `input.c` - Depends on r_gl_platform.h (GLFW)

**Level 4 (Depends on Level 0-3):**
- `mdl_bones.c` - Depends on studio.h, math library, util_logger.h
- `mdl_animations.c` - Depends on mdl_bones.h, math library, studio.h
- `mdl_bodypart.c` - Depends on studio.h, mdl_loader.h
- `shader.c` - Depends on r_gl_platform.h, util_logger.h

**Level 5 (Depends on most modules):**
- `r_draw.c` - Depends on: mdl (loader, bones, animations, bodypart), r (textures, camera), math, input, shader
- `input_handler.c` - Depends on: input.h, mdl_animations.h, mdl_bodypart.h

**Level 6 (Top-level coordination):**
- `cl_app.c` - Depends on: r_draw, mdl (all), input, util
- `cl_app_init.c` - Depends on: cl_app, r_draw, util_logger
- `main.c` - Depends on: cl_app, util_args, util_logger

**Key Architectural Rules:**
1. **Math library is independent** - No dependencies on MDL or Renderer
2. **MDL loader is independent of Renderer** - Can load models without rendering
3. **Logger is universal** - Every module can use it without circular dependencies
4. **Renderer depends on MDL** - Must know model structures to render them
5. **One-way dependencies** - No circular dependencies (e.g., MDL never imports Renderer)

#### 3.2.3 Build System (CMake)

Lambda uses CMake 3.15+ for cross-platform builds.

**Key CMakeLists.txt Features:**

**Platform Detection:**
```cmake
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")     # macOS
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")      # Linux
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")    # Windows
```

**Compiler Flags:**
- C11 standard enforced
- `-Wall -Wextra -Wpedantic` for strict warnings
- Debug: `-g -O0 -DDEBUG`
- Release: `-O3 -DNDEBUG`

**Conditional Compilation:**
- `GLEW_REQUIRED` defined on Linux/Windows only
- `SHADER_DIR` passed as compile-time constant

**Build Targets:**

- `Lambda` - Main executable
- `make run` - Build and execute
- `make clean-all` - Remove build directory

**Homebrew Integration (macOS):**
CMake automatically detects Homebrew prefix and adds paths for GLFW/GLEW.

### 3.3 Design Philosophy

#### 3.3.1 Why C Instead of C++

Lambda Model Viewer is written in **pure C11**, not C++. This was a deliberate architectural decision:

**Reasons for Choosing C:**

1. **Simplicity and Transparency**
   - No hidden costs (vtables, constructors, exceptions, RTTI)
   - Every operation is explicit in the code
   - Easier to understand memory layout and performance characteristics

2. **Compatibility with Valve's SDK**
   - Valve's studio.h is pure C
   - No name mangling issues when interfacing with C libraries
   - Easier to understand Half-Life engine source code (also C)

3. **Learning Value**
   - Forces explicit resource management
   - Manual memory management builds deeper understanding
   - No "magic" - every allocation and deallocation is visible

4. **Performance Predictability**
   - No hidden allocations (unlike std::vector, std::string)
   - Direct control over memory layout
   - Easier to profile and optimize

5. **Smaller Binary Size**
   - No C++ runtime overhead
   - No template bloat
   - Minimal dependencies

**Trade-offs Accepted:**
- Manual memory management (more error-prone)
- No RAII (must manually clean up resources)
- No STL containers (use manual arrays)
- More verbose code in some cases

#### 3.3.2 Memory Management Strategy

Lambda uses **manual memory management** with strict patterns:

**Allocation Patterns:**

1. **Stack Allocation (Preferred)**
   - Used for temporary variables, small arrays
   - Automatic cleanup (no free() needed)
   - Example: `vec3_t position = {0, 0, 0};`

2. **Heap Allocation (When Necessary)**
   - Used for large data, variable-size arrays, persistent data
   - Must be manually freed
   - Example: `unsigned char *data = malloc(file_size);`

**Resource Ownership Rules:**

1. **Loader Owns Data**
   - `mdl_loader` allocates model data
   - `mdl_loader` provides cleanup function
   - Caller must not free loader-owned pointers

2. **Explicit Cleanup Functions**
   - Every module with dynamic allocation has `_cleanup()` or `_shutdown()` function
   - Example: `cleanup_renderer()`, `mdl_free_texture()`, `logger_shutdown()`

3. **No Hidden Allocations**
   - Functions that allocate memory document it clearly
   - Caller must know who owns memory

**Error Handling:**
- Return codes (0 = success, non-zero = error) for system functions
- NULL returns for allocation failures
- Cleanup on error path (goto cleanup pattern)

**Memory Safety Practices:**
- Always check malloc/calloc return values
- Initialize pointers to NULL
- Set pointers to NULL after free
- Bounds checking for array access

#### 3.3.3 Error Handling Approach

Lambda uses **return codes + logging**, not exceptions (C doesn't have exceptions).

**Error Handling Patterns:**

1. **Return Code Convention**
   ```c
   int function_that_can_fail(void) {
       if (error_condition) {
           LOG_ERRORF("module", "What went wrong");
           return -1;  // Non-zero = error
       }
       return 0;  // Zero = success
   }
   ```

2. **NULL Returns for Pointers**
   ```c
   void *allocate_something(void) {
       void *ptr = malloc(size);
       if (!ptr) {
           LOG_ERRORF("module", "Out of memory");
           return NULL;
       }
       return ptr;
   }
   ```

3. **Goto Cleanup Pattern**
   ```c
   int complex_function(void) {
       void *res1 = NULL, *res2 = NULL;
       
       res1 = malloc(size1);
       if (!res1) goto cleanup;
       
       res2 = malloc(size2);
       if (!res2) goto cleanup;
       
       // Success
       return 0;
       
   cleanup:
       free(res1);
       free(res2);
       return -1;
   }
   ```

**Error Reporting Hierarchy:**
- **FATAL**: Unrecoverable error, program must exit (e.g., OpenGL init failure)
- **ERROR**: Serious problem, operation failed (e.g., file not found)
- **WARN**: Problem but can continue (e.g., missing optional texture)
- **INFO**: Normal operation messages
- **DEBUG/TRACE**: Development diagnostics (removed in production)

**No Exceptions:**
- C has no exceptions
- Cannot "throw" errors up the call stack
- Must explicitly check return values
- Advantages: Predictable control flow, no hidden costs

#### 3.3.4 Platform Abstraction

Lambda supports **macOS, Linux, and Windows** through conditional compilation.

**Platform Detection:**
```c
// platform.h
#if defined(__APPLE__) && defined(__MACH__)
    #define PLATFORM_MACOS 1
#elif defined(__linux__)
    #define PLATFORM_LINUX 1
#elif defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS 1
#endif
```

**OpenGL Platform Abstraction:**
```c
// r_gl_platform.h
#ifdef PLATFORM_MACOS
    #include <OpenGL/gl3.h>       // macOS native OpenGL
    #define GLEW_REQUIRED 0
#else
    #include <GL/glew.h>          // Linux/Windows need GLEW
    #include <GL/gl.h>
    #define GLEW_REQUIRED 1
#endif
```

**Platform-Specific Initialization:**
```c
#if GLEW_REQUIRED
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        LOG_FATALF("renderer", "GLEW init failed: %s", glewGetErrorString(err));
    }
#endif
```

**File Path Handling:**
- Shader paths use compile-time constant: `SHADER_DIR`
- CMake sets `SHADER_DIR` to absolute path
- No platform-specific path separators needed

**Window System:**
- GLFW provides platform abstraction
- No direct OS window API calls
- Cross-platform input handling through GLFW

**Future Portability Considerations:**
- OpenGL deprecated on macOS (may need Metal/MoltenVK in future)
- Vulkan port would require separate render backend
- Core logic (MDL loading, animations, math) is platform-agnostic

---

**Section 3 Complete!** 

Next up: **Section 4** (execution flow - we'll do this AFTER documenting main.c), or jump straight to **Section 5** (main.c documentation).


---

## 4. Program Execution Flow

### 4.1 Application Lifecycle

Lambda Model Viewer follows a traditional application lifecycle with three distinct phases: **Startup**, **Runtime**, and **Shutdown**. Understanding this lifecycle is crucial for debugging, extending functionality, and maintaining the codebase.

#### 4.1.1 Startup Phase

**Duration**: From program launch until first frame rendered (~100-500ms depending on model size)

**Purpose**: Initialize all subsystems, load resources, prepare for rendering

**Order of Operations** (strict order must be maintained):

1. **Command-Line Parsing** (`parse_args()`)
   - Parse argc/argv into structured `app_args_t`
   - Handle special flags: `--help`, `--version`, `--dump-only`
   - Validate model file path exists
   - Set logging verbosity level
   - **Early exit cases**: Help/version display, invalid arguments

2. **Logger Initialization** (`logger_init()`)
   - Configure log level from arguments
   - Open log file if specified
   - Set up category filtering
   - **Why first**: All subsequent operations need logging for diagnostics

3. **Application State Initialization** (`app_init()`)
   - Allocate and zero-initialize `app_state_t` structure
   - Store reference to command-line arguments
   - Set initial values (animation speed = 1.0, looping = true, etc.)

4. **OpenGL Context Creation** (via GLFW)
   - Initialize GLFW library
   - Set OpenGL version hints (4.1 Core on macOS, 4.5 elsewhere)
   - Create window with specified dimensions (800x600 default)
   - Make OpenGL context current
   - **Critical**: GLEW initialization on Linux/Windows (after context creation)

5. **Renderer Initialization** (`init_renderer()`)
   - Query OpenGL capabilities (version, vendor, GLSL version)
   - Enable depth testing, configure viewport
   - Compile and link shaders (vertex, fragment)
   - Create fallback white texture (2x2 RGBA)
   - Generate VAO/VBO for geometry
   - Set up input callbacks (keyboard, mouse, scroll)

6. **Model Loading** (`mdl_load_file()`)
   - Read main .mdl file into memory
   - Validate magic number (IDST) and version (10)
   - Parse header structure (`studiohdr_t`)
   - Load external texture file (T.mdl) if exists
   - Load sequence group files (01.mdl, 02.mdl, etc.) if exist
   - Extract and upload textures to OpenGL
   - **Memory note**: Model data remains in memory for entire runtime

7. **Bone System Setup** (`SetUpBones()`)
   - Initialize bone transformation matrices to T-pose
   - Build bone hierarchy from parent indices
   - Apply bind pose rotations and translations
   - Store in `g_bonetransformations[128]` global array

8. **Animation System Initialization** (`mdl_animation_init()`)
   - Initialize animation state structure
   - Set default sequence (index 0, usually "idle")
   - Detect if sequence is looping from flags
   - Set playback speed to 1.0
   - Initialize frame timer

9. **Input System Initialization** (`Input_Init()`)
   - Zero-initialize input state arrays
   - Set up previous/current frame button states
   - Register GLFW callbacks for events

10. **Camera Initialization** (manual - not yet using r_camera.c)
    - Set initial zoom level (0.15 for typical models)
    - Set rotation angles (0, 0, 0)
    - Position camera behind model

**Startup Success Indicators**:
- Window appears and shows model
- No FATAL log messages
- `app_init()` returns `APP_INIT_SUCCESS` (0)

**Common Startup Failures**:
- GLFW init failure → No windowing support on system
- OpenGL version too old → Update graphics drivers
- Model file not found → Check file path
- Invalid MDL file → Corrupted or wrong format
- Shader compilation failure → GLSL version mismatch

#### 4.1.2 Runtime Phase

**Duration**: From first frame until user closes window (seconds to hours)

**Purpose**: Main application loop - process input, update state, render frames

**Frame Loop Structure** (60 FPS target, ~16.67ms per frame):

```
while (!glfwWindowShouldClose(window)) {
    1. Calculate delta time
    2. Process input events
    3. Update animation
    4. Update camera
    5. Render frame
    6. Swap buffers
    7. Poll events
}
```

**Detailed Frame Breakdown**:

**1. Delta Time Calculation** (~0.01ms)
```c
double current_time = glfwGetTime();
float delta_time = current_time - last_frame_time;
last_frame_time = current_time;
```
- **Purpose**: Frame-rate independent animation
- **Clamping**: Delta time capped at 33ms (30 FPS minimum)
- **Why**: Prevents huge time jumps if frame drops

**2. Input Processing** (~0.1ms)
```
glfwPollEvents() → GLFW callbacks fire → Input state updated
↓
Input_ProcessGameInput()
  ├─> Camera controls (WASD, QE, mouse drag, scroll)
  ├─> Animation controls (SPACE, LEFT/RIGHT, UP/DOWN, L, 0, I)
  ├─> Render mode toggles (F, P, R)
  └─> Bodypart switching (1-9 keys)
```
- **Previous/Current State**: Detect key press vs key held
- **Accumulation**: Mouse delta accumulated for smooth camera rotation

**3. Animation Update** (~0.2ms if animating)
```c
if (g_animation_enabled) {
    mdl_animation_update(&state, delta_time, header, data, seqgroups);
      ↓
    - Advance frame counter by (delta_time * fps)
    - Wrap frame at sequence length
    - Stop at last frame if not looping
}
```
- **Frame interpolation**: Uses fractional frames (e.g., frame 5.7)
- **Wrapping**: `frame -= (int)(frame / wrap_point) * wrap_point`

**4. Bone Transformation Update** (~0.5ms if animating)
```c
if (animating) {
    mdl_animation_calculate_bones() → Updates g_bonetransformations[128]
      ↓
    For each bone:
      - CalcBonePosition(frame, s, bone, anim, pos)
      - CalcBoneQuaternion(frame, s, bone, anim, quat)
      - Build local transform matrix
      - Concatenate with parent transform
      - Store in g_bonetransformations[i]
}
```
- **SLERP interpolation**: Smooth rotation between keyframes
- **Hierarchy traversal**: Parents computed before children
- **Result**: World-space bone matrices ready for rendering

**5. Vertex Skinning** (~1-3ms depending on vertex count)
```c
For each vertex:
    bone_index = vertex_to_bone_map[vertex]
    skinned_position = bone_matrix[bone_index] * original_position
    skinned_normal = bone_rotation[bone_index] * original_normal
```
- **Per-frame operation**: Vertices re-skinned every frame if animating
- **Optimization**: Only if animation is playing
- **Result**: Deformed mesh in world space

**6. Rendering** (~5-10ms)
```c
render_model()
  ↓
  1. Update vertex buffer with skinned positions/normals
  2. Set up view/projection matrices
  3. Bind shader program
  4. Set uniforms (matrices, light position, camera position)
  5. For each mesh:
       - Bind texture
       - glDrawArrays(GL_TRIANGLES, first, count)
```
- **Batching**: Draw calls grouped by texture
- **State changes**: Minimize texture swaps
- **Wireframe**: `glPolygonMode(GL_LINE)` if enabled

**7. Buffer Swap & Present** (~1-2ms, vsync dependent)
```c
glfwSwapBuffers(window);  // Present backbuffer to screen
```
- **Double buffering**: Prevents tearing
- **VSync**: May block to maintain 60 FPS cap

**8. Event Polling** (~0.1ms)
```c
glfwPollEvents();  // Process OS window events
```
- **Non-blocking**: Returns immediately
- **Fires callbacks**: Key press, mouse move, scroll, window resize

**Frame Time Budget** (16.67ms total for 60 FPS):
- Delta time calculation: 0.01ms
- Input processing: 0.1ms
- Animation update: 0.2ms
- Bone transforms: 0.5ms
- Vertex skinning: 1-3ms
- Rendering: 5-10ms
- Buffer swap: 1-2ms
- Event polling: 0.1ms
- **Total**: ~8-16ms (leaves headroom for 60 FPS)

**Runtime State Transitions**:
- **Animation toggle**: SPACE key → `g_animation_enabled = !g_animation_enabled`
- **Sequence change**: LEFT/RIGHT arrows → `mdl_animation_set_sequence()`
- **Wireframe toggle**: F key → `glPolygonMode(GL_LINE/GL_FILL)`
- **Bodypart change**: 1-9 keys → Rebuild vertex buffer with new submodel

#### 4.1.3 Shutdown Phase

**Duration**: ~50-200ms from window close to process exit

**Purpose**: Clean up resources, close files, free memory

**Shutdown Trigger**: 
- User closes window (X button)
- User presses ESC key
- `glfwWindowShouldClose(window)` returns true

**Order of Operations** (reverse of startup):

1. **Exit Render Loop**
   - `app_run()` returns to `main()`
   - Log frame count and exit message

2. **Input System Shutdown** (`Input_Shutdown()`)
   - Clear input state
   - Unregister callbacks (not strictly necessary, window is closing)

3. **Renderer Cleanup** (`cleanup_renderer()`)
   - Delete OpenGL objects:
     - `glDeleteVertexArrays(1, &VAO)`
     - `glDeleteBuffers(1, &VBO)`
     - `glDeleteBuffers(1, &EBO)`
     - `glDeleteProgram(shader_program)`
     - `glDeleteTextures()` for all loaded textures
   - Destroy GLFW window
   - Terminate GLFW library

4. **Model Data Cleanup** (`mdl_free_model()`)
   - Free model file buffer (`free(data)`)
   - Free texture data buffer if separate
   - Free sequence group buffers
   - Set pointers to NULL

5. **Texture Cleanup** (`mdl_free_texture()`)
   - Free texture metadata array
   - OpenGL textures already deleted in step 3

6. **Logger Shutdown** (`logger_shutdown()`)
   - Flush log buffer
   - Close log file if opened
   - Free log buffers

7. **Application State Cleanup**
   - Zero out `app_state_t` structure
   - Set `initialized = false`

8. **Return to OS**
   - `main()` returns `APP_INIT_SUCCESS` (0)
   - OS reclaims remaining memory
   - Process terminates

**Shutdown Success Indicators**:
- No error messages during cleanup
- Log file shows "Exiting render loop after X frames"
- Clean process exit (return code 0)

**Common Shutdown Issues**:
- Segfault during cleanup → Double-free or use-after-free
- Memory leaks → Forgot to free allocated buffers
- Hanging on exit → OpenGL driver issue (rare)

**Memory Cleanup Verification**:
- Use Valgrind (Linux) or Instruments (macOS) to detect leaks
- Expected leaks: Some GLFW/OpenGL internal state (not our problem)
- Target: 0 leaks from application code

### 4.2 Complete Call Chain from main()

**Visual Call Tree** (→ means "calls", ↓ means "then calls"):

```
main()
  │
  ├─→ parse_args(argc, argv, &args)
  │     ├─→ print_banner()
  │     ├─→ print_usage() [if --help]
  │     └─→ print_version_info() [if --version]
  │
  ├─→ app_init(&args)
  │     ├─→ logger_init(&log_options)
  │     │     └─→ logger_set_level()
  │     │
  │     ├─→ init_renderer(width, height, title)
  │     │     ├─→ glfwInit()
  │     │     ├─→ glfwCreateWindow()
  │     │     ├─→ glewInit() [Linux/Windows only]
  │     │     ├─→ load_shaders()
  │     │     │     ├─→ read_shader_source("textured.vert")
  │     │     │     ├─→ read_shader_source("textured.frag")
  │     │     │     ├─→ compile_shader(vertex_src, GL_VERTEX_SHADER)
  │     │     │     ├─→ compile_shader(fragment_src, GL_FRAGMENT_SHADER)
  │     │     │     └─→ create_shader_program(vs, fs)
  │     │     └─→ setup_triangle() [VAO/VBO creation]
  │     │
  │     ├─→ mdl_load_file(model_path, &header, &data, ...)
  │     │     ├─→ fopen(), fread(), fclose()
  │     │     ├─→ validate_header(header)
  │     │     ├─→ load_texture_file() [if T.mdl exists]
  │     │     └─→ load_sequence_groups() [if XX.mdl exist]
  │     │
  │     ├─→ mdl_load_textures(tex_header, tex_data, &g_textures)
  │     │     └─→ For each texture:
  │     │           ├─→ extract_texture_rgb()
  │     │           ├─→ glGenTextures()
  │     │           ├─→ glBindTexture()
  │     │           ├─→ glTexImage2D()
  │     │           └─→ glTexParameteri()
  │     │
  │     ├─→ set_model_data(header, data, tex_header, tex_data, seqgroups, ...)
  │     │     ├─→ mdl_animation_init(&g_anim_state)
  │     │     ├─→ mdl_animation_set_sequence(&g_anim_state, 0, ...)
  │     │     └─→ SetUpBones(header, data)
  │     │           └─→ For each bone:
  │     │                 ├─→ Math_AngleQuaternion(euler, quat)
  │     │                 ├─→ Math_QuaternionMatrix3x4(quat, &mat)
  │     │                 ├─→ Math_Mat3x4_ToMat4(&mat3x4, mat4)
  │     │                 └─→ Math_Mat4_Multiply(parent, local, result)
  │     │
  │     └─→ Input_Init(window)
  │
  ├─→ app_run()
  │     └─→ render_loop() [runs until window close]
  │           │
  │           └─→ while (!glfwWindowShouldClose(window)):
  │                 │
  │                 ├─→ glfwGetTime() [delta time calc]
  │                 │
  │                 ├─→ mdl_animation_update(&state, dt, ...) [if animating]
  │                 │     └─→ Advance frame counter, wrap at sequence end
  │                 │
  │                 ├─→ clear_screen()
  │                 │     └─→ glClearColor(), glClear()
  │                 │
  │                 ├─→ render_model(header, data)
  │                 │     │
  │                 │     ├─→ ProcessModelForRendering() [first frame only]
  │                 │     │     └─→ For each bodypart/model/mesh:
  │                 │     │           ├─→ TransformVertices(header, data, model, skinned_pos)
  │                 │     │           └─→ AddVertexToBuffer(v, n, s, t, texW, texH)
  │                 │     │
  │                 │     ├─→ mdl_animation_calculate_bones(...) [if animating]
  │                 │     │     └─→ For each bone:
  │                 │     │           ├─→ CalcBonePosition(frame, s, bone, anim, pos)
  │                 │     │           ├─→ CalcBoneQuaternion(frame, s, bone, anim, quat)
  │                 │     │           ├─→ Math_QuaternionMatrix4x4(quat, &local)
  │                 │     │           └─→ Math_Mat4_Multiply(parent, local, result)
  │                 │     │
  │                 │     ├─→ TransformVertices() [re-skin if animating]
  │                 │     │
  │                 │     ├─→ glUseProgram(shader_program)
  │                 │     ├─→ Math_Mat4_Identity(M)
  │                 │     ├─→ Math_Mat4_Rotate(M, rotation_y, y_axis)
  │                 │     ├─→ Math_Mat4_Rotate(M, rotation_x, x_axis)
  │                 │     ├─→ Math_Mat4_LookAt(camPos, target, up, V)
  │                 │     ├─→ Math_Mat4_Perspective(fov, aspect, near, far, P)
  │                 │     ├─→ glUniformMatrix4fv() [set MVP matrices]
  │                 │     │
  │                 │     └─→ For each draw range:
  │                 │           ├─→ glBindTexture(GL_TEXTURE_2D, texture)
  │                 │           └─→ glDrawArrays(GL_TRIANGLES, first, count)
  │                 │
  │                 ├─→ glfwSwapBuffers(window)
  │                 │
  │                 ├─→ glfwPollEvents()
  │                 │     └─→ [Triggers input callbacks]
  │                 │           ├─→ key_callback()
  │                 │           ├─→ mouse_button_callback()
  │                 │           ├─→ cursor_position_callback()
  │                 │           └─→ scroll_callback()
  │                 │
  │                 └─→ Input_ProcessGameInput(window, &cam_state, &anim_state)
  │                       ├─→ handle_camera_input()
  │                       ├─→ handle_animation_input()
  │                       └─→ handle_render_mode_input()
  │
  └─→ app_shutdown()
        ├─→ Input_Shutdown()
        ├─→ cleanup_renderer()
        │     ├─→ glDeleteVertexArrays(1, &VAO)
        │     ├─→ glDeleteBuffers(1, &VBO)
        │     ├─→ glDeleteProgram(shader_program)
        │     ├─→ glfwDestroyWindow(window)
        │     └─→ glfwTerminate()
        ├─→ mdl_free_texture(&g_textures)
        └─→ logger_shutdown()
```

**Critical Dependencies**:
- Logger must init before anything else (all modules log)
- OpenGL context must exist before shader compilation
- Model must load before textures can be extracted
- Bones must be set up before animation can play
- VAO/VBO must exist before rendering

### 4.3 State Machine Diagram

Lambda Model Viewer operates as a simple state machine with 4 states:

```
    ┌─────────────┐
    │   STARTUP   │ ← Parse args, init systems, load model
    └──────┬──────┘
           │
           ▼
    ┌─────────────┐
    │    IDLE     │ ← Waiting in main loop, no animation
    └──────┬──────┘
           │
           ├──────→ SPACE key pressed
           │
           ▼
    ┌─────────────┐
    │  ANIMATING  │ ← Main loop with animation update
    └──────┬──────┘
           │
           ├──────→ SPACE key pressed
           │
           ▼
    ┌─────────────┐
    │  SHUTDOWN   │ ← Cleanup and exit
    └─────────────┘
```

**State Details**:

**STARTUP** (Transient state, ~100-500ms)
- **Entry**: Program launch
- **Actions**: Initialize subsystems, load model
- **Exit condition**: Render loop starts
- **Next state**: IDLE or ANIMATING (depending on default animation setting)

**IDLE** (Stable state)
- **Entry**: From STARTUP or when animation is paused
- **Actions**: 
  - Process input
  - Render static model (no bone updates)
  - Camera can still move
  - Keyboard controls active
- **Exit condition**: User presses SPACE to start animation
- **Next state**: ANIMATING

**ANIMATING** (Stable state)
- **Entry**: User presses SPACE, or default for animated models
- **Actions**:
  - Process input
  - Update animation frame counter
  - Recalculate bone transformations
  - Re-skin vertices
  - Render animated model
- **Exit condition**: User presses SPACE to pause, or sequence ends (if not looping)
- **Next state**: IDLE or SHUTDOWN

**SHUTDOWN** (Transient state, ~50-200ms)
- **Entry**: User closes window or presses ESC
- **Actions**: Clean up OpenGL, free memory, close files
- **Exit condition**: All cleanup complete
- **Next state**: Process exit

**State Variables**:
```c
app_state_t.running = true/false;        // Controls main loop
app_state_t.initialized = true/false;    // Safety check for operations
g_animation_enabled = true/false;        // IDLE vs ANIMATING
```

### 4.4 Threading Model

Lambda Model Viewer is **single-threaded** with **no concurrency**.

**Design Decision**: 
- Single main thread handles everything: input, logic, rendering
- No background threads for loading or computation
- No mutexes, semaphores, or atomic operations needed

**Why Single-Threaded?**
1. **Simplicity**: Easier to understand, debug, and maintain
2. **OpenGL requirement**: OpenGL context is single-threaded
3. **Performance**: Not a bottleneck for typical Half-Life models (<5000 vertices)
4. **Determinism**: No race conditions, easier to reproduce bugs

**Implications**:
- Model loading blocks the main thread (acceptable for sub-500ms loads)
- Animation calculations happen on main thread (fast enough for 60 FPS)
- Input processing happens on main thread (low latency, sub-1ms)

**Future Considerations** (if needed):
- Background thread for model loading (splash screen during load)
- Worker threads for vertex skinning (if models exceed 50k vertices)
- Would require careful OpenGL context sharing

**Current Performance**:
- Typical frame time: 8-16ms (60+ FPS achievable)
- Animation update: <1ms for typical model (128 bones)
- Vertex skinning: 1-3ms for typical model (2000-5000 vertices)
- Input latency: <1ms (same frame response)

### 4.5 Memory Lifecycle

Lambda's memory management follows strict ownership and lifetime rules.

#### 4.5.1 Static/Global Memory

**Lifetime**: Program start → Program exit

**Examples**:
```c
static app_state_t g_app_state;              // Application state
static mat4 g_bonetransformations[128];      // Bone matrices
static float render_vertex_buffer[32768*8];  // Pre-allocated render buffer
```

**Characteristics**:
- Zero-initialized at startup
- Never freed (OS reclaims at process exit)
- Thread-safe (single thread, no concurrent access)
- Fixed size (no dynamic growth)

**Advantages**:
- No allocation overhead
- Cache-friendly (contiguous memory)
- No fragmentation
- Fast access (no pointer chasing)

**Disadvantages**:
- Fixed limits (MAXSTUDIOBONES = 128, MAXSTUDIOVERTS = 2048)
- Memory used even if not needed
- Cannot exceed limits without recompilation

#### 4.5.2 Heap Memory (Dynamic Allocation)

**Lifetime**: Varies by ownership

**Category 1: Per-Model Lifetime** (loaded → unloaded)
```c
unsigned char *data = malloc(file_size);      // Model file buffer
unsigned char *tex_data = malloc(tex_size);   // Texture file buffer
mdl_texture_set_t g_textures.textures = malloc(count * sizeof(mdl_texture_t));
```
- **Allocated during**: `mdl_load_file()`
- **Freed during**: `app_shutdown()` via cleanup functions
- **Ownership**: MDL loader owns these pointers
- **Lifetime**: Entire runtime (model stays loaded)

**Category 2: OpenGL Resources** (init → shutdown)
```c
GLuint VAO, VBO, EBO;           // OpenGL buffer objects
GLuint shader_program;          // Compiled shader program
GLuint texture_ids[100];        // OpenGL texture objects
```
- **Allocated during**: `init_renderer()`, texture loading
- **Freed during**: `cleanup_renderer()`
- **Ownership**: OpenGL driver manages actual memory
- **Lifetime**: Entire runtime (from init to shutdown)

**Category 3: Transient Allocations** (function scope)
```c
char *shader_source = read_shader_source("file.vert");  // Freed after compilation
```
- **Allocated during**: Specific function execution
- **Freed during**: Same function (before return)
- **Ownership**: Function owns pointer
- **Lifetime**: Milliseconds (duration of function call)

#### 4.5.3 Memory Ownership Rules

**Rule 1: Allocator Frees**
- Whoever calls `malloc()` is responsible for `free()`
- Example: `mdl_loader.c` allocates model data → must provide `mdl_free_model()`

**Rule 2: No Shared Ownership**
- Each pointer has exactly one owner
- No reference counting or shared_ptr equivalent
- If function returns pointer, ownership transfers to caller

**Rule 3: NULL After Free**
```c
free(ptr);
ptr = NULL;  // Prevents use-after-free bugs
```

**Rule 4: Cleanup Functions Mirror Init Functions**
```c
init_renderer()    → cleanup_renderer()
mdl_load_file()    → mdl_free_model()
logger_init()      → logger_shutdown()
```

#### 4.5.4 Memory Leak Prevention

**Strategy 1: Goto Cleanup Pattern**
```c
int function(void) {
    void *res1 = NULL, *res2 = NULL;
    
    res1 = malloc(size1);
    if (!res1) goto cleanup;
    
    res2 = malloc(size2);
    if (!res2) goto cleanup;
    
    return SUCCESS;
    
cleanup:
    free(res1);
    free(res2);
    return ERROR;
}
```

**Strategy 2: Shutdown Functions**
- Every subsystem has shutdown function that frees its allocations
- Called in reverse order of initialization
- Idempotent (safe to call multiple times)

**Strategy 3: Valgrind Testing** (Linux)
```bash
valgrind --leak-check=full ./Lambda scientist.mdl
```
- Detects memory leaks
- Shows allocation/free mismatch
- Reports unfreed blocks at exit

**Current Leak Status** (v0.3.0):
- Application code: 0 leaks
- GLFW/OpenGL internal state: ~100 bytes (not our responsibility)
- Acceptable leak threshold: <1 KB from system libraries

---

**Section 4 Complete!**

Next: **Section 5 - main.c Documentation** (the real learning begins!)

---

## 5. Entry Point: main.c

### 5.1 File Overview

#### 5.1.1 File Location

```
src/main.c
```

This is the entry point of the entire application - the first user-written code that executes when you run `./Lambda` from the command line.

#### 5.1.2 Purpose and Responsibilities

**Primary Purpose:** Orchestrate the application's lifecycle from startup to shutdown.

**Key Responsibilities:**
1. Receive command-line arguments from the operating system
2. Parse user-provided flags and options
3. Initialize all subsystems in correct order
4. Run the main application loop
5. Cleanup all resources on exit
6. Return appropriate exit codes to the shell

**What This File Does NOT Do:**
- Does NOT implement any business logic
- Does NOT directly interact with OpenGL, models, or rendering
- Does NOT parse MDL files or handle animations
- Does NOT process user input or manage windows

**Design Philosophy:** main.c is a **thin orchestration layer**. It delegates all actual work to specialized modules. This keeps the entry point simple, readable, and maintainable.

#### 5.1.3 Dependencies

**Direct Includes:**
```c
#include "cl/cl_app.h"       // Application init/run/shutdown functions
#include "mdl/mdl_report.h"  // Model debugging/dumping (unused in main, but linked)
#include "util/util_args.h"  // Command-line argument parsing

#include <stdio.h>           // Standard I/O (printf, fprintf)
#include <stdlib.h>          // Standard library (EXIT_SUCCESS, EXIT_FAILURE)
```

**Why These Includes?**
- `cl_app.h`: Provides `app_init()`, `app_run()`, `app_shutdown()` - the core lifecycle functions
- `util_args.h`: Provides `parse_args()` and the `app_args_t` structure
- `mdl_report.h`: Included for potential debug dumps (not currently used in main.c, but part of the build)
- `stdio.h/stdlib.h`: Standard C library headers (currently not directly used in main.c, but good practice)

**Indirect Dependencies (Transitive):**
Through the includes above, main.c indirectly depends on:
- GLFW (window management)
- OpenGL (rendering)
- Math library (transformations)
- Model loader (MDL parsing)
- Logger (diagnostic output)

**Dependency Level:** Level 6 (top of dependency hierarchy - depends on everything, nothing depends on it)

#### 5.1.4 Global Variables

**Definition:**
```c
static app_args_t args = { 0 };
```

**Analysis:**

**What is `app_args_t`?**
A structure defined in `util/util_args.h` that holds all parsed command-line arguments. See Section 6.2.3 for complete structure definition.

**Why `static`?**
The `static` keyword at file scope (outside any function) means **internal linkage** - this variable is:
1. **Private to main.c** - Other .c files cannot access it, even with `extern` declaration
2. **Exists for entire program lifetime** - Allocated when program starts, deallocated when program exits
3. **Zero-initialized by default** - All fields set to 0/NULL/false before main() runs

**Common Misconception:** Many developers think `static` makes a variable "global" or "visible everywhere". **This is backwards. At file scope, `static` **restrict visibility to just this file - it's an encapsulation/privacy mechanism.

**Why File-Scope Instead of Local to main()?**

- Could have declared inside `main()` and passed to functions
- File-scope approach chosen because:
  - Simplifies function signatures (don't need to pass args around)
  - Centralizes argument storage in one location
  - `static` ensures it's private to this file (good encapsulation)

**Why `= { 0 }`?**
This is **aggregate initialization** - it initializes all structure fields to zero/NULL/false. Equivalent to:
```c
args.model_path = NULL;
args.dump_level = 0;
args.dump_only = false;
// ... all other fields set to 0/NULL/false ...
```

**Note:** In C, `static` variables are zero-initialized by default even without `= { 0 }`, but explicitly writing it makes the intent clear and improves readability.

---

### 5.2 main() - Application Entry Point

#### 5.2.1 Function Signature

```c
int main( int argc, char const *argv[] )
```

**Breaking Down the Signature:**

**Return Type: `int`**
- Every C program must return an integer to the operating system
- This is the **exit code** or **return code**
- By UNIX convention: 0 means success, non-zero means failure
- The shell uses this to determine if your program succeeded

**Function Name: `main`**
- Special function recognized by the C runtime
- Execution starts here after C runtime initialization
- You don't call `main()` yourself - the OS does

**Parameter 1: `int argc`**
- **"Argument Count"**
- Number of command-line arguments passed to the program
- **Always at least 1** (argv[0] is the program name)
- Examples:
  - `./Lambda` → argc = 1
  - `./Lambda scientist.mdl` → argc = 2
  - `./Lambda scientist.mdl --verbose --dump` → argc = 4

**Parameter 2: `char const *argv[]`**

This is the most complex part - let's break it down piece by piece:

**`char const *` - Pointer to Constant Character**
- `char` = single character (1 byte)
- `const` = cannot be modified (read-only)
- `*` = pointer (memory address)
- So: "a pointer to a character that cannot be changed"

**Why `const`?**
The operating system gives us the command-line arguments as **read-only strings**. We are NOT allowed to modify them. If you try:
```c
argv[1][0] = 'X';  // ERROR - Attempting to modify const data
```
The compiler will reject this (compile error) or if you force it with a cast, you'll get undefined behavior (likely a crash). The `const` keyword is the compiler's way of protecting you from this mistake.

**`[]` - Array**
- `argv[]` is an array of pointers
- Each element is a `char const *` (pointer to read-only string)

**Visual Representation:**
```
Command line: ./Lambda scientist.mdl --verbose

Memory layout:
argv → [ptr0,        ptr1,            ptr2,             NULL]
        ↓             ↓               ↓
     "./Lambda" "scientist.mdl" "--verbose"

argc = 3
argv[0] → "./Lambda"       (program name)
argv[1] → "scientist.mdl"  (first argument)
argv[2] → "--verbose"      (second argument)
argv[3] → NULL             (terminator)
```

**Why `argv[0]` is the Program Name:**
This is a UNIX convention. The shell always passes the program name as the first argument. This allows programs to:
- Print their own name in error messages: `fprintf(stderr, "%s: error\n", argv[0]);`
- Detect how they were invoked (useful for programs with multiple names via symlinks)

**Array Termination:**
The `argv` array is **NULL-terminated** - `argv[argc]` is always NULL. This provides two ways to iterate:
```c
// Method 1: Use argc
for (int i = 0; i < argc; i++) {
    printf("%s\n", argv[i]);
}

// Method 2: Check for NULL
for (int i = 0; argv[i] != NULL; i++) {
    printf("%s\n", argv[i]);
}
```

**Alternative Signatures:**
These are all equivalent in C:
```c
int main(int argc, char const *argv[])
int main(int argc, char const **argv)     // Array decays to pointer
int main(int argc, const char *argv[])    // const position doesn't matter
int main(int argc, const char **argv)
```

#### 5.2.2 Purpose

**High-Level Purpose:**
The `main()` function orchestrates the four-phase application lifecycle:

1. **Phase 1: Argument Parsing** - Understand what the user wants
2. **Phase 2: Initialization** - Set up all subsystems
3. **Phase 3: Execution** - Run the main loop
4. **Phase 4: Cleanup** - Free all resources and exit cleanly

**Why This Structure?**
This is a standard pattern for command-line applications:
- **Fail fast** - Parse arguments first, exit early if invalid (don't waste time initializing)
- **Centralized setup** - All initialization in one place
- **Single responsibility** - Each function does one thing
- **Guaranteed cleanup** - Always call shutdown, even on errors

#### 5.2.3 Parameters

**`argc` - Argument Count**

| Type | Range | Description |
|------|-------|-------------|
| `int` | 1 to ~INT_MAX | Number of command-line arguments (including program name) |

**Guaranteed Constraints:**
- `argc >= 1` always (argv[0] is program name)
- `argc` matches the number of non-NULL entries in argv
- In practice, shells limit argc to ~1,000,000 arguments

**`argv` - Argument Vector**

| Type | Lifetime | Description |
|------|----------|-------------|
| `char const *[]` | Entire program | Array of pointers to null-terminated strings |

**Guaranteed Constraints:**
- `argv[0]` through `argv[argc-1]` are valid pointers to strings
- `argv[argc]` is NULL (terminator)
- All strings are **null-terminated** (end with `\0` character)
- Memory is managed by OS - **do not free() these strings**

**What is a Null-Terminated String?**

In C, strings are not objects with a length field - they're just arrays of characters with a special terminator:

```c
// The string "hello" in memory:
['h', 'e', 'l', 'l', 'o', '\0']
  0    1    2    3    4    5    ← indices

// Without the \0, the computer doesn't know where the string ends!
```

**Why Null Termination?**
Functions like `strlen()`, `strcmp()`, `printf("%s")` work by reading characters until they hit `\0`. Without the terminator, they'd read past the end of the string into random memory (undefined behavior).

**Example:**
```c
// If argv[1] = "scientist.mdl"
// Memory layout:
['s','c','i','e','n','t','i','s','t','.','m','d','l','\0']

strlen(argv[1]) → 13  // Counts characters until \0
```

#### 5.2.4 Return Value

**Type:** `int`

**Possible Values:**

| Constant | Value | Meaning | When Used |
|----------|-------|---------|-----------|
| `APP_INIT_SUCCESS` | 0 | Normal success | Application ran to completion |
| `APP_INIT_EXIT_SUCCESS` | 1 | Clean early exit | Showed help/version, or error occurred |

**The Confusing Part - Why This Looks Backwards:**

In UNIX, the convention is:
- **Return 0 from main() = Success**
- **Return non-zero from main() = Failure**

But Lambda uses a custom internal convention:
- **APP_INIT_SUCCESS (0) = Internal success** - "Continue to next phase"
- **APP_INIT_EXIT_SUCCESS (1) = External success** - "Exit cleanly without running app"

**Why Two "Success" Codes?**

Consider these scenarios:

**Scenario 1: User runs `./Lambda --version`**
1. `parse_args()` sees `--version` flag
2. `parse_args()` prints version info
3. `parse_args()` returns `APP_INIT_EXIT_SUCCESS` (1) - meaning "I did what you asked, now exit"
4. `main()` checks: `parse_args() != APP_INIT_SUCCESS` → TRUE (1 ≠ 0)
5. `main()` returns `APP_INIT_EXIT_SUCCESS` (1)
6. Shell receives exit code 1 (by UNIX convention, EXIT_SUCCESS)

**Scenario 2: User runs `./Lambda scientist.mdl`**
1. `parse_args()` parses model path successfully
2. `parse_args()` returns `APP_INIT_SUCCESS` (0) - meaning "Continue to app_init()"
3. `app_init()` sets up OpenGL, loads model, etc.
4. `app_init()` returns `APP_INIT_SUCCESS` (0) - meaning "Continue to app_run()"
5. `app_run()` runs the main loop
6. `app_shutdown()` cleans up
7. `main()` returns `APP_INIT_SUCCESS` (0)
8. Shell receives exit code 0 (by UNIX convention, EXIT_SUCCESS)

**Scenario 3: User runs `./Lambda nonexistent.mdl`**
1. `parse_args()` parses arguments successfully
2. `parse_args()` returns `APP_INIT_SUCCESS` (0)
3. `app_init()` tries to load "nonexistent.mdl"
4. Model loading fails
5. `app_init()` logs error message
6. `app_init()` returns `APP_INIT_EXIT_SUCCESS` (1) - meaning "Error occurred, exit cleanly"
7. `main()` checks: `app_init() != APP_INIT_SUCCESS` → TRUE (1 ≠ 0)
8. `main()` returns `APP_INIT_EXIT_SUCCESS` (1)
9. Shell receives exit code 1

**Why Not Use Standard EXIT_SUCCESS/EXIT_FAILURE?**

Lambda uses internal constants instead of stdlib constants because:
1. **Semantic clarity** - `APP_INIT_EXIT_SUCCESS` clearly means "exit now, but cleanly"
2. **Distinguishes internal vs external success** - 0 means "continue", 1 means "stop"
3. **Allows future expansion** - Can add more return codes if needed (e.g., -1 for fatal errors)

**Mental Model:**
- `APP_INIT_SUCCESS` (0) = "Green light, keep going"
- `APP_INIT_EXIT_SUCCESS` (1) = "Stop sign, exit cleanly"

#### 5.2.5 Complete Source Code

```c
/*
 * ═══════════════════════════════════════════════════════════════════════════
 *   Half-Life Model Viewer
 * ═══════════════════════════════════════════════════════════════════════════
 *
 *   Copyright (c) 1996-2002, Valve LLC. All rights reserved.
 *
 *   This product contains software technology licensed from Id
 *   Software, Inc. ("Id Technology"). Id Technology (c) 1996 Id Software, Inc.
 *   All Rights Reserved.
 *
 *   Use, distribution, and modification of this source code and/or resulting
 *   object code is restricted to non-commercial enhancements to products from
 *   Valve LLC. All other use, distribution, or modification is prohibited
 *   without written permission from Valve LLC.
 *
 * ───────────────────────────────────────────────────────────────────────────
 *   Author: Karlo Siric
 *   Purpose: Main Entry point for the Application.
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include "cl/cl_app.h"
#include "mdl/mdl_report.h"
#include "util/util_args.h"

#include <stdio.h>
#include <stdlib.h>

static app_args_t args = { 0 };

int main( int argc, char const *argv[] ) {
	if ( parse_args( argc, argv, &args ) != APP_INIT_SUCCESS ) {
		return ( APP_INIT_EXIT_SUCCESS );
	}

	if ( app_init( &args ) != APP_INIT_SUCCESS ) {
		return ( APP_INIT_EXIT_SUCCESS );
	}

	app_run();

	app_shutdown();

	return ( APP_INIT_SUCCESS );
}
```

#### 5.2.6 Line-by-Line Explanation

Let's walk through every single line of executable code in main():

---

**Line 30: Global Variable Declaration**
```c
static app_args_t args = { 0 };
```

**What happens here:**
1. **Before main() runs**, C runtime allocates memory for `args` structure
2. **All fields initialized to zero** (0 for integers, NULL for pointers, false for booleans)
3. **Memory location is fixed** for entire program lifetime
4. **Only visible within main.c** (due to `static` keyword)

**Memory Layout Example:**
```c
// If app_args_t is defined as:
typedef struct {
    char *model_path;        // NULL
    dump_level_t dump_level; // 0 (DUMP_NONE)
    bool dump_only;          // false (0)
    bool quiet;              // false (0)
    log_detail_t log_level;  // 0 (LOG_NORMAL)
    char *log_file;          // NULL
    bool show_help;          // false (0)
    bool show_version;       // false (0)
} app_args_t;

// After initialization, args = { NULL, 0, 0, 0, 0, NULL, 0, 0 }
```

**Why This Matters:**
- `parse_args()` will **modify** this structure based on command-line flags
- Other functions can **read** this structure to determine what the user wants
- Zero-initialization ensures no garbage values (undefined behavior prevented)

---

**Line 32: Function Entry**
```c
int main( int argc, char const *argv[] ) {
```

**What happens here:**
1. **Operating system calls main()** after C runtime initialization
2. **Stack frame created** for main() (local variables, return address)
3. **Parameters populated by OS:**
   - `argc` = number of command-line arguments (including program name)
   - `argv` = array of pointers to null-terminated strings

**Example Execution:**

User types:
```bash
./Lambda scientist.mdl --verbose
```

OS sets up:
```c
argc = 3
argv[0] = "./Lambda"         // Program name (how it was invoked)
argv[1] = "scientist.mdl"    // First user argument
argv[2] = "--verbose"        // Second user argument
argv[3] = NULL               // Terminator
```

**Control Flow:**
- Execution jumps to line 33 (first statement inside main)

---

**Lines 33-35: Argument Parsing Phase**
```c
if ( parse_args( argc, argv, &args ) != APP_INIT_SUCCESS ) {
    return ( APP_INIT_EXIT_SUCCESS );
}
```

**What happens here:**

**Step 1: Function Call**
```c
parse_args( argc, argv, &args )
```
- Calls `parse_args()` function (defined in `util/util_args.c`, see Section 6.3.1)
- Passes three arguments:
  1. `argc` - passed by value (copy of the integer)
  2. `argv` - passed by value (copy of the pointer, but points to same strings)
  3. `&args` - **address of args** (pass-by-reference)

**Why `&args` (Address-Of Operator)?**

C is **pass-by-value by default** - function parameters are copies. If we wrote:
```c
parse_args( argc, argv, args )  // WRONG! Passes a COPY of args
```
Then `parse_args()` would receive a **copy** of the `args` structure, modify the copy, and our original `args` in main.c would remain unchanged.

By passing `&args` (the **memory address** of args), we're telling `parse_args()`:
"Here's where `args` lives in memory. Modify it directly at this address."

**Visual Analogy:**
- Pass-by-value = "Here's a photocopy of the document. Mark it up all you want."
- Pass-by-reference = "Here's the address of the filing cabinet. Go modify the original document."

**Step 2: Inside parse_args() (See Section 6.3.1 for details)**
```c
// Simplified version of what parse_args() does:
int parse_args(int argc, const char *argv[], app_args_t *args) {
    // Loop through argv, looking for flags:
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            print_usage();
            return APP_INIT_EXIT_SUCCESS;  // Exit early
        }
        else if (strcmp(argv[i], "--version") == 0) {
            print_version_info();
            return APP_INIT_EXIT_SUCCESS;  // Exit early
        }
        else if (strcmp(argv[i], "--verbose") == 0) {
            args->log_level = LOG_VERBOSE;  // Modify original args
        }
        // ... more flag parsing ...
        else {
            // Assume it's the model path
            args->model_path = argv[i];
        }
    }
    return APP_INIT_SUCCESS;  // Continue to app_init()
}
```

**Step 3: Return Value Check**
```c
if ( parse_args(...) != APP_INIT_SUCCESS )
```

**Case A: User ran `./Lambda --help`**
- `parse_args()` prints help text
- `parse_args()` returns `APP_INIT_EXIT_SUCCESS` (value 1)
- Condition evaluates: `1 != 0` → **TRUE**
- Enter if-block, return `APP_INIT_EXIT_SUCCESS` to OS
- **Program exits** (never calls app_init, app_run, or app_shutdown)

**Case B: User ran `./Lambda scientist.mdl`**
- `parse_args()` sets `args.model_path = "scientist.mdl"`
- `parse_args()` returns `APP_INIT_SUCCESS` (value 0)
- Condition evaluates: `0 != 0` → **FALSE**
- Skip if-block, continue to line 37

**Why This Design?**
- **Fail fast** - If arguments are invalid or user just wants help/version, exit immediately
- **Don't waste resources** - No point initializing OpenGL if we're just showing help
- **Clean separation** - Argument parsing doesn't know about rendering, models, etc.

---

**Lines 37-39: Initialization Phase**
```c
if ( app_init( &args ) != APP_INIT_SUCCESS ) {
    return ( APP_INIT_EXIT_SUCCESS );
}
```

**What happens here:**

**Step 1: Function Call**
```c
app_init( &args )
```
- Calls `app_init()` function (defined in `cl/cl_app.c`, see Section 7.X)
- Passes **address of args** so app_init() can read configuration

**Step 2: Inside app_init() (Simplified - See Section 7.X for full details)**
```c
int app_init(app_args_t *args) {
    // Initialize logger
    if (logger_init(args->log_level, args->log_file) != 0) {
        return APP_INIT_EXIT_SUCCESS;  // Failed
    }

    // Initialize GLFW (window library)
    if (!glfwInit()) {
        logger_error("Failed to initialize GLFW");
        return APP_INIT_EXIT_SUCCESS;  // Failed
    }

    // Create window
    window = glfwCreateWindow(1024, 768, "Lambda", NULL, NULL);
    if (!window) {
        logger_error("Failed to create window");
        return APP_INIT_EXIT_SUCCESS;  // Failed
    }

    // Initialize OpenGL (GLEW)
    if (glewInit() != GLEW_OK) {
        logger_error("Failed to initialize GLEW");
        return APP_INIT_EXIT_SUCCESS;  // Failed
    }

    // Load model
    if (args->model_path) {
        model = mdl_load_file(args->model_path);
        if (!model) {
            logger_error("Failed to load model: %s", args->model_path);
            return APP_INIT_EXIT_SUCCESS;  // Failed
        }
    }

    // Initialize renderer, camera, etc.
    // ... more initialization ...

    return APP_INIT_SUCCESS;  // All systems go!
}
```

**Step 3: Return Value Check**
```c
if ( app_init(&args) != APP_INIT_SUCCESS )
```

**Case A: Initialization succeeds**
- All subsystems initialized successfully
- `app_init()` returns `APP_INIT_SUCCESS` (0)
- Condition evaluates: `0 != 0` → **FALSE**
- Skip if-block, continue to line 41

**Case B: Initialization fails (e.g., model file not found)**
- `app_init()` logs error message
- `app_init()` returns `APP_INIT_EXIT_SUCCESS` (1)
- Condition evaluates: `1 != 0` → **TRUE**
- Enter if-block, return `APP_INIT_EXIT_SUCCESS` to OS
- **Program exits** (never calls app_run or app_shutdown)

**Why Skip app_shutdown() on Failure?**
This might seem wrong - shouldn't we clean up even if init fails? The answer is:
- `app_init()` is responsible for cleaning up its **own** allocations if it fails
- If init never completes, there's nothing for `app_shutdown()` to clean up
- See Section 7.X for details on app_init()'s internal cleanup

---

**Line 41: Execution Phase**
```c
app_run();
```

**What happens here:**

**No Return Value Check?**
Unlike `parse_args()` and `app_init()`, we don't check `app_run()`'s return value. Why?

Looking at the signature (Section 7.X):
```c
void app_run(void);
```

`app_run()` returns `void` (nothing). It runs until the user closes the window, then returns.

**Step 1: Function Call**
- Calls `app_run()` (defined in `cl/cl_app.c`, see Section 7.X)
- **This function blocks** - doesn't return until user quits

**Step 2: Inside app_run() (Simplified - See Section 7.X for full details)**
```c
void app_run(void) {
    while (!glfwWindowShouldClose(window)) {
        // Calculate delta time
        double current_time = glfwGetTime();
        double delta_time = current_time - last_time;
        last_time = current_time;

        // Update animation
        if (animating) {
            current_frame += anim_speed * delta_time;
            if (current_frame >= sequence->numframes) {
                current_frame = 0;  // Loop
            }
        }

        // Render frame
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render_model(model, current_frame);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
```

**Key Points:**
- This is the **main loop** - runs 60 times per second (targeting 60 FPS)
- Each iteration:
  1. Calculates time since last frame (delta time)
  2. Updates animation state
  3. Renders the model
  4. Swaps front/back buffers (double buffering)
  5. Processes input events (keyboard, mouse)
- **Blocks until user closes window** (presses X button or hits Escape)

**Duration:**
- Could be **seconds** (user opens app and immediately closes)
- Could be **hours** (user studying model animations, rotating camera)
- Averages **60 iterations per second** (16.67ms per frame)

**Step 3: Return**
- User closes window
- `app_run()` returns control to main()
- Execution continues to line 43

---

**Line 43: Cleanup Phase**
```c
app_shutdown();
```

**What happens here:**

**Step 1: Function Call**
```c
app_shutdown()
```
- Calls `app_shutdown()` (defined in `cl/cl_app.c`, see Section 7.X)
- No parameters needed (uses global application state)
- Returns `void` (no return value to check)

**Step 2: Inside app_shutdown() (Simplified - See Section 7.X for full details)**
```c
void app_shutdown(void) {
    // Free model data
    if (model) {
        mdl_free_model(model);
        model = NULL;
    }

    // Cleanup renderer
    cleanup_renderer();

    // Destroy window
    if (window) {
        glfwDestroyWindow(window);
        window = NULL;
    }

    // Terminate GLFW
    glfwTerminate();

    // Shutdown logger
    logger_shutdown();
}
```

**Key Principle: Reverse Order Cleanup**

Notice the cleanup order is **exactly opposite** of initialization:

```
INITIALIZATION ORDER:          CLEANUP ORDER:
1. logger_init()         ←→   6. logger_shutdown()
2. glfwInit()            ←→   5. glfwTerminate()
3. glfwCreateWindow()    ←→   4. glfwDestroyWindow()
4. glewInit()            ←→   3. cleanup_renderer()
5. mdl_load_file()       ←→   2. mdl_free_model()
6. setup_renderer()      ←→   1. (first to cleanup)
```

**Why Reverse Order?**
Dependencies! You can't destroy the window before freeing OpenGL resources that depend on it. Think of it like:
- Init: Put on socks, then shoes
- Cleanup: Take off shoes, then socks

**Memory Leak Prevention:**
- Every `malloc()` has a matching `free()`
- Every `init()` has a matching `shutdown()`
- Current leak status: 0 bytes (v0.3.0-alpha)

---

**Line 45: Normal Exit**
```c
return ( APP_INIT_SUCCESS );
```

**What happens here:**

**Step 1: Return Value**
- Returns `APP_INIT_SUCCESS` (value 0) to the operating system
- By UNIX convention, 0 = success

**Step 2: C Runtime Cleanup**
After main() returns, C runtime:
1. Calls any `atexit()` registered functions (Lambda doesn't use these)
2. Flushes all open file buffers (`stdout`, `stderr`, log files)
3. Closes all open file descriptors
4. Deallocates static/global variables (including `args`)
5. Returns control to operating system

**Step 3: OS Cleanup**
Operating system:
1. Reclaims all process memory (heap, stack, globals)
2. Closes any remaining file descriptors
3. Updates exit code in process table
4. Signals parent process (shell) that child exited

**Step 4: Shell**
The shell (bash, zsh, etc.):
1. Receives exit code 0
2. Sets `$?` variable to 0 (in bash/zsh: `echo $?` prints 0)
3. Continues to next command or shows prompt

**Visual Timeline:**
```
User runs: ./Lambda scientist.mdl

↓
OS loads executable into memory
↓
C runtime initialization
↓
main(argc=2, argv=["./Lambda", "scientist.mdl"]) called
↓
parse_args() → APP_INIT_SUCCESS (0) ✓
↓
app_init() → APP_INIT_SUCCESS (0) ✓
↓
app_run() → runs main loop for 30 seconds → user closes window
↓
app_shutdown() → frees all resources
↓
return APP_INIT_SUCCESS (0)
↓
C runtime cleanup
↓
OS cleanup
↓
Shell shows prompt, $? = 0
```

---

#### 5.2.7 Execution Flow Diagram

**ASCII Flowchart:**

```
                    ┌─────────────────────────┐
                    │   OS Executes Binary    │
                    └───────────┬─────────────┘
                                │
                                ▼
                    ┌─────────────────────────┐
                    │  C Runtime Init         │
                    │  (Initialize globals)   │
                    └───────────┬─────────────┘
                                │
                                ▼
         ╔══════════════════════════════════════════╗
         ║  main(argc, argv)                        ║
         ║  Entry Point                             ║
         ╚════════════════╤═════════════════════════╝
                          │
                          ▼
         ┌────────────────────────────────────────┐
         │  parse_args(argc, argv, &args)         │
         │  ┌──────────────────────────────────┐  │
         │  │ • Parse command-line flags       │  │
         │  │ • Validate arguments             │  │
         │  │ • Fill args structure            │  │
         │  └──────────────────────────────────┘  │
         └────────────┬───────────┬───────────────┘
                      │           │
            SUCCESS   │           │  EXIT_SUCCESS
              (0)     │           │     (1)
                      │           │  (--help/--version
                      │           │   or invalid args)
                      │           │
                      ▼           ▼
         ┌────────────────┐   ┌──────────────────┐
         │  Continue      │   │  return (1)      │
         └────────┬───────┘   │  Exit Early      │
                  │           └──────────────────┘
                  ▼
         ┌────────────────────────────────────────┐
         │  app_init(&args)                       │
         │  ┌──────────────────────────────────┐  │
         │  │ • Initialize logger              │  │
         │  │ • Initialize GLFW                │  │
         │  │ • Create window                  │  │
         │  │ • Initialize OpenGL (GLEW)       │  │
         │  │ • Load MDL model                 │  │
         │  │ • Setup renderer                 │  │
         │  │ • Initialize camera              │  │
         │  └──────────────────────────────────┘  │
         └────────────┬───────────┬───────────────┘
                      │           │
            SUCCESS   │           │  EXIT_SUCCESS
              (0)     │           │     (1)
                      │           │  (init failed)
                      │           │
                      ▼           ▼
         ┌────────────────┐   ┌──────────────────┐
         │  Continue      │   │  return (1)      │
         └────────┬───────┘   │  Exit with error │
                  │           └──────────────────┘
                  ▼
         ┌────────────────────────────────────────┐
         │  app_run()                             │
         │  ┌──────────────────────────────────┐  │
         │  │ MAIN LOOP (blocks here)          │  │
         │  │                                  │  │
         │  │ while (!window_should_close) {   │  │
         │  │   • Calculate delta time         │  │
         │  │   • Process input                │  │
         │  │   • Update animation             │  │
         │  │   • Update camera                │  │
         │  │   • Render model                 │  │
         │  │   • Swap buffers                 │  │
         │  │   • Poll events                  │  │
         │  │ }                                │  │
         │  │                                  │  │
         │  │ Runs until user closes window    │  │
         │  └──────────────────────────────────┘  │
         └─────────────────┬──────────────────────┘
                           │
                           │ (user closed window)
                           │
                           ▼
         ┌────────────────────────────────────────┐
         │  app_shutdown()                        │
         │  ┌──────────────────────────────────┐  │
         │  │ • Free model data                │  │
         │  │ • Cleanup renderer               │  │
         │  │ • Destroy window                 │  │
         │  │ • Terminate GLFW                 │  │
         │  │ • Shutdown logger                │  │
         │  └──────────────────────────────────┘  │
         └─────────────────┬──────────────────────┘
                           │
                           ▼
         ┌────────────────────────────────────────┐
         │  return APP_INIT_SUCCESS (0)           │
         └─────────────────┬──────────────────────┘
                           │
                           ▼
         ┌────────────────────────────────────────┐
         │  C Runtime Cleanup                     │
         │  • Flush buffers                       │
         │  • Close files                         │
         │  • Free static variables               │
         └─────────────────┬──────────────────────┘
                           │
                           ▼
         ┌────────────────────────────────────────┐
         │  OS Cleanup                            │
         │  • Reclaim memory                      │
         │  • Update process table                │
         │  • Signal parent (shell)               │
         └─────────────────┬──────────────────────┘
                           │
                           ▼
                    ┌──────────────┐
                    │  Exit (0)    │
                    │  $? = 0      │
                    └──────────────┘
```

**Alternative Flows:**

**Flow 1: User runs `./Lambda --help`**
```
main() → parse_args() → [prints help] → return (1) → exit
         ↑────────────────────────────────┘
         Never calls app_init, app_run, app_shutdown
```

**Flow 2: User runs `./Lambda invalid.mdl` (file doesn't exist)**
```
main() → parse_args() (success) → app_init() → [error loading] → return (1) → exit
                                   ↑─────────────────────────────────┘
                                   Never calls app_run, app_shutdown
```

**Flow 3: User runs `./Lambda scientist.mdl` (normal execution)**
```
main() → parse_args() → app_init() → app_run() → [user closes] → app_shutdown() → return (0) → exit
```

#### 5.2.8 Error Handling

**Error Handling Strategy:**

Lambda uses a **propagate-and-exit** strategy at the main() level:

**What This Means:**
- main() does NOT handle errors itself
- main() delegates error handling to callees (`parse_args`, `app_init`)
- If callee encounters error, it:
  1. Logs error message (via logger system)
  2. Returns error code (`APP_INIT_EXIT_SUCCESS`)
  3. main() sees error code and exits immediately

**Why This Design?**

**Principle: Separation of Concerns**
- main() is **orchestration**, not **implementation**
- Error details belong in the functions that encounter them
- main() just decides: "Continue or stop?"

**Example: File Not Found Error**

**Bad Design (main() handles error):**
```c
int main(int argc, char const *argv[]) {
    parse_args(argc, argv, &args);

    // main() shouldn't know about file I/O!
    FILE *f = fopen(args.model_path, "rb");
    if (!f) {
        fprintf(stderr, "Error: Cannot open %s\n", args.model_path);
        return APP_INIT_EXIT_SUCCESS;
    }
    // ... more file handling ...
}
```

**Good Design (app_init() handles error):**
```c
// main.c
int main(int argc, char const *argv[]) {
    parse_args(argc, argv, &args);
    if (app_init(&args) != APP_INIT_SUCCESS) {
        return APP_INIT_EXIT_SUCCESS;  // Exit, error already logged
    }
    // ...
}

// cl_app.c
int app_init(app_args_t *args) {
    model = mdl_load_file(args->model_path);
    if (!model) {
        logger_error("Failed to load model: %s", args->model_path);
        logger_error("Ensure file exists and has .mdl extension");
        return APP_INIT_EXIT_SUCCESS;
    }
    // ...
}
```

**Error Categories:**

| Error Type | Handled By | Example |
|------------|------------|---------|
| Invalid arguments | `parse_args()` | Unknown flag, missing model path |
| Missing files | `app_init() → mdl_load_file()` | Model file not found |
| Corrupted data | `mdl_load_file()` | Invalid MDL header |
| System failures | `app_init()` | GLFW init failed, window creation failed |
| OpenGL errors | `app_init() → renderer` | GLEW init failed, shader compilation failed |

**Why No try/catch?**

C doesn't have exceptions (that's a C++ feature). Error handling in C uses:
1. **Return codes** (what Lambda uses)
2. **errno global** (used by stdlib functions like `fopen`)
3. **setjmp/longjmp** (like goto on steroids, rarely used)

**Return Code Pattern:**

```c
// Success: return 0
// Failure: return non-zero

if (function() != SUCCESS) {
    // Handle error
}
```

#### 5.2.9 Side Effects

**Side Effects of main():**

**Direct Side Effects:**
1. **Modifies `args` structure** (via `parse_args()`)
2. **Allocates heap memory** (via `app_init()` → model loading, OpenGL setup)
3. **Creates OS resources** (via `app_init()` → GLFW window)
4. **Writes to log file** (if `--log-file` specified)
5. **Reads model file from disk** (via `app_init()` → `mdl_load_file()`)
6. **Returns exit code to OS** (via `return` statement)

**Indirect Side Effects (via function calls):**

**Via `parse_args()`:**
- Reads from `argv` (OS-provided data)
- Writes to `stdout` (if --help/--version)
- Modifies `args` structure fields

**Via `app_init()`:**
- Allocates ~10 MB heap memory (model data, OpenGL buffers)
- Creates window (OS resource)
- Initializes OpenGL context (GPU state)
- Opens log file (file descriptor)
- Reads model file (disk I/O)

**Via `app_run()`:**
- Continuously polls input events (keyboard, mouse)
- Renders to screen (GPU operations)
- Swaps buffers (display updates)
- Runs for seconds/minutes/hours (time-dependent)

**Via `app_shutdown()`:**
- Frees heap memory
- Closes window (OS resource)
- Terminates GLFW (cleans up OS state)
- Closes log file

**Global State Modified:**

**File-Scope Variables:**
- `static app_args_t args` (modified by parse_args)

**External State (in other modules):**
- `app_state_t` structure in `cl_app.c` (holds window, model, camera, etc.)
- Logger state in `util_logger.c` (log file handle, verbosity level)
- GLFW global state (window management)
- OpenGL global state (GPU context)

**Side Effects That Are Intentional:**
- Loading model into memory (that's the point!)
- Displaying window (that's the point!)
- Writing to log file (debugging/diagnostics)

**Side Effects That Would Be Bugs:**
- Memory leaks (allocations without matching frees)
- File descriptor leaks (files left open)
- Modifying `argv` strings (const violation)

#### 5.2.10 Platform-Specific Behavior

**Cross-Platform Compatibility:**

Lambda is designed to run on:
- macOS (primary development platform)
- Linux (Ubuntu, Fedora, Arch, etc.)
- Windows (via MinGW or MSVC)

**Platform Differences in main():**

**Executable Name (`argv[0]`):**

| Platform | Example `argv[0]` |
|----------|-------------------|
| macOS    | `./Lambda` or `/Users/karlo/build/bin/Lambda` |
| Linux    | `./Lambda` or `/home/karlo/build/bin/Lambda` |
| Windows  | `.\\Lambda.exe` or `C:\\Users\\karlo\\build\\bin\\Lambda.exe` |

**Path Separators:**
- macOS/Linux: `/` (forward slash)
- Windows: `\` (backslash, but accepts `/` too)

**Newlines:**
- macOS/Linux: `\n` (LF)
- Windows: `\r\n` (CRLF)

Lambda normalizes these differences via:
- GLFW (cross-platform window/input)
- OpenGL (cross-platform graphics)
- Standard C library (cross-platform file I/O)

**Compilation Differences:**

**Compiler Flags:**
```cmake
# CMakeLists.txt handles platform differences
if(APPLE)
    target_link_libraries(Lambda "-framework Cocoa" "-framework OpenGL" "-framework IOKit")
elseif(UNIX)
    target_link_libraries(Lambda GL X11 pthread dl)
elseif(WIN32)
    target_link_libraries(Lambda opengl32 gdi32)
endif()
```

**Entry Point:**

| Platform | True Entry Point | Notes |
|----------|------------------|-------|
| macOS    | `_main` (symbol) | C runtime calls main() after initialization |
| Linux    | `_start` → `__libc_start_main` → `main` | glibc handles setup |
| Windows  | `WinMainCRTStartup` → `main` | MSVC runtime |

For user code, these differences are transparent - main() is always the entry point.

#### 5.2.11 Performance Characteristics

**Execution Time:**

**Breakdown by Phase:**

| Phase | Typical Time | Dominated By |
|-------|--------------|--------------|
| `parse_args()` | < 1 ms | String comparisons (`strcmp`) |
| `app_init()` | 50-200 ms | File I/O (loading MDL), OpenGL init |
| `app_run()` | Seconds to hours | User interaction (blocks until window closed) |
| `app_shutdown()` | 10-50 ms | Freeing memory, GLFW cleanup |

**Total Overhead (excluding app_run):**
- Best case: ~60 ms (small model, SSD)
- Worst case: ~250 ms (large model, HDD)
- Average: ~100 ms

**Performance Bottlenecks:**

**1. Model Loading (`app_init` → `mdl_load_file`)**
- Disk I/O: Reading file from disk
- Parsing: Decompressing vertices, building bone hierarchy
- GPU Upload: Transferring geometry to VRAM

**Optimization Potential:**
- Memory-map file instead of read()
- Lazy-load textures (only load when needed)
- Multi-threaded model loading (currently single-threaded)

**2. GLFW/GLEW Initialization**
- Creating OpenGL context (OS call)
- Enumerating GPU extensions (GLEW)

**Optimization Potential:**
- Share context between multiple windows (not applicable to this app)

**Memory Usage:**

| Component | Size (typical) | Notes |
|-----------|----------------|-------|
| `args` structure | ~64 bytes | Static allocation |
| MDL model data | 100 KB - 5 MB | Depends on model complexity |
| Textures | 256 KB - 2 MB | Palette-based, relatively small |
| OpenGL buffers | Same as model | Duplicate in VRAM |
| Window/GLFW | ~1 MB | OS resources |

**Total RSS (Resident Set Size):**
- Typical: ~10 MB
- With large model: ~20 MB
- Maximum observed: ~50 MB (very complex models)

**Stack Depth:**

```
main()
 ├─ parse_args()       (depth 2)
 ├─ app_init()         (depth 2)
 │   ├─ logger_init()      (depth 3)
 │   ├─ glfwInit()         (depth 3)
 │   ├─ mdl_load_file()    (depth 3)
 │   │   ├─ fread()            (depth 4)
 │   │   └─ parse_bones()      (depth 4)
 │   └─ setup_renderer()   (depth 3)
 ├─ app_run()          (depth 2)
 │   └─ render_frame()     (depth 3)
 │       └─ glDrawElements()  (depth 4)
 └─ app_shutdown()     (depth 2)
```

**Max stack depth:** ~6 levels (very shallow, no recursion)

**Stack size:** ~2 KB per frame × 6 frames = ~12 KB total (negligible)

#### 5.2.12 Example Usage Scenarios

**Scenario 1: Display Help**

**Command:**
```bash
./Lambda --help
```

**Execution Flow:**
1. `main()` starts
2. `parse_args()` sees `--help` flag
3. `parse_args()` calls `print_usage()` → prints help text to stdout
4. `parse_args()` returns `APP_INIT_EXIT_SUCCESS` (1)
5. `main()` checks: `1 != APP_INIT_SUCCESS` → TRUE
6. `main()` returns `APP_INIT_EXIT_SUCCESS` (1)
7. Program exits

**Output:**
```
Lambda Model Viewer v0.3.0-alpha

Usage: Lambda [OPTIONS] <model.mdl>

Options:
  --help              Show this help message
  --version           Show version information
  --verbose           Enable verbose logging
  --dump              Dump model information and exit
  --dump-extended     Dump detailed model information
  ...
```

**Exit code:** 1 (but still considered success by shell)

---

**Scenario 2: Show Version**

**Command:**
```bash
./Lambda --version
```

**Execution Flow:**
1. `main()` starts
2. `parse_args()` sees `--version` flag
3. `parse_args()` calls `print_version_info()` → prints version to stdout
4. `parse_args()` returns `APP_INIT_EXIT_SUCCESS` (1)
5. `main()` returns `APP_INIT_EXIT_SUCCESS` (1)
6. Program exits

**Output:**
```
Lambda Model Viewer v0.3.0-alpha
Build date: Jan 16 2025 14:32:11
Compiler: Apple clang version 15.0.0
OpenGL: 4.1
GLFW: 3.3.8
```

**Exit code:** 1

---

**Scenario 3: Load and Display Model**

**Command:**
```bash
./Lambda models/scientist.mdl
```

**Execution Flow:**
1. `main()` starts
2. `parse_args()` parses arguments:
   - `argv[0] = "./Lambda"`
   - `argv[1] = "models/scientist.mdl"`
   - Sets `args.model_path = "models/scientist.mdl"`
   - Returns `APP_INIT_SUCCESS` (0)
3. `app_init(&args)` initializes:
   - Logger
   - GLFW window
   - OpenGL context
   - Loads `models/scientist.mdl`
   - Returns `APP_INIT_SUCCESS` (0)
4. `app_run()` starts main loop:
   - Displays window with 3D model
   - User can rotate camera, play animations
   - Runs for 45 seconds
   - User presses Escape to quit
5. `app_shutdown()` cleans up:
   - Frees model data
   - Destroys window
   - Terminates GLFW
6. `main()` returns `APP_INIT_SUCCESS` (0)
7. Program exits

**Output:** (graphical window, minimal console output)
```
[INFO] Logger initialized: level=NORMAL
[INFO] GLFW initialized: version 3.3.8
[INFO] Window created: 1024x768
[INFO] OpenGL initialized: version 4.1
[INFO] Loading model: models/scientist.mdl
[INFO] Model loaded: 1234 vertices, 42 bones, 56 sequences
[INFO] Entering main loop
[INFO] Exiting main loop (45.2 seconds elapsed)
[INFO] Shutdown complete
```

**Exit code:** 0

---

**Scenario 4: Dump Model Info**

**Command:**
```bash
./Lambda models/scientist.mdl --dump
```

**Execution Flow:**
1. `main()` starts
2. `parse_args()` parses arguments:
   - Sets `args.model_path = "models/scientist.mdl"`
   - Sets `args.dump_only = true`
   - Returns `APP_INIT_SUCCESS` (0)
3. `app_init(&args)` sees `dump_only` flag:
   - Initializes logger only (no OpenGL/window)
   - Loads model
   - Calls `mdl_dump_info(model)` → prints to stdout
   - Returns `APP_INIT_EXIT_SUCCESS` (1) - "exit after dump"
4. `main()` returns `APP_INIT_EXIT_SUCCESS` (1)
5. Program exits

**Output:**
```
=== scientist.mdl ===
Bones: 42
  [0] Bip01 (parent: -1)
  [1] Bip01 Pelvis (parent: 0)
  ...
Sequences: 56
  [0] idle1 (24 frames, 30 fps)
  [1] walk (16 frames, 30 fps)
  ...
Textures: 3
  [0] scientist_head.bmp (256x256)
  ...
```

**Exit code:** 1

---

**Scenario 5: File Not Found**

**Command:**
```bash
./Lambda nonexistent.mdl
```

**Execution Flow:**
1. `main()` starts
2. `parse_args()` parses arguments:
   - Sets `args.model_path = "nonexistent.mdl"`
   - Returns `APP_INIT_SUCCESS` (0)
3. `app_init(&args)` tries to load model:
   - Calls `mdl_load_file("nonexistent.mdl")`
   - `mdl_load_file()` tries to open file → fails
   - `mdl_load_file()` logs error, returns NULL
   - `app_init()` sees NULL, logs error, returns `APP_INIT_EXIT_SUCCESS` (1)
4. `main()` returns `APP_INIT_EXIT_SUCCESS` (1)
5. Program exits

**Output:**
```
[ERROR] Failed to open file: nonexistent.mdl
[ERROR] No such file or directory
[ERROR] Failed to load model: nonexistent.mdl
```

**Exit code:** 1

#### 5.2.13 Related Functions

**Functions Called Directly by main():**

| Function | Defined In | Section | Purpose |
|----------|------------|---------|---------|
| `parse_args()` | `util/util_args.c` | 6.3.1 | Parse command-line arguments |
| `app_init()` | `cl/cl_app.c` | 7.X | Initialize all subsystems |
| `app_run()` | `cl/cl_app.c` | 7.X | Run main loop |
| `app_shutdown()` | `cl/cl_app.c` | 7.X | Cleanup all resources |

**Functions Called Indirectly (via call chain):**

See Section 4.2 for complete call graph. Key indirect calls:

**Via `parse_args()`:**
- `print_usage()` - Display help text
- `print_version_info()` - Display version
- `print_banner()` - Display ASCII art banner

**Via `app_init()`:**
- `logger_init()` - Initialize logging system
- `glfwInit()` - Initialize GLFW
- `glewInit()` - Initialize GLEW
- `mdl_load_file()` - Load MDL model
- `setup_renderer()` - Initialize OpenGL
- `Camera_Init()` - Initialize camera

**Via `app_run()`:**
- `glfwPollEvents()` - Process input
- `render_frame()` - Render model
- `glfwSwapBuffers()` - Swap front/back buffers

**Via `app_shutdown()`:**
- `mdl_free_model()` - Free model data
- `cleanup_renderer()` - Free OpenGL resources
- `glfwTerminate()` - Cleanup GLFW
- `logger_shutdown()` - Close log file

#### 5.2.14 Known Issues

**Current Issues (v0.3.0-alpha):**

**Issue #1: No Error Code Distinction**

**Problem:**
`APP_INIT_EXIT_SUCCESS` is used for both:
- Clean early exit (--help/--version) - not an error
- Actual errors (file not found, init failed) - real errors

**Impact:**
Cannot distinguish success from failure via exit code alone. Shell scripts can't detect failures:
```bash
./Lambda invalid.mdl || echo "Failed!"  # Doesn't work - always exits with 1
```

**Workaround:**
Check console output for `[ERROR]` messages.

**Future Fix:**
Add `APP_INIT_ERROR` (-1) for real failures, use `APP_INIT_EXIT_SUCCESS` (1) only for clean early exits.

---

**Issue #2: No Signal Handling**

**Problem:**
If user sends SIGINT (Ctrl+C) or SIGTERM, program exits immediately without cleanup.

**Impact:**
- Model data not freed (memory leak if run under Valgrind)
- Log file not flushed (last messages lost)
- GLFW state not cleaned up

**Workaround:**
Don't forcefully kill the program - close window normally.

**Future Fix:**
Add signal handlers:
```c
signal(SIGINT, signal_handler);   // Catch Ctrl+C
signal(SIGTERM, signal_handler);  // Catch kill command
```

---

**Issue #3: stdio.h/stdlib.h Unused**

**Problem:**
main.c includes `<stdio.h>` and `<stdlib.h>` but doesn't use them.

**Impact:**
Misleading for developers reading the code.

**Workaround:**
None needed - harmless.

**Future Fix:**
Remove unused includes, or use them (e.g., `fprintf` for errors).

#### 5.2.15 TODO Items

**Planned Improvements:**

**TODO #1: Add Signal Handling**
```c
// Catch Ctrl+C, terminate gracefully
static volatile sig_atomic_t interrupted = 0;

void signal_handler(int signum) {
    interrupted = 1;
}

int main(int argc, char const *argv[]) {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // In app_run(), check interrupted flag
    // Exit loop cleanly if interrupted
}
```

**TODO #2: Distinguish Exit Codes**
```c
#define APP_INIT_ERROR       -1  // Real error (file not found, malloc fail)
#define APP_INIT_SUCCESS      0  // Normal success
#define APP_INIT_EXIT_SUCCESS 1  // Clean early exit (--help, --version)

// Use APP_INIT_ERROR for failures:
if (app_init(&args) == APP_INIT_ERROR) {
    return EXIT_FAILURE;  // Return 1 to shell
}
```

**TODO #3: Add argc Validation**
```c
// Sanity check: argc should always be >= 1
if (argc < 1 || argv == NULL) {
    fprintf(stderr, "Fatal: Invalid arguments from OS\n");
    return EXIT_FAILURE;
}
```

**TODO #4: Add atexit() Handler**
```c
void emergency_cleanup(void) {
    // Called if exit() is called anywhere in program
    logger_shutdown();
    glfwTerminate();
}

int main(int argc, char const *argv[]) {
    atexit(emergency_cleanup);
    // ... rest of main ...
}
```

**TODO #5: Log Start/End**
```c
int main(int argc, char const *argv[]) {
    parse_args(argc, argv, &args);

    logger_init(args.log_level, args.log_file);
    logger_info("Lambda starting: version %s", VERSION);

    // ... rest of main ...

    logger_info("Lambda exiting: total runtime %.2f seconds", total_time);
    app_shutdown();
    return APP_INIT_SUCCESS;
}
```

---

**Section 5.2 Complete!**

Next: **Section 6 - Argument Parsing** (util/util_args.c)

---

## 6. Argument Parsing (util/util_args.h & util/util_args.c)

### 6.1 Module Overview

#### 6.1.1 Purpose

The argument parsing module is responsible for converting user-provided command-line arguments into a structured format that the application can use. This module is the **first point of interaction** between the user and the application.

**Core Responsibilities:**
1. Parse command-line flags and options
2. Validate user input (file existence, format, required arguments)
3. Display help and version information
4. Populate the `app_args_t` structure with parsed values
5. Handle errors gracefully with helpful messages

**Design Philosophy:**
- **User-friendly:** Clear error messages with suggestions
- **Fail-fast:** Validate immediately, before expensive initialization
- **Informative:** Detailed help and version output
- **Unix-style:** Standard flag conventions (--long, -short)

#### 6.1.2 File Locations

```
src/util/util_args.h    (Interface - 91 lines)
src/util/util_args.c    (Implementation - 331 lines)
```

#### 6.1.3 Dependencies

**Header Dependencies:**
```c
// util_args.h
#include <stdbool.h>    // bool type
#include <stdio.h>      // printf, fprintf

// util_args.c
#include "util_args.h"
#include "version.h"    // HLMV_VERSION_* macros
#include <string.h>     // strcmp, strlen
#include <ctype.h>      // tolower

#ifdef _WIN32
#include <io.h>         // _access (Windows)
#else
#include <unistd.h>     // access (Unix)
#include <sys/stat.h>   // File stat functions
#endif
```

**Why These Dependencies?**
- `version.h`: Auto-generated header with build info (version, git commit, compiler, features)
- `string.h`: String comparison (`strcmp`) for flag matching
- `ctype.h`: Case-insensitive extension checking (`tolower`)
- Platform-specific includes: Cross-platform file existence checking

#### 6.1.4 Module Interface

**Public Functions (util_args.h):**

| Function | Purpose | Called By |
|----------|---------|-----------|
| `parse_args()` | Main parsing function | `main()` |
| `print_usage()` | Display help text | `parse_args()` (indirectly) |
| `print_banner()` | Display copyright banner | Currently unused |
| `print_version_info()` | Display version details | `parse_args()` (indirectly) |

**Private (Static) Functions (util_args.c):**

| Function | Purpose | Used By |
|----------|---------|---------|
| `file_exists()` | Check if file exists | `parse_args()` |
| `has_mdl_extensions()` | Validate .mdl extension | `parse_args()` |

**Public Types:**

| Type | Purpose |
|------|---------|
| `dump_level_t` | Enumeration for dump detail levels |
| `log_detail_t` | Enumeration for logging verbosity |
| `app_args_t` | Structure holding all parsed arguments |

---

### 6.2 Data Structures

#### 6.2.1 dump_level_t Enumeration

**Definition:**
```c
typedef enum {
    DUMP_NONE = 0,  // No dump - just run viewer
    DUMP_BASIC,     // --dump: Overview (header, bones, sequences)
    DUMP_EXTENDED   // --dump-ex: Deep dive (vertices, meshes, raw data)
} dump_level_t;
```

**Purpose:**
Controls how much model information to print to stdout.

**Values:**

| Value | Integer | Flag | Output |
|-------|---------|------|--------|
| `DUMP_NONE` | 0 | (none) | No dump, run viewer normally |
| `DUMP_BASIC` | 1 | `--dump`, `-d` | Print model header, bones, sequences |
| `DUMP_EXTENDED` | 2 | `--dump-ex`, `-dx` | Print everything including vertices, meshes, raw offsets |

**Why Enum Instead of Boolean?**

Could have used `bool dump_basic; bool dump_extended;` but enum is better because:
1. **Mutually exclusive:** Can't have both BASIC and EXTENDED simultaneously
2. **Scalable:** Easy to add DUMP_MINIMAL, DUMP_JSON, etc. in future
3. **Type-safe:** Compiler prevents invalid values
4. **Self-documenting:** `dump_level == DUMP_BASIC` is clearer than `dump_basic && !dump_extended`

**Default Value:** `DUMP_NONE` (0) - assigned in `parse_args()` initialization.

**Usage Example:**
```c
// In model dumping code:
if (args->dump_level >= DUMP_BASIC) {
    print_model_header();
    print_bones();
    print_sequences();
}

if (args->dump_level >= DUMP_EXTENDED) {
    print_vertices();
    print_meshes();
    print_raw_offsets();
}
```

---

#### 6.2.2 log_detail_t Enumeration

**Definition:**
```c
typedef enum {
    LOG_LEVEL_QUIET = 0,   // Only errors
    LOG_LEVEL_NORMAL,      // Info and above
    LOG_LEVEL_VERBOSE,     // Debug and above
    LOG_LEVEL_TRACE        // Everything including trace
} log_detail_t;
```

**Purpose:**
Controls logging verbosity throughout the application.

**Values:**

| Value | Integer | Flags | What Gets Logged |
|-------|---------|-------|------------------|
| `LOG_LEVEL_QUIET` | 0 | `--quiet`, `-q` | ERROR only |
| `LOG_LEVEL_NORMAL` | 1 | (default) | ERROR, WARN, INFO |
| `LOG_LEVEL_VERBOSE` | 2 | `--verbose`, `-vv` | ERROR, WARN, INFO, DEBUG |
| `LOG_LEVEL_TRACE` | 3 | `--trace` | ERROR, WARN, INFO, DEBUG, TRACE |

**Log Level Hierarchy:**

```
QUIET (0)    →  ERROR
  ↓
NORMAL (1)   →  ERROR, WARN, INFO
  ↓
VERBOSE (2)  →  ERROR, WARN, INFO, DEBUG
  ↓
TRACE (3)    →  ERROR, WARN, INFO, DEBUG, TRACE
```

Higher levels include all lower levels. This is a standard logging pattern.

**Default Value:** `LOG_LEVEL_NORMAL` (1) - balanced verbosity.

**Why Four Levels?**

- **QUIET:** Production use, minimal noise
- **NORMAL:** Development use, important events
- **VERBOSE:** Debugging, detailed execution flow
- **TRACE:** Deep debugging, every function call (can be overwhelming)

**Usage Example:**
```c
// In logger system:
void logger_info(const char *fmt, ...) {
    if (current_log_level < LOG_LEVEL_NORMAL) return;  // Skip if quiet
    // ... print message ...
}

void logger_trace(const char *fmt, ...) {
    if (current_log_level < LOG_LEVEL_TRACE) return;  // Skip unless trace enabled
    // ... print message ...
}
```

---

#### 6.2.3 app_args_t Structure

**Definition:**
```c
typedef struct {
    const char *model_path;      // Path to .mdl file
    dump_level_t dump_level;     // Dump detail level
    bool dump_only;              // Exit after dump (no viewer)
    bool quiet;                  // Suppress all non-error output (deprecated, use log_level)
    log_detail_t log_level;      // Logging verbosity
    const char *log_file;        // Optional log file path
    bool show_help;              // Show usage
    bool show_version;           // Show version information
} app_args_t;
```

**Purpose:**
Central structure holding all parsed command-line arguments. This is the **contract** between `parse_args()` and the rest of the application.

**Memory Size:** ~32 bytes on 64-bit systems (2 pointers + 6 enums/bools)

**Field-by-Field Analysis:**

---

**Field 1: `const char *model_path`**

| Property | Value |
|----------|-------|
| **Type** | `const char *` (pointer to read-only string) |
| **Purpose** | Path to the .mdl file to load |
| **Set by** | User provides model path as positional argument |
| **Default** | `NULL` |
| **Validated** | Yes - must exist, must have .mdl extension |

**Why `const char *`?**
- Points directly to `argv[i]` string (no copying needed)
- `const` prevents accidental modification
- OS owns the memory, we just hold a pointer

**Examples:**
```c
// User runs: ./Lambda models/scientist.mdl
args.model_path = "models/scientist.mdl"

// User runs: ./Lambda /absolute/path/to/barney.mdl
args.model_path = "/absolute/path/to/barney.mdl"

// User runs: ./Lambda --help
args.model_path = NULL  // No model needed for help
```

**Validation:**
- Must not be `NULL` (unless showing help/version)
- File must exist on disk
- Must end with ".mdl" (case-insensitive)

---

**Field 2: `dump_level_t dump_level`**

| Property | Value |
|----------|-------|
| **Type** | `dump_level_t` enum |
| **Purpose** | How much model info to print |
| **Set by** | `--dump`, `--dump-ex` flags |
| **Default** | `DUMP_NONE` |
| **Validated** | No validation needed (enum enforces valid values) |

**Examples:**
```c
// ./Lambda model.mdl
args.dump_level = DUMP_NONE  // Just run viewer

// ./Lambda model.mdl --dump
args.dump_level = DUMP_BASIC  // Print overview

// ./Lambda model.mdl --dump-ex
args.dump_level = DUMP_EXTENDED  // Print everything
```

---

**Field 3: `bool dump_only`**

| Property | Value |
|----------|-------|
| **Type** | `bool` |
| **Purpose** | Exit after dumping (don't open viewer window) |
| **Set by** | `--dump-only` flag |
| **Default** | `false` |
| **Validated** | No validation needed |

**Why Separate from `dump_level`?**

These are orthogonal concerns:
- `dump_level`: **What** to dump (none/basic/extended)
- `dump_only`: **Whether to exit** after dumping

**Examples:**
```c
// ./Lambda model.mdl --dump
dump_level = DUMP_BASIC, dump_only = false  // Dump AND run viewer

// ./Lambda model.mdl --dump --dump-only
dump_level = DUMP_BASIC, dump_only = true   // Dump then exit

// ./Lambda model.mdl --dump-only
dump_level = DUMP_NONE, dump_only = true    // Exit without dump (weird but allowed)
```

**Typical Usage:**
```bash
# Generate a model report to file
./Lambda scientist.mdl --dump-ex --dump-only > report.txt
```

---

**Field 4: `bool quiet` (DEPRECATED)**

| Property | Value |
|----------|-------|
| **Type** | `bool` |
| **Purpose** | Suppress all non-error output (use `log_level` instead) |
| **Set by** | `--quiet`, `-q` flag |
| **Default** | `false` |
| **Status** | **DEPRECATED** - kept for backward compatibility |

**Why Deprecated?**

Initially had boolean `quiet` flag, but later added full log level system. Now:
- `--quiet` sets both `quiet = true` AND `log_level = LOG_LEVEL_QUIET`
- Only `log_level` is checked by logger
- `quiet` field is redundant

**Should Be Removed?**
Yes, but kept to avoid breaking code that might still check `args->quiet`. Will be removed in future version.

---

**Field 5: `log_detail_t log_level`**

| Property | Value |
|----------|-------|
| **Type** | `log_detail_t` enum |
| **Purpose** | Control logging verbosity |
| **Set by** | `--quiet`, `--verbose`, `--trace` flags |
| **Default** | `LOG_LEVEL_NORMAL` |
| **Validated** | No validation needed (enum enforces valid values) |

**Flag Mapping:**
```c
--quiet   → LOG_LEVEL_QUIET
(none)    → LOG_LEVEL_NORMAL  (default)
--verbose → LOG_LEVEL_VERBOSE
--trace   → LOG_LEVEL_TRACE
```

**Examples:**
```bash
./Lambda model.mdl --quiet     # Only errors
./Lambda model.mdl             # Info + warnings + errors (default)
./Lambda model.mdl --verbose   # Debug + info + warnings + errors
./Lambda model.mdl --trace     # Everything
```

---

**Field 6: `const char *log_file`**

| Property | Value |
|----------|-------|
| **Type** | `const char *` (pointer to read-only string) |
| **Purpose** | Optional log file path |
| **Set by** | `--log-file <path>` flag |
| **Default** | `NULL` |
| **Validated** | Yes - requires argument after flag |

**Examples:**
```bash
# Log to stdout (default)
./Lambda model.mdl

# Log to file
./Lambda model.mdl --log-file debug.log

# Log to file with trace level
./Lambda model.mdl --trace --log-file full_trace.log
```

**Validation:**
```c
if (strcmp(arg, "--log-file") == 0) {
    if (i + 1 >= argc) {
        fprintf(stderr, "ERROR: --log-file requires a path argument\n");
        return -1;
    }
    args->log_file = argv[++i];  // Grab next argument
}
```

**Memory Management:**
- Points directly to `argv[i]` (no allocation)
- OS owns the memory
- Logger opens file later during `logger_init()`

---

**Field 7: `bool show_help`**

| Property | Value |
|----------|-------|
| **Type** | `bool` |
| **Purpose** | Indicates user wants help text |
| **Set by** | `--help`, `-h` flag, or argc < 2 |
| **Default** | `false` |
| **Validated** | No validation needed |

**Triggers:**
1. Explicit: `./Lambda --help`
2. Implicit: `./Lambda` (no arguments)

**Execution Flow:**
```c
if (args->show_help) {
    print_usage(argv[0]);
    return APP_INIT_EXIT_SUCCESS;  // Exit early
}
```

---

**Field 8: `bool show_version`**

| Property | Value |
|----------|-------|
| **Type** | `bool` |
| **Purpose** | Indicates user wants version info |
| **Set by** | `--version`, `-v` flag |
| **Default** | `false` |
| **Validated** | No validation needed |

**Execution Flow:**
```c
if (args->show_version) {
    print_version_info();
    return APP_INIT_EXIT_SUCCESS;  // Exit early
}
```

---

**Structure Initialization:**

**In main.c:**
```c
static app_args_t args = { 0 };  // Zero-initialize all fields
```

**In parse_args():**
```c
// Explicit initialization (redundant with { 0 }, but good practice)
args->model_path = NULL;
args->dump_level = DUMP_NONE;
args->dump_only = false;
args->quiet = false;
args->log_level = LOG_LEVEL_NORMAL;
args->log_file = NULL;
args->show_help = false;
args->show_version = false;
```

**Why Initialize Twice?**
- `{ 0 }` in main.c: Safety net (ensures no garbage values)
- Explicit in parse_args(): Self-documenting, shows intent, allows changing defaults

---

### 6.3 Functions

#### 6.3.1 parse_args() - Main Parsing Function

**Signature:**
```c
int parse_args( int argc, const char *argv[], app_args_t *args );
```

**Purpose:**
Parse command-line arguments and populate the `app_args_t` structure.

**Parameters:**

| Parameter | Type | Direction | Description |
|-----------|------|-----------|-------------|
| `argc` | `int` | IN | Argument count from main() |
| `argv` | `const char *[]` | IN | Argument vector from main() |
| `args` | `app_args_t *` | OUT | Pointer to structure to populate |

**Return Value:**

| Value | Constant | Meaning |
|-------|----------|---------|
| 0 | Success | Arguments parsed successfully |
| -1 | Error | Invalid arguments, error printed to stderr |

**Important:** Unlike `APP_INIT_SUCCESS`, this function uses standard C conventions:
- `0` = success
- `-1` = error

**Execution Flow:**

```
START
 ↓
Initialize args with defaults
 ↓
argc < 2? ────YES────> set show_help = true, return 0
 ↓ NO
Loop through argv[1] to argv[argc-1]
 ↓
For each argument:
  ├─ --version/-v?     → set show_version=true, return 0
  ├─ --help/-h?        → set show_help=true, return 0
  ├─ --dump/-d?        → set dump_level=DUMP_BASIC
  ├─ --dump-ex/-dx?    → set dump_level=DUMP_EXTENDED
  ├─ --dump-only?      → set dump_only=true
  ├─ --quiet/-q?       → set log_level=QUIET
  ├─ --verbose/-vv?    → set log_level=VERBOSE
  ├─ --trace?          → set log_level=TRACE
  ├─ --log-file?       → grab next arg as log_file
  ├─ No '-' prefix?    → validate and set as model_path
  └─ Unknown?          → print error, return -1
 ↓
Validate: must have model_path unless show_help or show_version
 ↓
return 0
```

**Line-by-Line Walkthrough:**

**Lines 237-245: Initialize Defaults**
```c
args->model_path = NULL;
args->dump_level = DUMP_NONE;
args->dump_only = false;
args->quiet = false;
args->log_level = LOG_LEVEL_NORMAL;  // Default to normal
args->log_file = NULL;
args->show_help = false;
args->show_version = false;
```

**Why explicit initialization?**
- Defensive programming - ensures known state
- Self-documenting - shows all possible fields
- Allows changing defaults (e.g., `log_level = VERBOSE` for debug builds)

---

**Lines 247-251: No Arguments = Help**
```c
if ( argc < 2 ) {
    args->show_help = true;
    return ( 0 );
}
```

**Behavior:**
- User runs `./Lambda` with no arguments
- Set `show_help = true`
- Return success (0) - not an error, just show help

**Why argc < 2?**
- `argc == 1` means only program name (`argv[0]`) provided
- No model file or flags
- Assume user wants help

---

**Lines 254-320: Main Parsing Loop**
```c
for ( int i = 1; i < argc; i++ ) {
    const char *arg = argv[i];
    // ... flag matching ...
}
```

**Loop starts at i=1** because `argv[0]` is program name.

**Each iteration checks current argument against known flags.**

---

**Lines 258-261: Version Flag**
```c
if ( strcmp( arg, "--version" ) == 0 || strcmp( arg, "-v" ) == 0 ) {
    args->show_version = true;
    return ( 0 );
}
```

**Accepts:** `--version` or `-v`

**Behavior:**
- Set flag
- **Return immediately** (don't parse remaining args)
- Caller will call `print_version_info()` and exit

**Why return immediately?**
- Version flag is a "do this and exit" action
- No point parsing remaining flags

---

**Lines 263-266: Help Flag**
```c
else if ( strcmp( arg, "--help" ) == 0 || strcmp( arg, "-h" ) == 0 ) {
    args->show_help = true;
    return ( 0 );
}
```

Same behavior as version flag.

---

**Lines 268-277: Dump Flags**
```c
else if ( strcmp( arg, "--dump" ) == 0 || strcmp( arg, "-d" ) == 0 ) {
    args->dump_level = DUMP_BASIC;
} else if ( strcmp( arg, "--dump-ex" ) == 0 || strcmp( arg, "-dx" ) == 0 ) {
    args->dump_level = DUMP_EXTENDED;
} else if ( strcmp( arg, "--dump-only" ) == 0 ) {
    args->dump_only = true;
}
```

**No early return** - these can combine with other flags.

**Example:**
```bash
./Lambda model.mdl --dump --verbose --log-file debug.log
```
All three flags processed.

---

**Lines 279-285: Logging Level Flags**
```c
else if ( strcmp( arg, "--quiet" ) == 0 || strcmp( arg, "-q" ) == 0 ) {
    args->quiet = true;
    args->log_level = LOG_LEVEL_QUIET;
} else if ( strcmp( arg, "--verbose" ) == 0 || strcmp( arg, "-vv" ) == 0 ) {
    args->log_level = LOG_LEVEL_VERBOSE;
} else if ( strcmp( arg, "--trace" ) == 0 ) {
    args->log_level = LOG_LEVEL_TRACE;
}
```

**Note:** Sets both `quiet` and `log_level` for backward compatibility.

---

**Lines 286-292: Log File Flag**
```c
else if ( strcmp( arg, "--log-file" ) == 0 ) {
    if ( i + 1 >= argc ) {
        fprintf( stderr, "ERROR: --log-file requires a path argument\n" );
        return ( -1 );
    }
    args->log_file = argv[++i];
}
```

**This is a flag with argument.**

**Validation:**
- Check if next argument exists (`i + 1 < argc`)
- If not, print error and return -1
- If yes, grab next argument with `argv[++i]`

**Why `++i` instead of `i++`?**
- Pre-increment: increments `i`, then uses new value
- Advances loop counter to skip the log file path on next iteration
- Prevents treating log file path as a separate flag

**Example:**
```bash
./Lambda model.mdl --log-file debug.log --verbose
                               ↑         ↑
                               i         i+1 (skipped in loop)
```

---

**Lines 294-313: Model Path (Positional Argument)**
```c
else if ( arg[0] != '-' ) {
    if ( args->model_path == NULL ) {
        if ( !file_exists( arg ) ) {
            fprintf( stderr, "ERROR: File not found: '%s'\n", arg );
            return ( -1 );
        }
        if ( !has_mdl_extensions( arg ) ) {
            fprintf( stderr, "ERROR: Invalid file type '%s'\n", arg );
            fprintf( stderr, "       Only .mdl files are supported!\n" );
            return ( -1 );
        }
        args->model_path = arg;
    } else {
        fprintf( stderr, "ERROR: Multiple model files specified\n" );
        fprintf( stderr, "       Already have: %s\n", args->model_path );
        fprintf( stderr, "       Cannot use: %s\n", arg );
        return ( -1 );
    }
}
```

**Logic:**

1. **Detect positional arg:** Doesn't start with `-`
2. **Check if already have model:** Prevent multiple models
3. **Validate file exists:** Call `file_exists()` helper
4. **Validate extension:** Call `has_mdl_extensions()` helper
5. **Assign:** `args->model_path = arg`

**Error Messages:**

**File not found:**
```
ERROR: File not found: 'nonexistent.mdl'
```

**Wrong extension:**
```
ERROR: Invalid file type 'model.obj'
       Only .mdl files are supported!
```

**Multiple models:**
```
ERROR: Multiple model files specified
       Already have: scientist.mdl
       Cannot use: barney.mdl
```

---

**Lines 315-319: Unknown Flag**
```c
else {
    fprintf( stderr, "ERROR: Unknown option '%s'\n", arg );
    fprintf( stderr, "       Use --help for usage information\n" );
    return ( -1 );
}
```

Catches typos and invalid flags.

**Example:**
```
./Lambda model.mdl --verbos  (typo)

ERROR: Unknown option '--verbos'
       Use --help for usage information
```

---

**Lines 322-327: Final Validation**
```c
if ( !args->show_help && !args->show_version && args->model_path == NULL ) {
    fprintf( stderr, "ERROR: No model file specified\n" );
    fprintf( stderr, "       Use --help for usage information\n" );
    return ( -1 );
}
```

**Ensures:**
- If not showing help/version, must have model path

**Example:**
```
./Lambda --verbose  (no model)

ERROR: No model file specified
       Use --help for usage information
```

---

**Line 329: Success**
```c
return ( 0 );
```

All arguments parsed and validated successfully.

---

**Error Handling Strategy:**

**Immediate Failure:**
- Print error to stderr
- Include specific details (which file, which flag)
- Suggest `--help` for more info
- Return -1

**No Partial Success:**
- Either all arguments valid (return 0)
- Or any error encountered (return -1)
- Never returns with partially-filled `args` structure

**User-Friendly Messages:**
- Clear indication of what went wrong
- Specific file/flag mentioned
- Helpful suggestions

---

**Performance:**

| Operation | Complexity | Notes |
|-----------|------------|-------|
| Main loop | O(n) | n = argc (typically < 10) |
| `strcmp` | O(m) | m = flag length (< 20 chars) |
| `file_exists` | O(1) | System call |
| `has_mdl_extensions` | O(1) | Check last 4 chars |

**Total:** O(n × m) where n is tiny, so effectively O(1) in practice.

**Typical Time:** < 0.1 ms (negligible)

---

**Example Executions:**

**Example 1: Normal Usage**
```bash
./Lambda models/scientist.mdl --verbose
```
Result:
```c
args = {
    .model_path = "models/scientist.mdl",
    .dump_level = DUMP_NONE,
    .dump_only = false,
    .quiet = false,
    .log_level = LOG_LEVEL_VERBOSE,
    .log_file = NULL,
    .show_help = false,
    .show_version = false
}
return 0
```

---

**Example 2: Dump Mode**
```bash
./Lambda scientist.mdl --dump-ex --dump-only --log-file report.log
```
Result:
```c
args = {
    .model_path = "scientist.mdl",
    .dump_level = DUMP_EXTENDED,
    .dump_only = true,
    .quiet = false,
    .log_level = LOG_LEVEL_NORMAL,
    .log_file = "report.log",
    .show_help = false,
    .show_version = false
}
return 0
```

---

**Example 3: Help**
```bash
./Lambda --help
```
Result:
```c
args = {
    .model_path = NULL,  // Ignored
    .dump_level = DUMP_NONE,
    .dump_only = false,
    .quiet = false,
    .log_level = LOG_LEVEL_NORMAL,
    .log_file = NULL,
    .show_help = true,  ← Set
    .show_version = false
}
return 0  ← Early return
```

---

**Example 4: Error - File Not Found**
```bash
./Lambda nonexistent.mdl
```
Output to stderr:
```
ERROR: File not found: 'nonexistent.mdl'
```
Return: `-1`

---

**Example 5: Error - Multiple Models**
```bash
./Lambda scientist.mdl barney.mdl
```
Output to stderr:
```
ERROR: Multiple model files specified
       Already have: scientist.mdl
       Cannot use: barney.mdl
```
Return: `-1`

---

#### 6.3.2 file_exists() - Helper Function

**Signature:**
```c
static bool file_exists( const char *filepath );
```

**Purpose:**
Check if a file exists on the filesystem (cross-platform).

**Visibility:** `static` - private to util_args.c

**Parameters:**

| Parameter | Type | Description |
|-----------|------|-------------|
| `filepath` | `const char *` | Path to check |

**Return Value:**

| Value | Meaning |
|-------|---------|
| `true` | File exists |
| `false` | File doesn't exist or filepath is NULL |

**Implementation:**

```c
static bool file_exists( const char *filepath ) {
    if ( filepath == NULL ) {
        return false;
    }

#ifdef _WIN32
    return ( _access( filepath, 0 ) == 0 );
#else
    return ( access( filepath, F_OK ) == 0 );
#endif
}
```

**Platform-Specific:**

**Windows:**
```c
_access( filepath, 0 )
```
- `_access` = Windows version of access()
- `0` = check existence only (not permissions)
- Returns 0 if exists, -1 if not

**Unix/Linux/macOS:**
```c
access( filepath, F_OK )
```
- `access` = POSIX standard function
- `F_OK` = check file existence
- Returns 0 if exists, -1 if not

**Why Not fopen()?**

Could check existence by trying to open:
```c
FILE *f = fopen(filepath, "r");
if (f) {
    fclose(f);
    return true;
}
return false;
```

**Problems:**
- Side effects (modifies file access time)
- Permission issues (file exists but not readable)
- Race condition (file could be deleted between check and open)

**`access()` is better:**
- No side effects
- Explicitly checks existence (not permissions)
- Standard approach for "does file exist?" checks

**Edge Cases:**

```c
file_exists(NULL)           → false  (NULL check)
file_exists("")             → false  (empty path)
file_exists(".")            → true   (current directory)
file_exists("/nonexistent") → false
file_exists("/etc/passwd")  → true   (on Unix)
```

---

#### 6.3.3 has_mdl_extensions() - Helper Function

**Signature:**
```c
static bool has_mdl_extensions( const char *filepath );
```

**Purpose:**
Validate that filepath ends with ".mdl" (case-insensitive).

**Visibility:** `static` - private to util_args.c

**Parameters:**

| Parameter | Type | Description |
|-----------|------|-------------|
| `filepath` | `const char *` | Path to check |

**Return Value:**

| Value | Meaning |
|-------|---------|
| `true` | Ends with .mdl/.MDL/.Mdl/etc |
| `false` | Wrong extension, too short, or NULL |

**Implementation Analysis:**

```c
static bool has_mdl_extensions( const char *filepath ) {
    if ( filepath == NULL ) {
        return false;
    }

    size_t file_length = strlen( filepath );
    if ( file_length < 4 ) {
        return false;  // Must be at least ".mdl"
    }

    const char *dot_ptr = filepath + ( file_length - 4 );
    const char *expected_ptr = ".mdl";

    for ( int i = 0; i < 4; i++ ) {
        if ( i == 0 ) {
            if ( dot_ptr[i] != expected_ptr[i] ) {
                return false;  // Dot must match exactly
            }
        } else {
            if ( tolower( (unsigned char)dot_ptr[i] ) != expected_ptr[i] ) {
                return false;  // Letters case-insensitive
            }
        }
    }

    return true;
}
```

**Step-by-Step:**

**1. NULL Check:**
```c
if ( filepath == NULL ) return false;
```

**2. Length Check:**
```c
size_t file_length = strlen( filepath );
if ( file_length < 4 ) return false;
```
Minimum valid path: "a.mdl" (5 chars), but we check for 4 to allow ".mdl" itself.

**3. Get Last 4 Characters:**
```c
const char *dot_ptr = filepath + ( file_length - 4 );
```

**Pointer Arithmetic:**
```
filepath = "models/scientist.mdl"
           01234567890123456789 (indices)

file_length = 20
dot_ptr = filepath + (20 - 4) = filepath + 16
        = points to ".mdl"
```

**4. Compare With Case-Insensitivity:**
```c
for ( int i = 0; i < 4; i++ ) {
    if ( i == 0 ) {
        if ( dot_ptr[i] != expected_ptr[i] ) return false;
    } else {
        if ( tolower( (unsigned char)dot_ptr[i] ) != expected_ptr[i] ) return false;
    }
}
```

**Why Special Case for i==0?**
- Dot (`.`) must match exactly (no case variants)
- Letters (`m`, `d`, `l`) are case-insensitive

**Why `(unsigned char)` Cast?**
- `tolower()` requires `unsigned char` or EOF
- Without cast, negative char values cause undefined behavior
- Safety measure for non-ASCII characters

**Accepted:**
```
.mdl
.MDL
.Mdl
.mDl
.mdL
(any case combination)
```

**Rejected:**
```
.txt
.obj
mdl     (no dot)
.md     (too short)
.mdlx   (too long - but this would actually pass! Bug?)
```

**Bug: Doesn't Check for Characters After Extension**

```c
has_mdl_extensions("model.mdl.txt")  → true (BUG!)
```

This would pass because it only checks last 4 chars.

**Fix (Future TODO):**
```c
// Ensure .mdl is at end, not just in last 4 chars
const char *ext = strrchr(filepath, '.');
if (ext == NULL) return false;
return (strcasecmp(ext, ".mdl") == 0);
```

---

**Examples:**

```c
has_mdl_extensions("scientist.mdl")       → true
has_mdl_extensions("BARNEY.MDL")          → true
has_mdl_extensions("models/zombie.Mdl")   → true
has_mdl_extensions("model.obj")           → false
has_mdl_extensions("mdl")                 → false
has_mdl_extensions(".mdl")                → true (edge case)
has_mdl_extensions(NULL)                  → false
has_mdl_extensions("")                    → false
```

---

#### 6.3.4 print_usage() - Help Text Display

**Signature:**
```c
void print_usage( const char *program_name );
```

**Purpose:**
Print comprehensive help text to stdout.

**Parameters:**

| Parameter | Type | Description |
|-----------|------|-------------|
| `program_name` | `const char *` | Program name from argv[0] |

**Return Value:** `void` (no return value)

**Output Structure:**

1. **USAGE** - Command syntax
2. **OPTIONS** - All flags with descriptions
3. **LOGGING LEVELS** - Explanation of log verbosity
4. **EXAMPLES** - Real-world usage examples

**Full Output (Lines 169-225):**

```
USAGE:
  Lambda <model.mdl> [OPTIONS]

OPTIONS:
  --dump, -d
      Print basic model structure (header, bones, sequences)

  --dump-ex, -dx
      Print extended structure (vertices, meshes, raw offsets)

  --dump-only
      Dump structure and exit (no viewer window)

  --quiet, -q
      Quiet mode - only show errors

  --verbose, -vv
      Verbose mode - show debug messages

  --trace
      Trace mode - show all messages including trace

  --log-file <path>
      Write logs to specified file

  --version, -v
      Show detailed version information

  --help, -h
      Show this help message

LOGGING LEVELS:
  quiet   - Only errors
  normal  - Info, warnings, errors (default)
  verbose - Debug, info, warnings, errors
  trace   - All messages including trace

EXAMPLES:
  # Run viewer with model in models directory
  Lambda models/scientist.mdl

  # Run viewer with relative path
  Lambda ../models/HL1_Original/scientist.mdl

  # Print basic structure with verbose logging
  Lambda ../models/HL1_Original/scientist.mdl --dump --verbose

  # Print detailed structure and exit
  Lambda scientist.mdl --dump-ex --dump-only

  # Dump to file with trace logging
  Lambda scientist.mdl --dump-only --trace --log-file debug.log > report.txt

  # Show version information
  Lambda --version
```

**Why `program_name` Parameter?**

Allows flexible program name in examples:
```c
print_usage(argv[0]);
```

If invoked as `./Lambda`, examples show `Lambda`.
If invoked as `/usr/bin/Lambda`, examples show `Lambda`.

**Extract basename in examples:**
```c
printf( "  %s <model.mdl> [OPTIONS]\n\n", program_name );
```

**Design Principles:**

1. **Progressive Detail:** Usage → Options → Levels → Examples
2. **Self-Contained:** No need to consult external documentation
3. **Copy-Pasteable:** Examples can be run directly
4. **Standard Format:** Follows Unix man page conventions

---

#### 6.3.5 print_banner() - Copyright Banner

**Signature:**
```c
void print_banner( void );
```

**Purpose:**
Print copyright and license information.

**Parameters:** None

**Return Value:** `void`

**Output (Lines 140-164):**

```
═══════════════════════════════════════════════════════════════
  HALF-LIFE MODEL VIEWER - Lambda
  GoldSrc Studio Model Format Viewer
═══════════════════════════════════════════════════════════════

  Copyright (c) 1996-2002, Valve LLC. All rights reserved.

  This product contains software technology licensed from Id
  Software, Inc. ("Id Technology"). Id Technology (c) 1996
  Id Software, Inc. All Rights Reserved.

  Use, distribution, and modification of this source code and/or
  resulting object code is restricted to non-commercial
  enhancements to products from Valve LLC. All other use,
  distribution, or modification is prohibited without written
  permission from Valve LLC.

═══════════════════════════════════════════════════════════════
  Author: Karlo Siric
  Version: v0.3.0-alpha
═══════════════════════════════════════════════════════════════
```

**Usage:**
- Currently **not called** by any code
- Intended for potential GUI about dialog
- Or startup splash screen

**Styling Constants:**
```c
#define RULER_DOUBLE "═══════════════════════════════════════════════════════════════"
#define RULER_SINGLE "───────────────────────────────────────────────────────────────"
```

Provides visual separation.

---

#### 6.3.6 print_version_info() - Version Details

**Signature:**
```c
void print_version_info( void );
```

**Purpose:**
Print comprehensive build and system information.

**Parameters:** None

**Return Value:** `void`

**Output (Lines 101-135):**

```
═══════════════════════════════════════════════════════════════
  Half-Life Model Viewer - Lambda v0.3.0-alpha
═══════════════════════════════════════════════════════════════

  Build Information:
    Version:        v0.3.0-alpha
    Build Number:   42
    Build Date:     Jan 16 2025
    Build Time:     14:32:11
    Build Type:     Debug

  Git Information:
    Commit:         a3f89e2
    Branch:         main

  Platform:
    OS:             macOS
    Architecture:   arm64
    Compiler:       Apple Clang 15.0.0

  Features:
    Rendering:      Yes
    Animation:      Yes
    Textures:       Yes
    Bones:          Yes
    Dump:           Yes
    GUI:            No
    Export:         No
    Editing:        No

═══════════════════════════════════════════════════════════════
```

**Data Source:**

All values come from `version.h` (auto-generated by CMake):
```c
#define HLMV_VERSION_LONG   "Half-Life Model Viewer - Lambda v0.3.0-alpha"
#define HLMV_VERSION_STRING "v0.3.0-alpha"
#define HLMV_VERSION_BUILD  42
#define HLMV_BUILD_DATE     "Jan 16 2025"
#define HLMV_BUILD_TIME     "14:32:11"
#define HLMV_BUILD_TYPE     "Debug"
#define HLMV_GIT_COMMIT     "a3f89e2"
#define HLMV_GIT_BRANCH     "main"
#define HLMV_PLATFORM       "macOS"
#define HLMV_ARCH           "arm64"
#define HLMV_COMPILER       "Apple Clang 15.0.0"
#define HLMV_HAS_RENDERING  1
#define HLMV_HAS_ANIMATION  1
// ... etc ...
```

**Features Section:**

Shows which features are compiled in:
```c
printf( "    Rendering:      %s\n", ( HLMV_HAS_RENDERING ) ? "Yes" : "No" );
```

Useful for:
- Debugging build configurations
- Ensuring correct features enabled
- Reporting bugs (include version output in bug report)

**Why So Detailed?**

When user reports bug:
> "Lambda crashes on my model!"

Developer response:
> "Please run `./Lambda --version` and paste output."

Output reveals:
- Build type (Debug vs Release)
- Compiler version (potential compiler bugs)
- Git commit (exact source code version)
- Architecture (x86_64 vs arm64 differences)
- Enabled features (maybe GUI disabled but user expects it)

---

### 6.4 Argument Validation Rules

**Summary of All Validation:**

| Rule | Checked By | Error Message |
|------|------------|---------------|
| Model path must exist | `file_exists()` | "ERROR: File not found: '...'" |
| Model path must end with .mdl | `has_mdl_extensions()` | "ERROR: Invalid file type '...'" |
| Only one model path allowed | `parse_args()` loop | "ERROR: Multiple model files specified" |
| --log-file requires argument | `parse_args()` | "ERROR: --log-file requires a path argument" |
| Must have model path unless --help/--version | Final validation | "ERROR: No model file specified" |

**Valid Combinations:**

```bash
./Lambda model.mdl                          ✓
./Lambda model.mdl --dump --verbose         ✓
./Lambda --help                             ✓
./Lambda --version                          ✓
./Lambda model.mdl --dump-only              ✓
./Lambda --log-file test.log model.mdl      ✓
```

**Invalid Combinations:**

```bash
./Lambda                                    ✗ (no args, but shows help, so technically OK)
./Lambda --dump                             ✗ (no model path)
./Lambda model.obj                          ✗ (wrong extension)
./Lambda nonexistent.mdl                    ✗ (file doesn't exist)
./Lambda model1.mdl model2.mdl              ✗ (multiple models)
./Lambda model.mdl --log-file               ✗ (missing log file path)
./Lambda model.mdl --invalid-flag           ✗ (unknown flag)
```

---

**Section 6 Complete!**

Next: **Section 7 - Application Initialization** (cl/cl_app.c - app_init function)


---

## 7. Application Initialization (cl/cl_app.c & cl/cl_app_init.c)

### 7.1 Module Overview

#### 7.1.1 Purpose

The application initialization module is the **heart of the startup sequence**. It orchestrates all subsystem initialization in the correct order, handles different execution modes (viewer vs dump-only), and sets up the entire application state.

**Core Responsibilities:**
1. Initialize logging system with configured verbosity
2. Handle special modes (--help, --version, --dump-only)
3. Initialize OpenGL renderer (GLFW + GLEW)
4. Load MDL model file
5. Set up application state structure
6. Register input callbacks
7. Handle initialization failures gracefully

**Design Philosophy:**
- **Fail-fast validation:** Check parameters before expensive operations
- **Progressive initialization:** Each step depends on previous steps
- **Graceful degradation:** Clean up on failure
- **Separation of concerns:** Helper functions for each subsystem

