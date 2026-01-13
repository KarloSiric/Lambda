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
 *   Purpose: Grid layout for the application
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include "r_grid.h"
#include "r_gl_platform.h"
#include "r_draw.h"
#include "util_logger.h"
#include <stdlib.h>

#define GRID_SIZE 50         // Grid extends 50 units in each direction
#define GRID_SPACING 1.0f    // 1 unit between grid lines

static GLuint grid_vao = 0;
static GLuint grid_vbo = 0;
static GLuint grid_shader = 0;

static bool grid_enabled = true;
static int grid_vertex_count = 0;

// Ground plane
static GLuint ground_vao = 0;
static GLuint ground_vbo = 0;
static GLuint ground_shader = 0;

// Origin axes
static GLuint axes_vao = 0;
static GLuint axes_vbo = 0;
static GLuint axes_shader = 0;

void r_grid_init( float size, float spacing ) {
	// Here we initialize how many lines to draw and so forth
	int line_count = ( (int)size * 2 / (int)spacing ) + 1;
	grid_vertex_count = line_count * 2 * 2;

	float *vertices = malloc( grid_vertex_count * 3 * sizeof( float ) );
	int index = 0;

	// Lines parallel to Z axis (varying in Z direction)
	for ( float x = -size; x <= size; x += spacing ) {
		vertices[index++] = x;          // X position varies
		vertices[index++] = 0.0f;       // Y = 0 (GROUND PLANE!)
		vertices[index++] = -size; // Z start

		vertices[index++] = x;          // X position varies
		vertices[index++] = 0.0f;       // Y = 0 (GROUND PLANE!)
		vertices[index++] = size;  // Z end
	}

	// Lines parallel to X axis (varying in X direction)
	for ( float z = -size; z <= size; z += spacing ) {
		vertices[index++] = -size; // X start
		vertices[index++] = 0.0f;       // Y = 0 (GROUND PLANE!)
		vertices[index++] = z;          // Z position varies

		vertices[index++] = size;  // X end
		vertices[index++] = 0.0f;       // Y = 0 (GROUND PLANE!)
		vertices[index++] = z;          // Z position varies
	}

	glGenVertexArrays( 1, &grid_vao );
	glBindVertexArray( grid_vao );

	glGenBuffers( 1, &grid_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, grid_vbo );
	glBufferData( GL_ARRAY_BUFFER, grid_vertex_count * 3 * sizeof( float ), vertices, GL_STATIC_DRAW );

	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ), (void *)0 );
	glEnableVertexAttribArray( 0 );

	glBindVertexArray( 0 );

	free( vertices );

	// Now we need to add the shader programs to draw the grid itself
	char *vert_source = read_shader_source( "grid.vert" );
	char *frag_source = read_shader_source( "grid.frag" );

	if ( !vert_source ) {
		LOG_WARNF( "grid", "ERROR - Failed to load grid shaders: '%s'\n", vert_source );
		return;
	}

	if ( !frag_source ) {
		LOG_WARNF( "grid", "ERROR - Failed to load grid shaders: '%s'\n", frag_source );
		return;
	}

	GLuint vert_shader = compile_shader( vert_source, GL_VERTEX_SHADER );
	GLuint frag_shader = compile_shader( frag_source, GL_FRAGMENT_SHADER );

	grid_shader = create_shader_program( vert_shader, frag_shader );

	free( vert_source );
	free( frag_source );
}

void r_grid_draw( mat4 view, mat4 projection, float ground_z ) {
	if ( !grid_enabled || grid_vao == 0 || !grid_shader ) {
		return;
	}

	// CRITICAL: Activate grid shader BEFORE setting uniforms!
	glUseProgram( grid_shader );

	// Enable blending for transparent grid
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// Grid at specified Y position (model's feet)
	mat4 model = GLM_MAT4_IDENTITY_INIT;
	glm_translate( model, (vec3){ 0.0f, ground_z, 0.0f } );

	GLint model_loc = glGetUniformLocation( grid_shader, "model" );
	GLint view_loc = glGetUniformLocation( grid_shader, "view" );
	GLint projection_loc = glGetUniformLocation( grid_shader, "projection" );

	if ( model_loc != -1 ) {
		glUniformMatrix4fv( model_loc, 1, GL_FALSE, (const float *)model );
	}

	if ( view_loc != -1 ) {
		glUniformMatrix4fv( view_loc, 1, GL_FALSE, (const float *)view );
	}

	if ( projection_loc != -1 ) {
		glUniformMatrix4fv( projection_loc, 1, GL_FALSE, (const float *)projection );
	}

	// Make grid lines thicker to avoid GPU rendering artifacts
	glLineWidth( 2.5f );

	glBindVertexArray( grid_vao );
	glDrawArrays( GL_LINES, 0, grid_vertex_count );
	glBindVertexArray( 0 );

	// Reset line width
	glLineWidth( 1.0f );
}

void r_grid_toggle( void ) {
	grid_enabled = !grid_enabled;
	LOG_INFOF( "grid", "Grid %s", grid_enabled ? "enabled" : "disabled" );
}

bool r_grid_is_enabled( void ) {
	return grid_enabled;
}

// ═══════════════════════════════════════════════════════════
// GROUND PLANE - Solid colored rectangle beneath the grid
// ═══════════════════════════════════════════════════════════

