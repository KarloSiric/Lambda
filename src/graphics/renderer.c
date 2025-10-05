/*======================================================================
 *  File: renderer.c
 *  Project: shaders
 *  Author: karlosiric <email@example.com>
 *  Created: 2025-09-24 14:22:30
 *  Last Modified by: karlosiric
 *  Last Modified: 2025-10-05 19:51:55
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

#include "../graphics/textures.h"
#include "../mdl/bodypart_manager.h"
#include "../mdl/bone_system.h"

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/gl3.h>
#endif

// Diagnostic function will be added below
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>                        // For getcwd
#define MAX_DRAW_RANGES 4096

typedef struct
{
    mstudiomodel_t *model;
    vec3_t         *vertices;
    vec3_t         *normals;
    int             vertex_count;
    int             normal_count;
} current_model_data_t;

typedef struct
{
    GLuint tex;                          // GL texture to bind
    int    first;                        // first vertex in the big VBO
    int    count;                        // how many vertices to draw
} DrawRange;

static DrawRange g_ranges[MAX_DRAW_RANGES];
static int       g_num_ranges = 0;

static current_model_data_t g_current;

static GLuint g_white_tex = 0;

static vec3 skinned_positions[MAXSTUDIOVERTS];
static bool have_skinned_positions = false;

GLFWwindow *window            = NULL;
static bool wireframe_enabled = false;

static unsigned int VBO             = 0;
static unsigned int VAO             = 0;
static unsigned int EBO             = 0;                        // Element Buffer Object for indices
static unsigned int shader_program  = 0;
static unsigned int current_texture = 0;                        // Currently bound texture

// For Bone data
static int            m_numbones = 0;
static int            m_numverts = 0;
static mat4           m_bonetransform[MAXSTUDIOBONES];
static vec3           m_pxformverts[MAXSTUDIOVERTS];
static unsigned char *m_pvertbone;
static vec3          *m_pstudioverts;

extern float rotation_x;
extern float rotation_y;
extern float zoom;

// Model data
static float *model_vertices          = NULL;
static int    vertex_count            = 0;
static int    index_count             = 0;
static bool   debug_printed           = false;
static bool   bone_system_initialized = false;

// PRE-ALLOCATED BUFFERS (NO MALLOC IN RENDER LOOP)
#define MAX_RENDER_VERTICES 32768
static float render_vertex_buffer[MAX_RENDER_VERTICES * 8];                        // 3 pos + 3 normal + 2 uv
static int   total_render_vertices = 0;
static bool  model_processed       = false;

static studiohdr_t   *global_header = NULL;
static unsigned char *global_data   = NULL;

static studiohdr_t   *global_tex_header = NULL;
static unsigned char *global_tex_data   = NULL;

static mdl_texture_set_t g_textures = { NULL, 0 };

// Camera controls
float rotation_x = 0.0f;
float rotation_y = 0.0f;
float zoom       = 0.15f;                        // Even more zoomed out for scientist model

static bool   mouse_pressed = false;
static double last_x = 400, last_y = 225;

static void glfw_mouse_callback( GLFWwindow *window, double xpos, double ypos ) {
    if ( mouse_pressed )
    {
        float xoffset = xpos - last_x;
        float yoffset = ypos - last_y;

        rotation_y += xoffset * 0.01f;
        rotation_x -= yoffset * 0.01f;
    }

    last_x = xpos;
    last_y = ypos;
}

static void glfw_mouse_button_callback( GLFWwindow *window, int button, int action, int mods ) {
    if ( button == GLFW_MOUSE_BUTTON_LEFT )
    {
        mouse_pressed = ( action == GLFW_PRESS );
    }
}

static void glfw_scroll_callback( GLFWwindow *window, double xoffset, double yoffset ) {
    zoom *= ( 1.0f + yoffset * 0.1f );
    if ( zoom < 0.01f )
        zoom = 0.01f;
    if ( zoom > 2.0f )
        zoom = 2.0f;
}

static void glfw_error_callback( int error, const char *description ) {
    fprintf( stderr, "GLFW ERROR %d: %s\n", error, description );
}

static void glfw_key_callback( GLFWwindow *window, int key, int scancode, int action, int mods ) {
    ( void ) scancode;                        // Suppress unused parameter warning
    ( void ) mods;                            // Suppress unused parameter warning

    if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
    {
        glfwSetWindowShouldClose( window, GLFW_TRUE );
    }

    // Camera controls
    if ( action == GLFW_PRESS || action == GLFW_REPEAT )
    {
        switch ( key )
        {
        case GLFW_KEY_W:
            rotation_x -= 0.1f;
            break;                        // Tilt up
        case GLFW_KEY_S:
            rotation_x += 0.1f;
            break;                        // Tilt down
        case GLFW_KEY_A:
            rotation_y -= 0.1f;
            break;                        // Rotate left
        case GLFW_KEY_D:
            rotation_y += 0.1f;
            break;                        // Rotate right
        case GLFW_KEY_Q:
            zoom *= 1.1f;
            if ( zoom > 2.0f )
                zoom = 2.0f;
            break;                        // Zoom in with limit
        case GLFW_KEY_E:
            zoom *= 0.9f;
            if ( zoom < 0.1f )
                zoom = 0.1f;
            break;                              // Zoom out with limit
        case GLFW_KEY_R:                        // Reset view
            rotation_x = 0.0f;
            rotation_y = 0.0f;
            zoom       = 0.15f;                        // Reset to default zoom
            break;
        case GLFW_KEY_F:                        // Toggle wireframe
            wireframe_enabled = !wireframe_enabled;
            if ( wireframe_enabled )
            {
                glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
                printf( "Switched to wireframe mode\n" );
            }
            else
            {
                glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
                printf( "Switched to solid mode\n" );
            }
            break;
        case GLFW_KEY_P:                        // Toggle points
            glPolygonMode( GL_FRONT_AND_BACK, GL_POINT );
            printf( "Switched to points mode\n" );
            break;
        }
    }
}

float vertices[] = {
    -0.5f, -0.5f, 0.0f,                        // Bottom left
    0.5f,
    -0.5f,
    0.0f,                        // Bottom right
    0.0f,
    0.5f,
    0.0f                        // Top center
};

// DIAGNOSTIC FUNCTION TO UNDERSTAND THE MODEL DATA
void dump_complete_mdl_structure( void ) {
    if ( !global_header || !global_data )
        return;

    printf( "\n===============================================\n" );
    printf( "COMPLETE MDL STRUCTURE DIAGNOSTIC\n" );
    printf( "===============================================\n" );

    // 1. Header info
    printf( "\n1. HEADER INFO:\n" );
    printf( "   Model name: %s\n", global_header->name );
    printf( "   File size: %d bytes\n", global_header->length );
    printf( "   Bounding box: (%.2f,%.2f,%.2f) to (%.2f,%.2f,%.2f)\n", global_header->bbmin[0], global_header->bbmin[1], global_header->bbmin[2], global_header->bbmax[0], global_header->bbmax[1], global_header->bbmax[2] );
    printf( "   BBox dimensions: %.2f x %.2f x %.2f\n", global_header->bbmax[0] - global_header->bbmin[0], global_header->bbmax[1] - global_header->bbmin[1], global_header->bbmax[2] - global_header->bbmin[2] );

    // 2. Bodyparts
    printf( "\n2. BODYPARTS (%d total):\n", global_header->numbodyparts );
    mstudiobodyparts_t *bodyparts = ( mstudiobodyparts_t * ) ( global_data + global_header->bodypartindex );

    for ( int bp = 0; bp < global_header->numbodyparts && bp < 2; bp++ )
    {
        printf( "\n   Bodypart %d: '%s'\n", bp, bodyparts[bp].name );
        printf( "      Models: %d\n", bodyparts[bp].nummodels );

        mstudiomodel_t *models = ( mstudiomodel_t * ) ( global_data + bodyparts[bp].modelindex );
        mstudiomodel_t *model  = &models[0];
        printf( "      Model 0: '%s'\n", model->name );
        printf( "         Vertices: %d at offset 0x%X\n", model->numverts, model->vertindex );

        // Check actual vertex data
        vec3_t *verts = ( vec3_t * ) ( global_data + model->vertindex );

        // Find min/max of raw vertices
        float min_x = 10000, max_x = -10000;
        float min_y = 10000, max_y = -10000;
        float min_z = 10000, max_z = -10000;

        for ( int v = 0; v < model->numverts; v++ )
        {
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
        for ( int v = 0; v < 3 && v < model->numverts; v++ )
        {
            printf( "            V%d: (%.6f, %.6f, %.6f)\n", v, verts[v][0], verts[v][1], verts[v][2] );
        }

        // Check hex data
        unsigned char *as_bytes = ( unsigned char * ) ( global_data + model->vertindex );
        printf( "         First 12 bytes at vertex offset: " );
        for ( int i = 0; i < 12; i++ )
        {
            printf( "%02X ", as_bytes[i] );
        }
        printf( "\n" );
    }

    printf( "\n===============================================\n" );
}

// Updated ProcessModelForRendering to extract normals and UVs
void ProcessModelForRendering( void ) {
    if ( !global_header || !global_data )
    {
        fprintf( stderr, "ERROR - Invalid argument pointers value passed!\n" );
        return;
    }

    total_render_vertices = 0;
    g_num_ranges          = 0;

    mstudiobodyparts_t *bodyparts = ( mstudiobodyparts_t * ) ( global_data + global_header->bodypartindex );

    // Build bones (pose) once before drawing
    SetUpBones( global_header, global_data );

    // Iterate through all bodyparts
    for ( int bp = 0; bp < global_header->numbodyparts; ++bp )
    {
        mstudiobodyparts_t *bpRec  = &bodyparts[bp];
        mstudiomodel_t     *models = ( mstudiomodel_t * ) ( global_data + bpRec->modelindex );

        // GET ONLY THE SELECTED MODEL FOR THIS BODYPART
        int selected_model_index = bodypart_get_model_index( bp );

        // Safety check
        if ( selected_model_index < 0 || selected_model_index >= bpRec->nummodels )
        {
            selected_model_index = 0;                        // Fallback to first model
        }

        // Get ONLY the selected model (not all of them!)
        mstudiomodel_t *model = &models[selected_model_index];

        // Debug output
        printf( "Bodypart %d '%s': Using model %d/%d '%s'\n", bp, bpRec->name, selected_model_index, bpRec->nummodels, model->name );

        g_current.model        = model;
        g_current.vertices     = ( vec3_t * ) ( global_data + model->vertindex );
        g_current.normals      = ( vec3_t * ) ( global_data + model->normindex );
        g_current.vertex_count = model->numverts;
        g_current.normal_count = model->numnorms;

        // Skin this model's vertices (fills skinned_positions[])
        TransformVertices( global_header, global_data, model, skinned_positions );
        have_skinned_positions = true;

        // All meshes for this model
        mstudiomesh_t *meshes = ( mstudiomesh_t * ) ( global_data + model->meshindex );

        // Skin table
        const short *skin_table  = ( const short * ) ( global_data + global_header->skinindex );
        const int    numskinref  = global_header->numskinref;
        const int    skin_family = 0;

        for ( int mesh = 0; mesh < model->nummesh; ++mesh )
        {
            const int norm_base = meshes[mesh].normindex;
            const int v_count   = g_current.vertex_count;
            const int n_count   = g_current.normal_count;

            // Resolve texture index via skin table
            int tex_index = meshes[mesh].skinref;
            if ( skin_table && numskinref > 0 && tex_index >= 0 && tex_index < numskinref )
            {
                tex_index = skin_table[skin_family * numskinref + tex_index];
            }

            // GL texture + size
            GLuint gl_tex = 0;
            int    texW = 1, texH = 1;
            if ( tex_index >= 0 && tex_index < g_textures.count )
            {
                gl_tex = g_textures.textures[tex_index].gl_id;
                texW   = g_textures.textures[tex_index].width;
                texH   = g_textures.textures[tex_index].height;

                if ( texW <= 0 )
                    texW = 1;
                if ( texH <= 0 )
                    texH = 1;
            }
            if ( !gl_tex && g_white_tex )
            {
                gl_tex = g_white_tex;
                texW   = 2;
                texH   = 2;
            }

            printf( "  mesh %d: skinref %d -> tex %d  GL=%u  %dx%d\n", mesh, meshes[mesh].skinref, tex_index, ( unsigned ) gl_tex, texW, texH );

            short *ptricmds = ( short * ) ( global_data + meshes[mesh].triindex );

            const int start_first = total_render_vertices;

            int s_min = 32767, s_max = -32768;
            int t_min = 32767, t_max = -32768;
            int dbg_count = 0;

            int i;
            while ( ( i = *( ptricmds++ ) ) )
            {
                if ( i < 0 )
                {
                    // -------- triangle fan --------
                    i = -i;

                    // read first 2 vertices
                    short v0 = ptricmds[0];
                    short n0 = ptricmds[1];
                    short s0 = ptricmds[2];
                    short t0 = ptricmds[3];
                    ptricmds = ( short * ) ( ( char * ) ptricmds + 4 * sizeof( short ) );

                    short v1 = ptricmds[0];
                    short n1 = ptricmds[1];
                    short s1 = ptricmds[2];
                    short t1 = ptricmds[3];
                    ptricmds = ( short * ) ( ( char * ) ptricmds + 4 * sizeof( short ) );

                    // >>> FIX: apply ON-SEAM rule and clear high bit on normal index
                    if ( n0 & 0x8000 )
                    {
                        s0 = ( short ) ( s0 + texW / 2 );
                    }
                    n0 &= 0x7FFF;
                    if ( n1 & 0x8000 )
                    {
                        s1 = ( short ) ( s1 + texW / 2 );
                    }
                    n1 &= 0x7FFF;

                    // norm indices are relative to this mesh's block
                    n0 = ( short ) ( n0 + norm_base );
                    n1 = ( short ) ( n1 + norm_base );

                    for ( int j = 2; j < i; ++j )
                    {
                        short v2 = ptricmds[0];
                        short n2 = ptricmds[1];
                        short s2 = ptricmds[2];
                        short t2 = ptricmds[3];
                        ptricmds = ( short * ) ( ( char * ) ptricmds + 4 * sizeof( short ) );

                        // >>> FIX: apply ON-SEAM rule and clear high bit
                        if ( n2 & 0x8000 )
                        {
                            s2 = ( short ) ( s2 + texW / 2 );
                        }
                        n2 &= 0x7FFF;

                        // update min/max + quick sample print
                        if ( dbg_count++ < 8 )
                            printf( "    s=%d t=%d  (tex %dx%d)\n", s0, t0, texW, texH );
                        s_min = ( s0 < s_min ) ? s0 : s_min;
                        s_max = ( s0 > s_max ) ? s0 : s_max;
                        t_min = ( t0 < t_min ) ? t0 : t_min;
                        t_max = ( t0 > t_max ) ? t0 : t_max;

                        // normals block offset
                        n2 = ( short ) ( n2 + norm_base );

                        // bounds guards
                        if ( v0 >= 0 && v0 < v_count && n0 >= 0 && n0 < n_count && v1 >= 0 && v1 < v_count && n1 >= 0 && n1 < n_count && v2 >= 0 && v2 < v_count && n2 >= 0 && n2 < n_count )
                        {
                            AddVertexToBuffer( v0, n0, s0, t0, ( float ) texW, ( float ) texH );
                            AddVertexToBuffer( v1, n1, s1, t1, ( float ) texW, ( float ) texH );
                            AddVertexToBuffer( v2, n2, s2, t2, ( float ) texW, ( float ) texH );
                        }

                        // roll forward
                        v1 = v2;
                        n1 = n2;
                        s1 = s2;
                        t1 = t2;
                    }
                }
                else
                {
                    // -------- triangle strip --------

                    // read first 2 vertices
                    short v0 = ptricmds[0];
                    short n0 = ptricmds[1];
                    short s0 = ptricmds[2];
                    short t0 = ptricmds[3];
                    ptricmds = ( short * ) ( ( char * ) ptricmds + 4 * sizeof( short ) );

                    short v1 = ptricmds[0];
                    short n1 = ptricmds[1];
                    short s1 = ptricmds[2];
                    short t1 = ptricmds[3];
                    ptricmds = ( short * ) ( ( char * ) ptricmds + 4 * sizeof( short ) );

                    // >>> FIX: seam + clear high bit
                    if ( n0 & 0x8000 )
                    {
                        s0 = ( short ) ( s0 + texW / 2 );
                    }
                    n0 &= 0x7FFF;
                    if ( n1 & 0x8000 )
                    {
                        s1 = ( short ) ( s1 + texW / 2 );
                    }
                    n1 &= 0x7FFF;

                    n0 = ( short ) ( n0 + norm_base );
                    n1 = ( short ) ( n1 + norm_base );

                    for ( int j = 2; j < i; ++j )
                    {
                        short v2 = ptricmds[0];
                        short n2 = ptricmds[1];
                        short s2 = ptricmds[2];
                        short t2 = ptricmds[3];
                        ptricmds = ( short * ) ( ( char * ) ptricmds + 4 * sizeof( short ) );

                        // >>> FIX: seam + clear high bit
                        if ( n2 & 0x8000 )
                        {
                            s2 = ( short ) ( s2 + texW / 2 );
                        }
                        n2 &= 0x7FFF;

                        if ( dbg_count++ < 8 )
                            printf( "    s=%d t=%d  (tex %dx%d)\n", s0, t0, texW, texH );
                        s_min = ( s0 < s_min ) ? s0 : s_min;
                        s_max = ( s0 > s_max ) ? s0 : s_max;
                        t_min = ( t0 < t_min ) ? t0 : t_min;
                        t_max = ( t0 > t_max ) ? t0 : t_max;

                        n2 = ( short ) ( n2 + norm_base );

                        // two-triangle parity
                        if ( ( j - 2 ) % 2 == 0 )
                        {
                            if ( v0 >= 0 && v0 < v_count && n0 >= 0 && n0 < n_count && v1 >= 0 && v1 < v_count && n1 >= 0 && n1 < n_count && v2 >= 0 && v2 < v_count && n2 >= 0 && n2 < n_count )
                            {
                                AddVertexToBuffer( v0, n0, s0, t0, ( float ) texW, ( float ) texH );
                                AddVertexToBuffer( v1, n1, s1, t1, ( float ) texW, ( float ) texH );
                                AddVertexToBuffer( v2, n2, s2, t2, ( float ) texW, ( float ) texH );
                            }
                        }
                        else
                        {
                            if ( v0 >= 0 && v0 < v_count && n0 >= 0 && n0 < n_count && v1 >= 0 && v1 < v_count && n1 >= 0 && n1 < n_count && v2 >= 0 && v2 < v_count && n2 >= 0 && n2 < n_count )
                            {
                                AddVertexToBuffer( v1, n1, s1, t1, ( float ) texW, ( float ) texH );
                                AddVertexToBuffer( v0, n0, s0, t0, ( float ) texW, ( float ) texH );
                                AddVertexToBuffer( v2, n2, s2, t2, ( float ) texW, ( float ) texH );
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

            // One draw range for this mesh
            if ( g_num_ranges < MAX_DRAW_RANGES )
            {
                g_ranges[g_num_ranges].tex   = gl_tex;
                g_ranges[g_num_ranges].first = start_first;
                g_ranges[g_num_ranges].count = total_render_vertices - start_first;
                g_num_ranges++;
            }
        }
    }

    printf( "Generated %d vertices (%d triangles) for bodygroup %d\n", total_render_vertices, total_render_vertices / 3, bodypart_get_bodygroup( ) );

    model_processed = true;
}

void AddVertexToBuffer( int vertex_index, int normal_index, short s, short t, float texW, float texH ) {
    if ( total_render_vertices >= MAX_RENDER_VERTICES )
        return;

    const int base = total_render_vertices * 8;

    /* ----- POSITION (your skinning path) ----- */
    vec3 P;
    if ( have_skinned_positions )
    {
        glm_vec3_copy( skinned_positions[vertex_index], P );
    }
    else
    {
        glm_vec3_copy( g_current.vertices[vertex_index], P );
    }

    const float viewer_scale = 0.1f;
    P[0] *= viewer_scale;
    P[1] *= viewer_scale;
    P[2] *= viewer_scale;

    /* ----- NORMAL (your bone transform) ----- */
    unsigned char *v2bone = ( unsigned char * ) ( global_data + g_current.model->vertinfoindex );
    int            bone   = v2bone ? v2bone[vertex_index] : 0;
    if ( bone < 0 || bone >= global_header->numbones )
        bone = 0;

    vec3 Nfile = {
        g_current.normals[normal_index][0],
        g_current.normals[normal_index][1],
        g_current.normals[normal_index][2] };
    vec3 Nrot;
    TransformNormalByBone( g_bonetransformations[bone], Nfile, Nrot );

    /* ----- AXIS REMAP (unchanged) ----- */
    float x  = P[0];
    float y  = P[1];
    float z  = P[2];
    float nx = Nrot[0];
    float ny = Nrot[1];
    float nz = Nrot[2];

    float Py = z;                         // Z -> Y
    float Pz = -y;                        // -Y -> Z
    float Ny = nz;
    float Nz = -ny;

    /* write position */
    render_vertex_buffer[base + 0] = x;
    render_vertex_buffer[base + 1] = Py;
    render_vertex_buffer[base + 2] = Pz;

    /* write normal */
    render_vertex_buffer[base + 3] = nx;
    render_vertex_buffer[base + 4] = Ny;
    render_vertex_buffer[base + 5] = Nz;

    /* ----- UVs: USE invW/invH PER TEXTURE (this is the fix) ----- */
    // s,t come from the tri-cmds as 16-bit texel coords for THIS mesh’s texture.
    // Convert to [0,1], sample at texel centers (+0.5), flip V for GL.

    /* s,t are 16-bit *texel* coords for THIS texture */
    float u = ( ( float ) s + 0.5f ) / ( float ) texW;
    float v = ( ( float ) t + 0.5f ) / ( float ) texH;

    // Debug UV mapping for specific textures
    if ( total_render_vertices < 50 )
    {                        // Only print first few to avoid spam
        printf( "Vertex %d: tex dims %fx%f, s=%d t=%d -> u=%.3f v=%.3f", total_render_vertices, texW, texH, s, t, u, v );
    }

    // v = 1.0f - v;

    if ( total_render_vertices < 50 )
    {
        printf( " -> v_flipped=%.3f\n", v );
    }

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

    total_render_vertices++;
}

