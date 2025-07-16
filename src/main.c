/*
* @Author: karlosiric
* @Date:   2025-07-15 13:56:37
* @Last Modified by:   karlosiric
* @Last Modified time: 2025-07-16 12:28:37
*/
#define OPENGL_SILENCE_DEPRECATION
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>
#include <cglm/cglm.h>



#define WIDTH                               800
#define HEIGHT                              600




int main() {
    
    GLFWwindow *window;

    GLfloat vertices[] = {
        -0.5f, -0.5f,                        // Bottom left
         0.5f, -0.5f,                        // right bottom
         0.0f,  0.5f                         // top center
    };

    GLuint vao;
    GLuint vbo;

    if (!glfwInit()) {
        fprintf(stderr, "Error initializing the GLFW library!\n"); 
        return (-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Model Viewer 3D", NULL, NULL);

    if (!window) {
        fprintf(stderr, "Couldn't create a window, terminating program!\n");
        glfwTerminate();
        return(-1);
    }

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glfwMakeContextCurrent(window);

    const GLubyte *version = glGetString(GL_VERSION);

    printf("OpenGL version in use: %s\n", version);

    while(!glfwWindowShouldClose(window)) {

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();

    return (0);
}
