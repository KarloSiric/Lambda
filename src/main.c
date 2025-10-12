/*======================================================================
   File: main.c
   Project: shaders
   Author: karlosiric <email@example.com>
   Created: 2025-10-08 16:59:01
   Last Modified by: karlosiric
   Last Modified: 2025-10-11 23:24:45
   ---------------------------------------------------------------------
   Description:
       
   ---------------------------------------------------------------------
   License: 
   Company: 
   Version: 0.1.0
 ======================================================================
                                                                       */

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

t_log_options log_options
    = { .file_path     = "../logs/viewer.log",
        .max_bytes     = 0,
        .max_files     = 0,
        .use_colors    = true,
        .json_lines    = false,
        .console_level = LOG_TRACE };

int main( int argc, char const *argv[] )
{
    logger_init( &log_options );
    logger_set_global_level( LOG_INFO );
    logger_set_category_level( "renderer", LOG_DEBUG );
    logger_set_category_level( "mdl", LOG_DEBUG );
    logger_set_category_level( "textures", LOG_DEBUG );
    logger_set_category_level( "renderer", LOG_DEBUG );

    LOG_INFOF( "app", "Logger Initialized" );
    LOG_INFOF( "app", "Application started: PID %d\n", getpid( ) );

    studiohdr_t   *main_header    = NULL;
    studiohdr_t   *texture_header = NULL;
    unsigned char *main_data      = NULL;
    unsigned char *texture_data   = NULL;

    if ( argc != 2 )
    {
        LOG_INFOF( "app", "Usage: %s <model.mdl>", argv[0] );
        logger_shutdown( );
        return ( 1 );
    }

    LOG_TIME_BLOCK( "load_model_with_texturs", "mdl" );
    mdl_result_t result = load_model_with_textures( argv[1], &main_header, &texture_header, &main_data, &texture_data );

    if ( result != MDL_SUCCESS )
    {
        LOG_ERRORF( "mdl", "Failed to load model! Error code: %d\n", result );
        return ( 1 );
    }

    print_complete_model_analysis( stdout, argv[1], main_header, texture_header, main_data, texture_data );

    LOG_INFOF( "renederer", "Initializing renderer ..." );

    if ( init_renderer( WIDTH, HEIGHT, "Half-Life Model Viewer" ) != 0 )
    {
        LOG_ERRORF( "renderer", "Failed to initialize renderer!" );
        free( main_data );
        if ( texture_data )
            free( texture_data );
        return ( 1 );
    }

    // ONLY THESE TWO LINES - NO COMPLEX PROCESSING:
    set_model_data( main_header, main_data, texture_header, texture_data );
    render_loop( );

    cleanup_renderer( );

    // Clean up memory
    free( main_data );
    if ( texture_data )
    {
        free( texture_data );
    }

    LOG_INFOF( "app", "Shutting down" );
    LOG_INFOF( "app", "Application PID: %d killed!\n", getpid( ) );
    logger_shutdown( );
    return ( 0 );
}