void setup_triangle( void ) {
    glGenBuffers( 1, &VBO );
    glBindBuffer( GL_ARRAY_BUFFER, VBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

    glGenVertexArrays( 1, &VAO );
    glBindVertexArray( VAO );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ), ( void * ) 0 );
    glEnableVertexAttribArray( 0 );
}

static char *read_shader_source( const char *filepath ) {
    printf( "DEBUG - Trying to load shader from: %s\n", filepath );
    FILE *file = fopen( filepath, "r" );
    if ( !file )
    {
        fprintf( stderr, "ERROR - Failed to open the shader file: '%s' \n", filepath );
        return NULL;
    }

    fseek( file, 0, SEEK_END );
    size_t length = ftell( file );
    rewind( file );

    char *buffer = malloc( ( length + 1 ) * sizeof( char ) );

    if ( !buffer )
    {
        fprintf( stderr, "ERROR - Failed to allocate memory for shader file buffer!\n" );
        fclose( file );
        return NULL;
    }

    fread( buffer, 1, length, file );
    buffer[length] = '\0';                        // Null terminate
    fclose( file );

    return buffer;
}

static GLuint compile_shader( const char *source, GLenum type ) {
    GLuint shader = glCreateShader( type );
    glShaderSource( shader, 1, &source, NULL );
    glCompileShader( shader );

    GLint success;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &success );

    if ( !success )
    {
        char shader_info[512];
        glGetShaderInfoLog( shader, 512, NULL, shader_info );
        fprintf( stderr, "ERROR - Failed to compile shaders: \n%s\n", shader_info );
        return ( 0 );
    }

    return shader;
}

