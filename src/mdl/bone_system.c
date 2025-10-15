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
 *   Purpose: Command-Line Argument Parser Implementation
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include "bone_system.h"


#include "../utils/logger.h"
#include <stdio.h>
#include <string.h>

mat4 g_bonetransformations[MAXSTUDIOBONES];

static inline void Mat4Copy( mat4 src, mat4 dst )
{
    glm_mat4_copy( src, dst );
}

void AngleQuaternion( const vec3 angles, versor q )
{
    
    float angle;
    float sr, sp, sy, cr, cp, cy;
    
    
    // Z    
    angle = angles[2] * 0.5f;
    sy = sinf(angle); 
    cy = cosf(angle); 
    
    // Y
    angle = angles[1] * 0.5f;
    sp = sinf(angle);
    cp = cosf(angle);
    
    // X
    angle = angles[0] * 0.5f;
    sr = sinf(angle);
    cr = cosf(angle);
    
    // Build quaternion [X, Y, Z, W]
    q[0] = sr*cp*cy - cr*sp*sy; // X
    q[1] = cr*sp*cy + sr*cp*sy; // Y
    q[2] = cr*cp*sy - sr*sp*cy; // Z
    q[3] = cr*cp*cy + sr*sp*sy; // W
    
}

void QuaternionMatrix( const versor q, mat4 out )
{
    // CGLM mat4 is column-major: mat[column][row]
    // This matches Sam Vanheer's mathlib.cpp QuaternionMatrix implementation
    // When converting row-major to column-major, mat[row][col] becomes mat[col][row]

    float x = q[0], y = q[1], z = q[2], w = q[3];

    // Column 0 (maps to Sam's Row 0)
    out[0][0] = 1.0f - 2.0f * (y*y + z*z);   // Sam's matrix[0][0]
    out[0][1] = 2.0f * (x*y + w*z);          // Sam's matrix[1][0]
    out[0][2] = 2.0f * (x*z - w*y);          // Sam's matrix[2][0]
    out[0][3] = 0.0f;

    // Column 1 (maps to Sam's Row 1)
    out[1][0] = 2.0f * (x*y - w*z);          // Sam's matrix[0][1]
    out[1][1] = 1.0f - 2.0f * (x*x + z*z);   // Sam's matrix[1][1]
    out[1][2] = 2.0f * (y*z + w*x);          // Sam's matrix[2][1]
    out[1][3] = 0.0f;

    // Column 2 (maps to Sam's Row 2)
    out[2][0] = 2.0f * (x*z + w*y);          // Sam's matrix[0][2]
    out[2][1] = 2.0f * (y*z - w*x);          // Sam's matrix[1][2]
    out[2][2] = 1.0f - 2.0f * (x*x + y*y);   // Sam's matrix[2][2]
    out[2][3] = 0.0f;

    // Column 3 (Translation - initialized to zero, will be set by caller)
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

    temp[0] = q1[3] * q2[0] + q1[0] * q2[3] + q1[1] * q2[2] - q1[2] * q2[1];    // X
    temp[1] = q1[3] * q2[1] + q1[1] * q2[3] + q1[2] * q2[0] - q1[0] * q2[2];    // Y
    temp[2] = q1[3] * q2[2] + q1[2] * q2[3] + q1[0] * q2[1] - q1[1] * q2[0];    // Z
    temp[3] = q1[3] * q2[3] - q1[0] * q2[0] - q1[1] * q2[1] - q1[2] * q2[2];    // W

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

void VectorTransforms( const vec3 in, const mat4 m, vec3 out )
{
    vec4 v = { in[0], in[1], in[2], 1.0f };
    vec4 r;
    glm_mat4_mulv( m, v, r );
    out[0] = r[0];
    out[1] = r[1];
    out[2] = r[2];
}

void TransformNormalByBone( const mat4 boneAbs, const vec3 in, vec3 out )
{
    mat3 R;
    glm_mat4_pick3( boneAbs, R );
    glm_mat3_mulv( R, in, out );
    glm_vec3_normalize( out );
}

void SetUpBones( studiohdr_t *header, unsigned char *data )
{
    LOG_DEBUGF("bones", "SetUpBones START: %d bones", header->numbones);
    fflush(stdout);
    
    if (!header || !data) {
        LOG_ERRORF("bones", "NULL parameters to SetUpBones!");
        return;
    }
    
    if (header->numbones <= 0 || header->numbones > MAXSTUDIOBONES) {
        LOG_ERRORF("bones", "Invalid bone count: %d (max: %d)", 
                  header->numbones, MAXSTUDIOBONES);
        return;
    }
    
    LOG_DEBUGF("bones", "Getting bones pointer at offset 0x%X", header->boneindex);
    fflush(stdout);
    
    mstudiobone_t *bones = ( mstudiobone_t * ) ( data + header->boneindex );
    
    LOG_DEBUGF("bones", "Bones pointer: %p", (void*)bones);
    fflush(stdout);

    for ( int i = 0; i < header->numbones; i++ )
    {
        LOG_TRACEF("bones", "  Processing bone %d/%d: %s", i, header->numbones - 1, bones[i].name);
        
        vec3 position  = { bones[i].value[0], bones[i].value[1], bones[i].value[2] };
        vec3 euler_rot = { bones[i].value[3], bones[i].value[4], bones[i].value[5] };
        
        LOG_TRACEF("bones", "    Position: (%.2f, %.2f, %.2f)", position[0], position[1], position[2]);
        LOG_TRACEF("bones", "    Rotation: (%.2f, %.2f, %.2f)", euler_rot[0], euler_rot[1], euler_rot[2]);

        versor q;
        mat4   R;
        
        LOG_TRACEF("bones", "    Converting to quaternion");
        AngleQuaternion( euler_rot, q );
        
        LOG_TRACEF("bones", "    Building rotation matrix");
        QuaternionMatrix( q, R );

        mat4 local;
        glm_mat4_copy( R, local );

        local[3][0] = position[0];
        local[3][1] = position[1];
        local[3][2] = position[2];
        
        LOG_TRACEF("bones", "    Parent bone: %d", bones[i].parent);

        if ( bones[i].parent >= 0 )
        {
            if (bones[i].parent >= header->numbones) {
                LOG_ERRORF("bones", "    Invalid parent %d for bone %d!", 
                          bones[i].parent, i);
                continue;
            }
            
            LOG_TRACEF("bones", "    Concatenating with parent %d", bones[i].parent);
            R_ConcatTransforms( g_bonetransformations[bones[i].parent], local, g_bonetransformations[i] );
        }
        else
        {
            LOG_TRACEF("bones", "    Root bone, copying local transform");
            Mat4Copy( local, g_bonetransformations[i] );
        }
        
        // Log every 5th bone to avoid spam
        if (i % 5 == 0 || i == header->numbones - 1) {
            LOG_DEBUGF("bones", "  Processed bone %d/%d", i, header->numbones - 1);
            fflush(stdout);
        }
    }
    
    LOG_DEBUGF("bones", "SetUpBones COMPLETE: %d bones processed", header->numbones);
    fflush(stdout);
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

// NOTE: SetUpBonesFromAnimation was removed because it had incorrect matrix conversion logic.
// Use mdl_animation_calculate_bones() from mdl_animations.c instead, which correctly
// handles bone transformations using quaternions.
