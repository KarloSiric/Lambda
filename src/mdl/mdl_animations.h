#ifndef MDLANIMATIONS_H
#define MDLANIMATIONS_H

#include "../studio.h"
#include "mdl_loader.h"

typedef struct {
    int   current_sequence;
    float current_frame;
    float frame_time;
    bool  is_looping;
} mdl_animation_state_t;

void mdl_animation_init( mdl_animation_state_t *state );

mdl_result_t mdl_animation_set_sequence(
    mdl_animation_state_t *state, int sequence_index, studiohdr_t *header, unsigned char *data );

void mdl_animation_update( mdl_animation_state_t *state, float delta_time, studiohdr_t *header, unsigned char *data );

mdl_result_t mdl_animation_calculate_bones(
    mdl_animation_state_t *state, studiohdr_t *header, unsigned char *data, float bone_matrices[MAXSTUDIOBONES][3][4] );

#endif


