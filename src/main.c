/*======================================================================
   File: main.c
   Project: Half-Life Model Viewer with Animation & Seqgroups
   Author: karlosiric
   Updated: 2025-10-14 (FINAL VERSION WITH SEQGROUPS)
   ---------------------------------------------------------------------
   Description:
       Main entry point with proper seqgroup loading
   ---------------------------------------------------------------------
   License: MIT
   Version: 1.0.0 - WORKING WITH ANIMATIONS!
 ======================================================================*/

#include "main.h"

#include "graphics/renderer.h"
#include "mdl/mdl_info.h"
#include "mdl/mdl_loader.h"
#include "studio.h"
#include "utils/logger.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else
#include <GL/gl3.h>
#endif

t_log_options log_options = {
    .file_path     = "../logs/viewer.log",
    .max_bytes     = 0,
    .max_files     = 0,
    .use_colors    = true,
    .json_lines    = false,
    .console_level = LOG_TRACE
};

int main(int argc, char const *argv[])
{
    // Initialize logger
    logger_init(&log_options);
    logger_set_global_level(LOG_INFO);
    logger_set_category_level("renderer", LOG_DEBUG);
    logger_set_category_level("mdl", LOG_DEBUG);
    logger_set_category_level("textures", LOG_DEBUG);

    LOG_INFOF("app", "Logger Initialized");
    LOG_INFOF("app", "Application started: PID %d\n", getpid());

    // Check arguments
    if (argc != 2) {
        LOG_INFOF("app", "Usage: %s <model.mdl>", argv[0]);
        logger_shutdown();
        return 1;
    }

    // ==================================================================
    // METHOD 1: Use the NEW create_mdl_model() function (RECOMMENDED!)
    // ==================================================================
    
    LOG_TIME_BLOCK("load_complete_model", "mdl");
    
    mdl_model_t *model = NULL;
    mdl_result_t result = create_mdl_model(argv[1], &model);
    
    if (result != MDL_SUCCESS) {
        LOG_ERRORF("mdl", "Failed to load model! Error code: %d\n", result);
        logger_shutdown();
        return 1;
    }
    
    LOG_INFOF("mdl", "Model loaded successfully!");
    LOG_INFOF("mdl", "  Main header: %p", (void*)model->header);
    LOG_INFOF("mdl", "  Texture header: %p", (void*)model->texture_header);
    LOG_INFOF("mdl", "  Sequence groups: %d", model->num_seqgroups);

    // Print complete model analysis
    print_complete_model_analysis(
        stdout, 
        argv[1], 
        model->header, 
        model->texture_header, 
        model->data, 
        model->texture_data
    );

    // Initialize renderer
    LOG_INFOF("renderer", "Initializing renderer...");
    if (init_renderer(WIDTH, HEIGHT, "Half-Life Model Viewer") != 0) {
        LOG_ERRORF("renderer", "Failed to initialize renderer!");
        free_model(model);
        logger_shutdown();
        return 1;
    }

    // ==================================================================
    // CRITICAL: Pass ALL data including seqgroups to renderer!
    // ==================================================================
    set_model_data(
        model->header,
        model->data,
        model->texture_header,
        model->texture_data,
        model->seqgroups,         // <-- NEW! Seqgroups parameter
        model->num_seqgroups      // <-- NEW! Number of seqgroups
    );

    // Start render loop
    render_loop();

    // Cleanup
    cleanup_renderer();
    free_model(model);  // This frees everything including seqgroups

    LOG_INFOF("app", "Shutting down");
    LOG_INFOF("app", "Application PID: %d killed!\n", getpid());
    logger_shutdown();
    
    return 0;
}
