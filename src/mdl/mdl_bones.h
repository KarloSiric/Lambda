#ifndef BONESYSTEM_H
#define BONESYSTEM_H

#include "math_types.h"
#include "studio.h"

#include <cglm/cglm.h>

extern math_mat4_t g_bonetransformations[MAXSTUDIOBONES];

void SetUpBones( studiohdr_t *header, unsigned char *data );

void TransformVertices( studiohdr_t *header, unsigned char *data, mstudiomodel_t *model, vec3 *out_vertices );

void TransformNormalByBone( const mat4 boneAbs, const vec3 in, vec3 out );

// ============================================================================
// OLD FUNCTIONS - Still needed by mdl_animations.c
// TODO: Remove these once mdl_animations.c is refactored to use Math library
// ============================================================================
void AngleQuaternion( const vec3 angles, versor q );
void QuaternionMatrix( const versor q, mat4 out );
void QuaternionSlerp( const versor q1, const versor q2, float t, versor out );
void R_ConcatTransforms( const mat4 parent, const mat4 local, mat4 out );

#endif
