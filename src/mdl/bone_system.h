#ifndef BONESYSTEM_H
#define BONESYSTEM_H

#include "../studio.h"

#include <cglm/cglm.h>

extern mat4 g_bonetransformations[MAXSTUDIOBONES];

void SetUpBones( studiohdr_t *header, unsigned char *data );

void TransformVertices( studiohdr_t *header, unsigned char *data, mstudiomodel_t *model, vec3 *out_vertices );

void TransformNormalByBone( const mat4 boneAbs, const vec3 in, vec3 out );

void AngleQuaternion( const vec3 angles, versor q );

void QuaternionMatrix( const versor q, mat4 out );

void QuaternionSlerp( const versor q1, const versor q2, float t, versor out );

void R_ConcatTransforms( const mat4 parent, const mat4 local, mat4 out );

void VectorTransforms( const vec3 in, const mat4 m, vec3 out );

void SetUpBonesFromAnimation( studiohdr_t *header, unsigned char *data, float anim_bones[MAXSTUDIOBONES][3][4] );

#endif
