#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else
#include <GL/gl3.h>
#endif

#endif
