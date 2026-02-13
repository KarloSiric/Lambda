/*
 * ═══════════════════════════════════════════════════════════════════════════
 *   r_compass.c  –  Orientation compass renderer
 *
 *   Draws three coloured axis arrows (X=red, Y=green, Z=blue) in a small
 *   viewport-corner widget.  Each axis has a shaft line plus four short
 *   arrowhead lines forming a cross-shaped tip in the axis' two perpendicular
 *   directions.
 *
 *   Vertex format: (x, y, z, r, g, b) – 6 floats.
 *   The compass is drawn using only the rotation part of the camera view
 *   matrix so it shows orientation, not position.
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include "r_compass.h"
#include "r_draw.h"         /* read_shader_source, compile_shader, create_shader_program */
#include "r_gl_platform.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── constants ─────────────────────────────────────────────────────────── */

/* 5 lines per axis (1 shaft + 4 arrowhead lines), 3 axes, 2 verts per line */
#define COMPASS_LINES   15
#define COMPASS_VERTS   ( COMPASS_LINES * 2 )       /* 30 */
#define COMPASS_FLOATS  ( COMPASS_VERTS * 6 )       /* 180 */

/* Shaft length and arrowhead geometry in local space [-1 .. +1] */
#define TIP   0.70f   /* tip position along axis   */
#define BASE  0.52f   /* arrowhead base position   */
#define HW    0.14f   /* arrowhead half-width      */

/* ── module state ──────────────────────────────────────────────────────── */
static GLuint s_vao      = 0;
static GLuint s_vbo      = 0;
static GLuint s_shader   = 0;

static GLint  s_loc_view = -1;
static GLint  s_loc_proj = -1;

/* ── helpers ───────────────────────────────────────────────────────────── */

static void push_line( float *dst, int *idx,
                        float x0, float y0, float z0,
                        float x1, float y1, float z1,
                        float r,  float g,  float b )
{
    int i = *idx;
    dst[i++] = x0; dst[i++] = y0; dst[i++] = z0;
    dst[i++] = r;  dst[i++] = g;  dst[i++] = b;
    dst[i++] = x1; dst[i++] = y1; dst[i++] = z1;
    dst[i++] = r;  dst[i++] = g;  dst[i++] = b;
    *idx = i;
}

/* ── public API ────────────────────────────────────────────────────────── */

void r_compass_init( void )
{
    float verts[COMPASS_FLOATS];
    int   idx = 0;

    /* Axis colours */
    const float rx = 1.00f, gx = 0.22f, bx = 0.22f;   /* X = red   */
    const float ry = 0.22f, gy = 1.00f, by = 0.22f;   /* Y = green */
    const float rz = 0.22f, gz = 0.45f, bz = 1.00f;   /* Z = blue  */

    /* ── X axis ─────────────────────────── */
    push_line( verts, &idx,  0,   0,   0,   TIP, 0,   0,   rx, gx, bx ); /* shaft */
    push_line( verts, &idx,  BASE, HW,  0,   TIP, 0,   0,   rx, gx, bx ); /* tip ↑ */
    push_line( verts, &idx,  BASE,-HW,  0,   TIP, 0,   0,   rx, gx, bx ); /* tip ↓ */
    push_line( verts, &idx,  BASE, 0,   HW,  TIP, 0,   0,   rx, gx, bx ); /* tip ● */
    push_line( verts, &idx,  BASE, 0,  -HW,  TIP, 0,   0,   rx, gx, bx ); /* tip ○ */

    /* ── Y axis ─────────────────────────── */
    push_line( verts, &idx,  0,   0,   0,   0,  TIP,  0,   ry, gy, by );
    push_line( verts, &idx,  HW,  BASE, 0,   0,  TIP,  0,   ry, gy, by );
    push_line( verts, &idx, -HW,  BASE, 0,   0,  TIP,  0,   ry, gy, by );
    push_line( verts, &idx,  0,   BASE, HW,  0,  TIP,  0,   ry, gy, by );
    push_line( verts, &idx,  0,   BASE,-HW,  0,  TIP,  0,   ry, gy, by );

    /* ── Z axis ─────────────────────────── */
    push_line( verts, &idx,  0,   0,   0,   0,   0,  TIP,  rz, gz, bz );
    push_line( verts, &idx,  HW,  0,   BASE, 0,   0,  TIP,  rz, gz, bz );
    push_line( verts, &idx, -HW,  0,   BASE, 0,   0,  TIP,  rz, gz, bz );
    push_line( verts, &idx,  0,   HW,  BASE, 0,   0,  TIP,  rz, gz, bz );
    push_line( verts, &idx,  0,  -HW,  BASE, 0,   0,  TIP,  rz, gz, bz );

    /* VAO / VBO */
    glGenVertexArrays( 1, &s_vao );
    glBindVertexArray( s_vao );

    glGenBuffers( 1, &s_vbo );
    glBindBuffer( GL_ARRAY_BUFFER, s_vbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof( verts ), verts, GL_STATIC_DRAW );

    /* attribute 0: position (xyz) */
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE,
                           6 * sizeof( float ), (void *)0 );
    glEnableVertexAttribArray( 0 );

    /* attribute 1: colour (rgb) */
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE,
                           6 * sizeof( float ),
                           (void *)( 3 * sizeof( float ) ) );
    glEnableVertexAttribArray( 1 );

    glBindVertexArray( 0 );

    /* Shader */
    char *vert_src = read_shader_source( "compass.vert" );
    char *frag_src = read_shader_source( "compass.frag" );

    if ( !vert_src || !frag_src ) {
        fprintf( stderr, "r_compass: failed to load shaders\n" );
        free( vert_src );
        free( frag_src );
        return;
    }

    GLuint vs = compile_shader( vert_src, GL_VERTEX_SHADER );
    GLuint fs = compile_shader( frag_src, GL_FRAGMENT_SHADER );
    s_shader  = create_shader_program( vs, fs );

    free( vert_src );
    free( frag_src );

    s_loc_view = glGetUniformLocation( s_shader, "view"       );
    s_loc_proj = glGetUniformLocation( s_shader, "projection" );
}

