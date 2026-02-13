/*
 * ═══════════════════════════════════════════════════════════════════════════
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ═══════════════════════════════════════════════════════════════════════════
 *
 *   Copyright (c) 1996-2002, Valve LLC. All rights reserved.
 *
 *   This product contains software technology licensed from Id
 *   Software, Inc. ("Id Technology"). Id Technology (c) 1996 Id Software, Inc.
 *   All Rights Reserved.
 *
 *   Use, distribution, and modification of this source code and/or resulting
 *   object code is restricted to non-commercial enhancements to products from
 *   Valve LLC. All other use, distribution, or modification is prohibited
 *   without written permission from Valve LLC.
 *
 * ───────────────────────────────────────────────────────────────────────────
 *   Author: Karlo Siric
 *   Purpose: Main drawing function responsible for all of the rendering
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include "r_draw.h"
#include "r_gl_platform.h"
#include "r_grid.h"
#include "r_textures.h"

#include "mdl_bodypart.h"
#include "mdl_bones.h"
#include "mdl_animations.h"
#include "mdl_bounds.h"
#include "shader.h"
#include "studio.h"
#include "util_logger.h"
#include "util_console.h"
#include "input.h"
#include "input_handler.h"
#include "math_matrix.h"
#include "math_vector.h"
#include "math_types.h"

#include <OpenGL/gltypes.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For strstr
#include <unistd.h> // For getcwd
#include <math.h>   // For cosf/sinf in orbit camera
#include <float.h>  // For FLT_MAX


static DrawRange g_ranges[MAX_DRAW_RANGES];
static int g_num_ranges = 0;

static current_model_data_t g_current;

static GLuint g_white_tex = 0;

static vec3 skinned_positions[MAXSTUDIOVERTS];
static bool have_skinned_positions = false;

GLFWwindow *window = NULL;
static bool wireframe_enabled = false;

static unsigned int bone_index_VBO = 0; // @Note: For GPU SKinning to save performance
static unsigned int VBO = 0;
static unsigned int VAO = 0;
static unsigned int EBO = 0; // Element Buffer Object for indices
static unsigned int shader_program = 0;
static unsigned int current_texture = 0; // Currently bound texture

// ═══════════════════════════════════════════════════════════════════════════
// PERFORMANCE OPTIMIZATION: Cached Uniform Locations
// ═══════════════════════════════════════════════════════════════════════════
// These are looked up ONCE after shader compilation instead of every frame.
// Before: glGetUniformLocation() called 4× per draw call × 29 draws = 116 lookups/frame
// After:  Cached lookups used directly = 0 lookups/frame
// Performance gain: ~116 string comparisons eliminated per frame!
static GLint u_model_loc = -1;
static GLint u_view_loc = -1;
static GLint u_proj_loc = -1;
static GLint u_tex_loc = -1;
static GLint u_lightPos_loc = -1;
static GLint u_viewPos_loc = -1;
static GLint u_masked_loc = -1;
static GLint u_fullbright_loc = -1;
static GLint u_additive_loc = -1;
static GLint u_chrome_loc = -1;
static GLint u_boneMatrices_loc = -1;

extern float rotation_x;
extern float rotation_y;
extern float zoom;

static bool bone_system_initialized = false;
static bool t_pose_bones_calculated = false; // Track if T-pose bones need recalculation

// PRE-ALLOCATED BUFFERS (NO MALLOC IN RENDER LOOP)
#define MAX_RENDER_VERTICES 32768
static float render_vertex_buffer[MAX_RENDER_VERTICES * 8]; // 3 pos + 3 normal + 2 uv
static int render_bone_indices[MAX_RENDER_VERTICES]; // new for the GPU Skinning
static int total_render_vertices = 0;
static bool model_processed = false;

static studiohdr_t *global_header = NULL;
static unsigned char *global_data = NULL;

static studiohdr_t *global_tex_header = NULL;
static unsigned char *global_tex_data = NULL;

static mdl_texture_set_t g_textures = { NULL, 0 };

// ANIMATIONS
mdl_animation_state_t g_anim_state;
static bool g_animation_enabled = false;
static double g_last_frame_time = 0.0;

// SEQGROUPS -- > newly added for testing animations
static mdl_seqgroup_blob_t *global_seqgroups = NULL;
static int global_num_seqgroups = 0;

// Camera controls - Fixed default position (like MilkShape 3D), never auto-adjusts
float rotation_x = 0.25f;   // ~14° tilt down - subtle elevation for good view
float rotation_y = 0.0f;    // Straight front view - model faces camera directly
float zoom = 0.15f;         // Camera distance ~33 units - shows full model in viewport

// Model rotation (separate from camera rotation, used when Shift is held)
static float model_rotation_x = 0.0f;
static float model_rotation_y = 0.0f;

// Camera mode: true = orbit camera (default), false = rotate model
static bool camera_orbit_mode = true;

// Skin family (texture set selection)
static int g_current_skin_family = 0;

// Model bounding box calculated from actual vertex data
static mdl_bounds_t g_model_bounds = { 0 };

// Model scale (you can tweak this)
static const float g_model_scale = 0.1f;

// Y offset so model's feet sit on the ground after scaling
static float g_model_ground_y = 0.0f;  // For animated sequences
static float g_model_ground_y_tpose = 0.0f;  // For T-pose

// Helper function to check if a sequence is available (has loaded sequence group data)
static bool is_sequence_available( int seq_index ) {
	if ( !global_header || !global_data || seq_index < 0 || seq_index >= global_header->numseq ) {
		return false;
	}

	mstudioseqdesc_t *sequences = (mstudioseqdesc_t *)( global_data + global_header->seqindex );
	mstudioseqdesc_t *seq = &sequences[seq_index];
	int seqgroup = seq->seqgroup;

	// Sequence group 0 is always available (embedded in main file)
	if ( seqgroup == 0 ) {
		return true;
	}

	// Check if external sequence group is loaded
	if ( !global_seqgroups || seqgroup >= global_num_seqgroups ) {
		return false;
	}

	return ( global_seqgroups[seqgroup].data != NULL );
}

static void glfw_error_callback( int error, const char *description ) {
	fprintf( stderr, "GLFW ERROR %d: %s\n", error, description );
}

float vertices[] = {
	-0.5f,
	-0.5f,
	0.0f, // Bottom left

	0.5f,
	-0.5f,
	0.0f, // Bottom right

	0.0f,
	0.5f,
	0.0f // Top center
};

// DIAGNOSTIC FUNCTION TO UNDERSTAND THE MODEL DATA
void dump_complete_mdl_structure( void ) {
	if ( !global_header || !global_data )
		return;

	printf( "\n===============================================\n" );
	printf( "COMPLETE MDL STRUCTURE DIAGNOSTIC\n" );
	printf( "=================================================\n" );

	// 1. Header info
	printf( "\n1. HEADER INFO:\n" );
	printf( "   Model name: %s\n", global_header->name );
	printf( "   File size: %d bytes\n", global_header->length );
	printf(
		"   Bounding box: (%.2f,%.2f,%.2f) to (%.2f,%.2f,%.2f)\n",
		global_header->bbmin[0],
		global_header->bbmin[1],
		global_header->bbmin[2],
		global_header->bbmax[0],
		global_header->bbmax[1],
		global_header->bbmax[2] );
	printf(
		"   BBox dimensions: %.2f x %.2f x %.2f\n",
		global_header->bbmax[0] - global_header->bbmin[0],
		global_header->bbmax[1] - global_header->bbmin[1],
		global_header->bbmax[2] - global_header->bbmin[2] );

	// 2. Bodyparts
	printf( "\n2. BODYPARTS (%d total):\n", global_header->numbodyparts );
	mstudiobodyparts_t *bodyparts = (mstudiobodyparts_t *)( global_data + global_header->bodypartindex );

	for ( int bp = 0; bp < global_header->numbodyparts && bp < 2; bp++ ) {
		printf( "\n   Bodypart %d: '%s'\n", bp, bodyparts[bp].name );
		printf( "     Models: %d\n", bodyparts[bp].nummodels );

		mstudiomodel_t *models = (mstudiomodel_t *)( global_data + bodyparts[bp].modelindex );
		mstudiomodel_t *model = &models[0];
		printf( "      Model 0: '%s'\n", model->name );
		printf( "         Vertices: %d at offset 0x%X\n", model->numverts, model->vertindex );

		// Check actual vertex data
		vec3_t *verts = (vec3_t *)( global_data + model->vertindex );

		// Find min/max of raw vertices
		float min_x = 10000, max_x = -10000;
		float min_y = 10000, max_y = -10000;
		float min_z = 10000, max_z = -10000;

		for ( int v = 0; v < model->numverts; v++ ) {
			if ( verts[v][0] < min_x )
				min_x = verts[v][0];
			if ( verts[v][0] > max_x )
				max_x = verts[v][0];
			if ( verts[v][1] < min_y )
				min_y = verts[v][1];
			if ( verts[v][1] > max_y )
				max_y = verts[v][1];
			if ( verts[v][2] < min_z )
				min_z = verts[v][2];
			if ( verts[v][2] > max_z )
				max_z = verts[v][2];
		}

		printf( "         RAW Vertex bounds:\n" );
		printf( "            X: %.3f to %.3f (width: %.3f)\n", min_x, max_x, max_x - min_x );
		printf( "            Y: %.3f to %.3f (depth: %.3f)\n", min_y, max_y, max_y - min_y );
		printf( "            Z: %.3f to %.3f (height: %.3f)\n", min_z, max_z, max_z - min_z );

		// Show first few vertices
		printf( "         First 3 vertices (RAW from file):\n" );
		for ( int v = 0; v < 3 && v < model->numverts; v++ ) {
			printf( "            V%d: (%.6f, %.6f, %.6f)\n", v, verts[v][0], verts[v][1], verts[v][2] );
		}

		// Check hex data
		unsigned char *as_bytes = (unsigned char *)( global_data + model->vertindex );
		printf( "         First 12 bytes at vertex offset: " );
		for ( int i = 0; i < 12; i++ ) {
			printf( "%02X ", as_bytes[i] );
		}
		printf( "\n" );
	}

	printf( "\n===============================================\n" );
}

void UpdateBonesForCurrentFrame( void ) {
	if ( !global_header || !global_data ) {
		return;
	}

	if ( g_animation_enabled && global_header->numseq > 0 ) {
		// Calculate animated bone transforms directly into g_bonetransformations
		mdl_animation_calculate_bones(
			&g_anim_state, global_header, global_data, global_seqgroups, g_bonetransformations );
	} else {
		// No animation - use static T-pose
		SetUpBones( global_header, global_data );
	}

	// Re-transform ALL vertices with updated bones
	mstudiobodyparts_t *bodyparts = (mstudiobodyparts_t *)( global_data + global_header->bodypartindex );

	for ( int bp = 0; bp < global_header->numbodyparts; ++bp ) {
		mstudiobodyparts_t *bpRec = &bodyparts[bp];
		mstudiomodel_t *models = (mstudiomodel_t *)( global_data + bpRec->modelindex );
		int selected_model_index = bodypart_get_model_index( bp );

		if ( selected_model_index < 0 || selected_model_index >= bpRec->nummodels ) {
			selected_model_index = 0;
		}

		mstudiomodel_t *model = &models[selected_model_index];

		// Transform vertices with updated bone matrices
		TransformVertices( global_header, global_data, model, skinned_positions );
		have_skinned_positions = true;
	}
}

// Updated ProcessModelForRendering to extract normals and UVs
void ProcessModelForRendering( void ) {
	LOG_INFOF( "renderer", "Processing model for rendering" );

	if ( !global_header || !global_data ) {
		LOG_FATALF(
			"renderer",
			"FATAL - Cannot process NULL model data! header=%p data=%p",
			(void *)global_header,
			(void *)global_data );
		fprintf( stderr, "ERROR - Invalid argument pointers value passed!\n" );
		return;
	}

	total_render_vertices = 0;
	g_num_ranges = 0;

	mstudiobodyparts_t *bodyparts = (mstudiobodyparts_t *)( global_data + global_header->bodypartindex );

	// Set up T-pose bones initially (animations update these per-frame)
	SetUpBones( global_header, global_data );

	// Iterate through all bodyparts
	for ( int bp = 0; bp < global_header->numbodyparts; ++bp ) {
		mstudiobodyparts_t *bpRec = &bodyparts[bp];
		mstudiomodel_t *models = (mstudiomodel_t *)( global_data + bpRec->modelindex );

		// GET ONLY THE SELECTED MODEL FOR THIS BODYPART
		int selected_model_index = bodypart_get_model_index( bp );

		// Safety check
		if ( selected_model_index < 0 || selected_model_index >= bpRec->nummodels ) {
			LOG_WARNF(
				"renderer",
				"Invalid model index %d for bodypart '%s' (max: %d), using 0",
				selected_model_index, bpRec->name,
				bpRec->nummodels - 1 );
			selected_model_index = 0; // Fallback to first model
		}

		// Get ONLY the selected model (not all of them!)
		mstudiomodel_t *model = &models[selected_model_index];

		g_current.model = model;
		g_current.vertices = (vec3_t *)( global_data + model->vertindex );
		g_current.normals = (vec3_t *)( global_data + model->normindex );
		g_current.vertex_count = model->numverts;
		g_current.normal_count = model->numnorms;

		// Skin this model's vertices (fills skinned_positions[])
		TransformVertices( global_header, global_data, model, skinned_positions );
		have_skinned_positions = true;

		// All meshes for this model
		mstudiomesh_t *meshes = (mstudiomesh_t *)( global_data + model->meshindex );

		// Skin table
		const short *skin_table = (const short *)( global_data + global_header->skinindex );
		const int numskinref = global_header->numskinref;

		for ( int mesh = 0; mesh < model->nummesh; ++mesh ) {
			const int norm_base = meshes[mesh].normindex;
			const int v_count = g_current.vertex_count;
			const int n_count = g_current.normal_count;

			// Resolve texture index via skin table
			int tex_index = meshes[mesh].skinref;
			if ( skin_table && numskinref > 0 && tex_index >= 0 && tex_index < numskinref ) {
				tex_index = skin_table[g_current_skin_family * numskinref + tex_index];
			}

			// GL texture + size + flags (PERFORMANCE: get flags here, not in render loop!)
			GLuint gl_tex = 0;
			int texW = 1, texH = 1;
			int tex_flags = 0; // STUDIO_NF_* flags
			if ( tex_index >= 0 && tex_index < g_textures.count ) {
				gl_tex = g_textures.textures[tex_index].gl_id;
				texW = g_textures.textures[tex_index].width;
				texH = g_textures.textures[tex_index].height;
				tex_flags = g_textures.textures[tex_index].flags; // Cache flags!

				if ( texW <= 0 )
					texW = 1;
				if ( texH <= 0 )
					texH = 1;
			}
			if ( !gl_tex && g_white_tex ) {
				gl_tex = g_white_tex;
				texW = 2;
				texH = 2;
			}

			short *ptricmds = (short *)( global_data + meshes[mesh].triindex );

			const int start_first = total_render_vertices;

			int s_min = 32767, s_max = -32768;
			int t_min = 32767, t_max = -32768;

			int i;
			while ( ( i = *( ptricmds++ ) ) ) {
				if ( i < 0 ) {
					// -------- triangle fan --------
					i = -i;

					// read first 2 vertices
					short v0 = ptricmds[0];
					short n0 = ptricmds[1];
					short s0 = ptricmds[2];
					short t0 = ptricmds[3];
					ptricmds = (short *)( (char *)ptricmds + 4 * sizeof( short ) );

					short v1 = ptricmds[0];
					short n1 = ptricmds[1];
					short s1 = ptricmds[2];
					short t1 = ptricmds[3];
					ptricmds = (short *)( (char *)ptricmds + 4 * sizeof( short ) );

					// >>> FIX: apply ON-SEAM rule and clear high bit on normal index
					if ( n0 & 0x8000 ) {
						s0 = (short)( s0 + texW / 2 );
					}
					n0 &= 0x7FFF;
					if ( n1 & 0x8000 ) {
						s1 = (short)( s1 + texW / 2 );
					}
					n1 &= 0x7FFF;

					// norm indices are relative to this mesh's block
					n0 = (short)( n0 + norm_base );
					n1 = (short)( n1 + norm_base );

					for ( int j = 2; j < i; ++j ) {
						short v2 = ptricmds[0];
						short n2 = ptricmds[1];
						short s2 = ptricmds[2];
						short t2 = ptricmds[3];
						ptricmds = (short *)( (char *)ptricmds + 4 * sizeof( short ) );

						// >>> FIX: apply ON-SEAM rule and clear high bit
						if ( n2 & 0x8000 ) {
							s2 = (short)( s2 + texW / 2 );
						}
						n2 &= 0x7FFF;

						// update min/max + quick sample printf
						s_min = ( s0 < s_min ) ? s0 : s_min;
						s_max = ( s0 > s_max ) ? s0 : s_max;
						t_min = ( t0 < t_min ) ? t0 : t_min;
						t_max = ( t0 > t_max ) ? t0 : t_max;

						// normals block offset
						n2 = (short)( n2 + norm_base );

						// bounds guards
						if ( v0 >= 0 && v0 < v_count && n0 >= 0 && n0 < n_count && v1 >= 0 && v1 < v_count && n1 >= 0 && n1 < n_count && v2 >= 0 && v2 < v_count && n2 >= 0 && n2 < n_count ) {
							AddVertexToBuffer( v0, n0, s0, t0, (float)texW, (float)texH );
							AddVertexToBuffer( v1, n1, s1, t1, (float)texW, (float)texH );
							AddVertexToBuffer( v2, n2, s2, t2, (float)texW, (float)texH );
						}

						// roll forward
						v1 = v2;
						n1 = n2;
						s1 = s2;
						t1 = t2;
					}
				} else {
					// -------- triangle strip --------

					// read first 2 vertices
					short v0 = ptricmds[0];
					short n0 = ptricmds[1];
					short s0 = ptricmds[2];
					short t0 = ptricmds[3];
					ptricmds = (short *)( (char *)ptricmds + 4 * sizeof( short ) );

					short v1 = ptricmds[0];
					short n1 = ptricmds[1];
					short s1 = ptricmds[2];
					short t1 = ptricmds[3];
					ptricmds = (short *)( (char *)ptricmds + 4 * sizeof( short ) );

					// >>> FIX: seam + clear high bit
					if ( n0 & 0x8000 ) {
						s0 = (short)( s0 + texW / 2 );
					}
					n0 &= 0x7FFF;
					if ( n1 & 0x8000 ) {
						s1 = (short)( s1 + texW / 2 );
					}
					n1 &= 0x7FFF;

					n0 = (short)( n0 + norm_base );
					n1 = (short)( n1 + norm_base );

					for ( int j = 2; j < i; ++j ) {
						short v2 = ptricmds[0];
						short n2 = ptricmds[1];
						short s2 = ptricmds[2];
						short t2 = ptricmds[3];
						ptricmds = (short *)( (char *)ptricmds + 4 * sizeof( short ) );

						// >>> FIX: seam + clear high bit
						if ( n2 & 0x8000 ) {
							s2 = (short)( s2 + texW / 2 );
						}
						n2 &= 0x7FFF;

						s_min = ( s0 < s_min ) ? s0 : s_min;
						s_max = ( s0 > s_max ) ? s0 : s_max;
						t_min = ( t0 < t_min ) ? t0 : t_min;
						t_max = ( t0 > t_max ) ? t0 : t_max;

						n2 = (short)( n2 + norm_base );

						// two-triangle parity
						if ( ( j - 2 ) % 2 == 0 ) {
							if ( v0 >= 0 && v0 < v_count && n0 >= 0 && n0 < n_count && v1 >= 0 && v1 < v_count && n1 >= 0 && n1 < n_count && v2 >= 0 && v2 < v_count && n2 >= 0 && n2 < n_count ) {
								AddVertexToBuffer( v0, n0, s0, t0, (float)texW, (float)texH );
								AddVertexToBuffer( v1, n1, s1, t1, (float)texW, (float)texH );
								AddVertexToBuffer( v2, n2, s2, t2, (float)texW, (float)texH );
							}
						} else {
							if ( v0 >= 0 && v0 < v_count && n0 >= 0 && n0 < n_count && v1 >= 0 && v1 < v_count && n1 >= 0 && n1 < n_count && v2 >= 0 && v2 < v_count && n2 >= 0 && n2 < n_count ) {
								AddVertexToBuffer( v1, n1, s1, t1, (float)texW, (float)texH );
								AddVertexToBuffer( v0, n0, s0, t0, (float)texW, (float)texH );
								AddVertexToBuffer( v2, n2, s2, t2, (float)texW, (float)texH );
							}
						}

						// roll forward
						v0 = v1;
						n0 = n1;
						s0 = s1;
						t0 = t1;
						v1 = v2;
						n1 = n2;
						s1 = s2;
						t1 = t2;
					}
				}
			}

			// One draw range for this mesh (PERFORMANCE: store flags to avoid search!)
			if ( g_num_ranges < MAX_DRAW_RANGES ) {
				g_ranges[g_num_ranges].tex = gl_tex;
				g_ranges[g_num_ranges].first = start_first;
				g_ranges[g_num_ranges].count = total_render_vertices - start_first;
				g_ranges[g_num_ranges].flags = tex_flags; // Store flags here!
				g_num_ranges++;
			}
		}
	}

	model_processed = true;

	if ( g_num_ranges == 0 ) {
		LOG_WARNF( "renderer", "No draw ranges created!" );
	}

	if ( total_render_vertices == 0 ) {
		LOG_WARNF( "renderer", "No vertices generated!" );
	}

	if ( total_render_vertices > MAX_RENDER_VERTICES ) {
		LOG_ERRORF(
			"renderer", "Vertex buffer overflow! %d > %d", total_render_vertices, MAX_RENDER_VERTICES );
	}

	LOG_INFOF( "renderer", "Model processed: %d vertices, %d draw calls", total_render_vertices, g_num_ranges );
}

void AddVertexToBuffer( int vertex_index, int normal_index, short s, short t, float texW, float texH ) {
	if ( total_render_vertices >= MAX_RENDER_VERTICES )
		return;

	const int base = total_render_vertices * 8;

	/* ----- POSITION ----- */
	// For GPU skinning: Upload rest-pose vertices in ORIGINAL coordinates
	// Bone transformation, scaling, and axis remapping happen in the shader
	vec3 P;
	Math_Vec3Copy( g_current.vertices[vertex_index], P );

	/* ----- NORMAL ----- */
	// For GPU skinning: Upload rest-pose normals in ORIGINAL coordinates
	unsigned char *v2bone = (unsigned char *)( global_data + g_current.model->vertinfoindex );
	int bone = v2bone ? v2bone[vertex_index] : 0;
	if ( bone < 0 || bone >= global_header->numbones )
		bone = 0;

	vec3 N;
	Math_Vec3Copy( g_current.normals[normal_index], N );

	/* Write position and normal (no transforms applied here!) */
	render_vertex_buffer[base + 0] = P[0];
	render_vertex_buffer[base + 1] = P[1];
	render_vertex_buffer[base + 2] = P[2];

	render_vertex_buffer[base + 3] = N[0];
	render_vertex_buffer[base + 4] = N[1];
	render_vertex_buffer[base + 5] = N[2];

	/* s,t are 16-bit *texel* coords for THIS texture */
	float u = ( (float)s + 0.5f ) / (float)texW;
	float v = ( (float)t + 0.5f ) / (float)texH;

	// v = 1.0f - v;

	/* Optional safety clamp */
	if ( u < 0.0f )
		u = 0.0f;
	else if ( u > 1.0f )
		u = 1.0f;
	if ( v < 0.0f )
		v = 0.0f;
	else if ( v > 1.0f )
		v = 1.0f;

	render_vertex_buffer[base + 6] = u;
	render_vertex_buffer[base + 7] = v;

	render_bone_indices[total_render_vertices] = bone;

	total_render_vertices++;
}