static GLuint create_shader_program( GLuint vertexShader, GLuint fragmentShader ) {
    GLuint program = glCreateProgram( );
    glAttachShader( program, vertexShader );
    glAttachShader( program, fragmentShader );
    glLinkProgram( program );

    GLint success;
    glGetProgramiv( program, GL_LINK_STATUS, &success );                        // Use glGetProgramiv for program
    if ( !success )
    {
        char infoLog[512];
        glGetProgramInfoLog( program, 512, NULL, infoLog );
        fprintf( stderr, "ERROR - Failed to link shaders to the program!\n%s\n", infoLog );
        return ( 0 );
    }

    glDeleteShader( vertexShader );
    glDeleteShader( fragmentShader );

    return program;
}

static int load_shaders( void ) {
    // Debug: Print current working directory
    char cwd[1024];
    if ( getcwd( cwd, sizeof( cwd ) ) != NULL )
    {
        printf( "DEBUG - Current working directory: %s\n", cwd );
    }

    // Try to load textured shaders first, fall back to basic if not found
    char *vertex_shader_file   = read_shader_source( "/Users/karlosiric/Documents/SublimeText Programming/C_Projects/ModelViewer/shaders/textured.vert" );
    char *fragment_shader_file = read_shader_source( "/Users/karlosiric/Documents/SublimeText Programming/C_Projects/ModelViewer/shaders/textured.frag" );

    if ( !vertex_shader_file || !fragment_shader_file )
    {
        printf( "Textured shaders not found, falling back to basic shaders\n" );
        if ( vertex_shader_file )
            free( vertex_shader_file );
        if ( fragment_shader_file )
            free( fragment_shader_file );

        vertex_shader_file   = read_shader_source( "/Users/karlosiric/Documents/SublimeText Programming/C_Projects/ModelViewer/shaders/basic.vert" );
        fragment_shader_file = read_shader_source( "/Users/karlosiric/Documents/SublimeText Programming/C_Projects/ModelViewer/shaders/basic.frag" );
    }
    else
    {
        printf( "Using textured shaders\n" );
    }

    if ( !vertex_shader_file || !fragment_shader_file )
    {
        fprintf( stderr, "ERROR - Failed to load shader files!\n" );
        if ( vertex_shader_file )
            free( vertex_shader_file );
        if ( fragment_shader_file )
            free( fragment_shader_file );
        return ( -1 );
    }
    printf( "Compiling vertex shader...\n" );
    GLuint vertexShader = compile_shader( vertex_shader_file, GL_VERTEX_SHADER );
    printf( "Compiling fragment shader...\n" );
    GLuint fragmentShader = compile_shader( fragment_shader_file, GL_FRAGMENT_SHADER );

    free( vertex_shader_file );
    free( fragment_shader_file );

    if ( vertexShader == 0 || fragmentShader == 0 )
    {
        return ( -1 );
    }

    shader_program = create_shader_program( vertexShader, fragmentShader );

    if ( shader_program == 0 )
    {
        fprintf( stderr, "ERROR - Failed to create properly a shader program!\n" );
        return ( -1 );
    }

    printf( "Shaders loaded and compiled successfully!\n" );
    printf( "Shader program up and ready to be used!\n" );

    return ( 0 );
}

