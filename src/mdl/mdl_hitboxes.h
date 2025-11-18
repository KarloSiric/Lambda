#ifndef HITBOXES_H
#define HITBOXES_H 

#include "studio.h"
#include "math_types.h"

#pragma once


#ifdef __cplusplus
extern "C" {
#endif


const mstudiobbox_t *mdl_hitbox_get_info( const studiohdr_t *header, const unsigned char *data, int index );

void mdl_hitbox_world_bounds( const mstudiobbox_t *hitbox, const math_mat4_t *bone_matrices, math_vec3_t out_min, math_vec3_t out_max );

void mdl_hitbox_print_dump( const studiohdr_t *header, const unsigned char *data );


#ifdef __cplusplus
}
#endif


#endif
