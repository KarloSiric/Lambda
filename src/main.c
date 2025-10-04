/*======================================================================
 *  File: main.c
 *  Project: shaders
 *  Author: karlosiric <email@example.com>
 *  Created: 2025-09-24 14:25:37
 *  Last Modified by: karlosiric
 *  Last Modified: 2025-10-03 13:54:11
 *----------------------------------------------------------------------
 *  Description:
 *
 *  Main entry point of the application itself
 *
 *----------------------------------------------------------------------
 *  License:
 *  Company:
 *  Version: 0.1.0
 *======================================================================
 */



#include "main.h"
#include "graphics/renderer.h"
#include "mdl/mdl_info.h"
#include "mdl/mdl_loader.h"
#include "studio.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else
#include <GL/gl3.h>
#endif



int main(int argc, char const *argv[])
{
    studiohdr_t   *main_header    = NULL;
    studiohdr_t   *texture_header = NULL;
    unsigned char *main_data      = NULL;
    unsigned char *texture_data   = NULL;

    if (argc != 2)
    {
        printf("Usage: %s <model.mdl>\n", argv[0]);
        return (1);
    }

    mdl_result_t result = load_model_with_textures(argv[1], &main_header, &texture_header, &main_data, &texture_data);

    if (result != MDL_SUCCESS)
    {
        printf("Failed to load model! Error code: %d\n", result);
        return (1);
    }

    print_complete_model_analysis(argv[1], main_header, texture_header, main_data, texture_data);

    printf("Initializing the renderer...\n");
    if (init_renderer(WIDTH, HEIGHT, "Half-Life Model Viewer") != 0)
    {
        printf("Failed to initialize renderer!\n");
        free(main_data);
        if (texture_data)
            free(texture_data);
        return (1);
    }

    // ONLY THESE TWO LINES - NO COMPLEX PROCESSING:
    set_model_data(main_header, main_data, texture_header, texture_data);
    render_loop();

    cleanup_renderer();

    // Clean up memory
    free(main_data);
    if (texture_data)
    {
        free(texture_data);
    }

    return (0);
}
