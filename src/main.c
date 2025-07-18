/*
* @Author: karlosiric
* @Date:   2025-07-15 13:56:37
* @Last Modified by:   karlosiric
* @Last Modified time: 2025-07-18 12:58:54
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
#include "obj_loader.h"
#include "mdl_loader.h"




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

    
    printf("Printing MDL file information ...\n");
    print_mdl_info("models/HL1_Original/scientist.mdl");    
    printf("\n\n");


    const GLubyte *version = glGetString(GL_VERSION);

    printf("OpenGL version in use: %s\n", version);


    char *vertexBuffer = read_shader_source("shaders/basic.vert");
    char *fragmentBuffer = read_shader_source("shaders/basic.frag");

    GLuint vertexShader = compile_shader(vertexBuffer, GL_VERTEX_SHADER);
    GLuint fragmentShader = compile_shader(fragmentBuffer, GL_FRAGMENT_SHADER);

    GLuint shaderProgram = create_shader_program(vertexShader, fragmentShader);

    free(vertexBuffer);
    free(fragmentBuffer);




    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

    mat4 model, view, projection;

    glm_mat4_identity(model);
    glm_mat4_identity(view);
    glm_mat4_identity(projection);

    glm_rotate(model, glm_rad(30.0f), (vec3){0.0f, 1.0f, 0.0f});
    glm_translate(view, (vec3){0.0f, 0.0f, -3.0f});

    glm_perspective(glm_rad(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f, projection);

    Model model_obj = load_obj_file("models/objs/triangle.obj");
    printf("Vertex count: %zu\n", model_obj.vertex_count);
    for (size_t i = 0; i < model_obj.vertex_count; i++) {
        printf("v %f %f %f\n", model_obj.vertices[i], model_obj.vertices[i+1], model_obj.vertices[i+2]);
    }

    GLuint vao;
    GLuint vbo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, model_obj.vertex_count * sizeof(float), model_obj.vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);


    while(!glfwWindowShouldClose(window)) {

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float *)model); 
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float *)view); 
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (float *)projection); 

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, model_obj.vertex_count / 3);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(shaderProgram);
    free_model(&model_obj);
    glfwTerminate();

    return (0);
}