void r_ground_init( float size ) {
	// Create a simple quad at Y=0
	float vertices[] = {
		// Two triangles forming a rectangle
		// Triangle 1
		-size, 0.0f, -size,
		 size, 0.0f, -size,
		 size, 0.0f,  size,
		// Triangle 2
		-size, 0.0f, -size,
		 size, 0.0f,  size,
		-size, 0.0f,  size
	};

	glGenVertexArrays( 1, &ground_vao );
	glBindVertexArray( ground_vao );

	glGenBuffers( 1, &ground_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, ground_vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ), (void *)0 );
	glEnableVertexAttribArray( 0 );

	glBindVertexArray( 0 );

	// Load ground plane shader (darker color than grid)
	char *vert_source = read_shader_source( "grid.vert" ); // Reuse grid vertex shader
	char *frag_source = read_shader_source( "ground.frag" ); // Use darker fragment shader

	if ( !vert_source || !frag_source ) {
		LOG_WARNF( "ground", "Failed to load ground shaders, using grid shader instead" );
		ground_shader = grid_shader;
		if ( vert_source ) free( vert_source );
		if ( frag_source ) free( frag_source );
	} else {
		GLuint vert_shader = compile_shader( vert_source, GL_VERTEX_SHADER );
		GLuint frag_shader = compile_shader( frag_source, GL_FRAGMENT_SHADER );

		ground_shader = create_shader_program( vert_shader, frag_shader );

		free( vert_source );
		free( frag_source );
	}

	LOG_DEBUGF( "ground", "Ground plane initialized (size: %.1f)", size );
}

void r_ground_draw( mat4 view, mat4 projection, float ground_y ) {
	if ( ground_vao == 0 || !ground_shader ) {
		return;
	}

	glUseProgram( ground_shader );

	// Enable blending
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// Position ground plane at specified Y
	mat4 model = GLM_MAT4_IDENTITY_INIT;
	glm_translate( model, (vec3){ 0.0f, ground_y - 0.01f, 0.0f } ); // Slightly below grid to avoid z-fighting

	GLint model_loc = glGetUniformLocation( ground_shader, "model" );
	GLint view_loc = glGetUniformLocation( ground_shader, "view" );
	GLint projection_loc = glGetUniformLocation( ground_shader, "projection" );

	if ( model_loc != -1 ) {
		glUniformMatrix4fv( model_loc, 1, GL_FALSE, (const float *)model );
	}

	if ( view_loc != -1 ) {
		glUniformMatrix4fv( view_loc, 1, GL_FALSE, (const float *)view );
	}

	if ( projection_loc != -1 ) {
		glUniformMatrix4fv( projection_loc, 1, GL_FALSE, (const float *)projection );
	}

	glBindVertexArray( ground_vao );
	glDrawArrays( GL_TRIANGLES, 0, 6 );
	glBindVertexArray( 0 );
}

// ═══════════════════════════════════════════════════════════
// ORIGIN AXES - X/Y/Z colored lines at origin
// ═══════════════════════════════════════════════════════════

void r_axes_init( float size ) {
	// 3 lines: X (red), Y (green), Z (blue)
	// Each line: 2 vertices * (3 pos + 3 color) = 6 floats per vertex
	float vertices[] = {
		// X axis - RED
		0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // Origin
		size, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // +X direction

		// Y axis - GREEN
		0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  // Origin
		0.0f, size, 0.0f,  0.0f, 1.0f, 0.0f,  // +Y direction

		// Z axis - BLUE
		0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,  // Origin
		0.0f, 0.0f, size,  0.0f, 0.0f, 1.0f   // +Z direction
	};

	glGenVertexArrays( 1, &axes_vao );
	glBindVertexArray( axes_vao );

	glGenBuffers( 1, &axes_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, axes_vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

	// Position attribute
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( float ), (void *)0 );
	glEnableVertexAttribArray( 0 );

	// Color attribute
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( float ), (void *)( 3 * sizeof( float ) ) );
	glEnableVertexAttribArray( 1 );

	glBindVertexArray( 0 );

	// Load axes shader
	char *vert_source = read_shader_source( "axes.vert" );
	char *frag_source = read_shader_source( "axes.frag" );

	if ( !vert_source || !frag_source ) {
		LOG_WARNF( "axes", "Failed to load axes shaders" );
		if ( vert_source ) free( vert_source );
		if ( frag_source ) free( frag_source );
		return;
	}

	GLuint vert_shader = compile_shader( vert_source, GL_VERTEX_SHADER );
	GLuint frag_shader = compile_shader( frag_source, GL_FRAGMENT_SHADER );

	axes_shader = create_shader_program( vert_shader, frag_shader );

	free( vert_source );
	free( frag_source );

	LOG_DEBUGF( "axes", "Origin axes initialized (size: %.1f)", size );
}

void r_axes_draw( mat4 view, mat4 projection, float ground_y ) {
	if ( axes_vao == 0 || !axes_shader ) {
		return;
	}

	glUseProgram( axes_shader );

	// Position axes at ground level
	mat4 model = GLM_MAT4_IDENTITY_INIT;
	glm_translate( model, (vec3){ 0.0f, ground_y, 0.0f } );

	GLint model_loc = glGetUniformLocation( axes_shader, "model" );
	GLint view_loc = glGetUniformLocation( axes_shader, "view" );
	GLint projection_loc = glGetUniformLocation( axes_shader, "projection" );

	if ( model_loc != -1 ) {
		glUniformMatrix4fv( model_loc, 1, GL_FALSE, (const float *)model );
	}

	if ( view_loc != -1 ) {
		glUniformMatrix4fv( view_loc, 1, GL_FALSE, (const float *)view );
	}

	if ( projection_loc != -1 ) {
		glUniformMatrix4fv( projection_loc, 1, GL_FALSE, (const float *)projection );
	}

	// Draw axes with much thicker lines
	glLineWidth( 5.0f );

	glBindVertexArray( axes_vao );
	glDrawArrays( GL_LINES, 0, 6 ); // 6 vertices (3 lines * 2 points)
	glBindVertexArray( 0 );

	// Reset line width
	glLineWidth( 1.0f );
}
