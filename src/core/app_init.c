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
 *   Purpose: Application initialization itself.
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include "app_init.h"

#include "app_config.h"
#include "../utils/logger.h"
#include "../graphics/renderer.h"
#include "../mdl/mdl_loader.h"

#include <stdio.h>

int app_init_logger(const app_args_t *args)
{
    t_log_options log_options;

    app_config_setup_logger_options(args, &log_options);

    logger_init(&log_options);

    logger_set_global_level(log_options.console_level);

    if (args->log_level >= LOG_LEVEL_VERBOSE)
    {
        logger_set_category_level("renderer", LOG_DEBUG);
        logger_set_category_level("mdl", LOG_DEBUG);
        logger_set_category_level("textures", LOG_DEBUG);
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

int app_load_model(const char *model_path, mdl_model_t **model_out)
{
    if (!model_path)
    {
        LOG_ERRORF("app", "Model path is NULL!");
        LOG_ERRORF("app", "Invalid parameter: model_path=%p", (void *)model_path);
        return (-1);
    }

    if (!model_out)
    {
        LOG_ERRORF("app", "Model Output pointer is NULL");
        LOG_ERRORF("app", "Invalid parameter: model_out=%p", (void *)model_out);
        return (-1);
    }

    LOG_INFOF("app", "Loading model: '%s'", model_path);

    mdl_result_t result = create_mdl_model(model_path, model_out);

    if (result != MDL_SUCCESS)
    {
        LOG_ERRORF("mdl", "Failed to load model: '%s' (error code: %d)",
                  model_path, result);
        return (-1);
    }
    
    mdl_model_t *model = *model_out;
    
    LOG_INFOF("mld", "Model loaded successfully!");
    LOG_INFOF("mdl", "   Bones: %d", model->header->numbones);
    LOG_INFOF("mdl", "   Bodyparts: %d", model->header->numbodyparts);
    LOG_INFOF("mdl", "   Sequences: %d", model->header->numseq);
    LOG_INFOF("mdl", "   Sequence groups: %d", model->num_seqgroups);
    
    return (0);
}


int app_init_renderer(int width, int height, const char *title)
{
    
    if (width <= 0 || height <= 0)
    {
        LOG_ERRORF("renderer", "Invalid window dimensions: %dx%d", width, height);
        return (-1);
    }
    
    if (!title)
    {
        LOG_ERRORF("renderer", "Window title is NULL");
        LOG_ERRORF("renderer", "Invalid parameter: title=%p", (void *)title);
        return (-1);
    }
    
    LOG_INFOF("renderer", "Initializing OpenGL renderer (%dx%d) ...",
              width, height);
    
    
    int result = init_renderer(width, height, title);
    
    if (result != MDL_SUCCESS)
    {
        LOG_ERRORF("renderer", "Failed to initialize renderer");
        return (-1);
    }
    
    LOG_INFOF("renderer", "Renderer initialized successfully!");
    
    return (0);
}
