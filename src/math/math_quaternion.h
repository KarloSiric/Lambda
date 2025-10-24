#ifndef MATH_QUATERNION_H
#define MATH_QUATERNION_H

#include "math_types.h"

void Math_AngleQuaternion( const math_vec3_t angles, math_quat_t q );

void Math_QuaternionMatrix3x4( const math_quat_t q, math_mat3x4_t *out );

void Math_QuaternionMatrix4x4( const math_quat_t q, math_mat4_t *out );

void Math_QuaternionMultiply( const math_quat_t q1, const math_quat_t q2, math_quat_t out );

void Math_QuaternionSlerp( const math_quat_t q1, const math_quat_t q2, float t, math_quat_t out );

void Math_QuaternionNormalize( math_quat_t q );

void Math_QuaternionInverse( const math_quat_t q, math_quat_t out );

#endif
