/*
 * ═══════════════════════════════════════════════════════════════════════════
 *   r_gizmo.c  –  Rotation gizmo renderer
 *
 *   Draws three line-loop rings (X=red, Y=green, Z=blue) around the model
 *   centre.  Vertex format: (x, y, z, axis_id) – 4 floats.
 *   Ring geometry is generated at radius=1 in local space; a model matrix
 *   translates to world centre and scales by the requested radius.
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include "r_gizmo.h"
#include "r_draw.h"       /* read_shader_source, compile_shader, create_shader_program */
#include "r_gl_platform.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* ── constants ─────────────────────────────────────────────────────────── */
#define GIZMO_SEGMENTS  64          /* line segments per ring              */
#define GIZMO_AXES      3
/* Each ring: SEGMENTS pairs of vertices (GL_LINES) → 2*SEGMENTS vertices */
#define GIZMO_VERTS     ( GIZMO_AXES * GIZMO_SEGMENTS * 2 )
/* 4 floats per vertex: x, y, z, axis_id */
#define GIZMO_FLOATS    ( GIZMO_VERTS * 4 )

/* ── module state ──────────────────────────────────────────────────────── */
static GLuint s_vao       = 0;
static GLuint s_vbo       = 0;
static GLuint s_shader    = 0;

static GLint  s_loc_model    = -1;
static GLint  s_loc_view     = -1;
static GLint  s_loc_proj     = -1;
static GLint  s_loc_hovered  = -1;
static GLint  s_loc_active   = -1;

/* ── helpers ───────────────────────────────────────────────────────────── */

/*
 * build_ring_vertices
 *   Fills `dst` with 2*GIZMO_SEGMENTS vertices for a unit-radius ring lying
 *   in the plane perpendicular to `axis` (0=X  1=Y  2=Z).
 *   Each vertex is 4 floats: (px, py, pz, axis_id).
 *   Returns pointer past the last written float.
 */
static float *build_ring_vertices( float *dst, int axis )
{
    for ( int i = 0; i < GIZMO_SEGMENTS; ++i ) {
        float t0 = ( 2.0f * (float)M_PI * i       ) / GIZMO_SEGMENTS;
        float t1 = ( 2.0f * (float)M_PI * (i + 1) ) / GIZMO_SEGMENTS;
        float c0 = cosf( t0 ), s0 = sinf( t0 );
        float c1 = cosf( t1 ), s1 = sinf( t1 );

        float v0[3], v1[3];

        switch ( axis ) {
        case 0: /* X ring – lies in YZ plane */
            v0[0] = 0.f;  v0[1] = c0;  v0[2] = s0;
            v1[0] = 0.f;  v1[1] = c1;  v1[2] = s1;
            break;
        case 1: /* Y ring – lies in XZ plane */
            v0[0] = c0;   v0[1] = 0.f; v0[2] = s0;
            v1[0] = c1;   v1[1] = 0.f; v1[2] = s1;
            break;
        default: /* Z ring – lies in XY plane */
            v0[0] = c0;   v0[1] = s0;  v0[2] = 0.f;
            v1[0] = c1;   v1[1] = s1;  v1[2] = 0.f;
            break;
        }

        /* vertex 0 */
        *dst++ = v0[0]; *dst++ = v0[1]; *dst++ = v0[2];
        *dst++ = (float)axis;
        /* vertex 1 */
        *dst++ = v1[0]; *dst++ = v1[1]; *dst++ = v1[2];
        *dst++ = (float)axis;
    }
    return dst;
}

/* ── public API ────────────────────────────────────────────────────────── */

