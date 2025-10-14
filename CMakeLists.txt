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

project(HalfLifeModelViewer VERSION 0.1.0 LANGUAGES C)

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
)

# ═══════════════════════════════════════════════════════════════════════════
#   Dependencies
# ═══════════════════════════════════════════════════════════════════════════

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# ═══════════════════════════════════════════════════════════════════════════
#   Source Files
# ═══════════════════════════════════════════════════════════════════════════

include_directories(src)

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

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    glfw 
    OpenGL::GL
    m  # Math library
)

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
