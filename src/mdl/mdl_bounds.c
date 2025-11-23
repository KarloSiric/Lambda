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
 *   Purpose: For calculating bounds of each model that the application loads
 * ═══════════════════════════════════════════════════════════════════════════
 */


#include "mdl_bounds.h"
#include "util_logger.h"
#include <float.h>


void mdl_bounds_calculate( const studiohdr_t *header, const unsigned char *data, mdl_bounds_t *bounds ) {
    
    // @Saftery checking always first
    if ( !header || !data || !bounds ) {
        LOG_WARNF( "bounds", "Invalid parameters passed to function: '%s'\n", __func__);
        return ;
    }
    
    
    float min_x = FLT_MAX;
    float min_y = FLT_MAX;
    float min_z = FLT_MAX;

    float max_x = FLT_MIN;
    float max_y = FLT_MIN;
    float max_z = FLT_MIN;

    // CRITICAL: Check sequence bounding boxes FIRST!
    // This accounts for animated poses (feet going down during walk, etc.)
    // T-pose vertices alone don't capture the full range of motion
    mstudioseqdesc_t *sequences = ( mstudioseqdesc_t *)( data + header->seqindex );
    for ( int seq = 0; seq < header->numseq; seq++ ) {
        mstudioseqdesc_t *sequence = &sequences[seq];

        // Update min/max with this sequence's bbox
        if ( sequence->bbmin[0] < min_x ) min_x = sequence->bbmin[0];
        if ( sequence->bbmin[1] < min_y ) min_y = sequence->bbmin[1];
        if ( sequence->bbmin[2] < min_z ) min_z = sequence->bbmin[2];

        if ( sequence->bbmax[0] > max_x ) max_x = sequence->bbmax[0];
        if ( sequence->bbmax[1] > max_y ) max_y = sequence->bbmax[1];
        if ( sequence->bbmax[2] > max_z ) max_z = sequence->bbmax[2];
    }

    LOG_DEBUGF( "bounds", "Sequence bounds: MIN(%.2f, %.2f, %.2f) MAX(%.2f, %.2f, %.2f)\n",
               min_x, min_y, min_z, max_x, max_y, max_z);

    // DO NOT check vertices! Sequence bboxes already contain the full range.
    // Checking vertices would make T-pose have different bounds than animations.
    // This ensures consistent ground positioning for ALL poses.
    
    
    bounds->min[0] = min_x;
    bounds->min[1] = min_y;
    bounds->min[2] = min_z;
    
    bounds->max[0] = max_x;
    bounds->max[1] = max_y;
    bounds->max[2] = max_z;
    
    bounds->valid = true;
    
    LOG_INFOF( "bounds", "Calculated bbox: MIN(%.2f, %.2f, %.2f) MAX(%.2f, %.2f, %.2f)\n",
              bounds->min[0], bounds->min[1], bounds->min[2], bounds->max[0], bounds->max[1], bounds->max[2]);
     
}


float mdl_bounds_get_height( const mdl_bounds_t *bounds ) {
    
    if ( !bounds || !bounds->valid ) {
        return ( 0.0f );
    }
    
    return ( bounds->max[2] - bounds->min[2] );
}

float mdl_bounds_get_width( const mdl_bounds_t *bounds ) {
    
    if ( !bounds || !bounds->valid ) {
        return ( 0.0f );
    }
    
    return ( bounds->max[0] - bounds->min[0] );
}

float mdl_bounds_get_depth( const mdl_bounds_t *bounds ) {
    
    if ( !bounds || !bounds->valid ) {
        return ( 0.0f );
    }
    
    return ( bounds->max[1] - bounds->min[1] );
}


