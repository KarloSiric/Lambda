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

void Math_AngleQuaternion(const math_vec3_t angles, math_quat_t q)
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





