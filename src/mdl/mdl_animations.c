/*======================================================================
   File: mdl_animations.c
   Project: shaders
   Author: karlosiric <email@example.com>
   Created: 2025-10-10 11:47:17
   Last Modified by: karlosiric
   Last Modified: 2025-10-13 16:34:44
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

// Calculate bone rotation using quaternion interpolation (SLERP)
// This matches Valve's CalcBoneQuaternion exactly
void CalcBoneQuaternion( int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim,
                         unsigned char *data, mstudioseqdesc_t *seq, versor q )
{
    vec3_t angle1, angle2;

    // Process all 3 rotation channels (X, Y, Z)
    for ( int j = 0; j < 3; j++ )
    {
        if ( panim->offset[j + 3] == 0 )
        {
            // No animation data, use bind pose
            angle2[j] = angle1[j] = pbone->value[j + 3];
        }
        else
        {
            mstudioanimvalue_t *panimvalue = ( mstudioanimvalue_t * ) ( data + seq->animindex + panim->offset[j + 3] );
            int k = frame;

            // Find the animation data chunk that contains our frame
            while ( panimvalue->num.total <= k )
            {
                k -= panimvalue->num.total;
                panimvalue += panimvalue->num.valid + 1;
            }

            // Extract values for frame interpolation
            if ( panimvalue->num.valid > k )
            {
                angle1[j] = panimvalue[k + 1].value;

                if ( panimvalue->num.valid > k + 1 )
                {
                    angle2[j] = panimvalue[k + 2].value;
                }
                else
                {
                    if ( panimvalue->num.total > k + 1 )
                        angle2[j] = angle1[j];
                    else
                        angle2[j] = panimvalue[panimvalue->num.valid + 2].value;
                }
            }
            else
            {
                angle1[j] = panimvalue[panimvalue->num.valid].value;
                if ( panimvalue->num.total > k + 1 )
                {
                    angle2[j] = angle1[j];
                }
                else
                {
                    angle2[j] = panimvalue[panimvalue->num.valid + 2].value;
                }
            }

            // Apply scale and add to bind pose
            angle1[j] = pbone->value[j + 3] + angle1[j] * pbone->scale[j + 3];
            angle2[j] = pbone->value[j + 3] + angle2[j] * pbone->scale[j + 3];
        }

        // TODO: Add bone controller support if needed
        // if ( pbone->bonecontroller[j + 3] != -1 )
        // {
        //     angle1[j] += adj[pbone->bonecontroller[j + 3]];
        //     angle2[j] += adj[pbone->bonecontroller[j + 3]];
        // }
    }

    // If angles are different, do SLERP interpolation in quaternion space
    if ( !glm_vec3_eqv_eps( angle1, angle2 ) )
    {
        versor q1, q2;
        AngleQuaternion( angle1, q1 );
        AngleQuaternion( angle2, q2 );
        QuaternionSlerp( q1, q2, s, q );
    }
    else
    {
        // Angles are the same, just convert once
        AngleQuaternion( angle1, q );
    }
}

// Calculate bone position using linear interpolation
// This matches Valve's CalcBonePosition exactly
void CalcBonePosition( int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim,
                       unsigned char *data, mstudioseqdesc_t *seq, vec3_t pos )
{
    // Process all 3 position channels (X, Y, Z)
    for ( int j = 0; j < 3; j++ )
    {
        pos[j] = pbone->value[j]; // Start with bind pose

        if ( panim->offset[j] != 0 )
        {
            mstudioanimvalue_t *panimvalue = ( mstudioanimvalue_t * ) ( data + seq->animindex + panim->offset[j] );
            int k = frame;

            // Find span of values that includes the frame we want
            while ( panimvalue->num.total <= k )
            {
                k -= panimvalue->num.total;
                panimvalue += panimvalue->num.valid + 1;
            }

            // If we're inside the span
            if ( panimvalue->num.valid > k )
            {
                // And there's more data in the span
                if ( panimvalue->num.valid > k + 1 )
                {
                    pos[j] += ( panimvalue[k + 1].value * ( 1.0 - s ) + s * panimvalue[k + 2].value ) * pbone->scale[j];
                }
                else
                {
                    pos[j] += panimvalue[k + 1].value * pbone->scale[j];
                }
            }
            else
            {
                // Are we at the end of the repeating values section and there's another section with data?
                if ( panimvalue->num.total <= k + 1 )
                {
                    pos[j] += ( panimvalue[panimvalue->num.valid].value * ( 1.0 - s ) +
                                s * panimvalue[panimvalue->num.valid + 2].value ) * pbone->scale[j];
                }
                else
                {
                    pos[j] += panimvalue[panimvalue->num.valid].value * pbone->scale[j];
                }
            }
        }

        // TODO: Add bone controller support if needed
        // if ( pbone->bonecontroller[j] != -1 )
        // {
        //     pos[j] += adj[pbone->bonecontroller[j]];
        // }
    }
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
    
    // float frames_testing = 1 / seq->fps;

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

    // Calculate frame and interpolation value
    int   frame = ( int ) state->current_frame;
    float s     = state->current_frame - ( float ) frame;

    // Process each bone
    for ( int i = 0; i < header->numbones; i++ )
    {
        mstudiobone_t *bone      = &bones[i];
        mstudioanim_t *bone_anim = &anim_data[i];

        // Calculate bone rotation using SLERP (quaternion interpolation)
        versor q;
        CalcBoneQuaternion( frame, s, bone, bone_anim, data, seq, q );

        // Calculate bone position using linear interpolation
        vec3_t pos;
        CalcBonePosition( frame, s, bone, bone_anim, data, seq, pos );

        // DEBUG: Print bone 0 data
        if ( i == 0 && frame < 2 )
        {
            printf( "=== BONE 0 DEBUG (frame=%d, s=%.3f) ===\n", frame, s );
            printf( "  Quaternion: [%.3f, %.3f, %.3f, %.3f]\n", q[0], q[1], q[2], q[3] );
            printf( "  Position: [%.3f, %.3f, %.3f]\n", pos[0], pos[1], pos[2] );
        }

        // Convert quaternion to rotation matrix
        mat4 local;
        QuaternionMatrix( q, local );

        // Set translation in the 4th column (column-major format)
        local[3][0] = pos[0];
        local[3][1] = pos[1];
        local[3][2] = pos[2];

        // DEBUG: Print bone 0 local matrix
        if ( i == 0 && frame < 2 )
        {
            printf( "  Local matrix (column-major):\n" );
            printf( "    [%.3f %.3f %.3f %.3f]\n", local[0][0], local[0][1], local[0][2], local[0][3] );
            printf( "    [%.3f %.3f %.3f %.3f]\n", local[1][0], local[1][1], local[1][2], local[1][3] );
            printf( "    [%.3f %.3f %.3f %.3f]\n", local[2][0], local[2][1], local[2][2], local[2][3] );
            printf( "    [%.3f %.3f %.3f %.3f]\n", local[3][0], local[3][1], local[3][2], local[3][3] );
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

        // DEBUG: Print bone 0 final matrix
        if ( i == 0 && frame < 2 )
        {
            printf( "  Final matrix (column-major):\n" );
            printf( "    [%.3f %.3f %.3f %.3f]\n", bone_transformations[i][0][0], bone_transformations[i][0][1], bone_transformations[i][0][2], bone_transformations[i][0][3] );
            printf( "    [%.3f %.3f %.3f %.3f]\n", bone_transformations[i][1][0], bone_transformations[i][1][1], bone_transformations[i][1][2], bone_transformations[i][1][3] );
            printf( "    [%.3f %.3f %.3f %.3f]\n", bone_transformations[i][2][0], bone_transformations[i][2][1], bone_transformations[i][2][2], bone_transformations[i][2][3] );
            printf( "    [%.3f %.3f %.3f %.3f]\n\n", bone_transformations[i][3][0], bone_transformations[i][3][1], bone_transformations[i][3][2], bone_transformations[i][3][3] );
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
