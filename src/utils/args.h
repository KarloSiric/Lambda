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
 *   Purpose: Command-Line Argument Parser
 * ═══════════════════════════════════════════════════════════════════════════
 */

#ifndef ARGS_H
#define ARGS_H

#include <stdbool.h>

/*
 * Dump detail levels
 */
typedef enum {
    DUMP_NONE = 0,      // No dump - just run viewer
    DUMP_BASIC,         // --dump: Overview (header, bones, sequences)
    DUMP_EXTENDED       // --dump-ex: Deep dive (vertices, meshes, raw data)
} dump_level_t;

/*
 * Log detail levels
 */
typedef enum {
    LOG_LEVEL_QUIET = 0,  // Only errors
    LOG_LEVEL_NORMAL,     // Info and above
    LOG_LEVEL_VERBOSE,    // Debug and above
    LOG_LEVEL_TRACE       // Everything including trace
} log_detail_t;

/*
 * Parsed command-line arguments
 */
typedef struct {
    const char  *model_path;    // Path to .mdl file
    dump_level_t dump_level;    // Dump detail level
    bool         dump_only;     // Exit after dump (no viewer)
    bool         quiet;         // Suppress all non-error output (deprecated, use log_level)
    log_detail_t log_level;     // Logging verbosity
    const char  *log_file;      // Optional log file path
    bool         show_help;     // Show usage
    bool         show_version;  // Show version information
} app_args_t;

/*
 * Parse command-line arguments
 * Returns: 0 on success, -1 on error
 */
int parse_args(int argc, const char *argv[], app_args_t *args);

/*
 * Print usage information
 */
void print_usage(const char *program_name);

/*
 * Print copyright banner
 */
void print_banner(void);

/*
 * Print detailed version information
 */
void print_version_info(void);

#endif // ARGS_H
