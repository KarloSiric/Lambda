/*
* @Author: karlosiric
* @Date:   2025-07-16 12:30:02
* @Last Modified by:   karlosiric
* @Last Modified time: 2025-07-16 15:13:18
*/


#include "shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_shader_source(const char *filepath) {

    char *buffer;
    FILE *file = fopen(filepath, "r");
    if (!file) {
        fprintf(stderr, "Failed to open the file: '%s'\n", filepath);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    size_t length = ftell(file);

    rewind(file);

    buffer = (char *)malloc(length * sizeof(char) + 1);
    if (!buffer) {
        fprintf(stderr, "Failed to allocate memory space for shader source.\n");
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, length, file);
    buffer[length] = '\0';

    fclose(file);

    return buffer;
}

GLuint compile_shader(const char *source, GLenum type) {

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        fprintf(stderr, "Shader compilation failed (%s):\n%s\n",
                type == GL_SHADER_TYPE ? "VERTEX" : "FRAGMENT", infoLog);
        return (0);
    }

    return shader;
}

GLuint create_shader_program(GLuint vertexShader, GLuint fragmentShader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
        char programLog[512];
        glGetProgramInfoLog(program, 512, NULL, programLog);
        fprintf(stderr, "Program linking the shaders failed: \n'%s'\n", programLog);
        return (0);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

