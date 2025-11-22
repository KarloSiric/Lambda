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
#include <time.h>


#define MAX_SEARCH_PATHS                 8
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"


// @Note: Adding some global scope variables needed for work
static struct ma_engine g_audio_engine;
static bool g_audio_initialized = false;


static char g_sound_search_paths[MAX_SEARCH_PATHS][512];
static int g_num_search_paths = 0;


static bool g_sounds_available = false;
static bool g_random_seed_initialized = false;
static bool g_audio_enabled = false;  // OFF by default, toggle with key

// Debouncing to prevent sound overlap
#define SOUND_COOLDOWN_MS 1000  // Minimum time between same sound plays
static char g_last_played_sound[512] = {0};
static clock_t g_last_played_time = 0;

// Sound tracking for proper cleanup
#define MAX_ACTIVE_SOUNDS 16
static ma_sound g_active_sounds[MAX_ACTIVE_SOUNDS];
static bool g_sound_slots_active[MAX_ACTIVE_SOUNDS] = {0};


/* ****************
 *
 * HELPER FUNCTIONS
 *
 * ****************/

// Find a free sound slot or return -1 if all full
static int find_free_sound_slot( void ) {
    for ( int i = 0; i < MAX_ACTIVE_SOUNDS; i++ ) {
        if ( !g_sound_slots_active[i] ) {
            return i;
        }
    }
    return -1;
}

// Clean up finished sounds to free slots
static void cleanup_finished_sounds( void ) {
    for ( int i = 0; i < MAX_ACTIVE_SOUNDS; i++ ) {
        if ( g_sound_slots_active[i] ) {
            if ( !ma_sound_is_playing( &g_active_sounds[i] ) ) {
                ma_sound_uninit( &g_active_sounds[i] );
                g_sound_slots_active[i] = false;
            }
        }
    }
}

// Play a sound and track it
static bool play_and_track_sound( const char *file_path ) {
    // Clean up finished sounds first
    cleanup_finished_sounds();

    // Find a free slot
    int slot = find_free_sound_slot();
    if ( slot == -1 ) {
        fprintf( stderr, "[AUDIO] No free sound slots available\n" );
        return false;
    }

    // Initialize and play the sound
    ma_result result = ma_sound_init_from_file( &g_audio_engine, file_path, 0, NULL, NULL, &g_active_sounds[slot] );
    if ( result != MA_SUCCESS ) {
        return false;
    }

    result = ma_sound_start( &g_active_sounds[slot] );
    if ( result != MA_SUCCESS ) {
        ma_sound_uninit( &g_active_sounds[slot] );
        return false;
    }

    g_sound_slots_active[slot] = true;
    return true;
}

static void extract_sound_base( const char *path, char *base_out, size_t base_size, char *ext_out, size_t ext_size ) {
    strncpy( base_out, path, base_size - 1 );
    base_out[base_size - 1] = '\0';

    // Find the extension
    char *ext = strrchr( base_out, '.' );
    if ( ext ) {
        strncpy( ext_out, ext, ext_size - 1 );
        ext_out[ext_size - 1] = '\0';
        *ext = '\0';  // Remove extension from base
    } else {
        ext_out[0] = '\0';
        return;
    }

    // Find the last digits before extension
    int len = strlen( base_out );
    int i = len - 1;

    // Walk backwards to find where digits start
    while ( i >= 0 && base_out[i] >= '0' && base_out[i] <= '9' ) {
        i--;
    }

    // Remove the digits
    if ( i < len - 1 ) {
        base_out[i + 1] = '\0';
    }
}

static bool mdl_audio_play_random_sound( const char *sound_path ) {

    // Initialize random seed once
    if ( !g_random_seed_initialized ) {
        srand( (unsigned int)time( NULL ) );
        g_random_seed_initialized = true;
    }

    // Extract base pattern
    char base[512];
    char ext[16];
    extract_sound_base( sound_path, base, sizeof(base), ext, sizeof(ext) );

    printf( "[AUDIO] Searching for random variants of '%s' (base: '%s', ext: '%s')\n",
           sound_path, base, ext );

    // Try random numbers 1-10
    int attempts = 10;
    int random_start = rand() % 10 + 1;

    for ( int try_num = 0; try_num < attempts; try_num++ ) {
        int num = (random_start + try_num - 1) % 10 + 1;

        for ( int i = 0; i < g_num_search_paths; i++ ) {
            char full_path[1024];
            snprintf( full_path, sizeof(full_path), "%s/%s%d%s",
                     g_sound_search_paths[i], base, num, ext );

            if ( play_and_track_sound( full_path ) ) {
                printf( "[AUDIO] Playing random variant #%d: '%s'\n", num, full_path );
                return true;
            }

            // Try lowercase version of extension
            char *wav_upper = strstr( full_path, ".WAV" );
            if ( wav_upper ) {
                memcpy( wav_upper, ".wav", 4 );
                if ( play_and_track_sound( full_path ) ) {
                    printf( "[AUDIO] Playing random variant #%d (lowercase): '%s'\n", num, full_path );
                    return true;
                }
            }
        }
    }

    // Fallback: try the original filename without number
    printf( "[AUDIO] No numbered variants found, trying original filename: '%s'\n", sound_path );

    for ( int i = 0; i < g_num_search_paths; i++ ) {
        char full_path[1024];
        snprintf( full_path, sizeof(full_path), "%s/%s", g_sound_search_paths[i], sound_path );

        if ( play_and_track_sound( full_path ) ) {
            printf( "[AUDIO] Playing original file: '%s'\n", full_path );
            return true;
        }

        // Try lowercase extension
        char *wav_upper = strstr( full_path, ".WAV" );
        if ( wav_upper ) {
            memcpy( wav_upper, ".wav", 4 );
            if ( play_and_track_sound( full_path ) ) {
                printf( "[AUDIO] Playing original file (lowercase): '%s'\n", full_path );
                return true;
            }
        }
    }

    // Really couldn't find it
    fprintf( stderr, "[AUDIO] Sound not found: '%s' (tried variants and original)\n", sound_path );
    return false;
}


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

