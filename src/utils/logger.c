/*======================================================================
 *  File: logger.c
 *  Project: shaders
 *  Author: karlosiric <email@example.com>
 *  Created: 2025-10-05 22:01:03
 *  Last Modified by: karlosiric
 *  Last Modified: 2025-10-07 13:23:41
 *----------------------------------------------------------------------
 *  Description:
 *
 *----------------------------------------------------------------------
 *  License:
 *  Company:
 *  Version: 0.1.0
 *======================================================================
 */

#include "logger.h"

#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <io.h>
#include <windows.h>
#define isatty_isatty
#define fileno _fileno
#else
#include <pthread.h>
#include <unistd.h>
#endif

typedef struct category_level {
    char name[48];
    int  level;
    int  in_use;

} t_category_level;

// need to ensure one thread only writes to our .log file
static struct {
    t_log_options    opt;
    FILE            *fp;
    size_t           bytes;
    int              default_level;
    t_category_level cats[LOG_MAX_CATEGORIES];

#ifdef _WIN32
    CRITICAL_SECTION mtx;    // mutex for Windows
#else
    pthread_mutex_t mtx;    // mutex for macOS
#endif
    int console_tty;    // for terminal
    int vt_enabled;     //  enabling ANSI colors for the terminal
} G;

static void lock_( void )
{
#ifdef _WIN32
    EnterCriticalSection( &G.mtx );
#else
    pthread_mutex_lock( &G.mtx );
#endif
}

static void unlock_( void )
{
#ifdef _WIN32
    LeaveCriticalSection( &G.mtx );
#else
    pthread_mutex_unlock( &G.mtx );
#endif
}

static int is_tty_terminal( void )
{
    return isatty( fileno( stdout ) );
}

uint64_t logger_now_ms( void )
{
#ifdef _WIN32
    LARGE_INTEGER f, t;
    QueryPerformanceFrequency( &f );
    QueryPerformanceCounter( &t );
    return ( uint64_t ) ( ( 1000.0 * t.QuadPart ) / f.QuadPart );
#else
    struct timespec ts;
    clock_gettime( CLOCK_MONOTONIC, &ts );
    return ( uint64_t ) ts.tv_sec * 1000ULL + ( uint64_t ) ts.tv_nsec / 1000000ULL;
#endif
}

/**
 * @brief      Finding category that we want, since we can
 *             store up to 64 categories unique and we want
 *             to find each one we need.
 *
 * @param[in]  name  cat_find
 *
 * @return     { integer as in the category array index or -1 }
 */
static int cat_find( const char *name )
{
    for ( int i = 0; i < LOG_MAX_CATEGORIES; i++ )
    {
        if ( G.cats[i].in_use && strcmp( G.cats[i].name, name ) == 0 )
        {
            return i;
        }
    }
    return -1;
}

static int cat_alloc( const char *name )
{
    for ( int i = 0; i < LOG_MAX_CATEGORIES; i++ )
    {
        if ( !G.cats[i].in_use )
        {
            strncpy( G.cats[i].name, name, sizeof( G.cats[i].name ) - 1 );
            G.cats[i].name[sizeof( G.cats[i].name ) - 1] = '\0';
            G.cats[i].level                              = G.default_level;    //  start with default level
            G.cats[i].in_use                             = 1;
        }
    }

    return -1;
}

int logger_get_global_level( )
{ 
    return G.default_level; 
}


void logger_set_global_level(int level) {
    G.default_level = level;
    return;
}


static void write_console(const char *s, size_t n, int level) {
    
    if (level < G.opt.console_level) {
        return;
    }
    
    fwrite(s, 1, n, stdout);
    fflush(stdout); 
}