void setup_triangle( void ) {
	glGenBuffers( 1, &VBO );
	glGenBuffers( 1, &bone_index_VBO );
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

	glGenVertexArrays( 1, &VAO );
	glBindVertexArray( VAO );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ), (void *)0 );
	glEnableVertexAttribArray( 0 );
}

char *read_shader_source( const char *filename ) {
	char fullpath[512];
	snprintf( fullpath, sizeof( fullpath ), "%s/%s", SHADER_DIR, filename );

	FILE *file = fopen( fullpath, "rb" );
	if ( !file ) {
		fprintf( stderr, "ERROR - Failed to open the shader file: '%s' \n", fullpath );
		return NULL;
	}

	fseek( file, 0, SEEK_END );
	size_t length = ftell( file );
	rewind( file );

	char *buffer = malloc( ( length + 1 ) * sizeof( char ) );

	if ( !buffer ) {
		fprintf( stderr, "ERROR - Failed to allocate memory for shader file buffer!\n" );
		fclose( file );
		return NULL;
	}

	fread( buffer, 1, length, file );
	buffer[length] = '\0'; // Null terminate
	fclose( file );

	return buffer;
}

GLuint compile_shader( const char *source, GLenum type ) {
	GLuint shader = glCreateShader( type );
	glShaderSource( shader, 1, &source, NULL );
	glCompileShader( shader );

	GLint success;
	glGetShaderiv( shader, GL_COMPILE_STATUS, &success );

	if ( !success ) {
		char shader_info[512];
		glGetShaderInfoLog( shader, 512, NULL, shader_info );
		fprintf( stderr, "ERROR - Failed to compile shaders: \n%s\n", shader_info );
		return ( 0 );
	}

	return shader;
}