void r_gizmo_init( void )
{
    /* Build geometry */
    float verts[GIZMO_FLOATS];
    float *p = verts;
    p = build_ring_vertices( p, 0 );  /* X */
    p = build_ring_vertices( p, 1 );  /* Y */
    p = build_ring_vertices( p, 2 );  /* Z */

    /* VAO / VBO */
    glGenVertexArrays( 1, &s_vao );
    glBindVertexArray( s_vao );

    glGenBuffers( 1, &s_vbo );
    glBindBuffer( GL_ARRAY_BUFFER, s_vbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof( verts ), verts, GL_STATIC_DRAW );

    /* attribute 0: position (xyz) */
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE,
                           4 * sizeof( float ), (void *)0 );
    glEnableVertexAttribArray( 0 );

    /* attribute 1: axis_id (float scalar) */
    glVertexAttribPointer( 1, 1, GL_FLOAT, GL_FALSE,
                           4 * sizeof( float ),
                           (void *)( 3 * sizeof( float ) ) );
    glEnableVertexAttribArray( 1 );

    glBindVertexArray( 0 );

    /* Shader */
    char *vert_src = read_shader_source( "gizmo.vert" );
    char *frag_src = read_shader_source( "gizmo.frag" );

    if ( !vert_src || !frag_src ) {
        fprintf( stderr, "r_gizmo: failed to load gizmo shaders\n" );
        free( vert_src );
        free( frag_src );
        return;
    }

    GLuint vs = compile_shader( vert_src, GL_VERTEX_SHADER );
    GLuint fs = compile_shader( frag_src, GL_FRAGMENT_SHADER );
    s_shader  = create_shader_program( vs, fs );

    free( vert_src );
    free( frag_src );

    /* Cache uniform locations */
    s_loc_model   = glGetUniformLocation( s_shader, "model"       );
    s_loc_view    = glGetUniformLocation( s_shader, "view"        );
    s_loc_proj    = glGetUniformLocation( s_shader, "projection"  );
    s_loc_hovered = glGetUniformLocation( s_shader, "hoveredAxis" );
    s_loc_active  = glGetUniformLocation( s_shader, "activeAxis"  );
}

void r_gizmo_cleanup( void )
{
    if ( s_vbo )    { glDeleteBuffers( 1, &s_vbo );       s_vbo    = 0; }
    if ( s_vao )    { glDeleteVertexArrays( 1, &s_vao );  s_vao    = 0; }
    if ( s_shader ) { glDeleteProgram( s_shader );         s_shader = 0; }
}

void r_gizmo_draw( mat4 view, mat4 projection,
                   vec3 center, float radius,
                   int hovered, int active )
{
    if ( !s_shader || !s_vao ) return;

    /* Model matrix: translate to centre, scale by radius */
    mat4 model = GLM_MAT4_IDENTITY_INIT;
    glm_translate( model, center );
    glm_scale( model, (vec3){ radius, radius, radius } );

    glUseProgram( s_shader );

    /* Depth: disable write so rings don't occlude the model when behind it,
     * but keep depth test so rings that are truly behind other geometry fade
     * naturally.  Blend for the alpha.                                    */
    GLboolean depth_write;
    glGetBooleanv( GL_DEPTH_WRITEMASK, &depth_write );
    glDepthMask( GL_FALSE );

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    if ( s_loc_model   != -1 ) glUniformMatrix4fv( s_loc_model,  1, GL_FALSE, (float*)model      );
    if ( s_loc_view    != -1 ) glUniformMatrix4fv( s_loc_view,   1, GL_FALSE, (float*)view       );
    if ( s_loc_proj    != -1 ) glUniformMatrix4fv( s_loc_proj,   1, GL_FALSE, (float*)projection );
    if ( s_loc_hovered != -1 ) glUniform1i( s_loc_hovered, hovered );
    if ( s_loc_active  != -1 ) glUniform1i( s_loc_active,  active  );

    glLineWidth( 4.0f );
    glBindVertexArray( s_vao );
    glDrawArrays( GL_LINES, 0, GIZMO_VERTS );
    glBindVertexArray( 0 );
    glLineWidth( 1.0f );

    glDepthMask( depth_write );
    glUseProgram( 0 );
}