int init_renderer( int width, int height, const char *title ) {
    if ( !glfwInit( ) )
    {
        fprintf( stderr, "ERROR - Failed to initialize the GLFW, glfw_init() FAILED!\n" );
        return ( -1 );
    }

#ifdef __APPLE__
    /* MacOS M chips specific */
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
#elif _WIN32
    /* Windows: Can go up to the latest version supported by drivers */
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 5 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
#else
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 5 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
#endif

    GLFWmonitor *primary = glfwGetPrimaryMonitor( );
    ( void ) primary;                        // We get this for future use but don't use it yet
    window = glfwCreateWindow( width, height, title, NULL, NULL );

    if ( !window )
    {
        glfwTerminate( );
        return ( -1 );
    }

    glfwMakeContextCurrent( window );

    glfwSetKeyCallback( window, glfw_key_callback );
    glfwSetErrorCallback( glfw_error_callback );

    // Add mouse controls
    glfwSetCursorPosCallback( window, glfw_mouse_callback );
    glfwSetMouseButtonCallback( window, glfw_mouse_button_callback );
    glfwSetScrollCallback( window, glfw_scroll_callback );

    glEnable( GL_DEPTH_TEST );
    glViewport( 0, 0, width, height );
    // DISABLE CULLING TO SEE IF TRIANGLES ARE BACKWARDS
    glDisable( GL_CULL_FACE );                        // Changed from glEnable

    // Enable point size for vertex visualization
    glEnable( GL_PROGRAM_POINT_SIZE );
    glPointSize( 5.0f );                        // Make points bigger

    setup_triangle( );

    if ( load_shaders( ) != 0 )
    {
        fprintf( stderr, "ERROR - Failed to load shaders!\n" );
        return ( -1 );
    }

    // Fallback 2x2 white texture so meshes always draw
    glGenTextures( 1, &g_white_tex );
    glBindTexture( GL_TEXTURE_2D, g_white_tex );
    unsigned char white[] = {
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 };

    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, white );
    glBindTexture( GL_TEXTURE_2D, 0 );

    printf( "White fallback texture created with GL ID: %u\n", g_white_tex );

    printf( "OpenGL Version: %s\n", glGetString( GL_VERSION ) );

    printf( "\n=== CONTROLS ===\n" );
    printf( "W/S: Tilt up/down\n" );
    printf( "A/D: Rotate left/right\n" );
    printf( "Q/E: Zoom in/out\n" );
    printf( "F: Toggle wireframe/solid\n" );
    printf( "P: Points mode\n" );
    printf( "R: Reset view\n" );
    printf( "ESC: Exit\n" );
    printf( "================\n\n" );

    return ( 0 );
}

