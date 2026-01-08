#ifndef RENDERER_H
#define RENDERER_H

#include "r_gl_platform.h"

#include "studio.h"
#include "mdl/mdl_loader.h"  // <-- Need this for mdl_seqgroup_blob_tS
#include "math_types.h"

#include <stdbool.h>


extern GLFWwindow *window;


#define ASPECT_RATIO ((float)(16.0f / 9.0f))
#define WIDTH ((int)1400)
#define HEIGHT ((int)(WIDTH / ASPECT_RATIO))


int  init_renderer(int width, int height, const char *title);
void cleanup_renderer(void);

void render_loop(void);
bool should_close_window(void);

float compute_model_floor_y( void );

// ═══════════════════════════════════════════════════════════════════════════
// LAYERED RENDERING API
// ═══════════════════════════════════════════════════════════════════════════
// Layer 1: Pure OpenGL drawing (just bind and draw)
void draw_model_geometry( void );

// Layer 2: Model rendering with custom matrices (Qt/GUI uses this)
void render_model_with_matrices( mat4 view, mat4 proj, mat4 model );

// Layer 3: Scene rendering (grid/axes + model)
void render_scene( mat4 view, mat4 proj, mat4 model );

// Layer 4: Full rendering with camera (CLI version uses this)
void render_model(studiohdr_t *header, unsigned char *data);

void set_wireframe_mode(bool enabled);
void set_current_texture(unsigned int texture_id);

// Skin family controls
void next_skin_family(void);
void prev_skin_family(void);
int get_current_skin_family(void);
int get_num_skin_families(void);

// Camera mode controls
void set_camera_orbit_mode(bool orbit);
bool get_camera_orbit_mode(void);

// Model rotation controls (used when Shift is held)
float *get_model_rotation_x_ptr(void);
float *get_model_rotation_y_ptr(void);

// Ground offset control
float *get_ground_offset_ptr(void);

void set_model_data(
    studiohdr_t *header,
    unsigned char *data,
    studiohdr_t *tex_header,
    unsigned char *tex_data,
    mdl_seqgroup_blob_t *seqgroups,     
    int num_seqgroups                    
);


// Adding shader programs that other files can use
char *read_shader_source( const char *filename );

GLuint compile_shader( const char *source, GLenum type );

GLuint create_shader_program( GLuint vertexShader, GLuint fragmentShader );

int load_shaders( void );


void clear_screen(void);

void setup_triangle(void);  // Creates VAO/VBO for model rendering
void UpdateBonesForCurrentFrame(void);
void ProcessModelForRendering(void);
void AddVertexToBuffer(int vIndex, int nIndex, short s, short t, float texW, float texH);


#endif 


