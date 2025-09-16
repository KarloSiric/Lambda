/*
 * Renderer implementation
 * We'll build this step by step
 */

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include "renderer.h"

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else 
#include <GL/gl3.h>
#endif

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // For getcwd

GLFWwindow *window = NULL;
static bool wireframe_enabled = false;

static unsigned int VBO = 0;
static unsigned int VAO = 0;
static unsigned int EBO = 0;  // Element Buffer Object for indices
static unsigned int shader_program = 0;

// Model data
static float *model_vertices = NULL;
static int vertex_count = 0;
static int index_count = 0;
static bool debug_printed = false;

// Camera controls
static float rotation_x = 0.0f;
static float rotation_y = 0.0f;
static float zoom = 0.15f;  // Even more zoomed out for scientist model

static void glfw_error_callback(int error, const char *description) {
    fprintf(stderr, "GLFW ERROR %d: %s\n",
            error, description);
}

static void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    (void)scancode; // Suppress unused parameter warning
    (void)mods;     // Suppress unused parameter warning
    
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    
    // Camera controls
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch(key) {
            case GLFW_KEY_W: rotation_x -= 0.1f; break;  // Tilt up
            case GLFW_KEY_S: rotation_x += 0.1f; break;  // Tilt down
            case GLFW_KEY_A: rotation_y -= 0.1f; break;  // Rotate left
            case GLFW_KEY_D: rotation_y += 0.1f; break;  // Rotate right
            case GLFW_KEY_Q: zoom *= 1.1f; if(zoom > 2.0f) zoom = 2.0f; break;   // Zoom in with limit
            case GLFW_KEY_E: zoom *= 0.9f; if(zoom < 0.1f) zoom = 0.1f; break;   // Zoom out with limit
            case GLFW_KEY_R: // Reset view
                rotation_x = 0.0f;
                rotation_y = 0.0f;
                zoom = 0.15f;  // Reset to default zoom
                break;
            case GLFW_KEY_F: // Toggle wireframe
                wireframe_enabled = !wireframe_enabled;
                if (wireframe_enabled) {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    printf("Switched to wireframe mode\n");
                } else {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    printf("Switched to solid mode\n");
                }
                break;
            case GLFW_KEY_P: // Toggle points
                glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
                printf("Switched to points mode\n");
                break;
        }
    }
}

float vertices[] = {
    -0.5f, -0.5f, 0.0f,  // Bottom left
     0.5f, -0.5f, 0.0f,  // Bottom right  
     0.0f,  0.5f, 0.0f   // Top center  
};


void setup_triangle(void) {

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
}

void setup_model_vertices(float *vertices, int count) {
    // Store the vertex data
    model_vertices = vertices;
    vertex_count = count;
    index_count = 0;  // No indices - use regular vertex array drawing
    
    printf("Loading %d vertices into OpenGL buffers...\n", count);
    
    // Debug: Print some vertices to verify they're reasonable
    printf("Sample vertices:\n");
    for (int i = 0; i < 5 && i < count; i++) {
        printf("  Vertex %d: (%.3f, %.3f, %.3f)\n", i, 
               vertices[i*3], vertices[i*3+1], vertices[i*3+2]);
    }
    
    // Clean up any existing buffers
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    
    // Generate and bind VAO first
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    // Generate and bind vertex buffer
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, count * 3 * sizeof(float), vertices, GL_STATIC_DRAW);
    
    // Setup vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    
    // Unbind
    glBindVertexArray(0);
    
    printf("Model vertices loaded successfully!\n");
}

static char *read_shader_source(const char *filepath) { 
    printf("DEBUG - Trying to load shader from: %s\n", filepath);
    FILE *file = fopen(filepath, "r");
    if (!file) {
        fprintf(stderr, "ERROR - Failed to open the shader file: '%s' \n",
                filepath);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    size_t length = ftell(file);
    rewind(file);

    char *buffer = malloc((length + 1) * sizeof(char));

    if (!buffer) {
        fprintf(stderr, "ERROR - Failed to allocate memory for shader file buffer!\n");
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, length, file);
    buffer[length] = '\0';  // Null terminate
    fclose(file);

    return buffer;
}

static GLuint compile_shader(const char *source, GLenum type) {

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        char shader_info[512];
        glGetShaderInfoLog(shader, 512, NULL, shader_info);
        fprintf(stderr, "ERROR - Failed to compile shaders: \n%s\n", shader_info);
        return (0);
    } 

    return shader;
}