GLuint create_shader_program( GLuint vertexShader, GLuint fragmentShader ) {
	GLuint program = glCreateProgram();
	glAttachShader( program, vertexShader );
	glAttachShader( program, fragmentShader );
	glLinkProgram( program );

	GLint success;
	glGetProgramiv( program, GL_LINK_STATUS,
					&success ); // Use glGetProgramiv for program
	if ( !success ) {
		char infoLog[512];
		glGetProgramInfoLog( program, 512, NULL, infoLog );
		fprintf( stderr, "ERROR - Failed to link shaders to the program!\n%s\n", infoLog );
		return ( 0 );
	}

	glDeleteShader( vertexShader );
	glDeleteShader( fragmentShader );

	return program;
}

int load_shaders( void ) {
	char *vertex_shader_file = read_shader_source( "textured.vert" );

	char *fragment_shader_file = read_shader_source( "textured.frag" );

	if ( !vertex_shader_file || !fragment_shader_file ) {
		if ( vertex_shader_file )
			free( vertex_shader_file );
		if ( fragment_shader_file )
			free( fragment_shader_file );

		vertex_shader_file = read_shader_source( "basic.vert" );
		fragment_shader_file = read_shader_source( "basic.frag" );
	}

	if ( !vertex_shader_file || !fragment_shader_file ) {
		fprintf( stderr, "ERROR - Failed to load shader files!\n" );
		if ( vertex_shader_file )
			free( vertex_shader_file );
		if ( fragment_shader_file )
			free( fragment_shader_file );
		return ( -1 );
	}
	GLuint vertexShader = compile_shader( vertex_shader_file, GL_VERTEX_SHADER );
	GLuint fragmentShader = compile_shader( fragment_shader_file, GL_FRAGMENT_SHADER );

	free( vertex_shader_file );
	free( fragment_shader_file );

	if ( vertexShader == 0 || fragmentShader == 0 ) {
		return ( -1 );
	}

	shader_program = create_shader_program( vertexShader, fragmentShader );

	if ( shader_program == 0 ) {
		fprintf( stderr, "ERROR - Failed to create properly a shader program!\n" );
		return ( -1 );
	}

	// PERFORMANCE: Cache all uniform locations once (not every frame!)
	u_model_loc = glGetUniformLocation( shader_program, "model" );
	u_view_loc = glGetUniformLocation( shader_program, "view" );
	u_proj_loc = glGetUniformLocation( shader_program, "projection" );
	u_tex_loc = glGetUniformLocation( shader_program, "tex" );
	u_lightPos_loc = glGetUniformLocation( shader_program, "lightPos" );
	u_viewPos_loc = glGetUniformLocation( shader_program, "viewPos" );
	u_masked_loc = glGetUniformLocation( shader_program, "u_masked" );
	u_fullbright_loc = glGetUniformLocation( shader_program, "u_fullbright" );
	u_additive_loc = glGetUniformLocation( shader_program, "u_additive" );
	u_chrome_loc = glGetUniformLocation( shader_program, "u_chrome" );
	u_boneMatrices_loc = glGetUniformLocation( shader_program, "boneMatrices" );

	return ( 0 );
}

