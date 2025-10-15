#ifndef PLATFORM_H
#define PLATFORM_H

// Platform detection
#if defined(__APPLE__) || defined(__MACH__)
    #define PLATFORM_MACOS 1
#elif defined(__linux__)
    #define PLATFORM_LINUX 1
#elif defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS 1
#endif

// Silence deprecation warnings on macOS (if not already defined)
#ifdef PLATFORM_MACOS
    #ifndef GL_SILENCE_DEPRECATION
        #define GL_SILENCE_DEPRECATION
    #endif
#endif

// NO OpenGL includes here!
// Use gl_platform.h for OpenGL headers instead

#endif // PLATFORM_H
