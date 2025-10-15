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
#include <GL/gl3.h>
#include <GL/glew.h>
#endif


#endif
