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

#include "mdl_bones.h"
#include "math_matrix.h"
#include "math_quaternion.h"
#include "math_vector.h"
#include "math_types.h"
#include "util/util_logger.h"

#include <stdio.h>

math_mat4_t g_bonetransformations[MAXSTUDIOBONES];

// ============================================================================
// OLD FUNCTIONS - Still needed by mdl_animations.c
// TODO: Remove these once mdl_animations.c is refactored to use Math library
// ============================================================================

void AngleQuaternion( const vec3 angles, versor q ) {
	float angle;
	float sr, sp, sy, cr, cp, cy;

	angle = angles[2] * 0.5f;
	sy = sinf( angle );
	cy = cosf( angle );

	angle = angles[1] * 0.5f;
	sp = sinf( angle );
	cp = cosf( angle );

	angle = angles[0] * 0.5f;
	sr = sinf( angle );
	cr = cosf( angle );

	q[0] = sr * cp * cy - cr * sp * sy;
	q[1] = cr * sp * cy + sr * cp * sy;
	q[2] = cr * cp * sy - sr * sp * cy;
	q[3] = cr * cp * cy + sr * sp * sy;
}

void QuaternionMatrix( const versor q, mat4 out ) {
	float x = q[0], y = q[1], z = q[2], w = q[3];

	out[0][0] = 1.0f - 2.0f * ( y * y + z * z );
	out[0][1] = 2.0f * ( x * y + w * z );
	out[0][2] = 2.0f * ( x * z - w * y );
	out[0][3] = 0.0f;

	out[1][0] = 2.0f * ( x * y - w * z );
	out[1][1] = 1.0f - 2.0f * ( x * x + z * z );
	out[1][2] = 2.0f * ( y * z + w * x );
	out[1][3] = 0.0f;

	out[2][0] = 2.0f * ( x * z + w * y );
	out[2][1] = 2.0f * ( y * z - w * x );
	out[2][2] = 1.0f - 2.0f * ( x * x + y * y );
	out[2][3] = 0.0f;

	out[3][0] = 0.0f;
	out[3][1] = 0.0f;
	out[3][2] = 0.0f;
	out[3][3] = 1.0f;
}

void QuaternionSlerp( const versor q1, const versor q2, float t, versor out ) {
	glm_quat_slerp( q1, q2, t, out );
}

void R_ConcatTransforms( const mat4 parent, const mat4 local, mat4 out ) {
	glm_mat4_mul( parent, local, out );
}

void TransformNormalByBone( const mat4 boneAbs, const vec3 in, vec3 out ) {
	mat3 R;
	glm_mat4_pick3( boneAbs, R );
	glm_mat3_mulv( R, in, out );
	glm_vec3_normalize( out );
}

// ============================================================================
// END OLD FUNCTIONS
// ============================================================================

void SetUpBones( studiohdr_t *header, unsigned char *data ) {
	LOG_DEBUGF( "bones", "SetUpBones START: %d bones", header->numbones );
	fflush( stdout );

	if ( !header || !data ) {
		LOG_ERRORF( "bones", "NULL parameters to SetUpBones!" );
		return;
	}

	if ( header->numbones <= 0 || header->numbones > MAXSTUDIOBONES ) {
		LOG_ERRORF( "bones", "Invalid bone count: %d (max: %d)",
					header->numbones, MAXSTUDIOBONES );
		return;
	}

	LOG_DEBUGF( "bones", "Getting bones pointer at offset 0x%X", header->boneindex );
	fflush( stdout );

	mstudiobone_t *bones = (mstudiobone_t *)( data + header->boneindex );

	LOG_DEBUGF( "bones", "Bones pointer: %p", (void *)bones );
	fflush( stdout );

	for ( int i = 0; i < header->numbones; i++ ) {
		LOG_TRACEF( "bones", "  Processing bone %d/%d: %s", i, header->numbones - 1, bones[i].name );

		vec3 position = { bones[i].value[0], bones[i].value[1], bones[i].value[2] };
		vec3 euler_rot = { bones[i].value[3], bones[i].value[4], bones[i].value[5] };

		LOG_TRACEF( "bones", "    Position: (%.2f, %.2f, %.2f)", position[0], position[1], position[2] );
		LOG_TRACEF( "bones", "    Rotation: (%.2f, %.2f, %.2f)", euler_rot[0], euler_rot[1], euler_rot[2] );

		math_quat_t q;
		math_mat3x4_t R;
		math_mat4_t R_Mat4;
		math_mat4_t local;

		LOG_TRACEF( "bones", "    Converting to quaternion" );
		// AngleQuaternion( euler_rot, q );
		Math_AngleQuaternion( euler_rot, q );

		LOG_TRACEF( "bones", "    Building rotation matrix" );
		// QuaternionMatrix( q, &R );
		Math_QuaternionMatrix3x4( q, &R );

		Math_Mat3x4_ToMat4( &R, R_Mat4 );

		// glm_mat4_copy( &R, local );

		Math_Mat4_Copy( R_Mat4, local );

		local[3][0] = position[0];
		local[3][1] = position[1];
		local[3][2] = position[2];

		LOG_TRACEF( "bones", "    Parent bone: %d", bones[i].parent );

		if ( bones[i].parent >= 0 ) {
			if ( bones[i].parent >= header->numbones ) {
				LOG_ERRORF( "bones", "    Invalid parent %d for bone %d!",
							bones[i].parent, i );
				continue;
			}

			LOG_TRACEF( "bones", "    Concatenating with parent %d", bones[i].parent );
			// R_ConcatTransforms( g_bonetransformations[bones[i].parent], local, g_bonetransformations[i] );
			Math_Mat4_Multiply( g_bonetransformations[bones[i].parent], local, g_bonetransformations[i] );
		} else {
			LOG_TRACEF( "bones", "    Root bone, copying local transform" );
			// Mat4Copy( local, g_bonetransformations[i] );
			Math_Mat4_Copy( local, g_bonetransformations[i] );
		}

		// Log every 5th bone to avoid spam
		if ( i % 5 == 0 || i == header->numbones - 1 ) {
			LOG_DEBUGF( "bones", "  Processed bone %d/%d", i, header->numbones - 1 );
			fflush( stdout );
		}
	}

	LOG_DEBUGF( "bones", "SetUpBones COMPLETE: %d bones processed", header->numbones );
	fflush( stdout );
}

void TransformVertices( studiohdr_t *header, unsigned char *data, mstudiomodel_t *model, vec3 *out_vertices ) {
	vec3 *vertices = (vec3 *)( data + model->vertindex );

	unsigned char *v2bone = (unsigned char *)( data + model->vertinfoindex );

	for ( int i = 0; i < model->numverts; i++ ) {
		int bone = v2bone[i];
		if ( bone < 0 || bone >= header->numbones ) {
			bone = 0;
		}
        Math_Vec3TransformMat4( vertices[i], g_bonetransformations[bone], out_vertices[i] );
	}
}

// NOTE: SetUpBonesFromAnimation was removed because it had incorrect matrix conversion logic.
// Use mdl_animation_calculate_bones() from mdl_animations.c instead, which correctly
// handles bone transformations using quaternions.