int init_renderer( int width, int height, const char *title ) {
	LOG_INFOF( "renderer", "Initializing renderer: %dx%d", width, height );

	if ( !glfwInit() ) {
		LOG_FATALF( "renderer", "Failed to initialize GLFW" );
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

	LOG_DEBUGF( "renderer", "GLFW initialized successfully!\n" );

// ═══════════════════════════════════════════════════════════════
// Platform-specific OpenGL version hints
// ═══════════════════════════════════════════════════════════════
#ifdef __APPLE__
	/* macOS: Limited to OpenGL 4.1 Core Profile */
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
	LOG_DEBUGF( "renderer", "Platform: macOS - OpenGL 4.1 Core Profile" );
#elif defined( _WIN32 )
	/* Windows: Can use latest OpenGL version */
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 5 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	LOG_DEBUGF( "renderer", "Platform: Windows - OpenGL 4.5 Core Profile" );
#else
	/* Linux: Can usually support OpenGL 4.5+ */
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 5 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	LOG_DEBUGF( "renderer", "Platform: Linux - OpenGL 4.5 Core Profile" );
#endif

	GLFWmonitor *primary = glfwGetPrimaryMonitor();
	(void)primary; // Reserved for future fullscreen support

	window = glfwCreateWindow( width, height, title, NULL, NULL );

	if ( !window ) {
		LOG_FATALF( "renderer", "Failed to create GLFW window" );
		fprintf( stderr, "Failed to create GLFW window\n" );
		glfwTerminate();
		return -1;
	}

	LOG_DEBUGF( "renderer", "Window created successfully!\n" );

	// Make the OpenGL context current
	glfwMakeContextCurrent( window );

// ═══════════════════════════════════════════════════════════════
// CRITICAL: Initialize GLEW on Linux/Windows
// macOS doesn't need GLEW - it uses native OpenGL framework
// ═══════════════════════════════════════════════════════════════
#if GLEW_REQUIRED
	LOG_INFOF( "renderer", "Initializing GLEW (required on this platform)..." );

	// Enable experimental features for modern OpenGL
	glewExperimental = GL_TRUE;

	GLenum glew_err = glewInit(); // ← FIXED: Was glfwInit()!

	if ( glew_err != GLEW_OK ) {
		LOG_FATALF( "renderer", "Failed to initialize GLEW: %s", glewGetErrorString( glew_err ) );
		fprintf( stderr, "GLEW Error: %s\n", glewGetErrorString( glew_err ) );
		glfwDestroyWindow( window );
		glfwTerminate();
		return -1;
	}

	LOG_INFOF( "renderer", "GLEW initialized successfully" );
	LOG_DEBUGF( "renderer", "GLEW version: %s", glewGetString( GLEW_VERSION ) );

	// GLEW sometimes sets a spurious GL_INVALID_ENUM error during init
	// Clear it so it doesn't interfere with later error checking
	glGetError();

#else
	LOG_DEBUGF( "renderer", "GLEW not required (macOS native OpenGL)" );
#endif

	// ═══════════════════════════════════════════════════════════════
	// Query OpenGL information (now safe on all platforms)
	// ═══════════════════════════════════════════════════════════════
	const char *gl_version = (const char *)glGetString( GL_VERSION );
	const char *gl_vendor = (const char *)glGetString( GL_VENDOR );
	const char *gl_renderer = (const char *)glGetString( GL_RENDERER );
	const char *glsl_version = (const char *)glGetString( GL_SHADING_LANGUAGE_VERSION );

	if ( !gl_version || !glsl_version ) {
		LOG_FATALF( "renderer", "Failed to query OpenGL version - context may not be current" );
		fprintf( stderr, "ERROR: OpenGL context is invalid\n" );
		return -1;
	}

	LOG_INFOF( "renderer", "OpenGL Version: %s", gl_version );
	LOG_INFOF( "renderer", "OpenGL Vendor: %s", gl_vendor );
	LOG_INFOF( "renderer", "OpenGL Renderer: %s", gl_renderer );
	LOG_INFOF( "renderer", "GLSL Version: %s", glsl_version );

	// User-facing console output
	CONSOLE_INFO( "OpenGL %s", gl_version );
	CONSOLE_INFO( "GPU: %s", gl_renderer );
	CONSOLE_INFO( "GLSL: %s", glsl_version );

	// ═══════════════════════════════════════════════════════════════
	// Setup GLFW callbacks
	// ═══════════════════════════════════════════════════════════════
	glfwSetErrorCallback( glfw_error_callback );

	// ═══════════════════════════════════════════════════════════════
	// OpenGL state setup
	// ═══════════════════════════════════════════════════════════════
	glEnable( GL_DEPTH_TEST );
	glViewport( 0, 0, width, height );

	// Culling disabled to see if triangles are backwards
	glDisable( GL_CULL_FACE );

	// Enable point size for vertex visualization
	glEnable( GL_PROGRAM_POINT_SIZE );
	glPointSize( 5.0f );

	// ═══════════════════════════════════════════════════════════════
	// Initialize scene geometry and shaders
	// ═══════════════════════════════════════════════════════════════
	setup_triangle();

	if ( load_shaders() != 0 ) {
		LOG_FATALF( "renderer", "Failed to load shaders" );
		fprintf( stderr, "ERROR - Failed to load shaders!\n" );
		return -1;
	}

	// @Note(Karlo): Initializing the grid, ground plane, and origin axes
	r_grid_init( 200.0f, 5.0f );
	r_ground_init( 200.0f );
	r_axes_init( 20.0f );  // Axes extend 20 units from origin

	// ═══════════════════════════════════════════════════════════════
	// Create fallback white texture (so meshes always draw)
	// ═══════════════════════════════════════════════════════════════
	glGenTextures( 1, &g_white_tex );
	glBindTexture( GL_TEXTURE_2D, g_white_tex );

	unsigned char white[] = { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 };

	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, white );
	glBindTexture( GL_TEXTURE_2D, 0 );

	// ═══════════════════════════════════════════════════════════════
	// Print controls help
	// ═══════════════════════════════════════════════════════════════
	printf( "\n\n" );
	printf( "\n\n" );
	printf( "╔════════════════════════════════════╗\n" );
	printf( "║         MODEL VIEWER CONTROLS      ║\n" );
	printf( "╠════════════════════════════════════╣\n" );
	printf( "║ AXIS ORIENTATION                   ║\n" );
	printf( "║   X Axis: RED   (Right/Left)       ║\n" );
	printf( "║   Y Axis: GREEN (Up/Down)          ║\n" );
	printf( "║   Z Axis: BLUE  (Forward/Back)     ║\n" );
	printf( "╠════════════════════════════════════╣\n" );
	printf( "║ CAMERA CONTROLS                    ║\n" );
	printf( "║   W/S        : Tilt up/down        ║\n" );
	printf( "║   A/D        : Rotate left/right   ║\n" );
	printf( "║   Q/E        : Zoom in/out         ║\n" );
	printf( "║   Mouse Drag : Free rotation       ║\n" );
	printf( "║   Scroll     : Zoom                ║\n" );
	printf( "║   R          : Reset view          ║\n" );
	printf( "║   Shift+Move : Rotate model        ║\n" );
	printf( "╠════════════════════════════════════╣\n" );
	printf( "║ RENDER MODES                       ║\n" );
	printf( "║   F          : Toggle wireframe    ║\n" );
	printf( "║   P          : Points mode         ║\n" );
	printf( "║   G          : Toggle grid         ║\n" );
	printf( "╠════════════════════════════════════╣\n" );
	printf( "║ ANIMATION CONTROLS                 ║\n" );
	printf( "║   SPACE      : Toggle animation    ║\n" );
	printf( "║   LEFT/RIGHT : Prev/Next sequence  ║\n" );
	printf( "║   UP/DOWN    : Speed up/slow down  ║\n" );
	printf( "║   L          : Toggle looping      ║\n" );
	printf( "║   0          : Reset to frame 0    ║\n" );
	printf( "║   I          : Show anim info      ║\n" );
	printf( "╠════════════════════════════════════╣\n" );
	printf( "║   ESC        : Exit                ║\n" );
	printf( "╚════════════════════════════════════╝\n\n" );

	LOG_INFOF( "renderer", "Renderer initialized successfully" );

	return 0;
}

void cleanup_renderer( void ) {
	// FIrst we call the input cleaner
	Input_Shutdown();

	if ( VAO )
		glDeleteVertexArrays( 1, &VAO );
	if ( VBO )
		glDeleteBuffers( 1, &VBO );
	if ( EBO )
		glDeleteBuffers( 1, &EBO );
	if ( shader_program )
		glDeleteProgram( shader_program );
	if ( window ) {
		glfwDestroyWindow( window );
	}

	glfwTerminate();
	return;
}

