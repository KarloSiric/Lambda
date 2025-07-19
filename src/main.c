/*
* @Author: karlosiric
* @Date:   2025-07-15 13:56:37
* @Last Modified by:   karlosiric
* @Last Modified time: 2025-07-19 23:51:16
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

    /*
    TESTING PURPOSES ONLY
    // print_mdl_info("models/HL1_Original/scientist.mdl");    
    // printf("\n\n");
    // testing_mdl_file("models/HL1_Original/scientist.mdl");
    */

    printf("==== LOADING SCIENTIST ====\n");

    mdl_complete_model_s scientist = load_mdl_file("models/HL1_Original/scientist.mdl");

    if (scientist.total_model_count == 0) {
        printf("Failed to load MDL file: '%s' \n", scientist.filename);
        return (-1);
    }

    single_model_s *body = &scientist.models[0];    // Body
    single_model_s *head = &scientist.models[2];    // Einstein head  
    single_model_s *hand = &scientist.models[6];    // Needle hand
    
    printf("Test combination: %s + %s + %s\n", 
           body->model_name, head->model_name, hand->model_name);
    
    // Combine vertices
    int total_vertices = body->vertex_count + head->vertex_count + hand->vertex_count;
    float *combined_vertices = malloc(total_vertices * 3 * sizeof(float));
    
    // Copy data
    int offset = 0;
    memcpy(combined_vertices + offset, body->vertices, body->vertex_count * 3 * sizeof(float));
    offset += body->vertex_count * 3;
    memcpy(combined_vertices + offset, head->vertices, head->vertex_count * 3 * sizeof(float));
    offset += head->vertex_count * 3;
    memcpy(combined_vertices + offset, hand->vertices, hand->vertex_count * 3 * sizeof(float));
    
    printf("Ready to render: %d vertices\n", total_vertices);



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


    GLuint vao;
    GLuint vbo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, total_vertices * 3 * sizeof(float), combined_vertices, GL_STATIC_DRAW);

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
        glPointSize(5.0f);

        // TODO: Adding some rotation
        static float rotation = 0.0f;
        rotation += 0.005f;
        glm_mat4_identity(model);
        glm_rotate(model, rotation, (vec3){0.0f, 1.0f, 0.0f});
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float *)model);
        glDrawArrays(GL_POINTS, 0,  total_vertices);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(shaderProgram);
    free(combined_vertices);
    free_mdl_file(&scientist);
    glfwTerminate();

    return (0);
}
