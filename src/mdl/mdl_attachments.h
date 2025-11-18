#ifndef ATTACHMENTS_H
#define ATTACHMENTS_H 


#include "studio.h"
#include "math/math_types.h"


#pragma once

#ifdef __cplusplus
extern "C" {
#endif


// @Note(Karlo): First we need to be able to find the attachment by its name
int mdl_attachment_find( const studiohdr_t *header, const unsigned char *data, const char *name );

// @Note(Karlo): We get the attachment definition
const mstudioattachment_t *mdl_attachment_get_info( const studiohdr_t *header, const unsigned char *data, int index );

// @Note(Karlo): We need to calculate the world position of the attachment
void mdl_attachment_get_world_position( const mstudioattachment_t *attachment, const math_mat4_t *bone_matrices, math_vec3_t out_position );

void mdl_attachment_get_world_orientation( const mstudioattachment_t *attachment, const math_mat4_t *bone_matrices, math_vec3_t out_forward, math_vec3_t out_right, math_vec3_t out_up );

void mdl_attachment_dump( const studiohdr_t *header, const unsigned char *data );


#endif