void clear_screen( void ) {
	// Light cyan/blue background (professional model viewer style)
	glClearColor( 0.45f, 0.55f, 0.60f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
}

bool should_close_window( void ) {
	return glfwWindowShouldClose( window );
}

void render_loop( void ) {
	LOG_INFOF( "renderer", "Entering render loop" );

	double last_time = glfwGetTime();
	int frame_count = 0;

	g_last_frame_time = glfwGetTime(); // Initialize to current time

	while ( !glfwWindowShouldClose( window ) ) {
		// Calculate delta time
		double current_time = glfwGetTime();
		float delta_time = (float)( current_time - g_last_frame_time );
		g_last_frame_time = current_time;

		if ( delta_time > 0.033f ) {
			delta_time = 0.033f;
		}

		// FIX #4: Also handle negative deltas (clock skew)
		if ( delta_time < 0.0f ) {
			delta_time = 0.0f;
		}

		// Update animation state
		if ( g_animation_enabled && global_header && global_data ) {
			mdl_animation_update( &g_anim_state, delta_time, global_header, global_data, global_seqgroups );
		}

		// Clear and render
		clear_screen();

		// Always render grid and axes (even without a model)
		{
			// Get framebuffer size for aspect ratio
			int fbw, fbh;
			glfwGetFramebufferSize( window, &fbw, &fbh );
			float aspect = ( fbh > 0 ) ? (float)fbw / (float)fbh : 1.0f;

			// Calculate camera position from rotation angles
			float camDist = 5.0f / ( zoom > 0.001f ? zoom : 0.001f );
			vec3 camPos;
			vec3 target = { 0.0f, 0.0f, 0.0f };
			vec3 up = { 0.0f, 1.0f, 0.0f };

			float pitch = rotation_x;
			float yaw = rotation_y;

			// Convert spherical coordinates to cartesian
			camPos[0] = camDist * cosf( pitch ) * sinf( yaw );
			camPos[1] = camDist * sinf( pitch );
			camPos[2] = camDist * cosf( pitch ) * cosf( yaw );

			// Clamp camera Y to prevent going below ground
			float min_camera_height = 0.5f;
			if ( camPos[1] < min_camera_height ) {
				camPos[1] = min_camera_height;
			}

			// Create view and projection matrices
			mat4 V, P;
			Math_Mat4_LookAt( camPos, target, up, V );
			Math_Mat4_Perspective( 50.0f * MATH_DEG2RAD, aspect, 0.01f, 1000.0f, P );

			// Draw grid and axes at Y=0
			float ground_y = 0.0f;
			r_grid_draw( V, P, ground_y );
			r_axes_draw( V, P, ground_y );

			// Render model if loaded
			if ( global_header && global_data ) {
				// Identity model matrix (load at origin)
				mat4 M;
				Math_Mat4_Identity( M );

				// GROUND ALIGNMENT - Use different offsets for T-pose vs animation
				float ground_offset_y = g_animation_enabled ? g_model_ground_y : g_model_ground_y_tpose;

				// 1) Translate model so feet land on ground
				glm_translate( M, (vec3){ 0.0f, ground_offset_y, 0.0f } );

				// 2) Apply user rotations
				mat4 Rx = GLM_MAT4_IDENTITY_INIT;
				glm_rotate( Rx, model_rotation_x, (vec3){ 1, 0, 0 } );
				glm_mat4_mul( M, Rx, M );

				mat4 Ry = GLM_MAT4_IDENTITY_INIT;
				glm_rotate( Ry, model_rotation_y, (vec3){ 0, 1, 0 } );
				glm_mat4_mul( M, Ry, M );

				// 3) Rotate model to face camera (blue Z axis)
				// Half-Life models face +Y after axis remap, need -90° Y rotation to face +Z
				mat4 RyFace = GLM_MAT4_IDENTITY_INIT;
				glm_rotate( RyFace, -MATH_PI * 0.5f, (vec3){ 0, 1, 0 } );
				glm_mat4_mul( M, RyFace, M );

				// 4) Scale LAST because HL units are large
				mat4 S = GLM_MAT4_IDENTITY_INIT;
				glm_scale( S, (vec3){ g_model_scale, g_model_scale, g_model_scale } );
				glm_mat4_mul( M, S, M );

				// Draw the model
				render_model_with_matrices( V, P, M );
			}
		}

		glfwSwapBuffers( window );
		glfwPollEvents();

		// ═══════════════════════════════════════════════════════════
		// Process all input BEFORE updating (so previous != current)
		// ═══════════════════════════════════════════════════════════
		input_camera_state_t cam_state = {
			.rotation_x = &rotation_x,
			.rotation_y = &rotation_y,
			.zoom = &zoom,
			.wireframe_enabled = &wireframe_enabled,
			.model_rotation_x = get_model_rotation_x_ptr(),
			.model_rotation_y = get_model_rotation_y_ptr()
		};

		input_animation_state_t anim_state = {
			.animation_enabled = &g_animation_enabled,
			.anim_state = &g_anim_state,
			.header = global_header,
			.data = global_data,
			.seqgroups = global_seqgroups,
			.num_seqgroups = global_num_seqgroups,
			.model_processed = &model_processed
		};

		Input_ProcessGameInput( window, &cam_state, &anim_state );

		// Update AFTER processing (copy current → previous for next frame)
		Input_Update();

		frame_count++;
	}

	LOG_INFOF( "renderer", "Exiting render loop after %d frames", frame_count );
}

void set_wireframe_mode( bool enabled ) {
	wireframe_enabled = enabled; // Now it's used

	if ( wireframe_enabled ) {
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	} else {
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	}
}

void set_current_texture( unsigned int texture_id ) {
	current_texture = texture_id;
}

// ───────────────────────────────────────────────────────────────────────────
// Layer 4: Full rendering with camera (CLI version uses this)
// ───────────────────────────────────────────────────────────────────────────
void render_model( studiohdr_t *header, unsigned char *data ) {
	(void)header;
	(void)data;

	// Get framebuffer size for aspect ratio
	int fbw, fbh;
	glfwGetFramebufferSize( window, &fbw, &fbh );
	float aspect = ( fbh > 0 ) ? (float)fbw / (float)fbh : 1.0f;

	// Build model matrix following Sam Vanheer's approach
	// Transform order: Scale → Face camera → User rotations → Translate
	mat4 M;
	Math_Mat4_Identity( M );

	// GROUND ALIGNMENT - Use different offsets for T-pose vs animation
	float ground_offset_y = g_animation_enabled ? g_model_ground_y : g_model_ground_y_tpose;

	// 1) Translate model so feet land on ground
	glm_translate( M, (vec3){ 0.0f, ground_offset_y, 0.0f } );

	// 2) Apply user rotations
	mat4 Rx = GLM_MAT4_IDENTITY_INIT;
	glm_rotate( Rx, model_rotation_x, (vec3){ 1, 0, 0 } );
	glm_mat4_mul( M, Rx, M );

	mat4 Ry = GLM_MAT4_IDENTITY_INIT;
	glm_rotate( Ry, model_rotation_y, (vec3){ 0, 1, 0 } );
	glm_mat4_mul( M, Ry, M );

	// 3) Rotate model to face camera (blue Z axis)
	// Half-Life models face +Y after axis remap, need -90° Y rotation to face +Z
	mat4 RyFace = GLM_MAT4_IDENTITY_INIT;
	glm_rotate( RyFace, -MATH_PI * 0.5f, (vec3){ 0, 1, 0 } );
	glm_mat4_mul( M, RyFace, M );

	// 4) Scale LAST because HL units are large
	mat4 S = GLM_MAT4_IDENTITY_INIT;
	glm_scale( S, (vec3){ g_model_scale, g_model_scale, g_model_scale } );
	glm_mat4_mul( M, S, M );

	// Calculate camera position from rotation angles
	float camDist = 5.0f / ( zoom > 0.001f ? zoom : 0.001f );
	vec3 camPos;
	vec3 target = { 0.0f, 0.0f, 0.0f };  // Always look at origin
	vec3 up = { 0.0f, 1.0f, 0.0f };

	float pitch = rotation_x;
	float yaw = rotation_y;

	// Convert spherical coordinates to cartesian
	camPos[0] = camDist * cosf( pitch ) * sinf( yaw );
	camPos[1] = camDist * sinf( pitch );
	camPos[2] = camDist * cosf( pitch ) * cosf( yaw );

	// Clamp camera Y to prevent going below ground
	float min_camera_height = 0.5f;
	if ( camPos[1] < min_camera_height ) {
		camPos[1] = min_camera_height;
	}

	// Create view and projection matrices
	mat4 V;
	Math_Mat4_LookAt( camPos, target, up, V );
	mat4 P;
	Math_Mat4_Perspective( 50.0f * MATH_DEG2RAD, aspect, 0.01f, 1000.0f, P );

	// Render the full scene (Layer 3)
	render_scene( V, P, M );
}

// ═══════════════════════════════════════════════════════════════════════════
// LAYERED RENDERING API
// ═══════════════════════════════════════════════════════════════════════════
// These functions provide a clean separation between:
// - Pure OpenGL drawing (Layer 1)
// - Model rendering with custom matrices (Layer 2)
// - Scene rendering with grid/axes (Layer 3)
// - Full rendering with camera (Layer 4)
// ═══════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────
// Layer 1: Pure OpenGL drawing (lowest level - just draw triangles)
// ───────────────────────────────────────────────────────────────────────────
void draw_model_geometry( void ) {
	if ( total_render_vertices == 0 ) {
		return;
	}

	glBindVertexArray( VAO );
	glBindBuffer( GL_ARRAY_BUFFER, VBO );

	// Set up vertex attributes
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof( float ), (void *)( 0 ) );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof( float ), (void *)( 3 * sizeof( float ) ) );
	glEnableVertexAttribArray( 1 );
	glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof( float ), (void *)( 6 * sizeof( float ) ) );
	glEnableVertexAttribArray( 2 );

	// Bone index attribute for GPU skinning
	glBindBuffer( GL_ARRAY_BUFFER, bone_index_VBO );
	glVertexAttribIPointer( 3, 1, GL_INT, sizeof( int ), (void *)0 );
	glEnableVertexAttribArray( 3 );

	// PERFORMANCE: Use cached uniform location
	if ( u_tex_loc != -1 ) {
		glUniform1i( u_tex_loc, 0 );
	}

	// PERFORMANCE: Track GL state to avoid redundant calls
	static bool blend_enabled = false;

	// Draw each texture range (single pass like CLI version)
	for ( int r = 0; r < g_num_ranges; ++r ) {
		GLuint tex_to_bind = g_ranges[r].tex ? g_ranges[r].tex : g_white_tex;
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, tex_to_bind );

		// PERFORMANCE: Use flags stored in draw range (no search needed!)
		int flags = g_ranges[r].flags;

		// PERFORMANCE: Only change blend state if needed
		bool needs_blend = ( flags & STUDIO_NF_ADDITIVE ) != 0;
		if ( needs_blend != blend_enabled ) {
			if ( needs_blend ) {
				glEnable( GL_BLEND );
				glBlendFunc( GL_SRC_ALPHA, GL_ONE );
			} else {
				glDisable( GL_BLEND );
			}
			blend_enabled = needs_blend;
		}

		// PERFORMANCE: Use cached uniform locations (not glGetUniformLocation!)
		if ( u_masked_loc != -1 )
			glUniform1i( u_masked_loc, ( flags & STUDIO_NF_MASKED ) != 0 );
		if ( u_fullbright_loc != -1 )
			glUniform1i( u_fullbright_loc, ( flags & STUDIO_NF_FULLBRIGHT ) != 0 );
		if ( u_additive_loc != -1 )
			glUniform1i( u_additive_loc, ( flags & STUDIO_NF_ADDITIVE ) != 0 );
		if ( u_chrome_loc != -1 )
			glUniform1i( u_chrome_loc, ( flags & STUDIO_NF_CHROME ) != 0 );

		glDrawArrays( GL_TRIANGLES, g_ranges[r].first, g_ranges[r].count );
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Layer 2: Model rendering with custom matrices (Qt/GUI uses this)
// ───────────────────────────────────────────────────────────────────────────
void render_model_with_matrices( mat4 view, mat4 proj, mat4 model ) {
	// ONE-TIME: Build mesh topology
	if ( !model_processed ) {
		ProcessModelForRendering();

		// Upload vertices to GPU
		glBindBuffer( GL_ARRAY_BUFFER, VBO );
		glBufferData( GL_ARRAY_BUFFER,
					  total_render_vertices * 8 * sizeof( float ),
					  render_vertex_buffer,
					  GL_STATIC_DRAW );

		// Upload bone indices for GPU skinning
		glBindBuffer( GL_ARRAY_BUFFER, bone_index_VBO );
		glBufferData( GL_ARRAY_BUFFER, total_render_vertices * sizeof( int ), render_bone_indices, GL_STATIC_DRAW );
	}

	if ( total_render_vertices == 0 ) {
		LOG_WARNF( "renderer", "No vertices to render!" );
		return;
	}

	// EVERY FRAME: Update bone matrices for GPU skinning
	if ( g_animation_enabled && global_header && global_data ) {
		// Animation enabled: Calculate animated bone transforms EVERY FRAME
		mdl_result_t anim_result = mdl_animation_calculate_bones(
			&g_anim_state, global_header, global_data, global_seqgroups, g_bonetransformations );

		// Mark T-pose as needing recalculation (since we're animating)
		t_pose_bones_calculated = false;

		// CRITICAL FIX: If sequence group is missing, fall back to T-pose
		if ( anim_result == MDL_ERROR_SEQUENCE_GROUP_MISSING ) {
			SetUpBones( global_header, global_data );
			t_pose_bones_calculated = true;
		}
	} else if ( global_header && global_data ) {
		// Animation disabled: Calculate T-pose bones ONCE
		if ( !t_pose_bones_calculated ) {
			SetUpBones( global_header, global_data );
			t_pose_bones_calculated = true;
		}
	}

	// CRITICAL: Use shader program FIRST before setting uniforms!
	glUseProgram( shader_program );

	// Upload bone matrices to shader (ALWAYS, whether animated or T-pose)
	// PERFORMANCE: Use cached uniform location
	if ( global_header && global_data && u_boneMatrices_loc != -1 ) {
		glUniformMatrix4fv( u_boneMatrices_loc, global_header->numbones, GL_FALSE,
							(const float *)g_bonetransformations );
	}

	// NOTE: Animation vertex updates will go here in the future
	// For now, we use GPU skinning via bone matrices (uploaded above)
	// CPU-side vertex rebuilding is NOT needed unless animations require it

	// Shader program already bound above (before setting bone matrices)

	// PERFORMANCE: Set matrices using cached uniform locations
	if ( u_model_loc != -1 )
		glUniformMatrix4fv( u_model_loc, 1, GL_FALSE, (const float *)model );
	if ( u_view_loc != -1 )
		glUniformMatrix4fv( u_view_loc, 1, GL_FALSE, (const float *)view );
	if ( u_proj_loc != -1 )
		glUniformMatrix4fv( u_proj_loc, 1, GL_FALSE, (const float *)proj );

	// Set lighting (3-point lighting)
	vec3 lightPos = { 5.0f, 8.0f, 5.0f };
	if ( u_lightPos_loc != -1 )
		glUniform3fv( u_lightPos_loc, 1, (const float *)lightPos );

	// Extract camera position from view matrix for chrome shader
	vec3 camPos;
	camPos[0] = -( view[0][0] * view[3][0] + view[0][1] * view[3][1] + view[0][2] * view[3][2] );
	camPos[1] = -( view[1][0] * view[3][0] + view[1][1] * view[3][1] + view[1][2] * view[3][2] );
	camPos[2] = -( view[2][0] * view[3][0] + view[2][1] * view[3][1] + view[2][2] * view[3][2] );

	if ( u_viewPos_loc != -1 )
		glUniform3fv( u_viewPos_loc, 1, (const float *)camPos );

	// Draw the model geometry (Layer 1)
	draw_model_geometry();
}