void r_compass_cleanup( void )
{
    if ( s_vbo )    { glDeleteBuffers( 1, &s_vbo );       s_vbo    = 0; }
    if ( s_vao )    { glDeleteVertexArrays( 1, &s_vao );  s_vao    = 0; }
    if ( s_shader ) { glDeleteProgram( s_shader );         s_shader = 0; }
}

void r_compass_draw( mat4 view, int vpWidth, int vpHeight )
{
    if ( !s_shader || !s_vao ) return;
    ( void )vpHeight;   /* y is fixed at MARGIN from the GL bottom edge */

    /* ── Compass viewport: 90×90 px, bottom-right corner, 12 px margin ── */
    const int SIZE   = 90;
    const int MARGIN = 12;
    int cx = vpWidth  - SIZE - MARGIN;
    int cy = MARGIN;

    /* Save and override GL viewport */
    GLint savedVP[4];
    glGetIntegerv( GL_VIEWPORT, savedVP );
    glViewport( cx, cy, SIZE, SIZE );

    /* Rotation-only view: zero out the translation column */
    mat4 rotView;
    glm_mat4_copy( view, rotView );
    rotView[3][0] = 0.0f;
    rotView[3][1] = 0.0f;
    rotView[3][2] = 0.0f;
    rotView[3][3] = 1.0f;

    /* Orthographic projection to fit the unit axis geometry */
    mat4 ortho;
    glm_ortho( -1.1f, 1.1f, -1.1f, 1.1f, -10.0f, 10.0f, ortho );

    glUseProgram( s_shader );

    /* Draw on top of everything – disable depth test and culling */
    GLboolean depthTest = glIsEnabled( GL_DEPTH_TEST );
    GLboolean cullFace  = glIsEnabled( GL_CULL_FACE  );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_CULL_FACE  );

    if ( s_loc_view != -1 ) glUniformMatrix4fv( s_loc_view, 1, GL_FALSE, (float *)rotView );
    if ( s_loc_proj != -1 ) glUniformMatrix4fv( s_loc_proj, 1, GL_FALSE, (float *)ortho   );

    glLineWidth( 2.5f );
    glBindVertexArray( s_vao );
    glDrawArrays( GL_LINES, 0, COMPASS_VERTS );
    glBindVertexArray( 0 );
    glLineWidth( 1.0f );

    /* Restore GL state */
    if ( depthTest ) glEnable( GL_DEPTH_TEST );
    if ( cullFace  ) glEnable( GL_CULL_FACE  );
    glUseProgram( 0 );

    /* Restore viewport */
    glViewport( savedVP[0], savedVP[1], savedVP[2], savedVP[3] );
}