void mdl_audio_toggle_enabled( void ) {
    g_audio_enabled = !g_audio_enabled;
    printf( "[AUDIO] Event sounds %s\n", g_audio_enabled ? "ENABLED" : "MUTED" );
}

bool mdl_audio_is_enabled( void ) {
    return g_audio_enabled;
}

void mdl_audio_stop_all_sounds( void ) {
    if ( !g_audio_initialized ) {
        return;
    }

    // Stop all playing sounds by stopping then immediately restarting the engine
    // This kills all active sounds but keeps the engine ready to play new ones
    ma_engine_stop( &g_audio_engine );
    ma_engine_start( &g_audio_engine );

    // Reset debouncing tracking
    g_last_played_sound[0] = '\0';
    g_last_played_time = 0;

    printf( "[AUDIO] Stopped all sounds\n" );
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

    // Check if audio is muted
    if ( !g_audio_enabled ) {
        return false;  // Silently skip when muted
    }

    // NOTE: Debouncing removed - it was blocking legitimate events at different frames
    // Sound stopping when switching sequences handles the real spam issue

    // Check if this is a random sound (starts with '*')
    if ( relative_path[0] == '*' ) {
        bool success = mdl_audio_play_random_sound( relative_path + 1 );  // Skip the '*'

        // Update debouncing tracking on success
        if ( success ) {
            strncpy( g_last_played_sound, relative_path, sizeof(g_last_played_sound) - 1 );
            g_last_played_sound[sizeof(g_last_played_sound) - 1] = '\0';
            g_last_played_time = clock();
        }

        return success;
    }


    for ( int i = 0; i < g_num_search_paths; i++ ) {
        char full_path[1024];

        snprintf( full_path, sizeof( full_path ), "%s/%s", g_sound_search_paths[i], relative_path );

        if ( play_and_track_sound( full_path ) ) {
            printf( "[AUDIO] Playing sound: '%s'\n", full_path );

            // Update debouncing tracking
            strncpy( g_last_played_sound, relative_path, sizeof(g_last_played_sound) - 1 );
            g_last_played_sound[sizeof(g_last_played_sound) - 1] = '\0';
            g_last_played_time = clock();

            return true;
        }
        // @Cleanup fix(Karlo): Adding to try the lowercase in case the sounds dont want to play
        char full_path_lower[1024];
        snprintf( full_path_lower, sizeof( full_path_lower ), "%s/%s", g_sound_search_paths[i], relative_path );

        char *wav_check = strstr( full_path_lower, ".WAV" );
        if ( wav_check ) {
            memcpy( wav_check, ".wav", 4 );

            if ( play_and_track_sound( full_path_lower ) ) {
                printf( "[AUDIO] Playing sound: '%s'\n", full_path_lower );

                // Update debouncing tracking
                strncpy( g_last_played_sound, relative_path, sizeof(g_last_played_sound) - 1 );
                g_last_played_sound[sizeof(g_last_played_sound) - 1] = '\0';
                g_last_played_time = clock();

                return true;
            }
        }
    }

    // @Note: Not playing anything
    fprintf( stderr, "[AUDIO] Sound not found: '%s'\n", relative_path );
    return false;
}


