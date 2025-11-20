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
 *   Purpose: Audio system engine API implementation for events and sequences
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include "mdl_audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_SEARCH_PATHS                 8
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

// @Note: Adding some global scope variables needed for work
static struct ma_engine g_audio_engine;
static bool g_audio_initialized = false;


static char g_sound_search_paths[MAX_SEARCH_PATHS][512];
static int g_num_search_paths = 0;


static bool g_sounds_available = false;


bool mdl_audio_init( void ) {
    
    if ( g_audio_initialized ) {
        return true;
    }
    
    ma_result result = ma_engine_init( NULL, &g_audio_engine );
    
    if ( result != MA_SUCCESS ) {
        fprintf(stderr, "[AUDIO] Failed to initialize audio engine.\n");
        return false;
    }
    
    g_audio_initialized = true;
    
    printf( "[AUDIO] Audio engine initialized successfully!\n" );
    
    return true;
}


void mdl_audio_shutdown( void ) {
    
    if ( !g_audio_initialized ) {
        return ;
    }
    
    ma_engine_uninit( &g_audio_engine );
    g_audio_initialized = false;
    g_num_search_paths = 0;
    g_sounds_available = false;
    
    printf( "[AUDIO] Audio Engine shuwtdown.\n");
    
}

// @Note: Helper function for finding out where the 
static bool mdl_audio_directory_exists( const char *path ) {
    
    struct stat info;
    
    if ( stat(path, &info) != 0 ) {
        return false;
    }
    
    return ( info.st_mode & S_IFDIR ) != 0; 
}

// @Note: Need to extract things so -> /path/to/model.mdl -> /path/to
static void mdl_audio_extract_directory( const char *path, char *out_path, size_t out_size ) {
     
     strncpy( out_path, path, out_size - 1 );
     out_path[out_size - 1] = '\0'; 
     
     char *last_slash = strrchr( out_path, '/' );
     char *last_backslash = strrchr( out_path, '\\' );
     
     char *last_sep = ( last_slash > last_backslash ) ? last_slash : last_backslash;
     
     if ( last_sep ) {
        *last_sep = '\0';
     } 
}


bool mdl_audio_is_ready( void ) {
    // @Note: Just checking whether we have initialized and if sounds are ready
    
    return ( g_audio_initialized && g_sounds_available );
}


void mdl_audio_set_sound_directory( const char *sound_dir ) {
    
    // @Note: Always need to do a safety checing
    if ( sound_dir == NULL || sound_dir[0] == '\0' ) {
        fprintf( stderr, "[AUDIO] Invalid sound directory path\n" );
        return ;
    }
    
    
    // Checking if the directory exists
    if ( !mdl_audio_directory_exists( sound_dir ) ) {
        fprintf( stderr, "[AUDIO] Directory does not exist: '%s'\n", sound_dir );
        return ;
    }
    
    
    // @Note: Important we need to make space for the following paths if user specified his own path
    
    for ( int i = g_num_search_paths; i > 0; i-- ) {
        strcpy( g_sound_search_paths[i], g_sound_search_paths[i - 1] );
    }
    
    // Now we need to insert the following
    strncpy( g_sound_search_paths[0], sound_dir, sizeof( g_sound_search_paths[0] ) - 1 );
    
    g_sound_search_paths[0][sizeof(g_sound_search_paths[0]) - 1] = '\0';
    
    g_num_search_paths++;
    g_sounds_available = true;
    
    printf( "[AUDIO] Added sound directory (priority): %s\n", sound_dir );
}


bool mdl_audio_play_event_sound( const char *relative_path ) {
    
    
    // @Note: Bunch of safety checking, this is important
    if ( relative_path == NULL || relative_path[0] == '\0' ) {
        return false;
    }
    
    if ( !g_audio_initialized ) {
        fprintf( stderr, "[AUDIO] Cannot play sound: engine not initialized.\n");
        return false;
    }
    
    if ( !g_sounds_available ) {
        return false;
    }
    
    
    for ( int i = 0; i < g_sound_search_paths; i++ ) {
        
    }
    
    
    
    
    
    
    
    
}















