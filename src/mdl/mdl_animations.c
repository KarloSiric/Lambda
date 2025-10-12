/*======================================================================
   File: mdl_animations.c
   Project: shaders
   Author: karlosiric <email@example.com>
   Created: 2025-10-10 11:47:17
   Last Modified by: karlosiric
   Last Modified: 2025-10-12 20:32:06
   ---------------------------------------------------------------------
   Description: MDL Animation System
       
   ---------------------------------------------------------------------
   License: MIT License
   Company: /
   Version: 0.1.0
 ======================================================================
                                                                       */

#include "mdl_animations.h"
#include "bone_system.h"

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
    // ROW-MAJOR 3x4 matrix multiplication - Matches Valve's ConcatTransforms exactly
    // result = parent * local
    // Format: matrix[row][col]

    result[0][0] = parent_matrix[0][0] * local_matrix[0][0] + parent_matrix[0][1] * local_matrix[1][0]
                   + parent_matrix[0][2] * local_matrix[2][0];
    result[0][1] = parent_matrix[0][0] * local_matrix[0][1] + parent_matrix[0][1] * local_matrix[1][1]
                   + parent_matrix[0][2] * local_matrix[2][1];
    result[0][2] = parent_matrix[0][0] * local_matrix[0][2] + parent_matrix[0][1] * local_matrix[1][2]
                   + parent_matrix[0][2] * local_matrix[2][2];
    result[0][3] = parent_matrix[0][0] * local_matrix[0][3] + parent_matrix[0][1] * local_matrix[1][3]
                   + parent_matrix[0][2] * local_matrix[2][3] + parent_matrix[0][3];

    result[1][0] = parent_matrix[1][0] * local_matrix[0][0] + parent_matrix[1][1] * local_matrix[1][0]
                   + parent_matrix[1][2] * local_matrix[2][0];
    result[1][1] = parent_matrix[1][0] * local_matrix[0][1] + parent_matrix[1][1] * local_matrix[1][1]
                   + parent_matrix[1][2] * local_matrix[2][1];
    result[1][2] = parent_matrix[1][0] * local_matrix[0][2] + parent_matrix[1][1] * local_matrix[1][2]
                   + parent_matrix[1][2] * local_matrix[2][2];
    result[1][3] = parent_matrix[1][0] * local_matrix[0][3] + parent_matrix[1][1] * local_matrix[1][3]
                   + parent_matrix[1][2] * local_matrix[2][3] + parent_matrix[1][3];

    result[2][0] = parent_matrix[2][0] * local_matrix[0][0] + parent_matrix[2][1] * local_matrix[1][0]
                   + parent_matrix[2][2] * local_matrix[2][0];
    result[2][1] = parent_matrix[2][0] * local_matrix[0][1] + parent_matrix[2][1] * local_matrix[1][1]
                   + parent_matrix[2][2] * local_matrix[2][1];
    result[2][2] = parent_matrix[2][0] * local_matrix[0][2] + parent_matrix[2][1] * local_matrix[1][2]
                   + parent_matrix[2][2] * local_matrix[2][2];
    result[2][3] = parent_matrix[2][0] * local_matrix[0][3] + parent_matrix[2][1] * local_matrix[1][3]
                   + parent_matrix[2][2] * local_matrix[2][3] + parent_matrix[2][3];
}

