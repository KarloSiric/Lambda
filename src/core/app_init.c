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
 *   Purpose: Command-Line Argument Parser
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include "app_init.h"

#include "../utils/logger.h"
#include "app_config.h"

#include <stdio.h>

int app_init_logger( const app_args_t *args )
{
    t_log_options log_options;

    app_config_setup_logger_options( args, &log_options );

    logger_init( &log_options );

    int global_level = app_config_get_log_level( args );

    logger_set_global_level( global_level );

    if ( args->log_level >= LOG_LEVEL_VERBOSE )
    {
        logger_set_category_level( "renderer", LOG_DEBUG );
        logger_set_category_level( "mdl", LOG_DEBUG );
        logger_set_category_level( "textures", LOG_DEBUG );
        logger_set_category_level("animation", LOG_DEBUG);
        logger_set_category_level("seqgroup", LOG_DEBUG);
    }

    if (args->log_level == LOG_LEVEL_TRACE)
    {
        logger_set_category_level("renderer", LOG_TRACE);
        logger_set_category_level("mdl", LOG_TRACE);
        logger_set_category_level("textures", LOG_TRACE);
        logger_set_category_level("animation", LOG_TRACE);
        logger_set_category_level("seqgroup", LOG_TRACE);
    }
    
    return 0;
}
