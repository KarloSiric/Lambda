/*======================================================================
 *  File: bone_system.c
 *  Project: shaders
 *  Author: karlosiric <email@example.com>
 *  Created: 2025-09-26 12:16:24
 *  Last Modified by: karlosiric
 *  Last Modified: 2025-09-26 18:29:24
 *----------------------------------------------------------------------
 *  Description:
 *      
 *----------------------------------------------------------------------
 *  License: 
 *  Company: 
 *  Version: 0.1.0
 *======================================================================
 */


#include "bone_system.h"
#include <string.h>



mat4 g_bonetransformations[MAXSTUDIOBONES];



static inline void Mat4Copy(mat4 src, mat4 dst) {
    glm_mat4_copy(src, dst);
}


void AngleQuaternion(const vec3 angles, versor q) {

    versor qx, qy, qz, temp;
    glm_quatv(qx, angles[0], (vec3){1.0f, 0.0f, 0.0f});
    glm_quatv(qy, angles[1], (vec3){0.0f, 1.0f, 0.0f});
    glm_quatv(qz, angles[2], (vec3){0.0f, 0.0f, 1.0f});

    glm_quat_mul(qz, qy, temp);
    glm_quat_mul(temp, qx, q);

}


void QuaternionMatrix(const versor q, mat4 out) {

    glm_quat_mat4(q, out);

    out[3][0] = out[3][1] = out[3][2] = 0.0f;

    out[3][3] = 1.0f;

}


void R_ConcatTransforms(const vec4 *parent, const vec4 *local, vec4 *out) {

    glm_mat4_mul(parent, local, out);
}

void VectorTransforms(const float *in, const vec4 *m, float *out) {

    vec4 v = { in[0], in[1], in[2], 1.0f };
    vec4 r;
    glm_mat4_mulv(m, v, r);
    out[0] = r[0];
    out[1] = r[1];
    out[2] = r[2];

}


void TransformNormalByBone(const vec4 *boneAbs, const float *in, float *out) {

    mat3 R;
    glm_mat4_pick3(boneAbs, R);
    glm_mat3_mulv(R, in, out);
    glm_vec3_normalize(out);

}




