/*======================================================================
   File: bone_system.c
Project: shaders
   Author: karlosiric <email@example.com>
   Created: 2025-10-08 11:11:35
   Last Modified by: karlosiric
   Last Modified: 2025-10-11 23:38:57
   ---------------------------------------------------------------------
   Description:
       
   ---------------------------------------------------------------------
   License: 
   Company: 
   Version: 0.1.0
 ======================================================================
                                                                       */

#include "bone_system.h"

#include <stdio.h>
#include <string.h>

mat4 g_bonetransformations[MAXSTUDIOBONES];

static inline void Mat4Copy( mat4 src, mat4 dst )
{
    glm_mat4_copy( src, dst );
}

void AngleQuaternion( const vec3 angles, versor q )
{
    versor qx, qy, qz, temp;
    glm_quatv( qx, angles[0], ( vec3 ) { 1.0f, 0.0f, 0.0f } );
    glm_quatv( qy, angles[1], ( vec3 ) { 0.0f, 1.0f, 0.0f } );
    glm_quatv( qz, angles[2], ( vec3 ) { 0.0f, 0.0f, 1.0f } );

    glm_quat_mul( qz, qy, temp );
    glm_quat_mul( temp, qx, q );
}

void QuaternionMatrix( const versor q, mat4 out )
{
    glm_quat_mat4( q, out );

    out[3][0] = out[3][1] = out[3][2] = 0.0f;

    out[3][3] = 1.0f;
}

void R_ConcatTransforms( const mat4 parent, const mat4 local, mat4 out )
{
    glm_mat4_mul( parent, local, out );
}

void VectorTransforms( const float *in, const vec4 *m, float *out )
{
    vec4 v = { in[0], in[1], in[2], 1.0f };
    vec4 r;
    glm_mat4_mulv( m, v, r );
    out[0] = r[0];
    out[1] = r[1];
    out[2] = r[2];
}

void TransformNormalByBone( const vec4 *boneAbs, const float *in, float *out )
{
    mat3 R;
    glm_mat4_pick3( boneAbs, R );
    glm_mat3_mulv( R, in, out );
    glm_vec3_normalize( out );
}

void SetUpBones( studiohdr_t *header, unsigned char *data )
{
    mstudiobone_t *bones = ( mstudiobone_t * ) ( data + header->boneindex );

    for ( int i = 0; i < header->numbones; i++ )
    {
        vec3 position  = { bones[i].value[0], bones[i].value[1], bones[i].value[2] };
        vec3 euler_rot = { bones[i].value[3], bones[i].value[4], bones[i].value[5] };

        versor q;
        mat4   R;
        AngleQuaternion( euler_rot, q );
        QuaternionMatrix( q, R );

        mat4 local;
        glm_mat4_copy( R, local );

        local[3][0] = position[0];
        local[3][1] = position[1];
        local[3][2] = position[2];

        if ( bones[i].parent >= 0 )
        {
            R_ConcatTransforms( g_bonetransformations[bones[i].parent], local, g_bonetransformations[i] );
        }
        else
        {
            Mat4Copy( local, g_bonetransformations[i] );
        }
    }
}

void TransformVertices( studiohdr_t *header, unsigned char *data, mstudiomodel_t *model, vec3 *out_vertices )
{
    vec3 *vertices = ( vec3 * ) ( data + model->vertindex );

    unsigned char *v2bone = ( unsigned char * ) ( data + model->vertinfoindex );

    for ( int i = 0; i < model->numverts; i++ )
    {
        int bone = v2bone[i];
        if ( bone < 0 || bone >= header->numbones )
        {
            bone = 0;
        }
        VectorTransforms( vertices[i], g_bonetransformations[bone], out_vertices[i] );
    }
}

void SetUpBonesFromAnimation( studiohdr_t *header, unsigned char *data, float anim_bones[MAXSTUDIOBONES][3][4] )
{
    mstudiobone_t *bones = ( mstudiobone_t * ) ( data + header->boneindex );

    for ( int i = 0; i < header->numbones; i++ )
    {
        // Convert 3x4 to 4x4 - CORRECT format (no transpose needed)
        mat4 local;

        // Copy rotation matrix directly (3x3 part)
        local[0][0] = anim_bones[i][0][0];
        local[0][1] = anim_bones[i][0][1];
        local[0][2] = anim_bones[i][0][2];
        local[0][3] = 0.0f;

        local[1][0] = anim_bones[i][1][0];
        local[1][1] = anim_bones[i][1][1];
        local[1][2] = anim_bones[i][1][2];
        local[1][3] = 0.0f;

        local[2][0] = anim_bones[i][2][0];
        local[2][1] = anim_bones[i][2][1];
        local[2][2] = anim_bones[i][2][2];
        local[2][3] = 0.0f;

        // Copy translation (4th column)
        local[3][0] = anim_bones[i][0][3];
        local[3][1] = anim_bones[i][1][3];
        local[3][2] = anim_bones[i][2][3];
        local[3][3] = 1.0f;

        // Concatenate with parent bone transform
        if ( bones[i].parent >= 0 )
        {
            R_ConcatTransforms( g_bonetransformations[bones[i].parent], local, g_bonetransformations[i] );
        }
        else
        {
            Mat4Copy( local, g_bonetransformations[i] );
        }
    }
}
