#ifndef SHADER_H
#define SHADER_H 

#include <OpenGL/gl3.h>

char *read_shader_source(const char *filepath);
GLuint compile_shader(const char *source, GLenum type);
GLuint create_shader_program(GLuint vertexShader, GLuint fragmentShader);


#endif
