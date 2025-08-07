/*
 * Renderer implementation
 * We'll build this step by step
 */

#include "renderer.h"



#ifdef __APPLE__
#define  GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else 
#include <GL/gl3.h>
#endif

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

static GLFWwindow *window = NULL;
static bool wireframe_enabled = false;

static unsigned int VBO = 0;
static unsigned int VAO = 0;
static unsigned int shader_program = 0;

static void glfw_error_callback(int error, const char *description) {
    fprintf(stderr, "GLFW ERROR %d: %s\n",
            error, description);
}

static void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
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

static char *read_shader_source(const char *filepath) { 
    FILE *file = fopen(filepath, "r");
    if (!file) {
        fprintf(stderr, "ERROR - Failed to open the shader file: '%s' \n",
                filepath);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    size_t length = ftell(file);
    rewind(file);

    char *buffer = malloc(length * sizeof(char));

    if (!buffer) {
        fprintf(stderr, "ERROR - Failed to allocate memory for shader file buffer!\n");
        fclose(file);
        return NULL;
    }


    fread(buffer, 1, length, file);
    buffer[length] = '\0';
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
        fprintf(stderr, "ERROR - Failed to compile shaders!\n");
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
    glGetShaderiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        fprintf(stderr, "ERROR - Failed to link shaders to the program!\n");
        return (0);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

static int load_shaders(void) {
    char *vertex_shader_file = read_shader_source("../shaders/basic.vert");
    char *fragment_shader_file = read_shader_source("../shaders/basic.frag");

    if (!vertex_shader_file || !fragment_shader_file) {
        if (vertex_shader_file) free(vertex_shader_file);
        if (fragment_shader_file) free(fragment_shader_file);
    }

    GLuint vertexShader = compile_shader(vertex_shader_file, GL_VERTEX_SHADER);
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

        // TODO (Karlo): Here we need to add our drawing function that will draw everything
        // Need to implement that here later!
        
        glfwSwapBuffers(window);

        glfwPollEvents();
    }

}

void set_wireframe_mode(bool enabled) {
    bool wireframe_mode = enabled;

    if (wireframe_mode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}   

void render_model(studiohdr_t *header, unsigned char *data) {
    // TODO(Karlo): Need to implement for the data of the texture and model information
    // THis needs to be given to the buffers and draw from 
    
    glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

