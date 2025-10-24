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
 *   Purpose: Main Entry point for the Application.
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include "core/app.h"
#include "mdl/mdl_report.h"
#include "utils/args.h"

#include <stdio.h>
#include <stdlib.h>

static app_args_t args = { 0 };

int main( int argc, char const *argv[] ) {
    
	if ( parse_args( argc, argv, &args ) != APP_INIT_SUCCESS ) {
		return ( APP_INIT_EXIT_SUCCESS );
	}
    
    

	if ( app_init( &args ) != APP_INIT_SUCCESS ) {
		return ( APP_INIT_EXIT_SUCCESS );
	}

	app_run();

	app_shutdown();

	return ( APP_INIT_SUCCESS );
}