void build_bone_matrix( vec3_t position, vec3_t rotation, float matrix[3][4] )
{
    // Valve's AngleMatrix uses ROW-MAJOR format: matrix[row][col]
    // angles[PITCH=0], angles[YAW=1], angles[ROLL=2]
    // MDL stores: rotation[0/1/2] in radians (Valve uses degrees, so we don't convert)

    float pitch = rotation[0];
    float yaw   = rotation[1];
    float roll  = rotation[2];

    float sy = sinf( yaw );
    float cy = cosf( yaw );
    float sp = sinf( pitch );
    float cp = cosf( pitch );
    float sr = sinf( roll );
    float cr = cosf( roll );

    // ROW-MAJOR format: matrix[row][col]
    // Row 0
    matrix[0][0] = cp * cy;
    matrix[0][1] = sr * sp * cy + cr * -sy;
    matrix[0][2] = ( cr * sp * cy + -sr * -sy );
    matrix[0][3] = position[0];

    // Row 1
    matrix[1][0] = cp * sy;
    matrix[1][1] = sr * sp * sy + cr * cy;
    matrix[1][2] = ( cr * sp * sy + -sr * cy );
    matrix[1][3] = position[1];

    // Row 2
    matrix[2][0] = -sp;
    matrix[2][1] = sr * cp;
    matrix[2][2] = cr * cp;
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

    if ( delta_time < 0.001f )
    {
        return;
    }

    mstudioseqdesc_t *sequences = ( mstudioseqdesc_t * ) ( data + header->seqindex );
    mstudioseqdesc_t *seq       = &sequences[state->current_sequence];

    if ( delta_time > ( 1.0f / seq->fps * 2.0f ) )
    {    // Allow 2x slowdown max
        delta_time = ( 1.0f / seq->fps * 2.0f );
    }

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

    float frames_to_advance = delta_time * frames_per_second;

    state->current_frame += frames_to_advance;

    // printf("DEBUG: Delta Time is: %f\n", delta_time);

    if ( state->current_frame >= seq->numframes )
    {
        if ( state->is_looping )
        {
            state->current_frame = fmodf( state->current_frame, ( float ) seq->numframes );
        }
        else
        {
            state->current_frame = ( float ) ( seq->numframes - 1 );
        }
    }
    else if ( state->current_frame < 0.0f )
    {
        state->current_frame = 0.0f;
    }

    return;
}

mdl_result_t mdl_animation_calculate_bones(
    mdl_animation_state_t *state, studiohdr_t *header, unsigned char *data, mat4 *bone_transformations )
{
    if ( !state || !header || !bone_transformations )
    {
        return MDL_ERROR_INVALID_PARAMETER;
    }
    mstudiobone_t *bones = ( mstudiobone_t * ) ( data + header->boneindex );

    mstudioseqdesc_t *sequences = ( mstudioseqdesc_t * ) ( data + header->seqindex );
    mstudioseqdesc_t *seq       = &sequences[state->current_sequence];

    mstudioanim_t *anim_data = ( mstudioanim_t * ) ( data + seq->animindex );

    // Debug: print bind pose for bone 0
    printf(
        "BONE 0 BIND POSE: pos=[%.3f, %.3f, %.3f] rot=[%.3f, %.3f, %.3f]\n",
        bones[0].value[0],
        bones[0].value[1],
        bones[0].value[2],
        bones[0].value[3],
        bones[0].value[4],
        bones[0].value[5] );
    printf(
        "BONE 0 SCALE: [%.3f, %.3f, %.3f, %.3f, %.3f, %.3f]\n",
        bones[0].scale[0],
        bones[0].scale[1],
        bones[0].scale[2],
        bones[0].scale[3],
        bones[0].scale[4],
        bones[0].scale[5] );

    for ( int i = 0; i < header->numbones; i++ )
    {
        mstudiobone_t *bone      = &bones[i];
        mstudioanim_t *bone_anim = &anim_data[i];

        // START WITH BIND POSE (Valve SDK approach)
        vec3_t position = {
            bone->value[0],    // X position from bind pose
            bone->value[1],    // Y position from bind pose
            bone->value[2]     // Z position from bind pose
        };
        vec3_t rotation = {
            bone->value[3],    // X rotation from bind pose
            bone->value[4],    // Y rotation from bind pose
            bone->value[5]     // Z rotation from bind pose
        };

        for ( int channel = 0; channel < 6; channel++ )
        {
            float delta = 0.0f;

            // Only read animation data if offset exists
            if ( bone_anim->offset[channel] != 0 )
            {
                // calc_bone_anim_value already multiplies by scale!
                delta
                    = calc_bone_anim_value( data, seq, bone_anim, channel, state->current_frame, bone->scale[channel] );

                if ( i == 0 )
                {    // Debug bone 0
                    printf( "BONE 0 CH %d: delta=%.3f, scale=%.3f, offset=%d\n", channel, delta, bone->scale[channel], bone_anim->offset[channel] );
                }
            }

            // ADD delta to bind pose (Valve's approach for both position AND rotation)
            if ( channel < 3 )
            {
                position[channel] += delta;
            }
            else
            {
                rotation[channel - 3] += delta;
                if ( i == 0 )
                {
                    printf( "  Rotation[%d]: bind=%.6f + delta=%.6f = %.6f\n",
                           channel - 3, bone->value[channel], delta, rotation[channel - 3] );
                }
            }
        }

        if ( i == 0 )
        {
            printf( "BONE 0 Final Euler: [%.6f, %.6f, %.6f]\n", rotation[0], rotation[1], rotation[2] );
        }

        // NOW convert final Euler angles to quaternion, then to matrix (Valve's approach!)
        versor q;
        AngleQuaternion( rotation, q );

        if ( i == 0 )
        {
            printf( "BONE 0 Quaternion: [%.3f, %.3f, %.3f, %.3f]\n", q[0], q[1], q[2], q[3] );
        }

        // Convert quaternion to rotation matrix
        mat4 local;
        QuaternionMatrix( q, local );

        // Set translation in the 4th column (column-major format)
        local[3][0] = position[0];
        local[3][1] = position[1];
        local[3][2] = position[2];

        // Debug bone 0 matrix
        if ( i == 0 )
        {
            printf( "BONE 0 Position: [%.3f, %.3f, %.3f]\n", position[0], position[1], position[2] );
            printf( "BONE 0 LOCAL MATRIX (column-major mat4):\n" );
            printf(
                "  [%.3f %.3f %.3f %.3f]\n",
                local[0][0],
                local[0][1],
                local[0][2],
                local[0][3] );
            printf(
                "  [%.3f %.3f %.3f %.3f]\n",
                local[1][0],
                local[1][1],
                local[1][2],
                local[1][3] );
            printf(
                "  [%.3f %.3f %.3f %.3f]\n",
                local[2][0],
                local[2][1],
                local[2][2],
                local[2][3] );
            printf(
                "  [%.3f %.3f %.3f %.3f]\n",
                local[3][0],
                local[3][1],
                local[3][2],
                local[3][3] );
        }

        // Concatenate with parent bone transform
        if ( bone->parent >= 0 )
        {
            R_ConcatTransforms( bone_transformations[bone->parent], local, bone_transformations[i] );
        }
        else
        {
            glm_mat4_copy( local, bone_transformations[i] );
        }
    }
    return MDL_SUCCESS;
}

