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
 *   Purpose: Math vector wrapper functions for cglm
 * ═══════════════════════════════════════════════════════════════════════════
 */


#include "math_vector.h"

void Math_Vec3Copy( math_vec3_t src, math_vec3_t dst )
{ 
    glm_vec3_copy( src, dst ); 
}

void Math_Vec3Add( math_vec3_t a, math_vec3_t b, math_vec3_t out )
{
    glm_vec3_add( a, b, out );
}

void Math_Vec3Sub( math_vec3_t a, math_vec3_t b, math_vec3_t out )
{
    glm_vec3_sub( a, b, out );
}

void Math_Vec3Scale( math_vec3_t a, float scale, math_vec3_t out )
{
    glm_vec3_scale( a, scale, out );
}

float Math_Vec3Dot( math_vec3_t a, math_vec3_t b )
{
    return glm_vec3_dot( a, b );
}

void Math_Vec3Cross( math_vec3_t a, math_vec3_t b, math_vec3_t out )
{
    glm_vec3_cross( a, b, out );
}

float Math_Vec3Length( math_vec3_t v )
{
    return glm_vec3_norm( v );
}

float Math_Vec3LengthSquared( math_vec3_t v ) 
{
    return glm_vec3_norm2( v );
}

void Math_Vec3Normalize( math_vec3_t v ) 
{
    glm_vec3_normalize( v );
}

bool Math_Vec3Compare(math_vec3_t a, math_vec3_t b, float epsilon)
{ 
    return fabsf(a[0] - b[0]) <= epsilon &&
           fabsf(a[1] - b[1]) <= epsilon &&
           fabsf(a[2] - b[2]) <= epsilon;
}

void Math_Vec3Transform(math_vec3_t in, const math_mat3x4_t *matrix, math_vec3_t out) 
{
    const float (*A)[4] = matrix->mat; 
    out[0] = A[0][0]*in[0] + A[0][1]*in[1] + A[0][2]*in[2] + A[0][3];
    out[1] = A[1][0]*in[0] + A[1][1]*in[1] + A[1][2]*in[2] + A[1][3];
    out[2] = A[2][0]*in[0] + A[2][1]*in[1] + A[2][2]*in[2] + A[2][3];
}

void Math_Vec3Rotate(math_vec3_t in, const math_mat3x4_t *matrix, math_vec3_t out) 
{
    const float (*A)[4] = matrix->mat;
    out[0] = A[0][0]*in[0] + A[0][1]*in[1] + A[0][2]*in[2];
    out[1] = A[1][0]*in[0] + A[1][1]*in[1] + A[1][2]*in[2];
    out[2] = A[2][0]*in[0] + A[2][1]*in[1] + A[2][2]*in[2];
}

void Math_Vec3IRotate(math_vec3_t in, const math_mat3x4_t *matrix, math_vec3_t out) 
{    
    const float (*A)[4] = matrix->mat;
    out[0] = A[0][0]*in[0] + A[1][0]*in[1] + A[2][0]*in[2];
    out[1] = A[0][1]*in[0] + A[1][1]*in[1] + A[2][1]*in[2];
    out[2] = A[0][2]*in[0] + A[1][2]*in[1] + A[2][2]*in[2];    
}
