/*======================================================================
 *  File: renderer.c
 *  Project: shaders
 *  Author: karlosiric <email@example.com>
 *  Created: 2025-09-24 14:22:30
 *  Last Modified by: karlosiric
 *  Last Modified: 2025-09-24 14:54:47
 *----------------------------------------------------------------------
 *  Description:
 *      
 *----------------------------------------------------------------------
 *  License: 
 *  Company: 
 *  Version: 0.1.0
 *======================================================================
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
#include <cglm/cglm.h>

GLFWwindow *window = NULL;
static bool wireframe_enabled = false;

static unsigned int VBO = 0;
static unsigned int VAO = 0;
static unsigned int EBO = 0;  // Element Buffer Object for indices
static unsigned int shader_program = 0;
static unsigned int current_texture = 0;  // Currently bound texture

// For Bone data
static int m_numbones = 0;
static int m_numverts = 0;
static mat4 m_bonetransform[MAXSTUDIOBONES];
static vec3 m_pxformverts[MAXSTUDIOVERTS];
static unsigned char *m_pvertbone;
static vec3 *m_pstudioverts;

extern float rotation_x;
extern float rotation_y;
extern float zoom;

// Model data
static float *model_vertices = NULL;
static int vertex_count = 0;
static int index_count = 0;
static bool debug_printed = false;
static bool bone_system_initialized = false;

// PRE-ALLOCATED BUFFERS (NO MALLOC IN RENDER LOOP)
#define MAX_RENDER_VERTICES 32768
static float render_vertex_buffer[MAX_RENDER_VERTICES * 8]; // 3 pos + 3 normal + 2 uv
static int total_render_vertices = 0;
static bool model_processed = false;

static studiohdr_t *global_header = NULL;
static unsigned char *global_data = NULL;

// Camera controls
float rotation_x = 0.0f;
float rotation_y = 0.0f;
float zoom = 0.15f;  // Even more zoomed out for scientist model

static void glfw_error_callback(int error, const char *description) {
    fprintf(stderr, "GLFW ERROR %d: %s\n", error, description);
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

// Add this function to initialize the bone system
void InitializeBoneSystem(studiohdr_t *header, unsigned char *data, int bodypart_index) {
    // Get the current model we're rendering
    mstudiobodypart_t *bodyparts = (mstudiobodypart_t *)(data + header->bodypartindex);
    mstudiomodel_t *models = (mstudiomodel_t *)(data + bodyparts[bodypart_index].modelindex);
    mstudiomodel_t *model = &models[0];
    
    // Set the counts
    m_numbones = header->numbones;
    m_numverts = model->numverts;
    
    // Set pointers to the actual data
    m_pstudioverts = (vec3_t *)(data + model->vertindex);           // Original vertices
    m_pvertbone = (unsigned char *)(data + model->vertinfoindex);   // Vertex-to-bone mapping
    
    printf("Bone system initialized: %d bones, %d vertices\n", m_numbones, m_numverts);
}

// Setting up bones using cglm 
void SetUpBones() {
    mstudiobone_t *bones = (mstudiobone_t *)(global_data + global_header->boneindex);
    
    // Create transformation matrices with better test transforms
    for (int i = 0; i < m_numbones; i++) {
        // Create identity matrix
        glm_mat4_identity(m_bonetransform[i]);
        
        // Apply subtle variations instead of dramatic transforms
        float variation = sin(i * 0.3f) * 0.05f;  // Small variation based on bone index
        vec3 scale = {1.0f + variation, 1.0f + variation, 1.0f + variation};
        glm_scale(m_bonetransform[i], scale);
    }
    
    // Transform all vertices
    for (int i = 0; i < m_numverts; i++) {
        int bone_index = m_pvertbone[i];
        
        // Ensure bone index is valid
        if (bone_index >= m_numbones) bone_index = 0;
        
        vec4 vertex_homogeneous = {
            m_pstudioverts[i][0], 
            m_pstudioverts[i][1], 
            m_pstudioverts[i][2], 
            1.0f
        };
        
        vec4 result;
        glm_mat4_mulv(m_bonetransform[bone_index], vertex_homogeneous, result);
        
        m_pxformverts[i][0] = result[0];
        m_pxformverts[i][1] = result[1];
        m_pxformverts[i][2] = result[2];
    }
}


// Updated ProcessModelForRendering to extract normals and UVs
void ProcessModelForRendering(void) {
    if (!global_header || !global_data || model_processed) return;
    
    printf("Processing model with PROPER normals and UVs...\n");
    total_render_vertices = 0;
    
    // Initialize and setup bones first
    if (!bone_system_initialized) {
        InitializeBoneSystem(global_header, global_data, 0);
        SetUpBones();
        bone_system_initialized = true;
    }
    
    mstudiobodypart_t *bodyparts = (mstudiobodypart_t *)(global_data + global_header->bodypartindex);
    
    // Process only the first bodypart (main body) to start with
    for (int bp = 0; bp < 1 && bp < global_header->numbodyparts; bp++) {
        if (bodyparts[bp].nummodels == 0) continue;
        
        mstudiomodel_t *models = (mstudiomodel_t *)(global_data + bodyparts[bp].modelindex);
        mstudiomodel_t *model = &models[0];
        
        if (model->numverts == 0 || model->nummesh == 0) continue;
        
        mstudiomesh_t *meshes = (mstudiomesh_t *)(global_data + model->meshindex);
        
        printf("Processing bodypart %d: %d meshes\n", bp, model->nummesh);
        
        // Process triangle commands for all meshes  
        for (int mesh = 0; mesh < model->nummesh && mesh < 5; mesh++) { // Limit to first 5 meshes
            short *ptricmds = (short *)(global_data + meshes[mesh].triindex);
            
            int command;
            while ((command = *(ptricmds++))) {
                bool is_fan = (command < 0);
                int vertex_count = is_fan ? -command : command;
                
                // Safety checks
                if (vertex_count < 3 || vertex_count > 50) {
                    ptricmds += vertex_count * 4;
                    continue;
                }
                
                // Store vertex data for this primitive
                struct {
                    int vertex_idx;
                    int normal_idx;
                    float u, v;
                } temp_data[50];
                
                for (int v = 0; v < vertex_count; v++, ptricmds += 4) {
                    temp_data[v].vertex_idx = ptricmds[0];
                    temp_data[v].normal_idx = ptricmds[1]; // NORMAL INDEX - this was missing!
                    temp_data[v].u = ptricmds[2];          // U coordinate
                    temp_data[v].v = ptricmds[3];          // V coordinate
                    
                    // Bounds check
                    if (temp_data[v].vertex_idx < 0 || temp_data[v].vertex_idx >= m_numverts) {
                        temp_data[v].vertex_idx = 0;
                    }
                    if (temp_data[v].normal_idx < 0 || temp_data[v].normal_idx >= model->numnorms) {
                        temp_data[v].normal_idx = 0;
                    }
                }
                
                // Convert to triangles with proper normal and UV data
                if (is_fan) {
                    for (int v = 1; v < vertex_count - 1; v++) {
                        if (total_render_vertices < MAX_RENDER_VERTICES - 3) {
                            AddVertexToBuffer(temp_data[0].vertex_idx, temp_data[0].normal_idx, temp_data[0].u, temp_data[0].v);
                            AddVertexToBuffer(temp_data[v].vertex_idx, temp_data[v].normal_idx, temp_data[v].u, temp_data[v].v);
                            AddVertexToBuffer(temp_data[v+1].vertex_idx, temp_data[v+1].normal_idx, temp_data[v+1].u, temp_data[v+1].v);
                        }
                    }
                } else {
                    for (int v = 0; v < vertex_count - 2; v++) {
                        if (total_render_vertices < MAX_RENDER_VERTICES - 3) {
                            if (v % 2 == 0) {
                                AddVertexToBuffer(temp_data[v].vertex_idx, temp_data[v].normal_idx, temp_data[v].u, temp_data[v].v);
                                AddVertexToBuffer(temp_data[v+1].vertex_idx, temp_data[v+1].normal_idx, temp_data[v+1].u, temp_data[v+1].v);
                                AddVertexToBuffer(temp_data[v+2].vertex_idx, temp_data[v+2].normal_idx, temp_data[v+2].u, temp_data[v+2].v);
                            } else {
                                AddVertexToBuffer(temp_data[v+1].vertex_idx, temp_data[v+1].normal_idx, temp_data[v+1].u, temp_data[v+1].v);
                                AddVertexToBuffer(temp_data[v].vertex_idx, temp_data[v].normal_idx, temp_data[v].u, temp_data[v].v);
                                AddVertexToBuffer(temp_data[v+2].vertex_idx, temp_data[v+2].normal_idx, temp_data[v+2].u, temp_data[v+2].v);
                            }
                        }
                    }
                }
                
                if (total_render_vertices > MAX_RENDER_VERTICES - 100) {
                    printf("Approaching buffer limit, stopping at %d vertices\n", total_render_vertices);
                    goto processing_complete;
                }
            }
        }
    }
    
    processing_complete:
    printf("PROPER processing complete: %d vertices with normals and UVs\n", total_render_vertices);
    model_processed = true;
}

// Updated AddVertexToBuffer to include normals and UVs
void AddVertexToBuffer(int vertex_index, int normal_index, float u, float v) {
    if (total_render_vertices >= MAX_RENDER_VERTICES) return;
    
    // Get pointers to normal data
    vec3_t *normals = (vec3_t *)(global_data + ((mstudiomodel_t*)(global_data + ((mstudiobodypart_t*)(global_data + global_header->bodypartindex))->modelindex))->normindex);
    
    int base_index = total_render_vertices * 8;
    
    // Position (transformed by bones)
    render_vertex_buffer[base_index + 0] = m_pxformverts[vertex_index][0] * 0.05f;
    render_vertex_buffer[base_index + 1] = m_pxformverts[vertex_index][1] * 0.05f;
    render_vertex_buffer[base_index + 2] = m_pxformverts[vertex_index][2] * 0.05f;
    
    // Normal (this is KEY for proper lighting)
    render_vertex_buffer[base_index + 3] = normals[normal_index][0];
    render_vertex_buffer[base_index + 4] = normals[normal_index][1];
    render_vertex_buffer[base_index + 5] = normals[normal_index][2];
    
    // Texture coordinates (normalized)
    render_vertex_buffer[base_index + 6] = u / 64.0f; // Normalize by typical texture size
    render_vertex_buffer[base_index + 7] = v / 64.0f;
    
    total_render_vertices++;
}
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
        fprintf(stderr, "ERROR - Failed to open the shader file: '%s' \n", filepath);
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

    if (load_shaders() != 0) {
        fprintf(stderr, "ERROR - Failed to load shaders!\n");
        return (-1);
    }

    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
    
    printf("\n=== CONTROLS ===\n");
    printf("W/S: Tilt up/down\n");
    printf("A/D: Rotate left/right\n");
    printf("Q/E: Zoom in/out\n");
    printf("F: Toggle wireframe/solid\n");
    printf("P: Points mode\n");
    printf("R: Reset view\n");
    printf("ESC: Exit\n");
    printf("================\n\n");

    return (0);
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
    printf("Starting render loop...\n");
    
    while (!should_close_window()) {
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

// Updated render_model to use the new vertex format
void render_model(studiohdr_t *header, unsigned char *data) {
    (void)header; (void)data;
    
    if (!model_processed) {
        ProcessModelForRendering();
    }
    
    if (total_render_vertices == 0) return;
    
    glUseProgram(shader_program);
    
    // Pass uniforms
    GLint timeLocation = glGetUniformLocation(shader_program, "time");
    GLint rotXLocation = glGetUniformLocation(shader_program, "rotation_x");
    GLint rotYLocation = glGetUniformLocation(shader_program, "rotation_y");
    GLint zoomLocation = glGetUniformLocation(shader_program, "zoom");
    
    if (timeLocation != -1) glUniform1f(timeLocation, (float)glfwGetTime());
    if (rotXLocation != -1) glUniform1f(rotXLocation, rotation_x);
    if (rotYLocation != -1) glUniform1f(rotYLocation, rotation_y);
    if (zoomLocation != -1) glUniform1f(zoomLocation, zoom);
    
    // Upload vertex data with normals and UVs
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, total_render_vertices * 8 * sizeof(float), 
                 render_vertex_buffer, GL_STATIC_DRAW);
    
    // Position attribute (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal attribute (location 1) - NEW!
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Texture coordinate attribute (location 2) - NEW!
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glDrawArrays(GL_TRIANGLES, 0, total_render_vertices);
}


void set_model_data(studiohdr_t *header, unsigned char *data) {
    global_header = header;
    global_data = data;
    
    // Reset processing flags so model gets processed next render
    model_processed = false;
    bone_system_initialized = false;
    total_render_vertices = 0;
    
    printf("Model data set, will be processed on next render\n");
}
