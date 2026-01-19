cmake_minimum_required(VERSION 3.15)

# ═══════════════════════════════════════════════════════════════════════════
#   Git Information (before project())
# ═══════════════════════════════════════════════════════════════════════════

find_package(Git QUIET)
if(GIT_FOUND)
    # Get short commit hash
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    
    # Get branch name
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    
    # Get latest tag
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=0
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_TAG
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
else()
    set(GIT_COMMIT_HASH "unknown")
    set(GIT_BRANCH "unknown")
    set(GIT_TAG "")
endif()

# ═══════════════════════════════════════════════════════════════════════════
#   Project Definition
# ═══════════════════════════════════════════════════════════════════════════

project(Lambda VERSION 0.7.0 LANGUAGES C CXX)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set C++ standard (for Qt GUI)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ═══════════════════════════════════════════════════════════════════════════
#   Build Configuration
# ═══════════════════════════════════════════════════════════════════════════

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Git commit: ${GIT_COMMIT_HASH}")
message(STATUS "Git branch: ${GIT_BRANCH}")
if(GIT_TAG)
    message(STATUS "Git tag: ${GIT_TAG}")
endif()

# Compiler flags
set(CMAKE_C_FLAGS "-Wall -Wextra -Wpedantic")
set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG -O0")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

# Pass git information to code
add_compile_definitions(
    HLMV_GIT_COMMIT="${GIT_COMMIT_HASH}"
    HLMV_GIT_BRANCH="${GIT_BRANCH}"
    HLMV_GIT_TAG="${GIT_TAG}"
    SHADER_DIR="${CMAKE_SOURCE_DIR}/shaders"
)

# ═══════════════════════════════════════════════════════════════════════════
#   Platform Detection
# ═══════════════════════════════════════════════════════════════════════════

