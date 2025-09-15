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

static GLFWwindow *window = NULL;
static bool wireframe_enabled = false;

static unsigned int VBO = 0;
static unsigned int VAO = 0;
static unsigned int shader_program = 0;

// Model data
static float *model_vertices = NULL;
static int vertex_count = 0;
static bool debug_printed = false;

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
    
    printf("Loading %d vertices into OpenGL buffers...\n", count);
    
    // Generate and bind vertex buffer
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, count * 3 * sizeof(float), vertices, GL_STATIC_DRAW);
    
    // Setup vertex array object
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    
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

    return ( 0 );
}

void cleanup_renderer(void) {

    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
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
    
    while(!should_close_window()) {

        clear_screen();

        // Use our shader program
        glUseProgram(shader_program);
        
        // Pass time to shader for rotation
        float time = (float)glfwGetTime();
        GLint timeLocation = glGetUniformLocation(shader_program, "time");
        glUniform1f(timeLocation, time);
        
        // Bind our vertex array and draw the model
        glBindVertexArray(VAO);
        
        if (vertex_count > 0) {
            // Draw the model as wireframe triangles to see the structure
            if (!debug_printed) {
                printf("Rendering %d vertices as wireframe triangles\n", vertex_count);
                debug_printed = true;
            }
            
            // Enable wireframe mode to see the structure
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            
            // Draw triangles using every 3 consecutive vertices
            int triangle_count = vertex_count / 3;
            glDrawArrays(GL_TRIANGLES, 0, triangle_count * 3);
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

