#ifndef MDLANIMATIONS_H
#define MDLANIMATIONS_H

#include "../studio.h"
#include "mdl_loader.h"

typedef struct {
    int   current_sequence;
    float current_frame;
    bool  is_looping;
} mdl_animation_state_t;

void mdl_animation_init( mdl_animation_state_t *state );

mdl_result_t mdl_animation_set_sequence(
    mdl_animation_state_t *state, int sequence_index, studiohdr_t *header, unsigned char *data );

void mdl_animation_update( mdl_animation_state_t *state, float delta_time, studiohdr_t *header, unsigned char *data );

mdl_result_t mdl_animation_calculate_bones(
    mdl_animation_state_t *state, studiohdr_t *header, unsigned char *data, float bone_matrices[MAXSTUDIOBONES][3][4] );

// Matrix multiplication
void matrix_multiply_3x4( float result[3][4], float parent_matrix[3][4], float local_matrix[3][4] );

// Vertex transformation
void transform_vertex_by_bone( vec3_t result, vec3_t vertex, float bone_matrix[3][4] );

void mdl_animation_transform_all_vertices(
    studiohdr_t   *header,
    unsigned char *data,
    float ( *bone_matrices )[3][4],
    vec3_t *output_vertices,
    int     bodypart_index,
    int     model_index );

#endif
