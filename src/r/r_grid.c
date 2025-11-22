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


#define GRID_SIZE          100
#define GRID_SPACING       1.0f


static GLuint grid_vao = 0;
static GLuint grid_vbo = 0;
static GLuint grid_shader = 0;


static bool grid_enabled = true;
static int grid_vertex_count = 0;


void grid_init( void ) {
    
    // Here we initialize how many lines to draw and so forth
    int line_count = ( GRID_SIZE * 2  / GRID_SPACING ) + 1;     
    grid_vertex_count = line_count * 2 * 2;
    
    float *vertices = malloc( grid_vertex_count * 3 * sizeof( float ) );
    int index = 0;
    
    for ( int x = -GRID_SIZE; x <= GRID_SIZE; x+=GRID_SPACING ) {   
        
        vertices[index++] = x;           // X varies
        vertices[index++] = -GRID_SIZE;  // Y fixed (depth)
        vertices[index++] = 0.0f;        // Z = 0 (on ground!)

        vertices[index++] = x;           // X varies
        vertices[index++] = GRID_SIZE;   // Y fixed (depth)
        vertices[index++] = 0.0f;        // Z = 0 (on ground!)    
    }
    
    for ( int y = -GRID_SIZE; y <= GRID_SIZE; y+=GRID_SPACING ) {
        
        vertices[index++] = -GRID_SIZE;  // X fixed
        vertices[index++] = y;           // Y varies
        vertices[index++] = 0.0f;        // Z = 0 (on ground!)

        vertices[index++] = GRID_SIZE;   // X fixed  
        vertices[index++] = y;           // Y varies
        vertices[index++] = 0.0f;        // Z = 0 (on ground!)    
    } 
    
    
    glGenVertexArrays( 1, &grid_vao );
    glBindVertexArray( grid_vao );
    
    glGenBuffers( 1, &grid_vbo );
    glBindBuffer( GL_ARRAY_BUFFER, grid_vbo );
    glBufferData( GL_ARRAY_BUFFER, grid_vertex_count * 3 * sizeof( float ), vertices, GL_STATIC_DRAW );
    
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ), (void*)0 );
    glEnableVertexAttribArray( 0 );
    
    glBindVertexArray( 0 );
    
    free( vertices );
     
    // Now we need to add the shader programs to draw the grid itself
    char *vert_source = read_shader_source( "grid.vert" );
    char *frag_source = read_shader_source( "grid.frag" );
    
    if ( !vert_source ) {
        LOG_WARNF( "grid", "ERROR - Failed to load grid shaders: '%s'\n", vert_source );    
        return ;
    }
    
    if ( !frag_source ) {
        LOG_WARNF( "grid", "ERROR - Failed to load grid shaders: '%s'\n", frag_source );    
        return ;
    }
    
    GLuint vert_shader = compile_shader( vert_source, GL_VERTEX_SHADER );
    GLuint frag_shader = compile_shader( frag_source, GL_FRAGMENT_SHADER );
    
    grid_shader = create_shader_program( vert_shader, frag_shader );
    
    free( vert_source );
    free( frag_source );
}


void grid_render( vec4 *view, vec4 *projection, float ground_z ) {
    
    if ( !grid_enabled || grid_vao == 0 || !grid_shader ) {
        return ;
    }
    
    // @Note: We will pass the view and the projection from the ones we are using in draw.c
    
    mat4 model = GLM_MAT4_IDENTITY_INIT;
    glm_translate( model, ( vec3 ){0.0f, 0.0f, ground_z} );
    
    
    GLint model_loc = glGetUniformLocation( grid_shader, "model" );
    GLint view_loc = glGetUniformLocation( grid_shader, "view" );
    GLint projection_loc = glGetUniformLocation( grid_shader, "projection" );
    
    if ( model_loc != -1 ) {
        glUniformMatrix4fv( model_loc, 1, GL_FALSE, ( const float *)model );
    }
    
    if ( view_loc != -1 ) {
        glUniformMatrix4fv( view_loc, 1, GL_FALSE, ( const float *)view );
    }
    
    if ( projection_loc != -1 ) {
        glUniformMatrix4fv( projection_loc, 1, GL_FALSE, ( const float *)projection );
    }
    
    
    glBindVertexArray( grid_vao );
    glDrawArrays( GL_LINES, 0, grid_vertex_count );
    glBindVertexArray( 0 );
}




