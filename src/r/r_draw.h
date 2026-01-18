#ifndef RENDERER_H
#define RENDERER_H

#include "r_gl_platform.h"

#include "studio.h"
#include "mdl/mdl_loader.h" // For mdl_seqgroup_blob_t
#include "mdl/mdl_animations.h" // For mdl_animation_state_t
#include "r/r_textures.h" // For mdl_texture_set_t
#include "math_types.h"

#include <stdbool.h>

extern GLFWwindow *window;

#define ASPECT_RATIO ( (float)( 16.0f / 9.0f ) )
#define WIDTH ( (int)1400 )
#define HEIGHT ( (int)( WIDTH / ASPECT_RATIO ) )

#define MAX_DRAW_RANGES 4096
#define MAX_RENDER_VERTICES 32768
#define MAXSTUDIOVERTS 2048

// ═══════════════════════════════════════════════════════════════════════════
// SHARED STRUCTURES (used by both CLI and Qt)
// ═══════════════════════════════════════════════════════════════════════════

typedef struct {
	mstudiomodel_t *model;
	vec3_t *vertices;
	vec3_t *normals;
	int vertex_count;
	int normal_count;
} current_model_data_t;

typedef struct {
	GLuint tex; // GL texture to bind
	int first; // First vertex index in VBO
	int count; // Number of vertices to draw
	int flags; // Texture flags (STUDIO_NF_CHROME, STUDIO_NF_MASKED, etc.)
} DrawRange;

// ═══════════════════════════════════════════════════════════════════════════
// QT INSTANCE STRUCTURE (per-viewport rendering)
// ═══════════════════════════════════════════════════════════════════════════
// Each Qt ModelViewport owns one of these for independent rendering.
// CLI does NOT use this - it uses global state in r_draw.c

typedef struct {
	// OpenGL resources (per-viewport)
	GLuint vbo;
	GLuint vao;
	GLuint ebo;
	GLuint bone_index_vbo;
	GLuint shader_program;

	// Model data pointers (not owned, just referenced from ModelViewport's m_model)
	studiohdr_t *header;
	unsigned char *data;
	studiohdr_t *tex_header;
	unsigned char *tex_data;

	// Textures (owned by instance)
	mdl_texture_set_t textures;

	// Per-instance bone transformations (CRITICAL: initialized to identity!)
	mat4 bone_transformations[MAXSTUDIOBONES];

	// Animation state (per-viewport)
	mdl_animation_state_t anim_state;
	bool animation_enabled;
	double last_frame_time;

	// Sequence groups
	mdl_seqgroup_blob_t *seqgroups;
	int num_seqgroups;

	// Render buffers (pre-allocated, per-viewport)
	float render_vertex_buffer[MAX_RENDER_VERTICES * 8]; // 3 pos + 3 normal + 2 uv
	int render_bone_indices[MAX_RENDER_VERTICES];
	int total_render_vertices;

	// Draw ranges
	DrawRange ranges[MAX_DRAW_RANGES];
	int num_ranges;

	// Current model data during processing
	current_model_data_t current;

	// Flags
	bool model_processed;
	bool t_pose_bones_calculated;
	int current_skin_family;

	// Cached uniform locations (looked up once, not every frame)
	GLint u_model_loc;
	GLint u_view_loc;
	GLint u_proj_loc;
	GLint u_tex_loc;
	GLint u_lightPos_loc;
	GLint u_viewPos_loc;
	GLint u_masked_loc;
	GLint u_fullbright_loc;
	GLint u_additive_loc;
	GLint u_chrome_loc;
	GLint u_boneMatrices_loc;

	// Fallback white texture
	GLuint white_tex;

} r_qt_instance_t;

// ═══════════════════════════════════════════════════════════════════════════
// CLI RENDERER FUNCTIONS (use global state)
// ═══════════════════════════════════════════════════════════════════════════

int init_renderer( int width, int height, const char *title );
void cleanup_renderer( void );

void render_loop( void );
bool should_close_window( void );

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
void render_model( studiohdr_t *header, unsigned char *data );

void set_wireframe_mode( bool enabled );
void set_current_texture( unsigned int texture_id );

// Skin family controls
void next_skin_family( void );
void prev_skin_family( void );
int get_current_skin_family( void );
int get_num_skin_families( void );

// Camera mode controls
void set_camera_orbit_mode( bool orbit );
bool get_camera_orbit_mode( void );

// Model rotation controls (used when Shift is held)
float *get_model_rotation_x_ptr( void );
float *get_model_rotation_y_ptr( void );

// Ground offset control
float *get_ground_offset_ptr( void );

void set_model_data(
	studiohdr_t *header,
	unsigned char *data,
	studiohdr_t *tex_header,
	unsigned char *tex_data,
	mdl_seqgroup_blob_t *seqgroups,
	int num_seqgroups );

// Adding shader programs that other files can use
char *read_shader_source( const char *filename );

GLuint compile_shader( const char *source, GLenum type );

GLuint create_shader_program( GLuint vertexShader, GLuint fragmentShader );

int load_shaders( void );

void clear_screen( void );

void setup_triangle( void ); // Creates VAO/VBO for model rendering
void UpdateBonesForCurrentFrame( void );
void ProcessModelForRendering( void );
void AddVertexToBuffer( int vIndex, int nIndex, short s, short t, float texW, float texH );

// ═══════════════════════════════════════════════════════════════════════════
// QT RENDERER FUNCTIONS (per-instance, for Qt ModelViewport)
// ═══════════════════════════════════════════════════════════════════════════

// Create a new Qt viewport instance (allocates and initializes)
r_qt_instance_t *r_qt_create_instance( void );

// Destroy Qt viewport instance (frees all resources)
void r_qt_destroy_instance( r_qt_instance_t *inst );

// Set model data for Qt instance (like set_model_data but per-instance)
void r_qt_set_model_data(
	r_qt_instance_t *inst,
	studiohdr_t *header,
	unsigned char *data,
	studiohdr_t *tex_header,
	unsigned char *tex_data,
	mdl_seqgroup_blob_t *seqgroups,
	int num_seqgroups );

// Render model with Qt instance (like render_model_with_matrices but per-instance)
void r_qt_render_with_matrices(
	r_qt_instance_t *inst,
	mat4 view,
	mat4 proj,
	mat4 model );

#endif
