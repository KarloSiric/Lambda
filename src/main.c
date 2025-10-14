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
 *   Purpose: Command-Line Argument Parser Implementation
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include "main.h"

#include "platform.h"
#include "graphics/renderer.h"
#include "mdl/mdl_info.h"
#include "mdl/mdl_loader.h"
#include "mdl/mdl_report.h"
#include "studio.h"
#include "utils/args.h"
#include "utils/logger.h"

#include <stdio.h>
#include <stdlib.h>


t_log_options log_options = {
    .file_path     = "../logs/viewer.log",
    .max_bytes     = 0,
    .max_files     = 0,
    .use_colors    = true,
    .json_lines    = false,
    .console_level = LOG_ERROR  // Default to ERROR (quiet)
};

int main(int argc, char const *argv[])
{
    app_args_t args;
    if (parse_args(argc, argv, &args) != 0) {
        return 1;  // Error already printed by parse_args
    }
    
    // Show banner ALWAYS (Valve copyright)
    print_banner();
    
    // Show version if requested
    if (args.show_version) {
        print_version_info();
        return 0;
    }
    
    // Show help if requested
    if (args.show_help) {
        print_usage(argv[0]);
        return 0;
    }
    
    if (args.quiet) {
        log_options.console_level = LOG_ERROR;  // Only errors
        log_options.use_colors = false;
    } else {
        log_options.console_level = LOG_INFO;
        log_options.use_colors = true;
    }
    
    // CRITICAL FIX: Respect log_level from CLI args!
    switch (args.log_level) {
        case LOG_LEVEL_QUIET:
            log_options.console_level = LOG_ERROR;
            log_options.use_colors = false;
            break;
        case LOG_LEVEL_NORMAL:
            log_options.console_level = LOG_INFO;
            break;
        case LOG_LEVEL_VERBOSE:
            log_options.console_level = LOG_DEBUG;
            break;
        case LOG_LEVEL_TRACE:
            log_options.console_level = LOG_TRACE;  // <-- THIS WAS MISSING!
            break;
    }
    
    // Use log file if specified
    if (args.log_file) {
        log_options.file_path = args.log_file;
    }
    
    logger_init(&log_options);
    
    // Set global level based on CLI
    int global_level = LOG_INFO;  // default
    switch (args.log_level) {
        case LOG_LEVEL_QUIET:   global_level = LOG_ERROR; break;
        case LOG_LEVEL_NORMAL:  global_level = LOG_INFO; break;
        case LOG_LEVEL_VERBOSE: global_level = LOG_DEBUG; break;
        case LOG_LEVEL_TRACE:   global_level = LOG_TRACE; break;
    }
    logger_set_global_level(global_level);
    
    // Set per-category levels for verbose/trace
    if (args.log_level >= LOG_LEVEL_VERBOSE) {
        logger_set_category_level("renderer", LOG_DEBUG);
        logger_set_category_level("mdl", LOG_DEBUG);
        logger_set_category_level("textures", LOG_DEBUG);
        logger_set_category_level("animation", LOG_DEBUG);
        logger_set_category_level("seqgroup", LOG_DEBUG);
    }
    
    if (args.log_level == LOG_LEVEL_TRACE) {
        logger_set_category_level("renderer", LOG_TRACE);
        logger_set_category_level("mdl", LOG_TRACE);
        logger_set_category_level("textures", LOG_TRACE);
        logger_set_category_level("animation", LOG_TRACE);
        logger_set_category_level("seqgroup", LOG_TRACE);
    }
    
    if (!args.quiet) {
        LOG_INFOF("app", "Loading model: %s", args.model_path);
    }
    
    mdl_model_t *model = NULL;
    mdl_result_t result = create_mdl_model(args.model_path, &model);
    
    if (result != MDL_SUCCESS) {
        fprintf(stderr, "ERROR: Failed to load model '%s' (error code: %d)\n", 
                args.model_path, result);
        logger_shutdown();
        return 1;
    }
    
    if (!args.quiet) {
        LOG_INFOF("mdl", "Model loaded successfully!");
        LOG_INFOF("mdl", "  Bones: %d", model->header->numbones);
        LOG_INFOF("mdl", "  Bodyparts: %d", model->header->numbodyparts);
        LOG_INFOF("mdl", "  Sequences: %d", model->header->numseq);
        LOG_INFOF("mdl", "  Sequence groups: %d", model->num_seqgroups);
    }
    
    if (args.dump_level == DUMP_BASIC) {
        print_complete_model_analysis(stdout, args.model_path, model->header, 
                                       model->texture_header, model->data, model->texture_data);
        
        // Also print sequence group info
        print_sequence_group_info(stdout, model->seqgroups, model->num_seqgroups);
    }
    else if (args.dump_level == DUMP_EXTENDED) {
        print_extended_model_dump(stdout, args.model_path, model->header,
                                 model->texture_header, model->data, model->texture_data);
        
        // Also print sequence group info
        print_sequence_group_info(stdout, model->seqgroups, model->num_seqgroups);
    }    
    
    if (args.dump_only) {
        if (!args.quiet) {
            LOG_INFOF("app", "Dump complete. Exiting (--dump-only mode)");
        }
        free_model(model);
        logger_shutdown();
        return 0;  // Exit without opening viewer
    }
    
    if (!args.quiet) {
        LOG_INFOF("renderer", "Initializing OpenGL renderer...");
    }
    
    if (init_renderer(WIDTH, HEIGHT, "Half-Life Model Viewer") != 0) {
        fprintf(stderr, "ERROR: Failed to initialize renderer\n");
        free_model(model);
        logger_shutdown();
        return 1;
    }
    
    // Pass model data to renderer
    set_model_data(
        model->header,
        model->data,
        model->texture_header,
        model->texture_data,
        model->seqgroups,
        model->num_seqgroups
    );
    
    if (!args.quiet) {
        LOG_INFOF("renderer", "Starting render loop...");
    }
    
    // Start render loop (blocks until window closes)
    render_loop();
    
    if (!args.quiet) {
        LOG_INFOF("app", "Shutting down...");
    }
    
    cleanup_renderer();
    free_model(model);
    logger_shutdown();
    
    return 0;
}
