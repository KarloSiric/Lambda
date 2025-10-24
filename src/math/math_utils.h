#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include "math_types.h"

float Math_Clamp( float value, float min, float max );

float Math_Min( float a, float b );
float Math_Max( float a, float b );

float Math_Lerp( float a, float b, float t );

void Math_Vec3Lerp( const math_vec3_t a, const math_vec3_t b, float t, math_vec3_t out );

bool Math_FloatEqual( float a, float b, float epsilon );

#endif 
