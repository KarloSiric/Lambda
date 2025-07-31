/*
* Half-Life MDL Model Viewer - Using New Integration Function
*/
#define OPENGL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>
#include <cglm/cglm.h>
#include "shader.h"
#include "mdl_loader.h"

#define WIDTH  1200
#define HEIGHT 800

int main() {
    
    if (!glfwInit()) {
        fprintf(stderr, "Error initializing GLFW!\n"); 
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Half-Life Scientist Viewer - Use Arrow Keys & WASD", NULL, NULL);
    
    if (!window) {
        fprintf(stderr, "Failed to create window!\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glEnable(GL_DEPTH_TEST);
    // Disable culling to see if triangles are backwards
    // glEnable(GL_CULL_FACE);  
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Wireframe to see both models

    // 🚀 USE NEW ENHANCED INTEGRATION FUNCTION!
    printf("🔬 Loading HeadCrab for testing...\n");
    mdl_complete_model_s *complete_model = mdl_load_complete_file("models/HL1_Original/headcrab.mdl");

    if (!complete_model) {
        printf("❌ Failed to load MDL file!\n");
        glfwTerminate();
        return -1;
    }

    // Print what models we have
    printf("\n=== 🎨 AVAILABLE MODELS ===\n");
    for (int i = 0; i < complete_model->total_model_count; i++) {
        printf("Model[%d]: '%s' (bodypart: '%s') - %d vertices, %d triangles\n", 
               i, complete_model->models[i].model_name, complete_model->models[i].bodypart_name,
               complete_model->models[i].vertex_count, complete_model->models[i].triangle_count);
        
        // Debug: Print first few vertices to see if they make sense
        if (complete_model->models[i].vertex_count > 0 && complete_model->models[i].vertices) {
            printf("  First 3 vertices:\n");
            for (int v = 0; v < 3 && v < complete_model->models[i].vertex_count; v++) {
                printf("    [%d]: (%.2f, %.2f, %.2f)\n", v,
                       complete_model->models[i].vertices[v*3+0],
                       complete_model->models[i].vertices[v*3+1], 
                       complete_model->models[i].vertices[v*3+2]);
            }
            
            // Check for reasonable coordinate ranges
            float min_x = 999999, max_x = -999999;
            float min_y = 999999, max_y = -999999;
            float min_z = 999999, max_z = -999999;
            
            for (int v = 0; v < complete_model->models[i].vertex_count; v++) {
                float x = complete_model->models[i].vertices[v*3+0];
                float y = complete_model->models[i].vertices[v*3+1];
                float z = complete_model->models[i].vertices[v*3+2];
                
                if (x < min_x) min_x = x; if (x > max_x) max_x = x;
                if (y < min_y) min_y = y; if (y > max_y) max_y = y;
                if (z < min_z) min_z = z; if (z > max_z) max_z = z;
            }
            
            printf("  Bounds: X[%.2f to %.2f] Y[%.2f to %.2f] Z[%.2f to %.2f]\n",
                   min_x, max_x, min_y, max_y, min_z, max_z);
        }
    }
    
    printf("\n=== 🦴 BONE SYSTEM ===\n");
    printf("Loaded %d bones and %d textures\n", complete_model->bone_count, complete_model->texture_count);

    // 🎯 Set up OpenGL buffers for ALL models
    GLuint vaos[10];    
    GLuint vbos[10];    
    GLuint texcoord_vbos[10];
    GLuint ebos[10];    
    int valid_models = 0;

    // Set up each valid model (skip blank models)
    for (int i = 0; i < complete_model->total_model_count && valid_models < 10; i++) {
        single_model_s *current_model = &complete_model->models[i];
        
        // Skip invalid models (like the blank needle)
        if (current_model->triangle_count <= 0 || !current_model->vertices) {
            printf("⏭️  Skipping model[%d] '%s' - no geometry\n", i, current_model->model_name);
            continue;
        }

        printf("🔧 Setting up model[%d]: %s (%d triangles, %d vertices)\n", 
               valid_models, current_model->model_name, current_model->triangle_count, current_model->vertex_count);
        
        // Debug: Check triangle indices
        if (current_model->triangle_indices) {
            printf("  First 9 triangle indices: ");
            for (int t = 0; t < 9 && t < current_model->triangle_count * 3; t++) {
                printf("%d ", current_model->triangle_indices[t]);
            }
            printf("\n");
        }

        // Generate and bind VAO
        glGenVertexArrays(1, &vaos[valid_models]);
        glBindVertexArray(vaos[valid_models]);

        // Set up vertex buffer (use transformed vertices if available)
        glGenBuffers(1, &vbos[valid_models]);
        glBindBuffer(GL_ARRAY_BUFFER, vbos[valid_models]);
        
        float *vertex_data = current_model->transformed_vertices ? 
                            current_model->transformed_vertices : current_model->vertices;
        
        glBufferData(GL_ARRAY_BUFFER, 
                     current_model->vertex_count * 3 * sizeof(float), 
                     vertex_data, GL_STATIC_DRAW);

        // Configure vertex position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        // Set up texture coordinate buffer if available
        if (current_model->texcoords && current_model->texcoord_count > 0) {
            glGenBuffers(1, &texcoord_vbos[valid_models]);
            glBindBuffer(GL_ARRAY_BUFFER, texcoord_vbos[valid_models]);
            glBufferData(GL_ARRAY_BUFFER, 
                         current_model->texcoord_count * 2 * sizeof(float), 
                         current_model->texcoords, GL_STATIC_DRAW);
            
            // Configure texture coordinate attribute
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            
            printf("  ✅ Set up %d texture coordinates\n", current_model->texcoord_count);
        } else {
            printf("  ⚠️  No texture coordinates for this model\n");
            texcoord_vbos[valid_models] = 0;
        }

        // Set up element buffer  
        glGenBuffers(1, &ebos[valid_models]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebos[valid_models]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                     current_model->triangle_count * 3 * sizeof(unsigned int), 
                     current_model->triangle_indices, GL_STATIC_DRAW);

        valid_models++;
    }

    printf("✅ Set up %d valid models for rendering\n", valid_models);

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
    GLint useTextureLoc = glGetUniformLocation(shaderProgram, "useTexture");
    GLint textureLoc = glGetUniformLocation(shaderProgram, "ourTexture");
    
    // Set texture unit
    glUseProgram(shaderProgram);
    glUniform1i(textureLoc, 0); // Use texture unit 0

    // 📐 Set up view and projection matrices
    mat4 model_matrix, view_matrix, projection_matrix;
    glm_mat4_identity(view_matrix);
    glm_translate(view_matrix, (vec3){0.0f, 0.0f, -15.0f});  // Simple straight-on view
    glm_perspective(glm_rad(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f, projection_matrix);
    
    // Add camera controls variables
    static float camera_distance = 15.0f;  // Reasonable distance
    static float camera_height = 0.0f;     // Level with model
    static float rotation_speed = 0.5f;
    static bool wireframe_mode = true;
    static bool last_f_pressed = false;

    // 🎬 Main render loop
    printf("\n🎮 Starting render loop...\n");
    printf("\n=== CAMERA CONTROLS ===\n");
    printf("UP/DOWN arrows: Zoom in/out\n");
    printf("W/S: Move camera up/down\n");
    printf("LEFT/RIGHT arrows: Change rotation speed\n");
    printf("F: Toggle wireframe/solid mode\n");
    printf("SPACE: Reset camera\n");
    printf("ESC: Exit\n\n");
    while (!glfwWindowShouldClose(window)) {
        
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);  // Dark blue background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Handle keyboard input for camera control
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) camera_distance -= 2.0f;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) camera_distance += 2.0f;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera_height += 1.0f;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera_height -= 1.0f;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) rotation_speed -= 0.1f;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) rotation_speed += 0.1f;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            // Reset view
            camera_distance = 15.0f;  // Reasonable default
            camera_height = 0.0f;     // Level view
            rotation_speed = 0.5f;
        }
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
        
        // Toggle wireframe mode
        bool f_pressed = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS;
        if (f_pressed && !last_f_pressed) {
            wireframe_mode = !wireframe_mode;
            if (wireframe_mode) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                printf("Switched to wireframe mode\n");
            } else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                printf("Switched to solid mode\n");
            }
        }
        last_f_pressed = f_pressed;
        
        // Clamp values
        if (camera_distance < 5.0f) camera_distance = 5.0f;   // Closer minimum
        if (camera_distance > 100.0f) camera_distance = 100.0f; // Closer maximum
        
        // Update view matrix based on controls
        glm_mat4_identity(view_matrix);
        glm_translate(view_matrix, (vec3){0.0f, camera_height, -camera_distance});

        // 🔄 Update rotation
        static float rotation = 0.0f;
        rotation += rotation_speed * 0.01f;  // Controllable rotation speed

        // 🎯 Set up model matrix (center and scale the model)
        glm_mat4_identity(model_matrix);
        
        // Apply rotation around Y-axis (vertical) for normal character rotation
        glm_rotate(model_matrix, rotation, (vec3){0.0f, 1.0f, 0.0f});  // Rotate around Y-axis
        
        // Scale appropriately
        glm_scale(model_matrix, (vec3){0.5f, 0.5f, 0.5f});  // Medium scale

        // Send matrices to shaders
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float*)model_matrix);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float*)view_matrix);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (float*)projection_matrix);

        // 🎨 RENDER ALL VALID MODELS WITH ENHANCED FEATURES
        for (int model_idx = 0; model_idx < valid_models; model_idx++) {
            single_model_s *current_model = &complete_model->models[model_idx];
            
            // Bind texture if available
            bool has_texture = false;
            if (complete_model->textures && complete_model->texture_count > 0) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, complete_model->textures[0].gl_texture_id);
                has_texture = true;
            }
            
            glUniform1i(useTextureLoc, has_texture ? 1 : 0);
            
            if (current_model->triangle_count > 0 && current_model->vertices) {
                glBindVertexArray(vaos[model_idx]);
                glDrawElements(GL_TRIANGLES, 
                              current_model->triangle_count * 3, 
                              GL_UNSIGNED_INT, 0);
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 🧹 Cleanup
    printf("\n🧹 Cleaning up resources...\n");
    for (int i = 0; i < valid_models; i++) {
        glDeleteVertexArrays(1, &vaos[i]);
        glDeleteBuffers(1, &vbos[i]);
        glDeleteBuffers(1, &ebos[i]);
        if (texcoord_vbos[i] != 0) {
            glDeleteBuffers(1, &texcoord_vbos[i]);
        }
    }
    
    glDeleteProgram(shaderProgram);
    mdl_free_complete_model(complete_model);  // Use new cleanup function
    glfwTerminate();

    printf("✅ Program completed successfully!\n");
    return 0;
}
