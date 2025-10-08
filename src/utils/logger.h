// =============================
// File: logger.h
// A tiny, fast, thread‑safe logger for C (single TU, no deps)
// Features: levels, categories, color console, JSON/file output,
// rotation, per-category levels, hexdumps, time blocks, assertions.
// =============================

#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LOG_ENABLE
#define LOG_ENABLE 1
#endif

#ifndef LOG_DEFAULT_LEVEL
#define LOG_DEFAULT_LEVEL 1 /* DEBUG */
#endif

#ifndef LOG_MAX_CATEGORIES
#define LOG_MAX_CATEGORIES 64
#endif

typedef enum {

    LOG_TRACE = 0,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL

} t_log_level;

typedef struct log_options {
    const char *file_path;        // path to log file (NULL = no file)
    size_t      max_bytes;        // rotate when file exceeds this (0 = no rotate)
    int         max_files;        // number of rotated files to keep
    bool        use_colors;       // colored console output
    bool        json_lines;       // write each record as a JSON line
    int         console_level;    // minimum level printed to console
    
} t_log_options;

int  logger_init( const t_log_options *opt );
void logger_shutdown( void );

void logger_set_global_level( int level );
int  logger_get_global_level( void );

int logger_set_category_level( const char *category, int level );
int logger_get_category_level( const char *category, int *out_level );

void logger_set_console_level( int level );
int logger_is_tty( void );

void logger_logv(
    int level, const char *category, const char *file, int line, const char *func, const char *fmt, va_list ap );
void logger_log( int level, const char *category, const char *file, int line, const char *func, const char *fmt, ... );

void logger_hexdump(
    int         level,
    const char *category,
    const char *file,
    int         line,
    const char *func,
    const void *data,
    size_t      len,
    const char *label );

uint64_t logger_now_ms( void );

// ======= MACROS ======= //

// ===== Macros =====

#if LOG_ENABLE

bool logger_should_log( int level, const char *category );

#ifndef LOG_CAT_DEFAULT
#define LOG_CAT_DEFAULT "app"
#endif

// variadic-comma helper: on non-MSVC, drop the comma if there are no args
#if defined( _MSC_VER )
#define LOG_VA_COMMA( ... ) , __VA_ARGS__
#else
#define LOG_VA_COMMA( ... ) , ##__VA_ARGS__
#endif

// base macro: send everything with auto file/line/func
#define LOG_AT( LVL, CAT, FMT, ... )                                                                                   \
    logger_log( ( LVL ), ( CAT ), __FILE__, __LINE__, __func__, ( FMT ) LOG_VA_COMMA( __VA_ARGS__ ) )

// convenience per-level macros
#define LOG_TRACEF( CAT, FMT, ... ) LOG_AT( LOG_TRACE, ( CAT ), ( FMT ), __VA_ARGS__ )
#define LOG_DEBUGF( CAT, FMT, ... ) LOG_AT( LOG_DEBUG, ( CAT ), ( FMT ), __VA_ARGS__ )
#define LOG_INFOF( CAT, FMT, ... )  LOG_AT( LOG_INFO, ( CAT ), ( FMT ), __VA_ARGS__ )
#define LOG_WARNF( CAT, FMT, ... )  LOG_AT( LOG_WARN, ( CAT ), ( FMT ), __VA_ARGS__ )
#define LOG_ERRORF( CAT, FMT, ... ) LOG_AT( LOG_ERROR, ( CAT ), ( FMT ), __VA_ARGS__ )
#define LOG_FATALF( CAT, FMT, ... ) LOG_AT( LOG_FATAL, ( CAT ), ( FMT ), __VA_ARGS__ )

// hexdump helper (already implemented in .c)
#define LOG_HEXDUMP( LVL, CAT, DATA, LEN, LABEL )                                                                      \
    logger_hexdump( ( LVL ), ( CAT ), __FILE__, __LINE__, __func__, ( DATA ), ( LEN ), ( LABEL ) )

// assert-like check: if (COND) ok; else log and optionally abort in debug
#ifndef LOG_ABORT_ON_CHECK_FAIL
#define LOG_ABORT_ON_CHECK_FAIL 0
#endif

#define LOG_CHECK( COND, CAT, FMT, ... )                                                                               \
    do                                                                                                                 \
    {                                                                                                                  \
        if ( !( COND ) )                                                                                               \
        {                                                                                                              \
            LOG_ERRORF( ( CAT ), "CHECK FAILED: %s | " FMT, #COND, __VA_ARGS__ );                                      \
            if ( LOG_ABORT_ON_CHECK_FAIL )                                                                             \
                abort( );                                                                                              \
        }                                                                                                              \
    } while ( 0 )

// timing a block: logs "<LABEL>: Xms" at INFO after the block runs
#define LOG_TIME_BLOCK( LABEL, CAT )                                                                                   \
    for ( uint64_t _t0 = logger_now_ms( ), _once = 1; _once;                                                           \
          LOG_INFOF( ( CAT ), "%s: %llums", ( LABEL ), ( unsigned long long ) ( logger_now_ms( ) - _t0 ) ),            \
                   _once = 0 )

#else    // LOG_ENABLE == 0  → compile out everything

#define LOG_AT( ... )         ( ( void ) 0 )
#define LOG_TRACEF( ... )     ( ( void ) 0 )
#define LOG_DEBUGF( ... )     ( ( void ) 0 )
#define LOG_INFOF( ... )      ( ( void ) 0 )
#define LOG_WARNF( ... )      ( ( void ) 0 )
#define LOG_ERRORF( ... )     ( ( void ) 0 )
#define LOG_FATALF( ... )     ( ( void ) 0 )
#define LOG_HEXDUMP( ... )    ( ( void ) 0 )
#define LOG_CHECK( ... )      ( ( void ) 0 )
#define LOG_TIME_BLOCK( ... ) for ( int _x = 0; !_x; _x = 1 )

#endif    // LOG_ENABLE
#ifdef __cplusplus
}    // extern "C"
#endif
