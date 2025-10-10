cmake_minimum_required(VERSION 3.15)
project(HalfLifeModelViewer VERSION 1.0 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_C_FLAGS "-Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")

# Find required packages
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# Include directories
include_directories(src)

# Source files
set(SOURCES
    src/main.c
    src/mdl/mdl_loader.c
    src/mdl/mdl_info.c
    src/mdl/bone_system.c
    src/mdl/bodypart_manager.c
    src/mdl/mdl_animations.c
    src/graphics/renderer.c
    src/graphics/camera.c
    src/graphics/textures.c
    src/utils/utils.c
    src/utils/mdl_messages.c
    src/utils/logger.c
    
)

# Create executable
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

# Install target
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Custom targets
add_custom_target(run
    COMMAND ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    src/mdl/bone_system.c
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
