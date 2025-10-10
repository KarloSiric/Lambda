/*======================================================================
   File: mdl_animations.c
   Project: shaders
   Author: karlosiric <email@example.com>
   Created: 2025-10-10 11:47:17
   Last Modified by: karlosiric
   Last Modified: 2025-10-10 12:20:09
   ---------------------------------------------------------------------
   Description:
       
   ---------------------------------------------------------------------
   License: 
   Company: 
   Version: 0.1.0
 ======================================================================
                                                                       */


#include "mdl_animations.h"
#include <string.h>
#include <math.h>
#include <stdio.h>


void mdl_animation_init(mdl_animation_state_t *state) {
    
    memset(state, 0, sizeof(*state));
    state->current_sequence = 0;
    state->current_frame = 0.0f;
    state->is_looping = false; 
}


mdl_result_t mdl_animation_set_sequence(mdl_animation_state_t *state, int sequence_index, studiohdr_t *header, unsigned char *data) {
    
    if (!state || !header || !data) { 
        return MDL_ERROR_INVALID_PARAMETER;
    }
    
    if (sequence_index >= header->numseq || sequence_index < 0) {
        return MDL_ERROR_INVALID_PARAMETER;
    }
    
    mstudioseqdesc_t *sequences = (mstudioseqdesc_t *)(data + header->seqindex);
    
    mstudioseqdesc_t *seq = &sequences[sequence_index];
    
    state->current_sequence = sequence_index;
    state->current_frame = 0.0f;
    state->frame_time = 0.0f;
    state->is_looping = (seq->flags & 0x01) ? 1 : 0;
    
    printf("Set animation to sequence %d: '%s' (%d frames @ %.1f fps)\n",
            sequence_index, seq->label, seq->numframes, seq->fps);    
     
    return MDL_SUCCESS;
}
