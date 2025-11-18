/*
 * ═══════════════════════════════════════════════════════════════════════════
 *   Half-Life Model Viewer
 * ═══════════════════════════════════════════════════════════════════════════
 *
 *   Copyright (c) 1996-2002, Valve LLC. All rights reserved.
 *
 *   This product contains software technology licensed from Id
 *   Software, Inc. ("Id Technology"). Id Technology (c) 1996 Id Software, Inc.
 *   All Rights Reserved.
 *
 *   Use, distribution, and modification of this source code and/or resulting
 *   object code is restricted to non-commercial enhancements to products from
 *   Valve LLC. All other use, distribution, or modification is prohibited
 *   without written permission from Valve LLC.
 *
 * ───────────────────────────────────────────────────────────────────────────
 *   Author: Karlo Siric
 *   Purpose: Hitboxes System implementation
 * ═══════════════════════════════════════════════════════════════════════════
 */


#include "mdl_hitboxes.h"
#include "math_vector.h"
#include <stdio.h>

const mstudiobbox_t* mdl_hitbox_get_info( const studiohdr_t *header,
                                           const unsigned char *data,
                                           int index ) { 
    
    if ( header == NULL || data == NULL || index < 0 || index >= header->numhitboxes ) {
        return NULL;
    }
    
    mstudiobbox_t *hitboxes = ( mstudiobbox_t *)( data + header->hitboxindex );
    
    return &hitboxes[index];
    
}


void mdl_hitbox_world_bounds( const mstudiobbox_t *hitbox,
                               const math_mat4_t *bone_matrices,
                               math_vec3_t out_min,
                               math_vec3_t out_max ) {
    
    if ( hitbox == NULL || bone_matrices == NULL || out_min == NULL || out_max == NULL ) {
        return ;
    }
    
    int bone_index = hitbox->bone;
    
    Math_Vec3TransformMat4( hitbox->bbmin, bone_matrices[bone_index], out_min );
    Math_Vec3TransformMat4( hitbox->bbmax, bone_matrices[bone_index], out_max );
    
    return ;
}


void mdl_hitbox_print_dump( const studiohdr_t *header,
                             const unsigned char *data ) {
    
    if ( header == NULL || data == NULL ) {
        return ;
    }
    
    printf("\n==== HITBOXES [%d] ====\n", header->numhitboxes );
    const mstudiobbox_t *hitboxes = ( mstudiobbox_t * )( data + header->hitboxindex );
    
    int numhitboxes = header->numhitboxes;  
    
    for ( int i = 0; i < numhitboxes; i++ ) {
        const mstudiobbox_t *hitbox = &hitboxes[i];
        printf( "Hitbox [%d] - '%d'\n", i, hitbox->group );
        printf( "       Bone:   %d\n", hitbox->bone );
        printf( "       Bbmin:  [%.2f, %.2f, %.2f]\n", hitbox->bbmin[0], hitbox->bbmin[1], hitbox->bbmin[2] );       
        printf( "       Bbmax:  [%.2f, %.2f, %.2f]\n", hitbox->bbmax[0], hitbox->bbmax[1], hitbox->bbmax[2] );       
    }
    
    return ;
}
