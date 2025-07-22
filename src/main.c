/*
* Clean, minimal Half-Life model viewer - multiple bodyparts
*/
#define OPENGL_SILENCE_DEPRECATION
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>
#include <cglm/cglm.h>
#include "shader.h"
#include "mdl_loader.h"

#define WIDTH  800
#define HEIGHT 600

int main() {
    
    if (!glfwInit()) {
        fprintf(stderr, "Error initializing GLFW!\n"); 
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Half-Life Model Viewer", NULL, NULL);
    
    if (!window) {
        fprintf(stderr, "Failed to create window!\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glEnable(GL_DEPTH_TEST);

    // Load the MDL file
    printf("Loading Barney...\n");
    mdl_complete_model_s model = load_mdl_file("models/HL1_Original/barney.mdl");

    if (model.total_model_count == 0) {
        printf("Failed to load MDL file!\n");
        glfwTerminate();
        return -1;
    }

    debugging_mdl_file("models/HL1_Original/barney.mdl");

    // Print what models we have
    printf("=== AVAILABLE MODELS ===\n");
    for (int i = 0; i < model.total_model_count; i++) {
        printf("Model[%d]: '%s' (bodypart: '%s') - %d vertices, %d triangles\n", 
               i, model.models[i].model_name, model.models[i].bodypart_name,
               model.models[i].vertex_count, model.models[i].triangle_count);
    }

    // Set up arrays for multiple models
    GLuint vaos[10];    
    GLuint vbos[10];    
    GLuint ebos[10];    
    int valid_models = 0;

    // Set up each valid model
    for (int i = 0; i < 1 && valid_models < 10; i++) {
        single_model_s *current_model = &model.models[i];
        
        // Skip invalid models
        if (current_model->triangle_count <= 0 || !current_model->vertices) {
            printf("Skipping model[%d] - invalid data\n", i);
            continue;
        }

        printf("Setting up model[%d]: %s\n", i, current_model->model_name);

        // Generate and bind VAO
        glGenVertexArrays(1, &vaos[valid_models]);
        glBindVertexArray(vaos[valid_models]);

        // Set up vertex buffer
        glGenBuffers(1, &vbos[valid_models]);
        glBindBuffer(GL_ARRAY_BUFFER, vbos[valid_models]);
        glBufferData(GL_ARRAY_BUFFER, 
                     current_model->vertex_count * 3 * sizeof(float), 
                     current_model->vertices, GL_STATIC_DRAW);

        // Set up element buffer
        glGenBuffers(1, &ebos[valid_models]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebos[valid_models]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                     current_model->triangle_count * 3 * sizeof(int), 
                     current_model->triangle_indices, GL_STATIC_DRAW);

        // Configure vertex attributes (position only)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        valid_models++;
    }

    printf("Set up %d valid models for rendering\n", valid_models);

    // Load minimal shaders
    char *vertexSource = read_shader_source("shaders/basic.vert");
    char *fragmentSource = read_shader_source("shaders/basic.frag");
    
    if (!vertexSource || !fragmentSource) {
        printf("Failed to load shaders!\n");
        return -1;
    }

    GLuint vertexShader = compile_shader(vertexSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compile_shader(fragmentSource, GL_FRAGMENT_SHADER);
    GLuint shaderProgram = create_shader_program(vertexShader, fragmentShader);

    free(vertexSource);
    free(fragmentSource);

    // Get uniform locations
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

    // Set up matrices
    mat4 model_matrix, view_matrix, projection_matrix;
    glm_mat4_identity(view_matrix);
    glm_translate(view_matrix, (vec3){0.0f, 0.0f, -50.0f});  // Move camera back
    glm_perspective(glm_rad(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f, projection_matrix);

    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Update rotation
        static float rotation = 0.0f;
        rotation += 0.01f;

        glm_mat4_identity(model_matrix);
        glm_rotate(model_matrix, rotation, (vec3){0.0f, 1.0f, 0.0f});
        glm_translate(model_matrix, (vec3){8.95f, 2.62f, -0.08f}); 
        glm_scale(model_matrix, (vec3){1.0f, 1.0f, 1.0f});  // Scale down

        // Send matrices to shaders
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float*)model_matrix);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float*)view_matrix);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (float*)projection_matrix);

        // RENDER ALL VALID MODELS
        for (int i = 0; i < valid_models; i++) {
            single_model_s *current_model = &model.models[i];
            
            glBindVertexArray(vaos[i]);
            glDrawElements(GL_TRIANGLES, 
                          current_model->triangle_count * 3, 
                          GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    for (int i = 0; i < valid_models; i++) {
        glDeleteVertexArrays(1, &vaos[i]);
        glDeleteBuffers(1, &vbos[i]);
        glDeleteBuffers(1, &ebos[i]);
    }
    
    glDeleteProgram(shaderProgram);
    free_mdl_file(&model);
    glfwTerminate();

    return 0;
}
