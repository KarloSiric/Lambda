/*======================================================================
   File: bone_system.c
Project: shaders
   Author: karlosiric <email@example.com>
   Created: 2025-10-08 11:11:35
   Last Modified by: karlosiric
   Last Modified: 2025-10-12 20:03:30
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
    // Valve's AngleQuaternion - uses HALF angles for proper quaternion conversion
    // angles[0] = PITCH (X), angles[1] = YAW (Y), angles[2] = ROLL (Z)
    float angle;
    float sr, sp, sy, cr, cp, cy;

    // Roll (Z) - half angle
    angle = angles[2] * 0.5f;
    sy = sinf(angle);
    cy = cosf(angle);

    // Yaw (Y) - half angle
    angle = angles[1] * 0.5f;
    sp = sinf(angle);
    cp = cosf(angle);

    // Pitch (X) - half angle
    angle = angles[0] * 0.5f;
    sr = sinf(angle);
    cr = cosf(angle);

    // Valve's exact quaternion formula: [X, Y, Z, W]
    q[0] = sr*cp*cy - cr*sp*sy;  // X
    q[1] = cr*sp*cy + sr*cp*sy;  // Y
    q[2] = cr*cp*sy - sr*sp*cy;  // Z
    q[3] = cr*cp*cy + sr*sp*sy;  // W
}

void QuaternionMatrix( const versor q, mat4 out )
{
    // Valve's QuaternionMatrix - converts quaternion to 4x4 rotation matrix
    // CGLM uses column-major format: out[col][row]
    // Valve's formula generates row-major, so we transpose during assignment

    // Row 0 (becomes column 0)
    out[0][0] = 1.0f - 2.0f * q[1] * q[1] - 2.0f * q[2] * q[2];
    out[0][1] = 2.0f * q[0] * q[1] + 2.0f * q[3] * q[2];
    out[0][2] = 2.0f * q[0] * q[2] - 2.0f * q[3] * q[1];
    out[0][3] = 0.0f;

    // Row 1 (becomes column 1)
    out[1][0] = 2.0f * q[0] * q[1] - 2.0f * q[3] * q[2];
    out[1][1] = 1.0f - 2.0f * q[0] * q[0] - 2.0f * q[2] * q[2];
    out[1][2] = 2.0f * q[1] * q[2] + 2.0f * q[3] * q[0];
    out[1][3] = 0.0f;

    // Row 2 (becomes column 2)
    out[2][0] = 2.0f * q[0] * q[2] + 2.0f * q[3] * q[1];
    out[2][1] = 2.0f * q[1] * q[2] - 2.0f * q[3] * q[0];
    out[2][2] = 1.0f - 2.0f * q[0] * q[0] - 2.0f * q[1] * q[1];
    out[2][3] = 0.0f;

    // Translation column (set to 0 for pure rotation)
    out[3][0] = 0.0f;
    out[3][1] = 0.0f;
    out[3][2] = 0.0f;
    out[3][3] = 1.0f;
}

void QuaternionMultiply( const versor q1, const versor q2, versor out )
{
    // Quaternion multiplication: out = q1 * q2
    // This combines two rotations (q1 applied first, then q2)
    versor temp;

    temp[0] = q1[3]*q2[0] + q1[0]*q2[3] + q1[1]*q2[2] - q1[2]*q2[1];  // X
    temp[1] = q1[3]*q2[1] + q1[1]*q2[3] + q1[2]*q2[0] - q1[0]*q2[2];  // Y
    temp[2] = q1[3]*q2[2] + q1[2]*q2[3] + q1[0]*q2[1] - q1[1]*q2[0];  // Z
    temp[3] = q1[3]*q2[3] - q1[0]*q2[0] - q1[1]*q2[1] - q1[2]*q2[2];  // W

    out[0] = temp[0];
    out[1] = temp[1];
    out[2] = temp[2];
    out[3] = temp[3];
}

void QuaternionSlerp( const versor q1, const versor q2, float t, versor out )
{
    glm_quat_slerp( q1, q2, t, out );
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

void TransformVertices( studiohdr_t *header, unsigned char *data, mstudiomodel_t *model, vec3 *out_vertices ){
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
        // Convert row-major 3x4 to column-major 4x4 for CGLM
        // anim_bones[bone][row][col] (row-major from mdl_animations.c)
        // CGLM mat4 is mat[col][row] (column-major)
        mat4 local;

        // Transpose the 3x3 rotation part
        local[0][0] = anim_bones[i][0][0];  // Row 0 -> Column 0
        local[0][1] = anim_bones[i][1][0];
        local[0][2] = anim_bones[i][2][0];
        local[0][3] = 0.0f;

        local[1][0] = anim_bones[i][0][1];  // Row 0 -> Column 1
        local[1][1] = anim_bones[i][1][1];
        local[1][2] = anim_bones[i][2][1];
        local[1][3] = 0.0f;

        local[2][0] = anim_bones[i][0][2];  // Row 0 -> Column 2
        local[2][1] = anim_bones[i][1][2];
        local[2][2] = anim_bones[i][2][2];
        local[2][3] = 0.0f;

        // Translation goes to 4th column
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
