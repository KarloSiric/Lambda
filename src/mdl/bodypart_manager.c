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
 *   Purpose: Command-Line Argument Parser Implementation
 * ═══════════════════════════════════════════════════════════════════════════
 */


#include "bodypart_manager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global state
static bodypart_state_t g_bodypart_state = { 0 };
static studiohdr_t     *g_header         = NULL;
static unsigned char   *g_data           = NULL;

// Debug string buffer
static char debug_string[1024];

void bodypart_init( void ) {
    memset( &g_bodypart_state, 0, sizeof( g_bodypart_state ) );
    g_bodypart_state.debug_mode = true;    // Enable debug by default for testing
    g_header                    = NULL;
    g_data                      = NULL;
}

void bodypart_set_model( studiohdr_t *header, unsigned char *data ) {
    g_header = header;
    g_data   = data;

    // Reset state for new model
    memset( &g_bodypart_state, 0, sizeof( g_bodypart_state ) );
    g_bodypart_state.debug_mode = true;

    if ( !header || !data )
    {
        printf( "BODYPART: No model loaded\n" );
        return;
    }

    printf( "\n=== BODYPART MANAGER INITIALIZED ===\n" );
    printf( "Model: %s\n", header->name );
    printf( "Number of bodyparts: %d\n", header->numbodyparts );

    // Print info about each bodypart
    mstudiobodyparts_t *bodyparts = ( mstudiobodyparts_t * ) ( data + header->bodypartindex );
    for ( int i = 0; i < header->numbodyparts; i++ )
    {
        printf(
            "  Bodypart %d: '%s' (%d models, base=%d)\n",
            i,
            bodyparts[i].name,
            bodyparts[i].nummodels,
            bodyparts[i].base );

        // List all model variations
        mstudiomodel_t *models = ( mstudiomodel_t * ) ( data + bodyparts[i].modelindex );
        for ( int j = 0; j < bodyparts[i].nummodels; j++ )
        {
            printf( "    Model %d: '%s'\n", j, models[j].name );
        }
    }
    printf( "=====================================\n\n" );
}

int bodypart_get_bodygroup( void ) {
    return g_bodypart_state.bodygroup;
}

void bodypart_set_bodygroup( int bodygroup ) {
    if ( !g_header || !g_data )
        return;

    g_bodypart_state.bodygroup = bodygroup;

    // Unpack the bodygroup value into individual submodel indices
    mstudiobodyparts_t *bodyparts = ( mstudiobodyparts_t * ) ( g_data + g_header->bodypartindex );

    for ( int i = 0; i < g_header->numbodyparts; i++ )
    {
        if ( bodyparts[i].nummodels <= 1 )
        {
            g_bodypart_state.submodel_indices[i] = 0;
        }
        else
        {
            g_bodypart_state.submodel_indices[i] = ( bodygroup / bodyparts[i].base ) % bodyparts[i].nummodels;
        }
    }
}

int bodypart_get_model_index( int bodypart_index ) {
    if ( !g_header || !g_data )
        return 0;
    if ( bodypart_index < 0 || bodypart_index >= g_header->numbodyparts )
        return 0;

    mstudiobodyparts_t *bodyparts = ( mstudiobodyparts_t * ) ( g_data + g_header->bodypartindex );
    mstudiobodyparts_t *bp        = &bodyparts[bodypart_index];

    // Calculate which model to use based on current bodygroup
    int model_index = ( g_bodypart_state.bodygroup / bp->base ) % bp->nummodels;

    // Ensure it's within bounds
    if ( model_index < 0 || model_index >= bp->nummodels )
    {
        model_index = 0;
    }

    return model_index;
}

void bodypart_next_submodel( void ) {
    if ( !g_header || !g_data )
        return;
    if ( g_bodypart_state.current_bodypart >= g_header->numbodyparts )
        return;

    mstudiobodyparts_t *bodyparts = ( mstudiobodyparts_t * ) ( g_data + g_header->bodypartindex );
    mstudiobodyparts_t *bp        = &bodyparts[g_bodypart_state.current_bodypart];

    if ( bp->nummodels <= 1 )
        return;    // No variations to cycle through

    // Get current model index for this bodypart
    int current = bodypart_get_model_index( g_bodypart_state.current_bodypart );
    int next    = ( current + 1 ) % bp->nummodels;

    // Calculate new bodygroup value
    g_bodypart_state.bodygroup = ( g_bodypart_state.bodygroup - ( current * bp->base ) ) + ( next * bp->base );
    g_bodypart_state.submodel_indices[g_bodypart_state.current_bodypart] = next;

    // Print what we switched to
    mstudiomodel_t *models = ( mstudiomodel_t * ) ( g_data + bp->modelindex );
    printf(
        "Bodypart %d '%s' -> Model %d '%s'\n", g_bodypart_state.current_bodypart, bp->name, next, models[next].name );
}

