#ifndef MATH_MATRIX_H
#define MATH_MATRIX_H

#include "math_types.h"

void Math_Mat3x4_Identity( math_mat3x4_t *matrix );

void Math_Mat3x4_Copy( const math_mat3x4_t *src, math_mat3x4_t *dst );

void Math_Mat3x4_ConcatTransforms( const math_mat3x4_t *in1, const math_mat3x4_t *in2, math_mat3x4_t *out );

void Math_Mat3x4_FromQuaternionPosition( const math_quat_t q, const math_vec3_t pos_v, math_mat3x4_t *out );

void Math_Mat4_Identity( math_mat4_t m );

void Math_Mat4_Copy( const math_mat4_t src, math_mat4_t dst );

void Math_Mat4_Multiply( const math_mat4_t a, const math_mat4_t b, math_mat4_t out );

void Math_Mat3x4_ToMat4( const math_mat3x4_t *mat3x4, math_mat4_t mat4 );

void Math_Mat4_ToMat3x4( const math_mat4_t mat4, math_mat3x4_t *mat3x4 );

#endif
