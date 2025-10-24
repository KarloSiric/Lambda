#ifndef MATH_ANGLES_H
#define MATH_ANGLES_H

#include "math_types.h"

void Math_AngleMatrix( const math_vec3_t angles, math_mat3x4_t *out );

void Math_AngleVectors( const math_vec3_t angles, math_vec3_t *forward, math_vec3_t *right, math_vec3_t *up );

void Math_VectorToAngles( const math_vec3_t vec, math_vec3_t angles );

float Math_AngleNormalize( float angle );

void Math_AngleDegToRad( const math_vec3_t degrees, math_vec3_t radians );

void Math_AngleRadToDeg( const math_vec3_t radians, math_vec3_t degrees );

#endif