// ───────────────────────────────────────────────────────────────────────────
// Layer 3: Scene rendering (grid/axes + model)
// ───────────────────────────────────────────────────────────────────────────
void render_scene( mat4 view, mat4 proj, mat4 model ) {
	// Draw grid and axes at Y=0 (NO ground plane - only grid)
	float ground_y = 0.0f;
	r_grid_draw( view, proj, ground_y );
	r_axes_draw( view, proj, ground_y );

	// Re-activate model shader after grid rendering
	glUseProgram( shader_program );

	// Draw model with provided matrices (Layer 2)
	render_model_with_matrices( view, proj, model );
}

void set_model_data( studiohdr_t *header, unsigned char *data, studiohdr_t *tex_header, unsigned char *tex_data, mdl_seqgroup_blob_t *seqgroups, int num_seqgroups ) {
	if ( !header || !data ) {
		LOG_ERRORF( "renderer", "NULL model data passed to renderer!" );
		return;
	}

	LOG_DEBUGF( "renderer", "Setting up render data: %d bones, %d bodyparts, %d sequences",
			   header->numbones, header->numbodyparts, header->numseq );

	global_header = header;
	global_data = data;
	global_tex_header = tex_header; // may be NULL
	global_tex_data = tex_data; // may be NULL
	global_seqgroups = seqgroups;
	global_num_seqgroups = num_seqgroups;

	model_processed = false;
	bone_system_initialized = false;
	t_pose_bones_calculated = false; // Reset T-pose calculation flag
	total_render_vertices = 0;

	// Free old textures
	if ( g_textures.textures ) {
		mdl_free_texture( &g_textures );
	}

	// Pick which header to use (embedded or T.mdl)
	const studiohdr_t *texHdr = mdl_pick_texture_header( header, tex_header );
	if ( texHdr ) {
		mdl_load_textures( texHdr, ( texHdr == header ) ? data : tex_data, &g_textures );
	}

	// Calculate bounding box from actual vertex data (for debug/display purposes)
	mdl_bounds_calculate( header, data, &g_model_bounds );

    // ═══════════════════════════════════════════════════════════════════════════
    // GROUND ALIGNMENT - Sam Vanheer's Method using SEQUENCE bounding box
    // Formula: modelPos.y = -sequence.bbmin[upAxis] * scale
    // Ground stays at Y=0, model is translated UP to place feet on ground
    // ═══════════════════════════════════════════════════════════════════════════

    mstudioseqdesc_t* sequences = (mstudioseqdesc_t*)(data + header->seqindex);
    mstudioseqdesc_t* chosen_sequence = NULL;

    // 1. Find "idle" sequence (preferred) or use sequence 0
    for (int i = 0; i < header->numseq; i++) {
        if (strcmp(sequences[i].label, "idle") == 0) {
            chosen_sequence = &sequences[i];
            printf("Ground alignment: Using 'idle' sequence (index %d)\n", i);
            break;
        }
    }

    if (!chosen_sequence && header->numseq > 0) {
        chosen_sequence = &sequences[0];
        printf("Ground alignment: Using sequence 0 ('%s')\n", chosen_sequence->label);
    }

    // 2. Calculate ground offset: translate model UP by -bbmin.z
    // HL Z (up) → GL Y (up) after shader axis remap
    // If bbmin.z is negative (feet below HL origin), offset is positive (move up)
    // If bbmin.z is positive (feet above HL origin), offset is negative (move down)
    if (chosen_sequence) {
        g_model_ground_y = -chosen_sequence->bbmin[2] * g_model_scale;
        printf("Ground alignment: sequence '%s' bbmin.z=%.2f → offset=%.4f\n",
               chosen_sequence->label, chosen_sequence->bbmin[2], g_model_ground_y);
    } else {
        g_model_ground_y = 0.0f;
        printf("WARNING: No sequences found, ground offset = 0\n");
    }

    // Use same offset for T-pose display
    g_model_ground_y_tpose = g_model_ground_y;


	// Adding animations initializing
	mdl_animation_init( &g_anim_state );

	if ( header && header->numseq > 0 ) {
		mdl_animation_set_sequence( &g_anim_state, 0, header, data, global_seqgroups );
		g_animation_enabled = true;
		g_last_frame_time = glfwGetTime();
	}

	LOG_INFOF( "renderer", "Model loaded successfully" );
}

// ═══════════════════════════════════════════════════════════
// SKIN FAMILY CONTROLS
// ═══════════════════════════════════════════════════════════

void next_skin_family( void ) {
	if ( !global_header || !global_data ) {
		return;
	}

	int num_families = global_header->numskinfamilies;
	if ( num_families <= 1 ) {
		printf( "Model only has 1 skin family\n" );
		return;
	}

	g_current_skin_family = ( g_current_skin_family + 1 ) % num_families;
	model_processed = false; // Force re-process to use new textures

	printf( "Skin family: %d/%d\n", g_current_skin_family + 1, num_families );
}

void prev_skin_family( void ) {
	if ( !global_header || !global_data ) {
		return;
	}

	int num_families = global_header->numskinfamilies;
	if ( num_families <= 1 ) {
		printf( "Model only has 1 skin family\n" );
		return;
	}

	g_current_skin_family = ( g_current_skin_family - 1 + num_families ) % num_families;
	model_processed = false; // Force re-process to use new textures

	printf( "Skin family: %d/%d\n", g_current_skin_family + 1, num_families );
}

int get_current_skin_family( void ) {
	return g_current_skin_family;
}

int get_num_skin_families( void ) {
	if ( !global_header ) {
		return 0;
	}
	return global_header->numskinfamilies;
}

// ═══════════════════════════════════════════════════════════
// CAMERA MODE CONTROLS
// ═══════════════════════════════════════════════════════════

void set_camera_orbit_mode( bool orbit ) {
	camera_orbit_mode = orbit;
	LOG_INFOF( "camera", "Camera mode: %s", orbit ? "ORBIT (camera moves)" : "MODEL ROTATE (model moves)" );
}

bool get_camera_orbit_mode( void ) {
	return camera_orbit_mode;
}

float *get_model_rotation_x_ptr( void ) {
	return &model_rotation_x;
}

float *get_model_rotation_y_ptr( void ) {
	return &model_rotation_y;
}

// ═══════════════════════════════════════════════════════════════════════════
// QT INSTANCE IMPLEMENTATION (per-viewport rendering for Qt GUI)
// ═══════════════════════════════════════════════════════════════════════════
// These functions provide per-viewport rendering for Qt ModelViewport widgets.
// Each Qt tab gets its own instance with independent state.
// CLI does NOT use these - it uses the global state above.

r_qt_instance_t* r_qt_create_instance(void) {
	LOG_DEBUGF("renderer", "Creating Qt viewport instance");

	r_qt_instance_t *inst = (r_qt_instance_t*)calloc(1, sizeof(r_qt_instance_t));
	if (!inst) {
		LOG_FATALF("renderer", "Failed to allocate Qt instance!");
		return NULL;
	}

	// Initialize bone transformations to identity matrices
	for (int i = 0; i < MAXSTUDIOBONES; i++) {
		Math_Mat4_Identity(inst->bone_transformations[i]);
	}

	// Initialize animation state
	mdl_animation_init(&inst->anim_state);

	// Create OpenGL resources (VAO/VBO for this viewport)
	glGenBuffers(1, &inst->vbo);
	glGenBuffers(1, &inst->bone_index_vbo);
	glGenBuffers(1, &inst->ebo);
	glGenVertexArrays(1, &inst->vao);

	LOG_DEBUGF("renderer", "Qt instance OpenGL resources created: VAO=%u VBO=%u",
	          inst->vao, inst->vbo);

	// Load shaders for this instance
	char *vertex_shader_file = read_shader_source("textured.vert");
	char *fragment_shader_file = read_shader_source("textured.frag");

	if (!vertex_shader_file || !fragment_shader_file) {
		LOG_FATALF("renderer", "Failed to load shader files for Qt instance!");
		free(inst);
		return NULL;
	}

	GLuint vertexShader = compile_shader(vertex_shader_file, GL_VERTEX_SHADER);
	GLuint fragmentShader = compile_shader(fragment_shader_file, GL_FRAGMENT_SHADER);

	free(vertex_shader_file);
	free(fragment_shader_file);

	if (vertexShader == 0 || fragmentShader == 0) {
		LOG_FATALF("renderer", "Failed to compile shaders for Qt instance!");
		free(inst);
		return NULL;
	}

	inst->shader_program = create_shader_program(vertexShader, fragmentShader);

	if (inst->shader_program == 0) {
		LOG_FATALF("renderer", "Failed to create shader program for Qt instance!");
		free(inst);
		return NULL;
	}

	// PERFORMANCE: Cache uniform locations (looked up once, not every frame!)
	inst->u_model_loc = glGetUniformLocation(inst->shader_program, "model");
	inst->u_view_loc = glGetUniformLocation(inst->shader_program, "view");
	inst->u_proj_loc = glGetUniformLocation(inst->shader_program, "projection");
	inst->u_tex_loc = glGetUniformLocation(inst->shader_program, "tex");
	inst->u_lightPos_loc = glGetUniformLocation(inst->shader_program, "lightPos");
	inst->u_viewPos_loc = glGetUniformLocation(inst->shader_program, "viewPos");
	inst->u_masked_loc = glGetUniformLocation(inst->shader_program, "u_masked");
	inst->u_fullbright_loc = glGetUniformLocation(inst->shader_program, "u_fullbright");
	inst->u_additive_loc = glGetUniformLocation(inst->shader_program, "u_additive");
	inst->u_chrome_loc = glGetUniformLocation(inst->shader_program, "u_chrome");
	inst->u_boneMatrices_loc = glGetUniformLocation(inst->shader_program, "boneMatrices");

	LOG_DEBUGF("renderer", "Qt instance shader uniforms cached");

	// Create fallback white texture
	glGenTextures(1, &inst->white_tex);
	glBindTexture(GL_TEXTURE_2D, inst->white_tex);

	unsigned char white[] = {255, 255, 255, 255, 255, 255, 255, 255,
	                         255, 255, 255, 255, 255, 255, 255, 255};

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Initialize flags
	inst->model_processed = false;
	inst->t_pose_bones_calculated = false;
	inst->animation_enabled = false;
	inst->current_skin_family = 0;
	inst->num_ranges = 0;
	inst->total_render_vertices = 0;

	LOG_DEBUGF("renderer", "Qt viewport instance created successfully");

	return inst;
}

