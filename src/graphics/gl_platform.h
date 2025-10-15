#ifndef GL_PLATFORM_H
#define GL_PLATFORM_H


/*
 * Cross-platform OpenGL header management
 * 
 * This header MUST be included instead of direct OpenGL headers.
 * It handles platform-specific differences between macOS, Linux, and Windows.
 * 
 * CRITICAL: On Linux/Windows, GLEW must be included BEFORE GLFW!
 */


// ═══════════════════════════════════════════════════════════════════════════
//   Platform Detection
// ═══════════════════════════════════════════════════════════════════════════

#if defined( __APPLE__ ) || defined( __MACH__ )
#define PLATFORM_MACOS 1
#define GLEW_REQUIRED  0
#elif defined( __linux__ )
#define PLATFORM_LINUX 1
#define GLEW_REQUIRED  1
#elif defined( _WIN32 ) || defined( _WIN64 )
#define PLATFORM_WINDOWS 1
#define GLEW_REQUIRED    1
#else
#error "Unsupported platform for OpenGL"
#endif

// ═══════════════════════════════════════════════════════════════════════════
//   OpenGL Headers (Platform-Specific Include Order)
// ═══════════════════════════════════════════════════════════════════════════

#if PLATFORM_MACOS
// macOS: Use native OpenGL framework headers (NO GLEW!)
#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif

#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>

#elif PLATFORM_LINUX || PLATFORM_WINDOWS
// Linux/Windows: GLEW MUST be included BEFORE GLFW!
#include <GL/glew.h>
#include <GLFW/glfw3.h>
    
#endif

// ═══════════════════════════════════════════════════════════════════════════
//   Helper Macros
// ═══════════════════════════════════════════════════════════════════════════

#ifdef DEBUG
    #define GL_CHECK_ERROR() \
        do { \
            GLenum err; \
            while ((err = glGetError()) != GL_NO_ERROR) { \
                fprintf(stderr, "OpenGL error at %s:%d - 0x%x\n", \
                        __FILE__, __LINE__, err); \
            } \
        } while(0)
#else
    #define GL_CHECK_ERROR() ((void)0)
#endif

#endif // GL_PLATFORM_H
