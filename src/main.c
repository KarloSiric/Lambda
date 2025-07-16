/*
* @Author: karlosiric
* @Date:   2025-07-15 13:56:37
* @Last Modified by:   karlosiric
* @Last Modified time: 2025-07-16 14:24:47
*/
#define OPENGL_SILENCE_DEPRECATION
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>
#include <cglm/cglm.h>
#include "shader.h"



#define WIDTH                               800
#define HEIGHT                              600

char *read_shader_source(const char *filepath);
GLuint compile_shader(const char *source, GLenum type);
GLuint create_shader_program(GLuint vertexShader, GLuint fragmentShader);


int main() {
    
    if (!glfwInit()) {
        fprintf(stderr, "Error initializing the GLFW library!\n"); 
        return (-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Model Viewer 3D", NULL, NULL);
    
    if (!window) {
        fprintf(stderr, "Couldn't create a window, terminating program!\n");
        glfwTerminate();
        return(-1);
    }

    glfwMakeContextCurrent(window);

    const GLubyte *version = glGetString(GL_VERSION);

    printf("OpenGL version in use: %s\n", version);



    char *vertexBuffer = read_shader_source("shaders/basic.vert");
    char *fragmentBuffer = read_shader_source("shaders/basic.frag");

    GLuint vertexShader = compile_shader(vertexBuffer, GL_VERTEX_SHADER);
    GLuint fragmentShader = compile_shader(fragmentBuffer, GL_FRAGMENT_SHADER);

    GLuint shaderProgram = create_shader_program(vertexShader, fragmentShader);

    free(vertexBuffer);
    free(fragmentBuffer);

    GLfloat vertices[] = {
        -0.5f, -0.5f,                        // Bottom left
         0.5f, -0.5f,                        // right bottom
         0.0f,  0.5f                         // top center
    };

    GLuint vao;
    GLuint vbo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);



    while(!glfwWindowShouldClose(window)) {

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(shaderProgram);
    glfwTerminate();

    return (0);
}
