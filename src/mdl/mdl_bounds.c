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

    // Calculate bounds from ACTUAL VERTICES (not sequence boxes!)
    // Sequence bboxes can be inaccurate or in wrong coordinate space
    mstudiobodyparts_t *bodyparts = (mstudiobodyparts_t *)( data + header->bodypartindex );

    for ( int bp = 0; bp < header->numbodyparts; bp++ ) {
        mstudiobodyparts_t *bodypart = &bodyparts[bp];
        mstudiomodel_t *models = (mstudiomodel_t *)( data + bodypart->modelindex );

        for ( int m = 0; m < bodypart->nummodels; m++ ) {
            mstudiomodel_t *model = &models[m];
            vec3_t *vertices = (vec3_t *)( data + model->vertindex );

            for ( int v = 0; v < model->numverts; v++ ) {
                if ( vertices[v][0] < min_x ) min_x = vertices[v][0];
                if ( vertices[v][1] < min_y ) min_y = vertices[v][1];
                if ( vertices[v][2] < min_z ) min_z = vertices[v][2];

                if ( vertices[v][0] > max_x ) max_x = vertices[v][0];
                if ( vertices[v][1] > max_y ) max_y = vertices[v][1];
                if ( vertices[v][2] > max_z ) max_z = vertices[v][2];
            }
        }
    }

    LOG_DEBUGF( "bounds", "Vertex bounds: MIN(%.2f, %.2f, %.2f) MAX(%.2f, %.2f, %.2f)\n",
               min_x, min_y, min_z, max_x, max_y, max_z);
    
    
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


