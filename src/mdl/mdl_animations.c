/*======================================================================
   File: mdl_animations.c
   Project: shaders
   Author: karlosiric <email@example.com>
   Created: 2025-10-10 11:47:17
   Last Modified by: karlosiric
   Last Modified: 2025-10-11 14:50:30
   ---------------------------------------------------------------------
   Description: MDL Animation System
       
   ---------------------------------------------------------------------
   License: MIT License
   Company: /
   Version: 0.1.0
 ======================================================================
                                                                       */

#include "mdl_animations.h"

#include <cglm/cglm.h>
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

void matrix_multiply_3x4( float result[3][4], float parent_matrix[3][4], float local_matrix[3][4] )
{
    for ( int i = 0; i < 3; i++ )
    {
        for ( int j = 0; j < 4; j++ )
        {
            // ROTATION
            if ( j < 3 )
            {
                result[i][j] = parent_matrix[i][0] * local_matrix[0][j] + parent_matrix[i][1] * local_matrix[1][j]
                               + parent_matrix[i][2] * local_matrix[2][j];
            }
            else
            {
                result[i][j] = parent_matrix[i][0] * local_matrix[0][j] + parent_matrix[i][1] * local_matrix[1][j]
                               + parent_matrix[i][2] * local_matrix[2][j] + parent_matrix[i][3];
            }
        }
    }
}

void build_bone_matrix( vec3_t position, vec3_t rotation, float matrix[3][4] )
{
    // 1. Calculate the cosine and sine for each rotation axis
    // we will be using Yaw Pitch Roll transformational matrix -> [z,y,x]
    
    /*
     * I will be ading proper cglm calculating simply because it is fast
     * This was for educational purposes, so I see what is what happening
     * and where
     */

    float phi   = rotation[0];    // Roll  (X)
    float theta = rotation[1];    // Pitch (Y)
    float psi   = rotation[2];    // Yaw   (Z)

    float sin_phi   = sinf( phi );
    float cos_phi   = cosf( phi );
    float sin_theta = sinf( theta );
    float cos_theta = cosf( theta );
    float sin_psi   = sinf( psi );
    float cos_psi   = cosf( psi );

    // 2. Build rotation matrix (Valve's AngleMatrix formula)
    // Row 0
    matrix[0][0] = cos_psi * cos_theta;
    matrix[0][1] = sin_phi * sin_theta * cos_psi - sin_psi * cos_phi;
    matrix[0][2] = sin_theta * cos_phi * cos_psi + sin_phi * sin_psi;

    // Row 1
    matrix[1][0] = sin_psi * cos_theta;
    matrix[1][1] = sin_phi * sin_psi * sin_theta + cos_phi * cos_psi;
    matrix[1][2] = sin_psi * sin_theta * cos_phi - sin_phi * cos_psi;

    // Row 2
    matrix[2][0] = -sin_theta;
    matrix[2][1] = sin_phi * cos_theta;
    matrix[2][2] = cos_phi * cos_theta;

    // 3. Adding translation to wrap up the complete matrix
    matrix[0][3] = position[0];
    matrix[1][3] = position[1];
    matrix[2][3] = position[2];
}

// TODO(Karlo): Now finished yet need to take more time on this.
float calc_bone_anim_value(
    unsigned char *data, mstudioseqdesc_t *seq, mstudioanim_t *bone_anim, int channel, float frame, float scale )
{
    mstudioanimvalue_t *anim_value = ( mstudioanimvalue_t * ) ( data + seq->animindex + bone_anim->offset[channel] );

    int   iframe = ( int ) frame;
    float s      = frame - ( float ) iframe;

    int k = iframe;

    while ( anim_value->num.total <= k )
    {
        k          -= anim_value->num.total;
        anim_value += anim_value->num.valid
                      + 1;    // moving it by that amount of structs, to skip and get to the next chunk to the header
    }

    // this time the anim_value points to the chunk that contains our frame at position k

    float value1 = 0.0f;
    float value2 = 0.0f;

    if ( anim_value->num.valid > k )
    {
        value1 = anim_value[k + 1].value;    // + 1 because of the header

        if ( anim_value->num.valid > k + 1 )
        {
            value2 = anim_value[k + 2].value;
        }
        else
        {
            value2 = value1;
        }
    }
    else
    {
        value1 = ( float ) anim_value[anim_value->num.valid].value;
        value2 = value1;
    }

    // Now we can interpolate the final result
    float result = value1 * ( 1.0f - s ) + value2 * s;

    return result * scale;
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

        float local_matrix[3][4];
        build_bone_matrix( position, rotation, local_matrix );    // -> this builds the proper local bone matrix

        if ( bone->parent == -1 )
        {
            // This is the root bone it has no parent bones
            memcpy( bone_matrices[i], local_matrix, sizeof( local_matrix ) );
        }
        else 
        {
                
        }
    }

    // now here we need to build the proper bone matrix from the rotation and position

    return MDL_SUCCESS;
}