void mdl_audio_configure_for_model( const char *model_path ) {
    
    if ( model_path == NULL ) {
        return ;
    }
    
    g_num_search_paths = 0;
    g_sounds_available = false;
    
    
    /*
    // @Note: THe idea here is the following: first we find the directory that contains the .mdl model file
    //        After that we will use that for seraching the common root dir that contains the models/ sound/ or sounds/
    //        Then once we have that we do a smart search pattern to try to locate the sound/ and use name of the event for sound
    //        to find the sound file and call it and play it.
    */
    char model_dir[512];
    mdl_audio_extract_directory( model_path, model_dir, sizeof( model_dir ) );
    
    printf( "[AUDIO] Configuring for model: %s\n", model_path );
    printf( "[AUDIO] Searching algorithm starts from position: %s\n", model_dir );
    
    
    char current_dir[1024];
    strncpy( current_dir, model_dir, sizeof( current_dir ) - 1 );
    current_dir[sizeof( current_dir ) -1] = '\0';
    
    // @Note: Main loop for doing the algorithm search.
    // Might not be the best and most effective but it is the best I came up with that time
    for ( int level = 0; level < 10; level++ ) {
        
        char models_check[1024];
        char sound_check[1024];
        char sounds_check[1024]; // plural version of sound dir path just in case
        
        
        snprintf( models_check, sizeof( models_check ), "%s/models", current_dir );
        snprintf( sound_check, sizeof( sound_check ), "%s/sound", current_dir );
        snprintf( sounds_check, sizeof( sounds_check ), "%s/sounds", current_dir );
        
        // @Note: if the root contains both the models/ and sound/ or sounds/ then we found the root directory
        
        bool has_models = mdl_audio_directory_exists( models_check );
        bool has_sound = mdl_audio_directory_exists( sound_check );
        bool has_sounds = mdl_audio_directory_exists( sounds_check );
       
        if ( has_models && ( has_sound || has_sounds ) ) {
            printf(" [AUDIO] Found root directory: %s\n", current_dir );
        
        
            // Now we need to store that if sound/ exists we store it and increment paths
            if ( has_sound && g_num_search_paths < MAX_SEARCH_PATHS ) {
                strncpy( g_sound_search_paths[g_num_search_paths], sound_check, 
                         sizeof( g_sound_search_paths[g_num_search_paths] ) -1 );
                g_sound_search_paths[g_num_search_paths][sizeof( g_sound_search_paths[g_num_search_paths]) - 1] = '\0';
                printf( "[AUDIO] Added: %s\n", sound_check );
                g_num_search_paths++;
            }
            
            
            // Adding also sounds/ if it exists, same thing as sound/
            if ( has_sounds && g_num_search_paths < MAX_SEARCH_PATHS ) {
                strncpy( g_sound_search_paths[g_num_search_paths], sounds_check, 
                         sizeof( g_sound_search_paths[g_num_search_paths]) - 1 );
                g_sound_search_paths[g_num_search_paths][sizeof( g_sound_search_paths[g_num_search_paths]) - 1] = '\0';
                printf( "[AUDIO] Added: %s\n", sounds_check );
                g_num_search_paths++;
            }
            
            
            // @Note: Adding valve original directory as the fallback in case the valve_hd models
            //        Sounds are not being found by the program.
            
            char parent_dir[1024];
            strncpy( parent_dir, current_dir, sizeof( parent_dir ) - 1 );
            parent_dir[sizeof( parent_dir ) -1] = '\0';
            
            char *dir_sep = strrchr( parent_dir, '/' );
            if ( !dir_sep ) {
                // Fallback checking for Windows OS
                dir_sep = strrchr( parent_dir, '\\' );
            }
            
            if ( dir_sep && dir_sep != parent_dir ) {
                *dir_sep = '\0';
                
                char fallback_dir[1024];
                snprintf( fallback_dir, sizeof( fallback_dir ), "%s/valve/sound", parent_dir );
                
                // Now we check if valve/sound exists in the first place and we add it
                if ( mdl_audio_directory_exists( fallback_dir ) && g_num_search_paths < MAX_SEARCH_PATHS ) {
                    strncpy( g_sound_search_paths[g_num_search_paths], fallback_dir, 
                            sizeof( g_sound_search_paths[g_num_search_paths]) - 1 );
                    g_sound_search_paths[g_num_search_paths][sizeof( g_sound_search_paths[g_num_search_paths]) - 1] = '\0';   
                    printf( "[AUDIO] Added Fallback: %s\n", fallback_dir );
                    g_num_search_paths++;
                }
                
                // Now on success we just stop searching for it all and we are done basically
                g_sounds_available = true;
                
                printf( "[AUDIO] Configured %d sound path(s)\n", g_num_search_paths );
                return ; // we found it and just exit the function and we are done                                        
            }            
        }
        
        // If we didnt find it then we need to move the root one level
        char *last_slash = strrchr( current_dir, '/' );
        char *last_backslash = strrchr( current_dir, '\\' );
        char *last_sep = ( last_slash > last_backslash ) ? last_slash : last_backslash;
        
        if ( !last_sep || last_sep == current_dir ) {
            break;
        }
        
        *last_sep = '\0';
         
    } // LOOP END
    
    // If we get here, we didn't find the root
    fprintf( stderr, "[AUDIO] Could not auto-detect sound directory\n" );
    fprintf( stderr, "[AUDIO] Searched up to 10 levels from model location\n" );
    fprintf( stderr, "[AUDIO] Use mdl_audio_set_sound_directory() to specify manually\n" );
} 
    