void bodypart_prev_submodel( void ) {
    if ( !g_header || !g_data )
        return;
    if ( g_bodypart_state.current_bodypart >= g_header->numbodyparts )
        return;

    mstudiobodyparts_t *bodyparts = ( mstudiobodyparts_t * ) ( g_data + g_header->bodypartindex );
    mstudiobodyparts_t *bp        = &bodyparts[g_bodypart_state.current_bodypart];

    if ( bp->nummodels <= 1 )
        return;

    int current = bodypart_get_model_index( g_bodypart_state.current_bodypart );
    int prev    = ( current - 1 + bp->nummodels ) % bp->nummodels;

    g_bodypart_state.bodygroup = ( g_bodypart_state.bodygroup - ( current * bp->base ) ) + ( prev * bp->base );
    g_bodypart_state.submodel_indices[g_bodypart_state.current_bodypart] = prev;

    mstudiomodel_t *models = ( mstudiomodel_t * ) ( g_data + bp->modelindex );
    printf(
        "Bodypart %d '%s' -> Model %d '%s'\n", g_bodypart_state.current_bodypart, bp->name, prev, models[prev].name );
}

void bodypart_select_next( void ) {
    if ( !g_header || !g_data )
        return;

    g_bodypart_state.current_bodypart = ( g_bodypart_state.current_bodypart + 1 ) % g_header->numbodyparts;

    mstudiobodyparts_t *bodyparts = ( mstudiobodyparts_t * ) ( g_data + g_header->bodypartindex );
    printf(
        "Selected bodypart %d: '%s'\n",
        g_bodypart_state.current_bodypart,
        bodyparts[g_bodypart_state.current_bodypart].name );
}

void bodypart_select_prev( void ) {
    if ( !g_header || !g_data )
        return;

    g_bodypart_state.current_bodypart
        = ( g_bodypart_state.current_bodypart - 1 + g_header->numbodyparts ) % g_header->numbodyparts;

    mstudiobodyparts_t *bodyparts = ( mstudiobodyparts_t * ) ( g_data + g_header->bodypartindex );
    printf(
        "Selected bodypart %d: '%s'\n",
        g_bodypart_state.current_bodypart,
        bodyparts[g_bodypart_state.current_bodypart].name );
}

void bodypart_reset_all( void ) {
    g_bodypart_state.bodygroup        = 0;
    g_bodypart_state.current_bodypart = 0;
    memset( g_bodypart_state.submodel_indices, 0, sizeof( g_bodypart_state.submodel_indices ) );

    printf( "All bodyparts reset to defaults\n" );
}

void bodypart_print_info( void ) {
    if ( !g_header || !g_data )
    {
        printf( "No model loaded\n" );
        return;
    }

    printf( "\n=== CURRENT BODYPART CONFIGURATION ===\n" );
    printf( "Bodygroup value: %d\n", g_bodypart_state.bodygroup );
    printf( "Selected bodypart: %d\n", g_bodypart_state.current_bodypart );

    mstudiobodyparts_t *bodyparts = ( mstudiobodyparts_t * ) ( g_data + g_header->bodypartindex );

    for ( int i = 0; i < g_header->numbodyparts; i++ )
    {
        int             model_idx = bodypart_get_model_index( i );
        mstudiomodel_t *models    = ( mstudiomodel_t * ) ( g_data + bodyparts[i].modelindex );

        const char *marker = ( i == g_bodypart_state.current_bodypart ) ? " <--" : "";
        printf(
            "  [%d] %s: Model %d/%d - '%s'%s\n",
            i,
            bodyparts[i].name,
            model_idx + 1,
            bodyparts[i].nummodels,
            models[model_idx].name,
            marker );
    }
    printf( "======================================\n\n" );
}

const char *bodypart_get_debug_string( void ) {
    if ( !g_header || !g_data )
    {
        sprintf( debug_string, "No model loaded" );
        return debug_string;
    }

    mstudiobodyparts_t *bodyparts     = ( mstudiobodyparts_t * ) ( g_data + g_header->bodypartindex );
    int                 current_model = bodypart_get_model_index( g_bodypart_state.current_bodypart );
    mstudiomodel_t *models = ( mstudiomodel_t * ) ( g_data + bodyparts[g_bodypart_state.current_bodypart].modelindex );
    
    sprintf(
        debug_string,
        "Bodygroup: %d | Part %d/%d: %s | Model %d/%d: %s\n"
        "Keys: [/] = Select Part, -/+ = Change Model, 0 = Reset",
        g_bodypart_state.bodygroup,
        g_bodypart_state.current_bodypart + 1,
        g_header->numbodyparts,
        bodyparts[g_bodypart_state.current_bodypart].name,
        current_model + 1,
        bodyparts[g_bodypart_state.current_bodypart].nummodels,
        models[current_model].name );

    return debug_string;
}

bool bodypart_should_render_model( int bodypart_index, int model_index ) {
    if ( !g_header || !g_data )
        return false;
    if ( bodypart_index < 0 || bodypart_index >= g_header->numbodyparts )
        return false;

    int selected_model = bodypart_get_model_index( bodypart_index );
    return ( model_index == selected_model );
}
