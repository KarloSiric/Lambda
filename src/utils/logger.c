/*======================================================================
 *  File: logger.c
 *  Project: shaders
 *  Author: karlosiric <email@example.com>
 *  Created: 2025-10-05 22:01:03
 *  Last Modified by: karlosiric
 *  Last Modified: 2025-10-07 12:18:17
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