void transform_vertex_by_bone( vec3_t result, vec3_t vertex, float bone_matrix[3][4] )
{
    // bone_matrix is in ROW-MAJOR format: bone_matrix[row][col]
    // Transform: result = bone_matrix * vertex (treating vertex as column vector)

    result[0] = bone_matrix[0][0] * vertex[0] + bone_matrix[0][1] * vertex[1] + bone_matrix[0][2] * vertex[2]
                + bone_matrix[0][3];

    result[1] = bone_matrix[1][0] * vertex[0] + bone_matrix[1][1] * vertex[1] + bone_matrix[1][2] * vertex[2]
                + bone_matrix[1][3];

    result[2] = bone_matrix[2][0] * vertex[0] + bone_matrix[2][1] * vertex[1] + bone_matrix[2][2] * vertex[2]
                + bone_matrix[2][3];
}

void mdl_animation_transform_all_vertices(
    studiohdr_t   *header,
    unsigned char *data,
    float ( *bone_matrices )[3][4],
    vec3_t *output_vertices,
    int     bodypart_index,
    int     model_index )
{
    if ( !header || !data || !bone_matrices || !output_vertices )
    {
        return;
    }

    mstudiobodyparts_t *bodyparts = ( mstudiobodyparts_t * ) ( data + header->bodypartindex );

    if ( bodypart_index >= header->numbodyparts )
    {
        return;
    }

    mstudiobodyparts_t *bodypart = &bodyparts[bodypart_index];
    mstudiomodel_t     *models   = ( mstudiomodel_t * ) ( data + bodypart->modelindex );

    if ( model_index >= bodypart->nummodels )
    {
        return;
    }

    mstudiomodel_t *model = &models[model_index];

    vec3_t *vertices = ( vec3_t * ) ( data + model->vertindex );

    unsigned char *vertex_bone = ( unsigned char * ) ( data + model->vertinfoindex );

    for ( int i = 0; i < model->numverts; i++ )
    {
        int bone_index = vertex_bone[i];

        transform_vertex_by_bone( output_vertices[i], vertices[i], bone_matrices[bone_index] );
    }
}
