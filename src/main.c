/*
* @Author: karlosiric
* @Date:   2025-07-15 13:56:37
* @Last Modified by:   karlosiric
* @Last Modified time: 2025-07-15 15:16:44
*/

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

int main() {

    GLFWwindow *window;

    float vertices[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.0f, 0.5f
    };
    GLuint VBO;                                 // same as unsinged int just a cross platform version

    // Step 1 is to initialize the GLFW library(window creation, audio, input, output, mouse whatever)
    if (!glfwInit()) {
        printf("Failed to initialize the GLFW\n");
        return (-1);
    }

    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "3D Model Viewer", NULL, NULL);
    if (!window) {
        printf("Failed to create window!\n");
        glfwTerminate();
        return (-1);
    }

    glfwMakeContextCurrent(window);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    if(glewInit() != GLEW_OK) {
        printf("Failed to initialize the GLEW library!\n");
        return (-1);
    }

    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

    while(!glfwWindowShouldClose(window)) {

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        glClear(GL_DEPTH_BUFFER_BIT);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();

    return (0);
}
