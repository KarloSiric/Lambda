/*======================================================================
 *  File: logger.c
 *  Project: shaders
 *  Author: karlosiric <email@example.com>
 *  Created: 2025-10-05 22:01:03
 *  Last Modified by: karlosiric
 *  Last Modified: 2025-10-07 16:50:28
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
#define isatty _isatty
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

static void wall_time_iso8601( char *buf, size_t n )
{
#ifdef _WIN32
    struct timespec ts;
    timespec_get( &ts, TIME_UTC );
    time_t    sec = ts.tv_sec;
    struct tm tmv;
    localtime_s( &tmv, &sec );
#else
    struct timespec ts;
    clock_gettime( CLOCK_REALTIME, &ts );
    time_t    sec = ts.tv_sec;
    struct tm tmv;
    localtime_r( &sec, &tmv );

#endif

    strftime( buf, n, "%Y-%m-%dT%H:%M:%S", &tmv );

    size_t len = strlen( buf );
    snprintf( buf + len, n - len, ".%03d", ( int ) ( ts.tv_nsec / 1000000 ) );
}

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

void logger_set_global_level( int level )
{
    G.default_level = level;
    return;
}

static void write_console( const char *s, size_t n, int level )
{
    if ( level < G.opt.console_level )
    {
        return;
    }

    fwrite( s, 1, n, stdout );
    fflush( stdout );
}

static void write_file( const char *s, size_t n )
{
    if ( !G.opt.file_path || !G.opt.file_path[0] )
    {
        return;
    }

    if ( !G.fp )
    {
        G.fp = fopen( G.opt.file_path, "ab" );    // appending, binary
        if ( !G.fp )
        {
            return;
        }
    }

    fwrite( s, 1, n, G.fp );
    fflush( G.fp );
    G.bytes += n;
}

static const char *LEVEL_NAME[] = { "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL" };

void logger_logv(
    int level, const char *category, const char *file, int line, const char *func, const char *fmt, va_list ap )
{
    if ( !logger_should_log( level, category ) )
    {
        return;
    }

    if ( level < LOG_TRACE )
        level = LOG_TRACE;
    if ( level > LOG_FATAL )
        level = LOG_FATAL;
    const char *cat      = ( category && category[0] ) ? category : LOG_CAT_DEFAULT;
    const char *src_file = file ? file : "?";
    const char *src_func = func ? func : "?";
    if ( !fmt )
        fmt = "";    // avoid null format

    char msg[2048];
    vsnprintf( msg, sizeof( msg ), fmt, ap );

    char ts[64];

    wall_time_iso8601( ts, sizeof( ts ) );

    // Final assembly
    char linebuf[2300];
    int  n = snprintf(
        linebuf,
        sizeof( linebuf ),
        "%s [%s] %s | %s:%d (%s) | %s\n",
        ts,
        LEVEL_NAME[level],
        cat,
        src_file,
        line,
        src_func,
        msg );

    if ( n < 0 )
    {
        return;
    }

    size_t len = ( size_t ) n;
    if ( len >= sizeof( linebuf ) )
    {
        len = sizeof( linebuf ) - 1;
    }

    lock_( );
    write_console( linebuf, len, level );
    write_file( linebuf, len );
    unlock_( );
}

void logger_log( int level, const char *category, const char *file, int line, const char *func, const char *fmt, ... )
{
    va_list ap;
    va_start( ap, fmt );
    logger_logv( level, category, file, line, func, fmt, ap );
    va_end( ap );
}



void logger_hexdump(int level, const char *category, const char *file, int line, const char *func, const void *data, size_t len, const char *label) {
    
    if (!logger_should_log(level, category) || !data || len == 0) {
        return;
    }
    
    const unsigned char *p = (const unsigned char *)data;
    char row[512];
    
    for (size_t i = 0; i < len; i+=16) { 
        
        size_t r = (len - i < 16) ? (len - i) : 16;
        
        int n = snprintf(row, sizeof(row), "%s +0x%04zx  ",
                        label ? label : "hexdump", i);
                   
    }
    
    
        
    
    
}



