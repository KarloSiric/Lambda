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
 *   Purpose: Math matrix wrapper functions for cglm
 * ═══════════════════════════════════════════════════════════════════════════
 */


#include "math_matrix.h"
#include "math_types.h"
#include <string.h>

void Math_Mat3x4_Identity( math_mat3x4_t *matrix )
{
    // just setting it to be 1 for all rotations and 0 for translation
    // make a pointer to an array of 4 floats
    float (*A)[4] = matrix->mat;
    
    A[0][0] = 1.0f; A[0][1] = 0.0f; A[0][2] = 0.0f; A[0][3] = 0.0f;
    A[1][0] = 0.0f; A[1][1] = 1.0f; A[1][2] = 0.0f; A[1][3] = 0.0f;
    A[2][0] = 0.0f; A[2][1] = 0.0f; A[2][2] = 1.0f; A[2][3] = 0.0f; 
}

void Math_Mat3x4_Copy( const math_mat3x4_t *src, math_mat3x4_t *dst )
{ 
    memcpy( dst, src, sizeof( *dst ) ); 
}


void Math_Mat3x4_ConcatTransforms( const math_mat3x4_t *in1, const math_mat3x4_t *in2, math_mat3x4_t *out )
{
    // for cleaner code, but it can be written without this as well
    const float (*RA)[4] = in1->mat;
    const float (*RB)[4] = in2->mat;
    float (*RC)[4] = out->mat;
    
    for ( int i = 0; i < 3; i++ ) {
        for ( int j = 0; j < 3; j++ ) {
            RC[i][j] = RA[i][0] * RB[0][j] +
                       RA[i][1] * RB[1][j] +
                       RA[i][2] * RB[2][j];
        }
    }
    
    // Now we need special translation part formula calculation
    // We are basically rotating the translation into right parent world and adding the offset translation
    RC[0][3] = RA[0][0] * RB[0][3] + RA[0][1] * RB[1][3] + RA[0][2] * RB[2][3] + RA[0][3];
    RC[1][3] = RA[1][0] * RB[0][3] + RA[1][1] * RB[1][3] + RA[1][2] * RB[2][3] + RA[1][3];
    RC[2][3] = RA[2][0] * RB[0][3] + RA[2][1] * RB[1][3] * RA[2][2] * RB[2][3] + RA[2][3];
     
} 




