/*
 * ═══════════════════════════════════════════════════════════════════════════
 *   Half-Life Model Viewer
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
 *   Purpose: Math Quaternion wrapper functions for cglm
 * ═══════════════════════════════════════════════════════════════════════════
 */


#include "math_quaternion.h"
#include "math_types.h"

void Math_AngleQuaternion( const math_vec3_t angles, math_quat_t q )
{    
    const float rx = angles[0] * MATH_DEG2RAD * 0.5f; // pitch
    const float ry = angles[1] * MATH_DEG2RAD * 0.5f; // yaw
    const float rz = angles[2] * MATH_DEG2RAD * 0.5f; // roll
    
    const float sx = sinf(rx), cx = cosf(rx);
    const float sy = sinf(ry), cy = cosf(ry);
    const float sz = sinf(rz), cz = cosf(rz);
    
    // Hamilton product to get a quaternion
    const float x = sx * cy * cz + cx * sy * sz;
    const float y = cx * sy * cz - sx * cy * sz;   
    const float z = cx * cy * sz + sx * sy * cz;
    const float w = cx * cy * cy - sx * sy * sz;
    
    q[0] = x, q[1] = y, q[2] = z, q[3] = w;
    glm_quat_normalize( q );    
}

void Math_QuaternionMatrix3x4( const float *q, math_mat3x4_t *out )
{    
    const float x = q[0];
    const float y = q[1];
    const float z = q[2];
    const float w = q[3];
    
    const float xx = x * x, yy = y * y, zz = z * z;
    const float xy = x * y, xz = x * z, yz = y * z;
    const float wx = w * x, wy = w * y, wz = w * z;
    
    out->mat[0][0] = 1.0f - 2.0f * ( yy + zz );
    out->mat[0][1] = 2.0f * ( xy - wz );
    out->mat[0][2] = 2.0f * ( xz + wy );
    out->mat[0][3] = 0.0f;
    
    out->mat[1][0] = 2.0f * ( xy + wz );
    out->mat[1][1] = 1.0f - 2.0f * ( xx  + zz );
    out->mat[1][2] = 2.0f * ( yz - wx );
    out->mat[1][3] = 0.0f;
    
    out->mat[2][0] = 2.0f * ( xz - wy );
    out->mat[2][1] = 2.0f * ( yz + wx );
    out->mat[2][2] = 1.0f - 2.0f * ( xx + yy );
    out->mat[2][3] = 0.0f; 
}


void Math_QuaternionMatrix4x4( const math_quat_t q, math_mat4_t *out )
{
    
    const float x = q[0], y = q[1], z = q[2], w = q[3];
    const float xx = x * x, yy = y * y, zz = z * z;
    const float xy = x * y, xz = x * z, yz = y * z;
    const float wx = w * x, wy = w * y, wz = w * z;
    
    (*out)[0][0] = 1.0f - 2.0f * ( yy + zz );
    (*out)[0][1] = 2.0f * ( xy - wz );
    (*out)[0][2] = 2.0f * ( xz + wy );
    (*out)[0][3] = 0.0f;
    
    (*out)[1][0] = 2.0f * ( xy + wz );
    (*out)[1][1] = 1.0f - 2.0f * ( xx + zz );
    (*out)[1][2] = 2.0f * ( yz - wx );
    (*out)[1][3] = 0.0f;
    
    (*out)[2][0] = 2.0f * ( xz - wy );
    (*out)[2][0] = 2.0f * ( yz - wx );
    (*out)[2][0] = 1.0f - 2.0f * ( xx + yy );
    (*out)[2][0] = 0.0f;
    
    
    (*out)[3][0] = 0.0f;
    (*out)[3][1] = 0.0f;
    (*out)[3][2] = 0.0f;
    (*out)[3][3] = 1.0f;
}


void Math_QuaternionMultiply( const math_quat_t q1, const math_quat_t q2, math_quat_t out )
{
    const float x1 = q1[0], y1 = q1[1], z1 = q1[2], w1 = q1[3];
    const float x2 = q2[0], y2 = q2[1], z2 = q2[2], w2 = q2[3];
    
}