void r_qt_destroy_instance(r_qt_instance_t *inst) {
	if (!inst) {
		return;
	}

	LOG_DEBUGF("renderer", "Destroying Qt viewport instance");

	// Free textures
	if (inst->textures.textures) {
		mdl_free_texture(&inst->textures);
	}

	// Delete OpenGL resources
	if (inst->vao) {
		glDeleteVertexArrays(1, &inst->vao);
	}
	if (inst->vbo) {
		glDeleteBuffers(1, &inst->vbo);
	}
	if (inst->ebo) {
		glDeleteBuffers(1, &inst->ebo);
	}
	if (inst->bone_index_vbo) {
		glDeleteBuffers(1, &inst->bone_index_vbo);
	}
	if (inst->shader_program) {
		glDeleteProgram(inst->shader_program);
	}
	if (inst->white_tex) {
		glDeleteTextures(1, &inst->white_tex);
	}

	// Free the instance itself
	free(inst);

	LOG_DEBUGF("renderer", "Qt viewport instance destroyed");
}

void r_qt_set_model_data(
	r_qt_instance_t *inst,
	studiohdr_t *header,
	unsigned char *data,
	studiohdr_t *tex_header,
	unsigned char *tex_data,
	mdl_seqgroup_blob_t *seqgroups,
	int num_seqgroups) {

	if (!inst) {
		LOG_ERRORF("renderer", "NULL instance passed to r_qt_set_model_data!");
		return;
	}

	if (!header || !data) {
		LOG_ERRORF("renderer", "NULL model data passed to Qt instance!");
		return;
	}

	LOG_DEBUGF("renderer", "Qt instance: Loading model: %d bones, %d bodyparts, %d sequences",
	         header->numbones, header->numbodyparts, header->numseq);

	// Store model data pointers (NOT owned by instance, just referenced)
	inst->header = header;
	inst->data = data;
	inst->tex_header = tex_header;
	inst->tex_data = tex_data;
	inst->seqgroups = seqgroups;
	inst->num_seqgroups = num_seqgroups;

	// Reset processing flags
	inst->model_processed = false;
	inst->t_pose_bones_calculated = false;
	inst->total_render_vertices = 0;
	inst->num_ranges = 0;

	// Free old textures
	if (inst->textures.textures) {
		mdl_free_texture(&inst->textures);
	}

	// Load textures for this instance
	const studiohdr_t *texHdr = mdl_pick_texture_header(header, tex_header);
	if (texHdr) {
		mdl_load_textures(texHdr, (texHdr == header) ? data : tex_data, &inst->textures);
	}

	// Initialize animation for this instance
	mdl_animation_init(&inst->anim_state);

	if (header && header->numseq > 0) {
		mdl_animation_set_sequence(&inst->anim_state, 0, header, data, seqgroups);
		inst->animation_enabled = true;
	} else {
		inst->animation_enabled = false;
	}

	LOG_DEBUGF("renderer", "Qt instance: Model data loaded successfully");
}

// ═══════════════════════════════════════════════════════════════════════════
// QT INSTANCE HELPER FUNCTIONS (internal, used by r_qt_render_with_matrices)
// ═══════════════════════════════════════════════════════════════════════════

static void r_qt_add_vertex_to_buffer(
	r_qt_instance_t *inst,
	int vertex_index,
	int normal_index,
	short s, short t,
	float texW, float texH) {

	if (inst->total_render_vertices >= MAX_RENDER_VERTICES) {
		return;
	}

	const int base = inst->total_render_vertices * 8;

	vec3 P;
	Math_Vec3Copy(inst->current.vertices[vertex_index], P);

	vec3 N;
	Math_Vec3Copy(inst->current.normals[normal_index], N);

	// Get bone index for this vertex
	unsigned char *v2bone = (unsigned char *)(inst->data + inst->current.model->vertinfoindex);
	int bone = v2bone ? v2bone[vertex_index] : 0;
	if (bone < 0 || bone >= inst->header->numbones) {
		bone = 0;
	}

	inst->render_vertex_buffer[base + 0] = P[0];
	inst->render_vertex_buffer[base + 1] = P[1];
	inst->render_vertex_buffer[base + 2] = P[2];

	inst->render_vertex_buffer[base + 3] = N[0];
	inst->render_vertex_buffer[base + 4] = N[1];
	inst->render_vertex_buffer[base + 5] = N[2];

	// UV coordinates
	float u = ((float)s + 0.5f) / texW;
	float v = ((float)t + 0.5f) / texH;

	// Clamp UV
	if (u < 0.0f) u = 0.0f;
	else if (u > 1.0f) u = 1.0f;
	if (v < 0.0f) v = 0.0f;
	else if (v > 1.0f) v = 1.0f;

	inst->render_vertex_buffer[base + 6] = u;
	inst->render_vertex_buffer[base + 7] = v;

	// Store bone index
	inst->render_bone_indices[inst->total_render_vertices] = bone;

	inst->total_render_vertices++;
}

static void r_qt_process_model(r_qt_instance_t *inst) {
	if (!inst || !inst->header || !inst->data) {
		return;
	}

	inst->total_render_vertices = 0;
	inst->num_ranges = 0;

	mstudiobodyparts_t *bodyparts = (mstudiobodyparts_t *)(inst->data + inst->header->bodypartindex);

	// Calculate initial T-pose bones
	SetUpBones(inst->header, inst->data);

	// Copy bones from global array to instance array
	for (int i = 0; i < inst->header->numbones; i++) {
		Math_Mat4_Copy(g_bonetransformations[i], inst->bone_transformations[i]);
	}
	inst->t_pose_bones_calculated = true;

	// Process each bodypart
	for (int bp = 0; bp < inst->header->numbodyparts; ++bp) {
		mstudiobodyparts_t *bpRec = &bodyparts[bp];
		mstudiomodel_t *models = (mstudiomodel_t *)(inst->data + bpRec->modelindex);

		// Get selected model for this bodypart from bodypart manager
		int selected_model_index = bodypart_get_model_index(bp);

		if (selected_model_index < 0 || selected_model_index >= bpRec->nummodels) {
			selected_model_index = 0;
		}

		mstudiomodel_t *model = &models[selected_model_index];

		inst->current.model = model;
		inst->current.vertices = (vec3_t *)(inst->data + model->vertindex);
		inst->current.normals = (vec3_t *)(inst->data + model->normindex);
		inst->current.vertex_count = model->numverts;
		inst->current.normal_count = model->numnorms;

		// Process meshes
		mstudiomesh_t *meshes = (mstudiomesh_t *)(inst->data + model->meshindex);
		const short *skin_table = (const short *)(inst->data + inst->header->skinindex);
		const int numskinref = inst->header->numskinref;

		for (int mesh = 0; mesh < model->nummesh; ++mesh) {
			const int norm_base = meshes[mesh].normindex;

			// Resolve texture
			int tex_index = meshes[mesh].skinref;
			if (skin_table && numskinref > 0 && tex_index >= 0 && tex_index < numskinref) {
				tex_index = skin_table[inst->current_skin_family * numskinref + tex_index];
			}

			GLuint gl_tex = 0;
			int texW = 1, texH = 1;
			int tex_flags = 0;

			if (tex_index >= 0 && tex_index < inst->textures.count) {
				gl_tex = inst->textures.textures[tex_index].gl_id;
				texW = inst->textures.textures[tex_index].width;
				texH = inst->textures.textures[tex_index].height;
				tex_flags = inst->textures.textures[tex_index].flags;

				if (texW <= 0) texW = 1;
				if (texH <= 0) texH = 1;
			}

			if (!gl_tex && inst->white_tex) {
				gl_tex = inst->white_tex;
				texW = 2;
				texH = 2;
			}

			short *ptricmds = (short *)(inst->data + meshes[mesh].triindex);
			const int start_first = inst->total_render_vertices;

			// Process triangle commands (same logic as CLI)
			int i;
			while ((i = *(ptricmds++))) {
				if (i < 0) {
					// Triangle fan
					i = -i;

					// Read first 2 vertices
					short v0 = ptricmds[0];
					short n0 = ptricmds[1];
					short s0 = ptricmds[2];
					short t0 = ptricmds[3];
					ptricmds = (short *)((char *)ptricmds + 4 * sizeof(short));

					short v1 = ptricmds[0];
					short n1 = ptricmds[1];
					short s1 = ptricmds[2];
					short t1 = ptricmds[3];
					ptricmds = (short *)((char *)ptricmds + 4 * sizeof(short));

					// Handle ON-SEAM
					if (n0 & 0x8000) s0 = (short)(s0 + texW / 2);
					n0 &= 0x7FFF;
					if (n1 & 0x8000) s1 = (short)(s1 + texW / 2);
					n1 &= 0x7FFF;

					n0 = (short)(n0 + norm_base);
					n1 = (short)(n1 + norm_base);

					for (int j = 2; j < i; ++j) {
						short v2 = ptricmds[0];
						short n2 = ptricmds[1];
						short s2 = ptricmds[2];
						short t2 = ptricmds[3];
						ptricmds = (short *)((char *)ptricmds + 4 * sizeof(short));

						if (n2 & 0x8000) s2 = (short)(s2 + texW / 2);
						n2 &= 0x7FFF;
						n2 = (short)(n2 + norm_base);

						// Bounds check
						if (v0 >= 0 && v0 < inst->current.vertex_count &&
						    n0 >= 0 && n0 < inst->current.normal_count &&
						    v1 >= 0 && v1 < inst->current.vertex_count &&
						    n1 >= 0 && n1 < inst->current.normal_count &&
						    v2 >= 0 && v2 < inst->current.vertex_count &&
						    n2 >= 0 && n2 < inst->current.normal_count) {

							r_qt_add_vertex_to_buffer(inst, v0, n0, s0, t0, (float)texW, (float)texH);
							r_qt_add_vertex_to_buffer(inst, v1, n1, s1, t1, (float)texW, (float)texH);
							r_qt_add_vertex_to_buffer(inst, v2, n2, s2, t2, (float)texW, (float)texH);
						}

						// Roll forward
						v1 = v2;
						n1 = n2;
						s1 = s2;
						t1 = t2;
					}
				} else {
					// Triangle strip - alternates winding every other triangle
					// Read first 2 vertices
					short v0 = ptricmds[0];
					short n0 = ptricmds[1];
					short s0 = ptricmds[2];
					short t0 = ptricmds[3];
					ptricmds = (short *)((char *)ptricmds + 4 * sizeof(short));

					short v1 = ptricmds[0];
					short n1 = ptricmds[1];
					short s1 = ptricmds[2];
					short t1 = ptricmds[3];
					ptricmds = (short *)((char *)ptricmds + 4 * sizeof(short));

					// Handle ON-SEAM
					if (n0 & 0x8000) s0 = (short)(s0 + texW / 2);
					n0 &= 0x7FFF;
					if (n1 & 0x8000) s1 = (short)(s1 + texW / 2);
					n1 &= 0x7FFF;

					n0 = (short)(n0 + norm_base);
					n1 = (short)(n1 + norm_base);

					for (int j = 2; j < i; ++j) {
						short v2 = ptricmds[0];
						short n2 = ptricmds[1];
						short s2 = ptricmds[2];
						short t2 = ptricmds[3];
						ptricmds = (short *)((char *)ptricmds + 4 * sizeof(short));

						if (n2 & 0x8000) s2 = (short)(s2 + texW / 2);
						n2 &= 0x7FFF;
						n2 = (short)(n2 + norm_base);

						// Parity for triangle strip winding
						if ((j - 2) % 2 == 0) {
							// Even: v0, v1, v2
							if (v0 >= 0 && v0 < inst->current.vertex_count &&
							    n0 >= 0 && n0 < inst->current.normal_count &&
							    v1 >= 0 && v1 < inst->current.vertex_count &&
							    n1 >= 0 && n1 < inst->current.normal_count &&
							    v2 >= 0 && v2 < inst->current.vertex_count &&
							    n2 >= 0 && n2 < inst->current.normal_count) {

								r_qt_add_vertex_to_buffer(inst, v0, n0, s0, t0, (float)texW, (float)texH);
								r_qt_add_vertex_to_buffer(inst, v1, n1, s1, t1, (float)texW, (float)texH);
								r_qt_add_vertex_to_buffer(inst, v2, n2, s2, t2, (float)texW, (float)texH);
							}
						} else {
							// Odd: v1, v0, v2 (swap winding)
							if (v0 >= 0 && v0 < inst->current.vertex_count &&
							    n0 >= 0 && n0 < inst->current.normal_count &&
							    v1 >= 0 && v1 < inst->current.vertex_count &&
							    n1 >= 0 && n1 < inst->current.normal_count &&
							    v2 >= 0 && v2 < inst->current.vertex_count &&
							    n2 >= 0 && n2 < inst->current.normal_count) {

								r_qt_add_vertex_to_buffer(inst, v1, n1, s1, t1, (float)texW, (float)texH);
								r_qt_add_vertex_to_buffer(inst, v0, n0, s0, t0, (float)texW, (float)texH);
								r_qt_add_vertex_to_buffer(inst, v2, n2, s2, t2, (float)texW, (float)texH);
							}
						}

						// Roll forward (keep both v0/v1, shift them)
						v0 = v1;
						n0 = n1;
						s0 = s1;
						t0 = t1;
						v1 = v2;
						n1 = n2;
						s1 = s2;
						t1 = t2;
					}
				}
			}

			// Create draw range for this mesh
			if (inst->num_ranges < MAX_DRAW_RANGES) {
				inst->ranges[inst->num_ranges].tex = gl_tex;
				inst->ranges[inst->num_ranges].first = start_first;
				inst->ranges[inst->num_ranges].count = inst->total_render_vertices - start_first;
				inst->ranges[inst->num_ranges].flags = tex_flags;
				inst->num_ranges++;
			}
		}
	}

	inst->model_processed = true;
}

