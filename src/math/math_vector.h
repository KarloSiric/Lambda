#ifndef MATH_VECTOR_H
#define MATH_VECTOR_H

// CGLM wrapper functions using consistent naming for math functions

#include "math_types.h"

void Math_Vec3Copy( math_vec3_t src, math_vec3_t dst );

void Math_Vec3Add( math_vec3_t a, math_vec3_t b, math_vec3_t out );

void Math_Vec3Sub( math_vec3_t a, math_vec3_t b, math_vec3_t out );

void Math_Vec3Scale( math_vec3_t v, float scale, math_vec3_t out );

float Math_Vec3Dot( math_vec3_t a, math_vec3_t b );

void Math_Vec3Cross( math_vec3_t a, math_vec3_t b, math_vec3_t out );

float Math_Vec3Length( math_vec3_t v );

float Math_Vec3LengthSquared( math_vec3_t v );

void Math_Vec3Normalize( math_vec3_t v );

bool Math_Vec3Compare( math_vec3_t a, math_vec3_t b, float epsilon );

void Math_Vec3Transform( math_vec3_t in, const math_mat3x4_t *matrix, math_vec3_t out );

void Math_Vec3Rotate( math_vec3_t in, const math_mat3x4_t *matrix, math_vec3_t out );

void Math_Vec3IRotate( math_vec3_t in, const math_mat3x4_t *matrix, math_vec3_t out );

void Math_Vec3TransformMat4( const math_vec3_t in, const math_mat4_t m, math_vec3_t out );

void Math_Vec3RotateMat4( const math_vec3_t in, const math_mat4_t m, math_vec3_t out );

#endif