message(STATUS "═══════════════════════════════════════════════════════════")
message(STATUS "Detecting platform...")

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(PLATFORM_MACOS TRUE)
    message(STATUS "Platform: macOS")
    
    # Detect Homebrew prefix
    execute_process(
        COMMAND brew --prefix
        OUTPUT_VARIABLE HOMEBREW_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
        RESULT_VARIABLE BREW_RESULT
    )
    
    if(BREW_RESULT EQUAL 0 AND HOMEBREW_PREFIX)
        message(STATUS "Homebrew found at: ${HOMEBREW_PREFIX}")
        
        # Add Homebrew paths to CMake search paths
        list(APPEND CMAKE_PREFIX_PATH 
            "${HOMEBREW_PREFIX}"
            "${HOMEBREW_PREFIX}/opt/glfw"
            "${HOMEBREW_PREFIX}/opt/glew"
        )
        
        # Also add pkg-config path for fallback
        set(ENV{PKG_CONFIG_PATH} "${HOMEBREW_PREFIX}/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
        
        message(STATUS "Added Homebrew paths to CMAKE_PREFIX_PATH")
    else()
        message(WARNING "Homebrew not found - manual paths may be needed")
    endif()
    
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(PLATFORM_LINUX TRUE)
    message(STATUS "Platform: Linux")
    
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(PLATFORM_WINDOWS TRUE)
    message(STATUS "Platform: Windows")
    
else()
    message(WARNING "Unknown platform: ${CMAKE_SYSTEM_NAME}")
endif()

# ═══════════════════════════════════════════════════════════════════════════
#   Dependencies (Cross-Platform)
# ═══════════════════════════════════════════════════════════════════════════

message(STATUS "═══════════════════════════════════════════════════════════")
message(STATUS "Finding dependencies...")

# ─────────────────────────────────────
# OpenGL
# ─────────────────────────────────────
find_package(OpenGL REQUIRED)
if(OpenGL_FOUND)
    message(STATUS "✓ OpenGL found")
else()
    message(FATAL_ERROR "OpenGL is required but not found")
endif()

# ─────────────────────────────────────
# GLFW3
# ─────────────────────────────────────
message(STATUS "Searching for GLFW3...")

# Try CMake config first (preferred)
find_package(glfw3 3.3 CONFIG QUIET)

if(glfw3_FOUND)
    message(STATUS "✓ GLFW3 found via CMake config")
    set(GLFW_TARGET glfw)
else()
    message(STATUS "  GLFW3 CMake config not found, trying pkg-config...")
    
    # Fallback to pkg-config
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_search_module(GLFW glfw3)
        if(GLFW_FOUND)
            message(STATUS "✓ GLFW3 found via pkg-config (version ${GLFW_VERSION})")
            set(GLFW_TARGET ${GLFW_LIBRARIES})
        else()
            message(FATAL_ERROR 
                "GLFW3 not found!\n"
                "Install instructions:\n"
                "  • macOS:   brew install glfw\n"
                "  • Arch:    sudo pacman -S glfw-x11\n"
                "  • Debian:  sudo apt install libglfw3-dev\n"
                "  • Windows: vcpkg install glfw3"
            )
        endif()
    else()
        message(FATAL_ERROR "GLFW3 not found and pkg-config is not available")
    endif()
endif()

# ─────────────────────────────────────
# GLEW
# ─────────────────────────────────────
message(STATUS "Searching for GLEW...")

# Try CMake config first
find_package(GLEW CONFIG QUIET)

if(GLEW_FOUND)
    message(STATUS "✓ GLEW found via CMake config")
    set(GLEW_TARGET GLEW::GLEW)
else()
    message(STATUS "  GLEW CMake config not found, trying pkg-config...")
    
    # Fallback to pkg-config (if available from GLFW search)
    if(PkgConfig_FOUND)
        pkg_search_module(GLEW glew)
        if(GLEW_FOUND)
            message(STATUS "✓ GLEW found via pkg-config (version ${GLEW_VERSION})")
            set(GLEW_TARGET ${GLEW_LIBRARIES})
        else()
            message(WARNING "GLEW not found - may cause issues on some platforms")
            if(PLATFORM_LINUX)
                message(STATUS "  Install: sudo pacman -S glew (Arch) or sudo apt install libglew-dev (Debian)")
            elseif(PLATFORM_MACOS)
                message(STATUS "  Install: brew install glew")
            endif()
        endif()
    else()
        message(WARNING "GLEW not found and pkg-config unavailable")
    endif()
endif()

# ─────────────────────────────────────
# Qt6 (for GUI Editor)
# ─────────────────────────────────────
message(STATUS "Searching for Qt6...")

if(PLATFORM_MACOS)
    set(CMAKE_PREFIX_PATH "/opt/homebrew/Cellar/qt/6.9.2")
endif()

find_package(Qt6 COMPONENTS Core Widgets Gui OpenGL OpenGLWidgets QUIET)

if(Qt6_FOUND)
    message(STATUS "✓ Qt6 found (version ${Qt6_VERSION})")
    # Note: AUTOMOC/AUTORCC/AUTOUIC will be set per-target for LambdaEditor only
else()
    message(STATUS "  Qt6 not found - GUI editor will not be built")
    message(STATUS "  Install: brew install qt (macOS)")
endif()

message(STATUS "═══════════════════════════════════════════════════════════")

# ═══════════════════════════════════════════════════════════════════════════
#   Source Files
# ═══════════════════════════════════════════════════════════════════════════

set(SOURCES
    # # Core
    # src/main.c
    # src/version.h
    # src/studio.h
    # src/core/app.c
    # src/core/app_config.c
    # src/core/app_init.c
    
    # # MDL subsystem
    # src/mdl/mdl_loader.c
    # src/mdl/mdl_info.c
    # src/mdl/mdl_report.c
    # src/mdl/bone_system.c
    # src/mdl/bodypart_manager.c
    # src/mdl/mdl_animations.c
    
    # # Graphics subsystem
    # src/graphics/renderer.c
    # src/graphics/camera.c
    # src/graphics/textures.c
    
    # # Utilities
    # src/utils/utils.c
    # src/utils/mdl_messages.c
    # src/utils/logger.c
    # src/utils/args.c
    # Adding new system for adding all the .c file
)

file(GLOB SRC_CL CONFIGURE_DEPENDS "src/cl/*.c")
file(GLOB SRC_R CONFIGURE_DEPENDS "src/r/*.c")
file(GLOB SRC_MDL CONFIGURE_DEPENDS "src/mdl/*.c")
# file(GLOB SRC_SYS CONFIGURE_DEPENDS "src/sys/*.c")
file(GLOB SRC_MATH CONFIGURE_DEPENDS "src/math/*.c")
file(GLOB SRC_UTIL CONFIGURE_DEPENDS "src/util/*.c")
file(GLOB SRC_INPUT CONFIGURE_DEPENDS "src/input/*.c")
file(GLOB SRC_SHADER CONFIGURE_DEPENDS "src/shader/*.c")
file(GLOB SRC_TOP CONFIGURE_DEPENDS "src/*.c")
file(GLOB EXTERNAL CONFIGURE_DEPENDS "external/*.c")


# C++
# Editor source files
file(GLOB SRC_EDITOR CONFIGURE_DEPENDS 
    "${CMAKE_SOURCE_DIR}/src/editor/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/editor/panels/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/editor/widgets/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/editor/dialogs/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/editor/bridge/*.cpp"
)

file(GLOB SRC_EDITOR_THEME CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/editor/theme/*.cpp"
)



# ═══════════════════════════════════════════════════════════════════════════
#   Executable
# ═══════════════════════════════════════════════════════════════════════════

add_executable(${PROJECT_NAME}
  ${SRC_TOP}
  ${SRC_CL}
  ${SRC_R}
  ${SRC_MDL}
  # ${SRC_SYS}
  ${SRC_MATH}
  ${SRC_UTIL}
  ${SRC_INPUT}
  ${SRC_SHADER}
  ${EXTERNAL}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
  SHADER_DIR="${CMAKE_SOURCE_DIR}/shaders"   # absolute path to repo’s shaders
)


# -------- Include directories --------
target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/src        # for studio.h, version.h, etc.
  ${CMAKE_SOURCE_DIR}/src/core
  ${CMAKE_SOURCE_DIR}/src/cl
  ${CMAKE_SOURCE_DIR}/src/r
  ${CMAKE_SOURCE_DIR}/src/mdl
  ${CMAKE_SOURCE_DIR}/src/math
  # ${CMAKE_SOURCE_DIR}/src/sys
  ${CMAKE_SOURCE_DIR}/src/util
  ${CMAKE_SOURCE_DIR}/src/input
  ${CMAKE_SOURCE_DIR}/src/shaders
  ${CMAKE_SOURCE_DIR}/external
  
)

# ---------- Warnings ----------
target_compile_options(${PROJECT_NAME} PRIVATE
  -Wall -Wextra -Wpedantic
  -Wno-unused-function          # temporary while stubbing
  -Wno-c23-extensions           # suppress Apple's gl3.h #warning noise
)


# Link OpenGL
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

# Link GLFW
if(glfw3_FOUND)
    # Using CMake config - modern imported target
    target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
elseif(GLFW_FOUND)
    # Using pkg-config - use variables
    target_link_libraries(${PROJECT_NAME} PRIVATE ${GLFW_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PRIVATE ${GLFW_INCLUDE_DIRS})
    target_compile_options(${PROJECT_NAME} PRIVATE ${GLFW_CFLAGS_OTHER})
endif()

# Link GLEW (if found)
if(GLEW_FOUND)
    if(TARGET GLEW::GLEW)
        # Using CMake config - modern imported target
        target_link_libraries(${PROJECT_NAME} PRIVATE GLEW::GLEW)
    else()
        # Using pkg-config - use variables
        target_link_libraries(${PROJECT_NAME} PRIVATE ${GLEW_LIBRARIES})
        target_include_directories(${PROJECT_NAME} PRIVATE ${GLEW_INCLUDE_DIRS})
        target_compile_options(${PROJECT_NAME} PRIVATE ${GLEW_CFLAGS_OTHER})
    endif()
endif()

# Link math library (Unix systems)
if(PLATFORM_LINUX OR PLATFORM_MACOS)
    target_link_libraries(${PROJECT_NAME} PRIVATE m)
endif()

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# ═══════════════════════════════════════════════════════════════════════════
#   Installation
# ═══════════════════════════════════════════════════════════════════════════

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Install documentation
install(FILES
    README.md
    LICENSE
    CHANGELOG.md
    DESTINATION share/doc/${PROJECT_NAME}
)

install(DIRECTORY docs/
    DESTINATION share/doc/${PROJECT_NAME}/docs
)

# ═══════════════════════════════════════════════════════════════════════════
#   Custom Targets
# ═══════════════════════════════════════════════════════════════════════════

# Run target
add_custom_target(run
    COMMAND ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running ${PROJECT_NAME}..."
)

# Clean target (extended)
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMENT "Removing entire build directory"
)

# Version info target
add_custom_target(version
    COMMAND ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME} --version
    DEPENDS ${PROJECT_NAME}
    COMMENT "Showing version information"
)

# ═══════════════════════════════════════════════════════════════════════════
#   Qt GUI Editor Application (C++)
# ═══════════════════════════════════════════════════════════════════════════
if(Qt6_FOUND)
    message(STATUS "Configuring LambdaEditor GUI...")

    set(EDITOR_SOURCES
        src/editor/main.cpp
        src/editor/MainWindow.cpp
        src/editor/widgets/ModelViewport.cpp
        src/editor/widgets/StatusBarWidget.cpp
        src/editor/widgets/ConsoleWidget.cpp
        src/editor/bridge/ConsoleBridge.cpp
        
    )

    set(EDITOR_HEADERS
        src/editor/MainWindow.h
        src/editor/widgets/ModelViewport.h
        src/editor/widgets/StatusBarWidget.h
        src/editor/widgets/ConsoleWidget.h
        src/editor/bridge/ConsoleBridge.h
    )

    #
    # 1) Generate Qt resources BEFORE the executable
    #
    qt_add_resources(LAMBDA_RESOURCES
        ${CMAKE_SOURCE_DIR}/resources/resources.qrc
    )

    #
    # 2) Editor target
    #
    add_executable(LambdaEditor
        ${EDITOR_SOURCES}
        ${SRC_EDITOR_THEME}
        ${LAMBDA_RESOURCES}
        # C backend sources (same as Lambda executable)
        ${SRC_CL}
        ${SRC_R}
        ${SRC_MDL}
        ${SRC_MATH}
        ${SRC_UTIL}
        ${SRC_INPUT}
        ${SRC_SHADER}
        ${EXTERNAL}    
    )

    #
    # Includes
    #
    target_include_directories(LambdaEditor PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/editor
        ${CMAKE_SOURCE_DIR}/src/editor/panels
        ${CMAKE_SOURCE_DIR}/src/editor/widgets
        ${CMAKE_SOURCE_DIR}/src/editor/dialogs
        ${CMAKE_SOURCE_DIR}/src/editor/bridge
        # C backend includes (same as Lambda)
        ${CMAKE_SOURCE_DIR}/src/cl
        ${CMAKE_SOURCE_DIR}/src/r
        ${CMAKE_SOURCE_DIR}/src/mdl
        ${CMAKE_SOURCE_DIR}/src/math
        ${CMAKE_SOURCE_DIR}/src/util
        ${CMAKE_SOURCE_DIR}/src/input
        ${CMAKE_SOURCE_DIR}/src/shaders
        ${CMAKE_SOURCE_DIR}/external 
    )

    #
    # Qt libraries
    #
    target_link_libraries(LambdaEditor PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Gui
        Qt6::OpenGL
        Qt6::OpenGLWidgets
        OpenGL::GL
        glfw
        m
    )

    #
    # Qt MOC / RCC / UIC
    #
    set_target_properties(LambdaEditor PROPERTIES
        AUTOMOC ON
        AUTORCC ON
        AUTOUIC ON
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
    
    message(STATUS "✓ LambdaEditor GUI target created")
endif()
