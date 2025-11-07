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

void Math_Mat3x4_Identity( math_mat3x4_t *matrix ) {
	// using memset for speed and efficiency
	memset( matrix->mat, 0, sizeof( matrix->mat ) );
	matrix->mat[0][0] = matrix->mat[1][1] = matrix->mat[2][2] = 1.0f;
}

void Math_Mat3x4_Copy( const math_mat3x4_t *src, math_mat3x4_t *dst ) {
	memcpy( dst, src, sizeof( *dst ) );
}

void Math_Mat3x4_ConcatTransforms( const math_mat3x4_t *in1, const math_mat3x4_t *in2, math_mat3x4_t *out ) {
	// for cleaner code, but it can be written without this as well
	const float ( *RA )[4] = in1->mat;
	const float ( *RB )[4] = in2->mat;
	float ( *RC )[4] = out->mat;

	for ( int i = 0; i < 3; i++ ) {
		for ( int j = 0; j < 3; j++ ) {
			RC[i][j] = RA[i][0] * RB[0][j] + RA[i][1] * RB[1][j] + RA[i][2] * RB[2][j];
		}
	}

	// Now we need special translation part formula calculation
	// We are basically rotating the translation into right parent world and adding the offset translation
	RC[0][3] = RA[0][0] * RB[0][3] + RA[0][1] * RB[1][3] + RA[0][2] * RB[2][3] + RA[0][3];
	RC[1][3] = RA[1][0] * RB[0][3] + RA[1][1] * RB[1][3] + RA[1][2] * RB[2][3] + RA[1][3];
	RC[2][3] = RA[2][0] * RB[0][3] + RA[2][1] * RB[1][3] + RA[2][2] * RB[2][3] + RA[2][3];
}

void Math_Mat3x4_FromQuaternionPosition( const math_quat_t q, const math_vec3_t pos_v, math_mat3x4_t *out ) {
	// because the original one is a const we cannot modify it
	math_quat_t q_copy;
	memcpy( q_copy, q, sizeof( math_quat_t ) );

	float x = q_copy[0], y = q_copy[1], z = q_copy[2], w = q_copy[3];

	float xx = x * x;
	float yy = y * y;
	float zz = z * z;
	float ww = w * w;
	float xy = x * y;
	float xz = x * z;
	float yz = y * z;
	float wx = w * x;
	float wy = w * y;
	float wz = w * z;

	float q_magnitude = sqrt( xx + yy + zz + ww );

	if ( q_magnitude < 1.0f ) {
		glm_quat_normalize( q_copy );
	}

	out->mat[0][0] = 1 - 2 * ( yy + zz );
	out->mat[0][1] = 2 * ( xy - wz );
	out->mat[0][2] = 2 * ( xz + wy );
	out->mat[0][3] = pos_v[0];

	// Row 1
	out->mat[1][0] = 2 * ( xy + wz );
	out->mat[1][1] = 1 - 2 * ( xx + zz );
	out->mat[1][2] = 2 * ( yz - wx );
	out->mat[1][3] = pos_v[1];

	// Row 2
	out->mat[2][0] = 2 * ( xz - wy );
	out->mat[2][1] = 2 * ( yz + wx );
	out->mat[2][2] = 1 - 2 * ( xx + yy );
	out->mat[2][3] = pos_v[2];
}

void Math_Mat4_Identity( math_mat4_t m ) {
	// can be done in different ways, I opted for memset because of speed and efficiency
	memset( m, 0, sizeof( math_mat4_t ) );
	m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
}

void Math_Mat4_Copy( const math_mat4_t src, math_mat4_t dst ) {
	memcpy( dst, src, sizeof( math_mat4_t ) );
}

void Math_Mat4_Multiply( const math_mat4_t a, const math_mat4_t b, math_mat4_t out ) {
	// CGLM uses column-major: mat4[col][row]
	// Matrix multiplication: out = a * b
	// For each column j in b: out[j] = a * b[j]

	// Use glm_mat4_mul for correct column-major multiplication
	glm_mat4_mul( a, b, out );
}

void Math_Mat3x4_ToMat4( const math_mat3x4_t *mat3x4, math_mat4_t mat4 ) {
	// mat3x4 is row-major: mat[row][col]
	// mat4 (CGLM) is column-major: mat[col][row]
	// We need to TRANSPOSE during conversion
	const float ( *A )[4] = mat3x4->mat;

	// Transpose: A[row][col] -> mat4[col][row]
	mat4[0][0] = A[0][0];
	mat4[0][1] = A[1][0];
	mat4[0][2] = A[2][0];
	mat4[0][3] = 0.0f;

	mat4[1][0] = A[0][1];
	mat4[1][1] = A[1][1];
	mat4[1][2] = A[2][1];
	mat4[1][3] = 0.0f;

	mat4[2][0] = A[0][2];
	mat4[2][1] = A[1][2];
	mat4[2][2] = A[2][2];
	mat4[2][3] = 0.0f;

	mat4[3][0] = A[0][3];
	mat4[3][1] = A[1][3];
	mat4[3][2] = A[2][3];
	mat4[3][3] = 1.0f;
}

void Math_Mat4_ToMat3x4( const math_mat4_t mat4, math_mat3x4_t *mat3x4 ) {
	// mat4 (CGLM) is column-major: mat[col][row]
	// mat3x4 is row-major: mat[row][col]
	// We need to TRANSPOSE during conversion
	float ( *A )[4] = mat3x4->mat;

	// Transpose: mat4[col][row] -> A[row][col]
	A[0][0] = mat4[0][0];
	A[0][1] = mat4[1][0];
	A[0][2] = mat4[2][0];
	A[0][3] = mat4[3][0];

	A[1][0] = mat4[0][1];
	A[1][1] = mat4[1][1];
	A[1][2] = mat4[2][1];
	A[1][3] = mat4[3][1];

	A[2][0] = mat4[0][2];
	A[2][1] = mat4[1][2];
	A[2][2] = mat4[2][2];
	A[2][3] = mat4[3][2];
}

// Rotates a matrix around an arbitrary axis
// This modifies the matrix in-place, applying a rotation transformation
void Math_Mat4_Rotate( math_mat4_t m, float angle_rad, const math_vec3_t axis ) {
	// glm_rotate applies rotation to existing matrix: m = m * R(angle, axis)
	// Used for model rotation (e.g., spinning character on turntable)
	glm_rotate( m, angle_rad, (vec3){ axis[0], axis[1], axis[2] } );
}

// Creates a view matrix for camera positioning
// "Look-at" transformation: camera at 'eye' position, looking at 'center' point
void Math_Mat4_LookAt( const math_vec3_t eye, const math_vec3_t center, const math_vec3_t up, math_mat4_t dest ) {
	// Constructs view matrix that transforms world coordinates to camera space
	// eye: camera position, center: target point, up: up direction (usually {0,1,0})
	glm_lookat( (vec3){ eye[0], eye[1], eye[2] },
	            (vec3){ center[0], center[1], center[2] },
	            (vec3){ up[0], up[1], up[2] },
	            dest );
}

// Creates a perspective projection matrix for 3D rendering
// This gives the "3D depth" effect where far objects appear smaller
void Math_Mat4_Perspective( float fovy_rad, float aspect, float near_plane, float far_plane, math_mat4_t dest ) {
	// fovy_rad: vertical field of view in radians (how wide camera sees)
	// aspect: screen width/height ratio
	// near/far: clipping planes (only objects between near and far are visible)
	glm_perspective( fovy_rad, aspect, near_plane, far_plane, dest );
}