static GLuint create_shader_program(GLuint vertexShader, GLuint fragmentShader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);  // Use glGetProgramiv for program
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        fprintf(stderr, "ERROR - Failed to link shaders to the program!\n%s\n", infoLog);
        return (0);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

static int load_shaders(void) {
    // Debug: Print current working directory
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("DEBUG - Current working directory: %s\n", cwd);
    }
    
    char *vertex_shader_file = read_shader_source("/Users/karlosiric/Documents/SublimeText Programming/C_Projects/ModelViewer/shaders/basic.vert");
    char *fragment_shader_file = read_shader_source("/Users/karlosiric/Documents/SublimeText Programming/C_Projects/ModelViewer/shaders/basic.frag");

    if (!vertex_shader_file || !fragment_shader_file) {
        fprintf(stderr, "ERROR - Failed to load shader files!\n");
        if (vertex_shader_file) free(vertex_shader_file);
        if (fragment_shader_file) free(fragment_shader_file);
        return (-1);
    }
    printf("Compiling vertex shader...\n");
    GLuint vertexShader = compile_shader(vertex_shader_file, GL_VERTEX_SHADER);
    printf("Compiling fragment shader...\n");
    GLuint fragmentShader = compile_shader(fragment_shader_file, GL_FRAGMENT_SHADER);

    free(vertex_shader_file);
    free(fragment_shader_file);

    if (vertexShader == 0 || fragmentShader == 0) {
        return (-1);
    }

    shader_program = create_shader_program(vertexShader, fragmentShader);

    if (shader_program == 0) {
        fprintf(stderr, "ERROR - Failed to create properly a shader program!\n");
        return (-1);
    }

    printf("Shaders loaded and compiled successfully!\n");
    printf("Shader program up and ready to be used!\n");

    return (0);
}


int init_renderer(int width, int height, const char *title) {  

    if (!glfwInit()) {
        fprintf(stderr, "ERROR - Failed to initialize the GLFW, glfw_init() FAILED!\n");
        return (-1);
    }


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
    (void)primary; // We get this for future use but don't use it yet
    window = glfwCreateWindow(width, height, title, NULL, NULL);

    if (!window) {
        glfwTerminate();
        return (-1);
    }

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, glfw_key_callback);
    glfwSetErrorCallback(glfw_error_callback);

    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, width, height);

    glEnable(GL_CULL_FACE);
    
    // Enable point size for vertex visualization
    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(5.0f);  // Make points bigger

    setup_triangle();


    if ( load_shaders() != 0 ) {
        fprintf(stderr, "ERROR - Failed to load shaders!\n");
        return ( -1 );
    }

    printf("OpenGL Version: %s\n",
            glGetString( GL_VERSION ));
    
    printf("\n=== CONTROLS ===\n");
    printf("W/S: Tilt up/down\n");
    printf("A/D: Rotate left/right\n");
    printf("Q/E: Zoom in/out\n");
    printf("F: Toggle wireframe/solid\n");
    printf("P: Points mode\n");
    printf("R: Reset view\n");
    printf("ESC: Exit\n");
    printf("================\n\n");

    return ( 0 );
}

void cleanup_renderer(void) {

    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
    if (shader_program) glDeleteProgram(shader_program);

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
    return glfwWindowShouldClose(window);
}

