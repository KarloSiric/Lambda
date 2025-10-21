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
 *   Purpose: Application configuration run time options.
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include "app_config.h"

void app_config_setup_logger_options( const app_args_t *args, t_log_options *log_opts )
{
    // First we always initialize to these values
    log_opts->max_bytes     = 0;
    log_opts->max_files     = 0;
    log_opts->use_colors    = true;
    log_opts->json_lines    = false;
    
    // Check if the quiet mode is being applied
    if ( args->quiet )
    {
        log_opts->console_level = LOG_ERROR;
        log_opts->use_colors    = false;
    }
    else
    {
        log_opts->console_level = LOG_INFO;
        log_opts->use_colors    = true;
    }
    
    // here we overwrite certain log option values that need to be changed
    switch ( args->log_level )
    {
        case LOG_LEVEL_QUIET:
            log_opts->console_level = LOG_ERROR;
            log_opts->use_colors    = false;
            break;
        case LOG_LEVEL_NORMAL:
            log_opts->console_level = LOG_INFO;
            break;
        case LOG_LEVEL_VERBOSE:
            log_opts->console_level = LOG_DEBUG;
            break;
        case LOG_LEVEL_TRACE:
            log_opts->console_level = LOG_TRACE;
            break;
    }

    // If we somehow specified a different location for log file
    // NOTE(Karlo): So this always gets checked regardless and overwrites it
    if ( args->log_file )
    {
        log_opts->file_path = args->log_file;
    }
    else 
    {
        log_opts->file_path = "..logs/viewer.log";   
    }
}
