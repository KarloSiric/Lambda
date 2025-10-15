#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>
#else
#ifdef _WIN32
    #include <windows.h>
#endif
#ifdef __lINUX__
#include <GL/glew.h>
#include <GL/glfw3.h>
#endif

#endif
#endif


