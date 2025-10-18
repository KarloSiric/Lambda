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

project(HalfLifeModelViewer VERSION 0.1.1 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

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

message(STATUS "═══════════════════════════════════════════════════════════")

# ═══════════════════════════════════════════════════════════════════════════
#   Source Files
# ═══════════════════════════════════════════════════════════════════════════

set(SOURCES
    # Core
    src/main.c
    src/version.h
    src/studio.h
    
    # MDL subsystem
    src/mdl/mdl_loader.c
    src/mdl/mdl_info.c
    src/mdl/mdl_report.c
    src/mdl/bone_system.c
    src/mdl/bodypart_manager.c
    src/mdl/mdl_animations.c
    
    # Graphics subsystem
    src/graphics/renderer.c
    src/graphics/camera.c
    src/graphics/textures.c
    
    # Utilities
    src/utils/utils.c
    src/utils/mdl_messages.c
    src/utils/logger.c
    src/utils/args.c
)

# ═══════════════════════════════════════════════════════════════════════════
#   Executable
# ═══════════════════════════════════════════════════════════════════════════

add_executable(${PROJECT_NAME} ${SOURCES})

target_compile_definitions(${PROJECT_NAME} PRIVATE
  SHADER_DIR="${CMAKE_SOURCE_DIR}/shaders"   # absolute path to repo’s shaders
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
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
