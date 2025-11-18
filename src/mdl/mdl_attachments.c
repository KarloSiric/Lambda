/*
 * ═══════════════════════════════════════════════════════════════════════════
 *   Half-Life Model Viewer/Editor ~ Lambda
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
 *   Purpose: Attachments system implementation
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include "mdl_attachments.h"
#include "math/math_vector.h"
#include "math/math_matrix.h"
#include "util/util_logger.h"
#include <stdio.h>
#include <string.h>


int mdl_attachment_find( const studiohdr_t *header,
                         const unsigned char *data,
                         const char *name ) {
    
    if ( header == NULL || data == NULL || name == NULL ) {
        return ( -1 );   
    }
    
    int numattachments = header->numattachments;
    
    mstudioattachment_t *attachments = ( mstudioattachment_t * )( data + header->attachmentindex );
    
    for ( int i = 0; i < numattachments; i++ ) {
        mstudioattachment_t *attachment = &attachments[i];
        
        if ( strcmp( attachment->name, name ) == 0 ) {
            return i;
        } 
    }
    
    // @Note(Karlo): Not found so we return -1 as if it doesnt matter at all.
	return -1;
}

const mstudioattachment_t* mdl_attachment_get_info( const studiohdr_t *header,
                                                     const unsigned char *data,
                                                     int index ) { 
    if ( header == NULL || data == NULL || index < 0 || index >= header->numattachments ) {
        return NULL;
    }
     
    mstudioattachment_t *attachments = ( mstudioattachment_t * )( data + header->attachmentindex );
    
	return &attachments[index];
}


void mdl_attachment_get_world_position( const mstudioattachment_t *attachment,
                                        const math_mat4_t *bone_matrices,
                                        math_vec3_t out_position ) {
    
    if ( attachment == NULL || bone_matrices == NULL || out_position == NULL ) {
        return ;
    }
    
    int bone_index = attachment->bone;

    Math_Vec3TransformMat4( attachment->org, bone_matrices[bone_index], out_position );

    return ;    
}

void mdl_attachment_get_world_orientation( const mstudioattachment_t *attachment,
                                           const math_mat4_t *bone_matrices,
                                           math_vec3_t out_forward,
                                           math_vec3_t out_right,
                                           math_vec3_t out_up ) {
 
    if ( attachment == NULL || bone_matrices == NULL || out_forward == NULL || out_right == NULL || out_up == NULL ) {
        return ;
    }
    
    int bone_index = attachment->bone;

    // @Note(Karlo): Here we use the rotate instead of Transform because that is for position
    // THis does only rotation and not rotation and translation
    Math_Vec3RotateMat4( attachment->vectors[0], bone_matrices[bone_index], out_forward );
    Math_Vec3RotateMat4( attachment->vectors[1], bone_matrices[bone_index], out_right );
    Math_Vec3RotateMat4( attachment->vectors[2], bone_matrices[bone_index], out_up );

    return ;
}


void mdl_attachment_dump( const studiohdr_t *header,
                          const unsigned char *data ) {
	// TODO: Implement debug dump
	// Similar to mdl_bonecontroller_dump()
    
    if ( header == NULL || data == NULL ) {
        return ;
    }
    
    printf( "\n ==== ATTACHMENTS (%d) ====\n", header->numattachments );
    
    int numattachments = header->numattachments;
    mstudioattachment_t *attachments = ( mstudioattachment_t * )( data + header->attachmentindex );
    
    for ( int i = 0; i < numattachments; i++ ) {
        mstudioattachment_t *attachment = &attachments[i];
        printf( "[%d] '%s'\n", i, attachment->name );
        printf( "      Bone:     %d\n", attachment->bone );
        printf( "      Type:     %d\n", attachment->type );
        printf( "      Position: [%.2f, %.2f, %.2f]\n", attachment->org[0], attachment->org[1], attachment->org[2] );
        printf( "      Forward:  [%.2f, %.2f, %.2f]\n", attachment->vectors[0][0], attachment->vectors[0][1], attachment->vectors[0][2] );
        printf( "      Right:    [%.2f, %.2f, %.2f]\n", attachment->vectors[1][0], attachment->vectors[1][1], attachment->vectors[1][2] );
        printf( "      Up:       [%.2f, %.2f, %.2f]\n", attachment->vectors[2][0], attachment->vectors[2][1], attachment->vectors[2][2] );
    }    
}
