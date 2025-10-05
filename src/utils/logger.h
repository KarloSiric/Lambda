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

#define LOG_AT( LVL, CAT, FMT, ... )              ...
#define LOG_TRACEF( CAT, FMT, ... )               ...
#define LOG_HEXDUMP( LVL, CAT, DATA, LEN, LABEL ) ...
#define LOG_CHECK( COND, CAT, FMT, ... )          ...
#define LOG_TIME_BLOCK( LABEL, CAT )                                                                                   \
    for ( uint64_t _t0 = ... )                                                                                         \
    ...



}
