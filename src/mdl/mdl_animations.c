/*======================================================================
   File: mdl_animations.c
   Project: shaders
   Author: karlosiric <email@example.com>
   Created: 2025-10-10 11:47:17
   Last Modified by: karlosiric
   Last Modified: 2025-10-10 23:09:16
   ---------------------------------------------------------------------
   Description:
       
   ---------------------------------------------------------------------
   License: 
   Company: 
   Version: 0.1.0
 ======================================================================
                                                                       */

#include "mdl_animations.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

void mdl_animation_init( mdl_animation_state_t *state )
{
    memset( state, 0, sizeof( *state ) );
    state->current_sequence = 0;
    state->current_frame    = 0.0f;
    state->is_looping       = false;
}

void build_bone_matrix(vec3_t position, vec3_t rotation, float matrix[3][3]) {
    
    // 1. Calculate the cosine and sine for each rotation axis
    
    float cx = cosf(rotation[0]);
    float sx = sinf(rotation[0]);
    
    float cy = cosf(rotation[1]);
    float sy = sinf(rotation[1]);
    
    float cz = cosf(rotation[2]);
    float sz = sinf(rotation[2]);
    
    // 2. Building the rotational matrix using Euler's angle formula
    matrix[0][0] = cy * cz;
    matrix[0][1] = cy * sz;
    matrix[0][2] = -sy;
    
    matrix[1][0] = sx * sy * cz - cx * sz;
    matrix[1][1] = sx * sy * sz - cx * cz;
    matrix[1][2] = sx * sy;
    
    matrix[2][0] = cx * sy * cz + sx * sz;
    matrix[2][1] = cx * sy * sz - sx * cz;
    matrix[2][2] = cx * cy;
    
    // 3. Adding translation 
    matrix[0][3] = position[0]; 
    matrix[1][3] = position[1]; 
    matrix[2][3] = position[2];  
}









mdl_result_t
mdl_animation_set_sequence( mdl_animation_state_t *state, int sequence_index, studiohdr_t *header, unsigned char *data )
{
    if ( !state || !header || !data )
    {
        return MDL_ERROR_INVALID_PARAMETER;
    }

    if ( sequence_index >= header->numseq || sequence_index < 0 )
    {
        return MDL_ERROR_INVALID_PARAMETER;
    }

    mstudioseqdesc_t *sequences = ( mstudioseqdesc_t * ) ( data + header->seqindex );

    mstudioseqdesc_t *seq = &sequences[sequence_index];

    state->current_sequence = sequence_index;
    state->current_frame    = 0.0f;
    state->frame_time       = 0.0f;
    state->is_looping       = ( seq->flags & 0x01 );

    printf(
        "Set animation to sequence %d: '%s' (%d frames @ %.1f fps)\n",
        sequence_index,
        seq->label,
        seq->numframes,
        seq->fps );

    return MDL_SUCCESS;
}

void mdl_animation_update( mdl_animation_state_t *state, float delta_time, studiohdr_t *header, unsigned char *data )
{
    // safety checking
    if ( !state || !header || !data )
    {
        return;
    }

    mstudioseqdesc_t *sequences = ( mstudioseqdesc_t * ) ( data + header->seqindex );
    mstudioseqdesc_t *seq       = &sequences[state->current_sequence];

    // still animation because it has no frames whatsoever so
    if ( seq->numframes <= 1 )
    {
        return;
    }

    float frames_per_second = seq->fps;

    /* 
     * Running the game @ 60 fps makes the animations smmother for the following reason.
     * So lets say we ahve 60 frames per second and that means the delta time 
     * is 1/60 -> 0.0167 s so that means each frame takes that amount of time to render, and 
     * that means we effectively can show 59 different frames in under a second for that one given animation.
     *  
     * That also means that if something has animations rnning at lets say @ 22 fps, meaning
     * its delta time is around 1/22 ~ 0.045 s meaning there is a smaller range taht can be shown.
     * That means each 0.045 another frame is being shown instead of every 0.0167 so it doesnt look
     * as smooth as the 60 fps of course. Now when our screen has elapsed 0.0167s aprox that means if we were to
     * do a math calculation of taking times 22 frames by that delta time it would come up to ~0.35 somethng.
     * Meaning only around 35 percent of the 1 frame has been shown and not even the full 1 frame of those 22 frames.
     * This gives us a much smoother look of animations in todays world.
     */
    state->frame_time += delta_time;

    float frames_to_advance = state->frame_time * frames_per_second;

    state->current_frame += frames_to_advance;

    // need to reset the frame time
    state->frame_time = 0.0f;

    if ( state->current_frame >= seq->numframes )
    {
        if ( state->is_looping )
        {
            state->current_frame = fmodf( state->current_frame, ( float ) seq->numframes );
        }
        else
        {
            state->current_frame = ( float ) ( state->current_frame - 1 );
        }
    }

    return;
}

mdl_result_t mdl_animation_calculate_bones(
    mdl_animation_state_t *state, studiohdr_t *header, unsigned char *data, float ( *bone_matrices )[3][4] )
{
    if ( !state || !header || !bone_matrices )
    {
        return MDL_ERROR_INVALID_PARAMETER;
    }
    mstudiobone_t *bones = ( mstudiobone_t * ) ( data + header->boneindex );

    mstudioseqdesc_t *sequences = ( mstudioseqdesc_t * ) ( data + header->seqindex );
    mstudioseqdesc_t *seq       = &sequences[state->current_sequence];

    mstudioanim_t *anim_data = ( mstudioanim_t * ) ( data + seq->animindex );
    // TODO(Karlo): Need to find the positon of each bone and then find the rotation of that bone...

    for ( int i = 0; i < header->numbones; i++ )
    {
        mstudiobone_t *bone      = &bones[i];
        mstudioanim_t *bone_anim = &anim_data[i];

        // Now we start with the default T pose position
        vec3_t position = {
            bone->value[0],
            bone->value[1],
            bone->value[2],
        };

        vec3_t rotation = {
            bone->value[3],
            bone->value[4],
            bone->value[5],
        };

        for ( int channel = 0; channel < 6; channel++ )
        {
            if ( bone_anim->offset[channel] != 0 )
            {
                // ANIMATED
                float value
                    = calc_bone_anim_value( data, seq, bone_anim, channel, state->current_frame, bone->scale[channel] );

                if ( channel < 3 )
                {
                    position[channel] += value;
                }
                else
                {
                    rotation[channel - 3] += value;
                }
            }
        }
    }

    // now here we need to build the proper bone matrix from the rotation and position
    build_bone_matrix(...);    
    

    return MDL_SUCCESS;
}
