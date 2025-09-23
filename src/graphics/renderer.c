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
static unsigned int current_texture = 0;  // Currently bound texture

// Model data
static float *model_vertices = NULL;
static int vertex_count = 0;
static int index_count = 0;
static bool debug_printed = false;

static studiohdr_t *global_header = NULL;
static unsigned char *global_data = NULL;

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
    
    // Try to load textured shaders first, fall back to basic if not found
    char *vertex_shader_file = read_shader_source("/Users/karlosiric/Documents/SublimeText Programming/C_Projects/ModelViewer/shaders/textured.vert");
    char *fragment_shader_file = read_shader_source("/Users/karlosiric/Documents/SublimeText Programming/C_Projects/ModelViewer/shaders/textured.frag");
    
    if (!vertex_shader_file || !fragment_shader_file) {
        printf("Textured shaders not found, falling back to basic shaders\n");
        if (vertex_shader_file) free(vertex_shader_file);
        if (fragment_shader_file) free(fragment_shader_file);
        
        vertex_shader_file = read_shader_source("/Users/karlosiric/Documents/SublimeText Programming/C_Projects/ModelViewer/shaders/basic.vert");
        fragment_shader_file = read_shader_source("/Users/karlosiric/Documents/SublimeText Programming/C_Projects/ModelViewer/shaders/basic.frag");
    } else {
        printf("Using textured shaders\n");
    }

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

// NOTE: Needs to be reimplemented properly missing some things and not working well.

void render_loop(void) {

    if (!should_close_window()) {

        clear_screen();

        if (global_header && global_data) {
            render_model(global_header, global_data);

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

void set_current_texture(unsigned int texture_id) {
    current_texture = texture_id;
    printf("Set current texture to ID: %u\n", texture_id);
}   

void render_model(studiohdr_t *header, unsigned char *data) {


    glUseProgram(0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-100, 100, -100, 100, -100, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef((float)glfwGetTime() * 50.0f, 0.0f, 1.0f, 0.0f);

    // Get first bodypart and model
    mstudiobodypart_t *bodyparts = (mstudiobodypart_t *)(data + header->bodypartindex);
    mstudiomodel_t *models = (mstudiomodel_t *)(data + bodyparts[0].modelindex);
    mstudiomodel_t *model = &models[0];

    vec3_t *vertices = (vec3_t *)(data + model->vertindex);

    mstudiomesh_t *meshes = (mstudiomesh_t *)(data + model->meshindex);

    for (int mesh = 0; mesh < model->nummesh; mesh++) {

        short *triangle_commands = (short *)(data + meshes[mesh].triindex);

        int i;
        while((i = *(triangle_commands++))) {
            if (i < 0) {
                glBegin(GL_TRIANGLE_FAN);
                i = -i;

            } else {
                glBegin(GL_TRIANGLE_STRIP);

            }

            for (; i > 0; i--, triangle_commands += 4) {
                int vertIndex = triangle_commands[0];
                glVertex3f(vertices[vertIndex][0], vertices[vertIndex][1], vertices[vertIndex][2]);
            }

            glEnd();
        }
    } 
 
}

void set_model_data(studiohdr_t *header, unsigned char *data) {

    global_header = header;
    global_data = data;


}



