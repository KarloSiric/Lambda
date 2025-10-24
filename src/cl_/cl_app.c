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

#include "cl_app.h"

#include "cl_app_init.h"
#include "utils/logger.h"
#include "graphics/renderer.h"
#include "mdl/mdl_loader.h"
#include "mdl/mdl_report.h"
#include <stdio.h>

static app_state_t g_app_state = { 0 };

static int handle_dump_mode( const app_args_t *args ) {
	mdl_model_t *local_model = NULL;

	if ( app_init_logger( args ) != APP_INIT_SUCCESS ) {
		fprintf( stderr, "ERROR: Failed to initialize logger\n" );
		return ( APP_INIT_ERROR );
	}

	LOG_INFO( "app", "Starting dump mode..." );
	if ( app_load_model( args->model_path, &local_model ) != APP_INIT_SUCCESS ) {
		LOG_ERRORF( "app", "Failed to load model: '%s'", local_model->header->name );
		logger_shutdown();
		return ( APP_INIT_ERROR );
	}

	if ( args->dump_level == DUMP_BASIC ) {
		print_complete_model_analysis(
			stdout,
			args->model_path,
			local_model->header,
			local_model->texture_header,
			local_model->data,
			local_model->texture_data );

		print_sequence_group_info(
			stdout,
			local_model->seqgroups,
			local_model->num_seqgroups );
	}

	else if ( args->dump_level == DUMP_EXTENDED ) {
		print_extended_model_dump(
			stdout,
			args->model_path,
			local_model->header,
			local_model->texture_header,
			local_model->data,
			local_model->texture_data );

		print_sequence_group_info(
			stdout,
			local_model->seqgroups,
			local_model->num_seqgroups );
	}

	free_model( local_model );

	if ( !args->quiet ) {
		LOG_INFO( "app", "Dump complete. Exiting." );
	}

	logger_shutdown();

	return ( APP_INIT_SUCCESS );
}

int app_init( const app_args_t *args ) {
	if ( !args ) {
		LOG_ERROR( "app", "Arguments pointer is NULL" );
		return ( APP_INIT_ERROR );
	}

	print_banner();

	// Show help if prompted for
	if ( args->show_help ) {
		print_usage( "Half-Life Model Viewer - Lambda" );
		return ( APP_INIT_EXIT_SUCCESS );
	}

	// Show version if prompted for
	if ( args->show_version ) {
		print_version_info();
		return ( APP_INIT_EXIT_SUCCESS );
	}

	if ( args->dump_only ) {
		return handle_dump_mode( args );
	}

	// Everything else continues just as it was

	if ( app_init_logger( args ) != APP_INIT_SUCCESS ) {
		LOG_ERROR( "app", "Failed to initialize logger" );
		return ( APP_INIT_ERROR );
	}

	LOG_INFO( "app", "Initializing application..." );

	if ( app_init_renderer( WIDTH, HEIGHT, "Half-Life Model Viewer - Lambda" ) != 0 ) {
		LOG_ERROR( "app", "Failed to initialize renderer" );
		logger_shutdown();
		return ( APP_INIT_ERROR );
	}

	extern GLFWwindow *window;
	g_app_state.window = window;

	if ( app_load_model( args->model_path, &g_app_state.model ) != APP_INIT_SUCCESS ) {
		LOG_ERROR( "app", "Failed to load model" );
		cleanup_renderer();
		logger_shutdown();
		return ( APP_INIT_ERROR );
	}

	set_model_data(
		g_app_state.model->header,
		g_app_state.model->data,
		g_app_state.model->texture_header,
		g_app_state.model->texture_data,
		g_app_state.model->seqgroups,
		g_app_state.model->num_seqgroups );

	g_app_state.current_sequence = 0;
	g_app_state.current_frame = 0;
	g_app_state.animation_playing = true;

	g_app_state.running = true;
	g_app_state.initialized = true;

	LOG_INFO( "app", "Application initialized successfully!\n" );

	return ( APP_INIT_SUCCESS );
}

int app_run( void ) {
	if ( !g_app_state.initialized ) {
		LOG_ERROR( "app", "Cannot run: Application not initialized" );
		return ( APP_INIT_ERROR );
	}

	LOG_INFO( "app", "Starting main loop..." );

	// NOTE(Karlo): Later we should move the rendering loop logic to here

	render_loop();

	g_app_state.running = false;

	LOG_INFO( "app", "Main loop exited" );

	return ( APP_INIT_SUCCESS );
}

void app_shutdown( void ) {
	if ( !g_app_state.initialized ) {
		// It is either already shut down or not initialized
		return;
	}

	LOG_INFO( "app", "Shutting down application..." );

	if ( g_app_state.model ) {
		free_model( g_app_state.model );
		g_app_state.model = NULL;
	}

	cleanup_renderer();

	g_app_state.initialized = false;
	g_app_state.running = false;

	logger_shutdown();
}