void cleanup_renderer( void ) {
    if ( VAO )
        glDeleteVertexArrays( 1, &VAO );
    if ( VBO )
        glDeleteBuffers( 1, &VBO );
    if ( EBO )
        glDeleteBuffers( 1, &EBO );
    if ( shader_program )
        glDeleteProgram( shader_program );

    if ( window )
    {
        glfwDestroyWindow( window );
        printf( "Shutting down Window ...\n" );
    }

    glfwTerminate( );
    printf( "GLFW Fully Terminated!\n" );
    return;
}

void clear_screen( void ) {
    glClearColor( 0.1f, 0.2f, 0.45f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
}

bool should_close_window( void ) {
    return glfwWindowShouldClose( window );
}

void render_loop( void ) {
    printf( "Starting render loop...\n" );

    while ( !should_close_window( ) )
    {
        clear_screen( );

        if ( global_header && global_data )
        {
            render_model( global_header, global_data );
        }

        glfwSwapBuffers( window );
        glfwPollEvents( );
    }
}

void set_wireframe_mode( bool enabled ) {
    wireframe_enabled = enabled;                        // Now it's used

    if ( wireframe_enabled )
    {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    }
    else
    {
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }
}

void set_current_texture( unsigned int texture_id ) {
    current_texture = texture_id;
    printf( "Set current texture to ID: %u\n", texture_id );
}

// Updated render_model to use the new vertex format
void render_model( studiohdr_t *header, unsigned char *data ) {
    ( void ) header;
    ( void ) data;

    if ( !model_processed )
    {
        ProcessModelForRendering( );
    }
    if ( total_render_vertices == 0 )
        return;

    glUseProgram( shader_program );

    /* --- Build matrices (cglm) --- */
    int fbw, fbh;
    glfwGetFramebufferSize( window, &fbw, &fbh );
    float aspect = ( fbh > 0 ) ? ( float ) fbw / ( float ) fbh : 1.0f;

    mat4 M;
    glm_mat4_identity( M );
    /* viewer rotations (Y then X) */
    glm_rotate( M, rotation_y, ( vec3 ) { 0.0f, 1.0f, 0.0f } );
    glm_rotate( M, rotation_x, ( vec3 ) { 1.0f, 0.0f, 0.0f } );
    
    /* simple camera */
    float camDist = 5.0f / ( zoom > 0.001f ? zoom : 0.001f );
    vec3  camPos  = { 0.0f, 0.0f, camDist };
    vec3  target  = { 0.0f, 3.0f, 0.0f };
    vec3  up      = { 0.0f, 2.0f, 0.0f };

    mat4 V;
    glm_lookat( camPos, target, up, V );
    mat4 P;
    glm_perspective( glm_rad( 50.0f ), aspect, 0.01f, 1000.0f, P );

    /* upload required uniforms (names MUST match your shaders) */
    GLint uModel = glGetUniformLocation( shader_program, "model" );
    GLint uView  = glGetUniformLocation( shader_program, "view" );
    GLint uProj  = glGetUniformLocation( shader_program, "projection" );
    if ( uModel != -1 )
        glUniformMatrix4fv( uModel, 1, GL_FALSE, ( const float * ) M );
    if ( uView != -1 )
        glUniformMatrix4fv( uView, 1, GL_FALSE, ( const float * ) V );
    if ( uProj != -1 )
        glUniformMatrix4fv( uProj, 1, GL_FALSE, ( const float * ) P );

    /* light + camera uniforms */
    vec3  lightPos = { 3.0f, 5.0f, 4.0f };
    GLint uLight   = glGetUniformLocation( shader_program, "lightPos" );
    GLint uViewP   = glGetUniformLocation( shader_program, "viewPos" );
    if ( uLight != -1 )
        glUniform3fv( uLight, 1, ( const float * ) lightPos );
    if ( uViewP != -1 )
        glUniform3fv( uViewP, 1, ( const float * ) camPos );
    // Upload vertex data (positions, normals, uvs)
    glBindVertexArray( VAO );
    glBindBuffer( GL_ARRAY_BUFFER, VBO );
    glBufferData( GL_ARRAY_BUFFER, ( GLsizeiptr ) ( total_render_vertices * 8 * sizeof( float ) ), render_vertex_buffer, GL_STATIC_DRAW );

    // vertex attribs: pos(0), normal(1), uv(2)
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof( float ), ( void * ) ( 0 ) );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof( float ), ( void * ) ( 3 * sizeof( float ) ) );
    glEnableVertexAttribArray( 1 );
    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof( float ), ( void * ) ( 6 * sizeof( float ) ) );
    glEnableVertexAttribArray( 2 );

    // bind sampler to unit 0 (once)
    // bind sampler (once before loop)
    GLint uTex = glGetUniformLocation( shader_program, "tex" );
    if ( uTex != -1 )
        glUniform1i( uTex, 0 );

    for ( int r = 0; r < g_num_ranges; ++r )
    {
        GLuint tex_to_bind = g_ranges[r].tex ? g_ranges[r].tex : g_white_tex;
        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, tex_to_bind );
        glDrawArrays( GL_TRIANGLES, g_ranges[r].first, g_ranges[r].count );
    }
}

void set_model_data( studiohdr_t *header, unsigned char *data, studiohdr_t *tex_header, unsigned char *tex_data ) {
    global_header     = header;
    global_data       = data;
    global_tex_header = tex_header;                        // may be NULL
    global_tex_data   = tex_data;                          // may be NULL

    model_processed         = false;
    bone_system_initialized = false;
    total_render_vertices   = 0;

    // Free old textures
    if ( g_textures.textures )
    {
        mdl_free_texture( &g_textures );
    }

    // Pick which header to use (embedded or T.mdl)
    const studiohdr_t *texHdr = mdl_pick_texture_header( header, tex_header );
    if ( texHdr )
    {
        mdl_result_t texRes = mdl_load_textures( texHdr, ( texHdr == header ) ? data : tex_data, &g_textures );
        if ( texRes == MDL_SUCCESS )
        {
            printf( "Loaded %d textures\n", g_textures.count );
        }
        else
        {
            printf( "Failed to load textures, error code: %d\n", texRes );
        }
    }
    else
    {
        printf( "No textures found for this model.\n" );
    }

    printf( "Model data set, will be processed on next render\n" );
}