void render_loop(void) {
    
    // Toggle between wireframe and points with 'W' key
    static bool show_wireframe = true;
    static bool last_w_pressed = false;
    
    while(!should_close_window()) {

        clear_screen();

        // Check for 'W' key to toggle wireframe/points
        bool w_pressed = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
        if (w_pressed && !last_w_pressed) {
            show_wireframe = !show_wireframe;
            printf("Switched to %s mode\n", show_wireframe ? "wireframe" : "points");
        }
        last_w_pressed = w_pressed;
        
        // Use our shader program
        glUseProgram(shader_program);
        
        // Pass camera controls to shader
        float time = (float)glfwGetTime();
        GLint timeLocation = glGetUniformLocation(shader_program, "time");
        GLint rotXLocation = glGetUniformLocation(shader_program, "rotation_x");
        GLint rotYLocation = glGetUniformLocation(shader_program, "rotation_y");
        GLint zoomLocation = glGetUniformLocation(shader_program, "zoom");
        GLint useTexLocation = glGetUniformLocation(shader_program, "useTexture");
        
        glUniform1f(timeLocation, time);
        glUniform1f(rotXLocation, rotation_x);
        glUniform1f(rotYLocation, rotation_y);
        glUniform1f(zoomLocation, zoom);
        
        // Enable textures!
        glUniform1i(useTexLocation, 1);  // Set to 1 to use textures
        
        // Tell the shader to use texture unit 0
        GLint textureLoc = glGetUniformLocation(shader_program, "texture1");
        glUniform1i(textureLoc, 0);  // Use texture unit 0
        
        // Bind texture unit 0 (you can bind different textures later)
        glActiveTexture(GL_TEXTURE0);
        // Try different texture IDs - check console for actual IDs
        glBindTexture(GL_TEXTURE_2D, 1);  // Start with ID 1
        
        // Bind our vertex array and draw the model
        glBindVertexArray(VAO);
        
        if (vertex_count > 0) {
            if (!debug_printed) {
                if (index_count > 0) {
                    printf("Rendering %d vertices with %d indices (%d triangles)\n", 
                           vertex_count, index_count, index_count / 3);
                } else {
                    printf("Rendering %d vertices as points (no triangles)\n", vertex_count);
                }
                debug_printed = true;
            }
            
            if (index_count > 0 && show_wireframe) {
                // Draw triangles using indices
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Wireframe
                glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_SHORT, 0);
            } else {
                // Draw as points
                glPointSize(3.0f);
                glDrawArrays(GL_POINTS, 0, vertex_count);
            }
        } else {
            // Fallback to triangle if no model loaded
            if (!debug_printed) {
                printf("No model vertices, drawing triangle\n");
                debug_printed = true;
            }
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void set_wireframe_mode(bool enabled) {
    wireframe_enabled = enabled;  // Now it's used

    if (wireframe_enabled) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}   

void render_model(studiohdr_t *header, unsigned char *data) {
    // TODO(Karlo): Need to implement for the data of the texture and model information
    // THis needs to be given to the buffers and draw from 
    (void)header; // Suppress unused parameter warning
    (void)data;   // Suppress unused parameter warning
    
    glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void setup_model_vertices_with_indices_and_texcoords(float *vertices, int vertex_count_param, 
                                                     unsigned short *indices, int index_count_param,
                                                     float *texcoords) {
    vertex_count = vertex_count_param;
    index_count = index_count_param;
    
    printf("Loading %d vertices and %d indices into OpenGL buffers...\n", vertex_count, index_count);
    
    // Generate and bind VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    // Generate and bind VBO for vertices
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * 3 * sizeof(float), vertices, GL_STATIC_DRAW);
    
    // Generate and bind EBO for indices
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(unsigned short), indices, GL_STATIC_DRAW);
    
    // Set vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Set texture coordinate attribute if provided
    if (texcoords) {
        unsigned int texCoordVBO;
        glGenBuffers(1, &texCoordVBO);
        glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
        glBufferData(GL_ARRAY_BUFFER, vertex_count * 2 * sizeof(float), texcoords, GL_STATIC_DRAW);
        
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        
        printf("Texture coordinates loaded successfully!\n");
    }
    
    // Unbind
    glBindVertexArray(0);
    
    printf("Model vertices and indices loaded successfully!\n");
}