void r_qt_render_with_matrices(
	r_qt_instance_t *inst,
	mat4 view,
	mat4 proj,
	mat4 model) {

	if (!inst) {
		return;
	}

	// Process model geometry once
	if (!inst->model_processed) {
		r_qt_process_model(inst);

		// Upload vertices to GPU
		glBindBuffer(GL_ARRAY_BUFFER, inst->vbo);
		glBufferData(GL_ARRAY_BUFFER,
		            inst->total_render_vertices * 8 * sizeof(float),
		            inst->render_vertex_buffer,
		            GL_STATIC_DRAW);

		// Upload bone indices
		glBindBuffer(GL_ARRAY_BUFFER, inst->bone_index_vbo);
		glBufferData(GL_ARRAY_BUFFER,
		            inst->total_render_vertices * sizeof(int),
		            inst->render_bone_indices,
		            GL_STATIC_DRAW);
	}

	if (inst->total_render_vertices == 0) {
		return;
	}

	// Update bone matrices
	if (inst->animation_enabled && inst->header && inst->data) {
		mdl_result_t anim_result = mdl_animation_calculate_bones(
			&inst->anim_state, inst->header, inst->data, inst->seqgroups,
			inst->bone_transformations);

		inst->t_pose_bones_calculated = false;

		if (anim_result == MDL_ERROR_SEQUENCE_GROUP_MISSING) {
			SetUpBones(inst->header, inst->data);
			for (int i = 0; i < inst->header->numbones; i++) {
				Math_Mat4_Copy(g_bonetransformations[i], inst->bone_transformations[i]);
			}
			inst->t_pose_bones_calculated = true;
		}
	} else if (inst->header && inst->data) {
		if (!inst->t_pose_bones_calculated) {
			SetUpBones(inst->header, inst->data);
			for (int i = 0; i < inst->header->numbones; i++) {
				Math_Mat4_Copy(g_bonetransformations[i], inst->bone_transformations[i]);
			}
			inst->t_pose_bones_calculated = true;
		}
	}

	// Activate shader
	glUseProgram(inst->shader_program);

	// Upload bone matrices to shader
	if (inst->header && inst->data && inst->u_boneMatrices_loc != -1) {
		glUniformMatrix4fv(inst->u_boneMatrices_loc, inst->header->numbones, GL_FALSE,
		                  (const float *)inst->bone_transformations);
	}

	// Set matrix uniforms
	if (inst->u_model_loc != -1) {
		glUniformMatrix4fv(inst->u_model_loc, 1, GL_FALSE, (const float *)model);
	}
	if (inst->u_view_loc != -1) {
		glUniformMatrix4fv(inst->u_view_loc, 1, GL_FALSE, (const float *)view);
	}
	if (inst->u_proj_loc != -1) {
		glUniformMatrix4fv(inst->u_proj_loc, 1, GL_FALSE, (const float *)proj);
	}

	// Set lighting
	vec3 lightPos = {5.0f, 8.0f, 5.0f};
	if (inst->u_lightPos_loc != -1) {
		glUniform3fv(inst->u_lightPos_loc, 1, (const float *)lightPos);
	}

	// Extract camera position from view matrix
	vec3 camPos;
	camPos[0] = -(view[0][0] * view[3][0] + view[0][1] * view[3][1] + view[0][2] * view[3][2]);
	camPos[1] = -(view[1][0] * view[3][0] + view[1][1] * view[3][1] + view[1][2] * view[3][2]);
	camPos[2] = -(view[2][0] * view[3][0] + view[2][1] * view[3][1] + view[2][2] * view[3][2]);

	if (inst->u_viewPos_loc != -1) {
		glUniform3fv(inst->u_viewPos_loc, 1, (const float *)camPos);
	}

	// Set up vertex attributes
	glBindVertexArray(inst->vao);
	glBindBuffer(GL_ARRAY_BUFFER, inst->vbo);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, inst->bone_index_vbo);
	glVertexAttribIPointer(3, 1, GL_INT, sizeof(int), (void *)0);
	glEnableVertexAttribArray(3);

	if (inst->u_tex_loc != -1) {
		glUniform1i(inst->u_tex_loc, 0);
	}

	glDisable(GL_BLEND);

	// Draw each range
	for (int r = 0; r < inst->num_ranges; ++r) {
		GLuint tex_to_bind = inst->ranges[r].tex ? inst->ranges[r].tex : inst->white_tex;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex_to_bind);

		int flags = inst->ranges[r].flags;

		if (flags & STUDIO_NF_ADDITIVE) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		} else {
			glDisable(GL_BLEND);
		}
		if (inst->u_masked_loc != -1) {
			glUniform1i(inst->u_masked_loc, (flags & STUDIO_NF_MASKED) != 0);
		}
		if (inst->u_fullbright_loc != -1) {
			glUniform1i(inst->u_fullbright_loc, (flags & STUDIO_NF_FULLBRIGHT) != 0);
		}
		if (inst->u_additive_loc != -1) {
			glUniform1i(inst->u_additive_loc, (flags & STUDIO_NF_ADDITIVE) != 0);
		}
		if (inst->u_chrome_loc != -1) {
			glUniform1i(inst->u_chrome_loc, (flags & STUDIO_NF_CHROME) != 0);
		}

		glDrawArrays(GL_TRIANGLES, inst->ranges[r].first, inst->ranges[r].count);
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// QT ANIMATION STATE FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════

void r_qt_set_animation_state( r_qt_instance_t *inst, const mdl_animation_state_t *state ) {
	if ( !inst || !state ) return;

	// Copy the animation state from viewport to render instance
	inst->anim_state.current_sequence = state->current_sequence;
	inst->anim_state.current_frame = state->current_frame;
	inst->anim_state.previous_frame = state->previous_frame;
	inst->anim_state.is_looping = state->is_looping;

	// Mark bones as needing recalculation
	inst->t_pose_bones_calculated = false;
}

void r_qt_set_animation_enabled( r_qt_instance_t *inst, bool enabled ) {
	if ( !inst ) return;
	inst->animation_enabled = enabled;
}

void r_qt_rebuild_mesh_data( r_qt_instance_t *inst ) {
	if ( !inst || !inst->header || !inst->data ) return;

	// Mark as needing reprocessing - the mesh data will be rebuilt
	// next frame when r_qt_process_model is called
	inst->model_processed = false;
	inst->total_render_vertices = 0;
	inst->num_ranges = 0;
}
