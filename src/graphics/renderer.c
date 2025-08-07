/*
 * Renderer implementation
 * We'll build this step by step
 */

#include "renderer.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else 
#include <GL/gl.h>
#endif

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

static GLFWwindow *window = NULL;
static bool wireframe_enabled = false;

int init_renderer(int width, int height, const char *title) {  


    #ifdef __APPLE__
        /* MacOS M chips specific */
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #elif _WIN32
        /* Windows: Can go up to the latest version supported by drivers */
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #else
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #endif

    GLFWmonitor *primary = glfwGetPrimaryMonitor();

    glfwInit();

    window = glfwCreateWindow(width, height, title, primary, NULL);

    if (!window) {
        glfwTerminate();
        return (-1);
    }

    glfwMakeContextCurrent(window);

    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, width, height);

    glEnable(GL_CULL_FACE);

    printf("OpenGL Version: %s\n",
            glGetString(GL_VERSION));

    return (0);
}

void cleanup_renderer(void) {

    if (window) {
        glfwDestroyWindow(window);
        printf("Shutting down Window ...\n");
    }

    glfwTerminate();

    printf("GLFW Fully Terminated!\n");

    return;
}

void clear_screen(void) {

    glClearColor(0.1f, 0.2f, 0.45f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

bool should_close_window(void) {

    return (glfwWindowShouldClose(window) ||
            (glfwGetKey(window, GLFW_KEY_ESCAPE)) == GLFW_PRESS);

}





