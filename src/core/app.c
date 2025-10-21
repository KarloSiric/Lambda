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
 *   Purpose: Application main runtime start point
 * ═══════════════════════════════════════════════════════════════════════════
 */


#include "app.h"

#include "app_init.h"
#include "../utils/logger.h"
#include "../graphics/renderer.h"
#include "../mdl/mdl_loader.h"


static app_state_t g_app_state = {0};


app_state_t *app_get_state(void)
{
    
    return &g_app_state;
    
}


int app_init(const app_args_t *args)
{
    
    
    if (!args)
    {
        LOG_ERRORF("app", "Arguments pointer is NULL");
        return ( -1 );
    }
    
    if (app_init_logger(args) != 0)
    {
        LOG_ERRORF("app", "Failed to initialize logger");
        return ( -1 );
    }
    
    LOG_INFOF("app", "Initializing application...");
    
    if (app_init_renderer(WIDTH, HEIGHT, "Half-Life Model Viewer - Lambda") != 0)
    {
        LOG_ERRORF("app", "Failed to initialize renderer");
        logger_shutdown();
        return ( -1 );
    }
    
    extern GLFWwindow *window;
    g_app_state.window = window;
    
    if (app_load_model(args->model_path, &g_app_state.model) != 0) 
    {
        LOG_ERRORF("app", "Failed to load model");
        cleanup_renderer();
        logger_shutdown();
        return ( -1 );
    }
    
    set_model_data(
        g_app_state.model->header, 
        g_app_state.model->data, 
        g_app_state.model->texture_header, 
        g_app_state.model->texture_data, 
        g_app_state.model->seqgroups, 
        g_app_state.model->num_seqgroups
    );
    
    
    g_app_state.current_sequence = 0;
    g_app_state.current_frame = 0;
    g_app_state.animation_playing = true;
    
    g_app_state.running = true;
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
}
