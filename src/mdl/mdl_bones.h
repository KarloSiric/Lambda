#ifndef BONESYSTEM_H
#define BONESYSTEM_H

#include "math_types.h"
#include "studio.h"

#include <cglm/cglm.h>

extern math_mat4_t g_bonetransformations[MAXSTUDIOBONES];

void SetUpBones( studiohdr_t *header, unsigned char *data );

void TransformVertices( studiohdr_t *header, unsigned char *data, mstudiomodel_t *model, vec3 *out_vertices );

void TransformNormalByBone( const mat4 boneAbs, const vec3 in, vec3 out );

void CenterBonesAtOrigin( studiohdr_t *header, math_mat4_t *bones );

#endif
