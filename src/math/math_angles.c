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
 *   Purpose: Math angles wrapper functions for cglm
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include "math_angles.h"
#include "math_types.h"
#include <math.h>

void Math_AngleMatrix( const math_vec3_t angles, math_mat3x4_t *out ) {
	// Following for GoldSrc convention -> pitch, yaw, roll [ x, y, z ]
    // Standard convention is sometimes used: Yaw, Pitch and Roll -> [ z, y, x ]
    // Standard rotation matrix no translation part at all

	float ( *A )[4] = out->mat;

	float pitch = glm_rad( angles[0] );
	float yaw = glm_rad( angles[1] );
	float roll = glm_rad( angles[2] );

	float sp = sinf( pitch ), cp = cosf( pitch );
	float sy = sinf( yaw ), cy = cosf( yaw );
	float sr = sinf( roll ), cr = cosf( roll );

	A[0][0] = cp * cy;
	A[0][1] = sr * sp * cy + cr * -sy;
	A[0][2] = ( cr * sp * cy ) + ( -sr * -sy );
	A[0][3] = 0.0f;

	A[1][0] = cp * sy;
	A[1][1] = sr * sp * sy + cr * cy;
	A[1][2] = cr * sp * sy + ( -sr * cy );
	A[1][3] = 0.0f;

	A[2][0] = -sp;
	A[2][1] = sr * cp;
	A[2][2] = cr * cp;
	A[2][3] = 0.0f;
}


void Math_AngleVectors( const math_vec3_t angles, math_vec3_t *forward, math_vec3_t *right, math_vec3_t *up )
{    
    float pitch = glm_rad( angles[0] );      
    float yaw = glm_rad( angles[1] );      
    float roll = glm_rad( angles[2] );      
    
    float sp = sinf( pitch ), cp = cosf( pitch );
    float sy = sinf( yaw ), cy = cosf( yaw );
    float sr = sinf( roll ), cr = cosf( roll );
    
    if ( forward )
    {
        (*forward)[0] = cp * cy;
        (*forward)[1] = cp * sy;
        (*forward)[2] = -sp;
    }   
    if ( right )
    {
        (*right)[0] = -sr * sp * cy + (-cr) * (-sy);
        (*right)[1] = -sr*sp*sy + (-cr) * cy;
        (*right)[2] = -sr*cp;
    }
    
    if ( up )
    {
        (*up)[0] = cr*sp*cy + (-sr) * (-sy);
        (*up)[1] = cr*sp*sy + (-sr) * cy;
        (*up)[2] = cr*cp;
    }
        
}


void Math_VectorToAngles( const math_vec3_t vec, math_vec3_t angles )
{ 
     float x = vec[0];
     float y = vec[1];
     float z = vec[2];
     
     float hyp = sqrtf( x * x + y * y );
     
     float yaw = atan2f( y, x ) * MATH_RAD2DEG;
     
     float roll = 
    
                
}

