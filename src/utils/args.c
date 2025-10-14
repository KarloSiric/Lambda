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
 *   Purpose: Command-Line Argument Parser Implementation
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include "args.h"
#include "../version.h"
#include <stdio.h>
#include <string.h>

// Styling constants
#define RULER_DOUBLE "═══════════════════════════════════════════════════════════════"
#define RULER_SINGLE "───────────────────────────────────────────────────────────────"

/*
 * Print detailed version information
 */
void print_version_info(void)
{
    printf("\n");
    printf("%s\n", RULER_DOUBLE);
    printf("  %s\n", HLMV_VERSION_LONG);
    printf("%s\n", RULER_DOUBLE);
    printf("\n");
    printf("  Build Information:\n");
    printf("    Version:        %s\n", HLMV_VERSION_STRING);
    printf("    Build Number:   %d\n", HLMV_VERSION_BUILD);
    printf("    Build Date:     %s\n", HLMV_BUILD_DATE);
    printf("    Build Time:     %s\n", HLMV_BUILD_TIME);
    printf("    Build Type:     %s\n", HLMV_BUILD_TYPE);
    printf("\n");
    printf("  Git Information:\n");
    printf("    Commit:         %s\n", HLMV_GIT_COMMIT);
    printf("    Branch:         %s\n", HLMV_GIT_BRANCH);
    printf("\n");
    printf("  Platform:\n");
    printf("    OS:             %s\n", HLMV_PLATFORM);
    printf("    Architecture:   %s\n", HLMV_ARCH);
    printf("    Compiler:       %s\n", HLMV_COMPILER);
    printf("\n");
    printf("  Features:\n");
    printf("    Rendering:      %s\n", (HLMV_HAS_RENDERING) ? "Yes" : "No");
    printf("    Animation:      %s\n", (HLMV_HAS_ANIMATION) ? "Yes" : "No");
    printf("    Textures:       %s\n", (HLMV_HAS_TEXTURES) ? "Yes" : "No");
    printf("    Bones:          %s\n", (HLMV_HAS_BONES) ? "Yes" : "No");
    printf("    Dump:           %s\n", (HLMV_HAS_DUMP) ? "Yes" : "No");
    printf("    GUI:            %s\n", (HLMV_HAS_GUI) ? "Yes" : "No");
    printf("    Export:         %s\n", (HLMV_HAS_EXPORT) ? "Yes" : "No");
    printf("    Editing:        %s\n", (HLMV_HAS_EDITING) ? "Yes" : "No");
    printf("\n");
    printf("%s\n", RULER_DOUBLE);
    printf("\n");
}

/*
 * Print copyright banner
 */
void print_banner(void)
{
    printf("\n");
    printf("%s\n", RULER_DOUBLE);
    printf("  HALF-LIFE MODEL VIEWER\n");
    printf("  GoldSrc Studio Model Format Viewer\n");
    printf("%s\n", RULER_DOUBLE);
    printf("\n");
    printf("  Copyright (c) 1996-2002, Valve LLC. All rights reserved.\n");
    printf("\n");
    printf("  This product contains software technology licensed from Id\n");
    printf("  Software, Inc. (\"Id Technology\"). Id Technology (c) 1996\n");
    printf("  Id Software, Inc. All Rights Reserved.\n");
    printf("\n");
    printf("  Use, distribution, and modification of this source code and/or\n");
    printf("  resulting object code is restricted to non-commercial\n");
    printf("  enhancements to products from Valve LLC. All other use,\n");
    printf("  distribution, or modification is prohibited without written\n");
    printf("  permission from Valve LLC.\n");
    printf("\n");
    printf("%s\n", RULER_SINGLE);
    printf("  Author: Karlo Siric\n");
    printf("  Version: %s\n", HLMV_VERSION_SHORT);
    printf("%s\n", RULER_DOUBLE);
    printf("\n");
}

/*
 * Print usage information
 */
void print_usage(const char *program_name)
{
    printf("USAGE:\n");
    printf("  %s <model.mdl> [OPTIONS]\n\n", program_name);
    
    printf("OPTIONS:\n");
    printf("  --dump, -d\n");
    printf("      Print basic model structure (header, bones, sequences)\n\n");
    
    printf("  --dump-ex, -dx\n");
    printf("      Print extended structure (vertices, meshes, raw offsets)\n\n");
    
    printf("  --dump-only\n");
    printf("      Dump structure and exit (no viewer window)\n\n");
    
    printf("  --quiet, -q\n");
    printf("      Quiet mode - only show errors\n\n");
    
    printf("  --verbose, -vv\n");
    printf("      Verbose mode - show debug messages\n\n");
    
    printf("  --trace\n");
    printf("      Trace mode - show all messages including trace\n\n");
    
    printf("  --log-file <path>\n");
    printf("      Write logs to specified file\n\n");
    
    printf("  --version, -v\n");
    printf("      Show detailed version information\n\n");
    
    printf("  --help, -h\n");
    printf("      Show this help message\n\n");
    
    printf("LOGGING LEVELS:\n");
    printf("  quiet   - Only errors\n");
    printf("  normal  - Info, warnings, errors (default)\n");
    printf("  verbose - Debug, info, warnings, errors\n");
    printf("  trace   - All messages including trace\n\n");
    
    printf("EXAMPLES:\n");
    printf("  # Run viewer with model in models directory\n");
    printf("  %s models/scientist.mdl\n\n", program_name);
    
    printf("  # Run viewer with relative path\n");
    printf("  %s ../models/HL1_Original/scientist.mdl\n\n", program_name);
    
    printf("  # Print basic structure with verbose logging\n");
    printf("  %s ../models/HL1_Original/scientist.mdl --dump --verbose\n\n", program_name);
    
    printf("  # Print detailed structure and exit\n");
    printf("  %s scientist.mdl --dump-ex --dump-only\n\n", program_name);
    
    printf("  # Dump to file with trace logging\n");
    printf("  %s scientist.mdl --dump-only --trace --log-file debug.log > report.txt\n\n", program_name);
    
    printf("  # Show version information\n");
    printf("  %s --version\n\n", program_name);
}

/*
 * Parse command-line arguments
 * Returns: 0 on success, -1 on error
 */
int parse_args(int argc, const char *argv[], app_args_t *args)
{
    // Initialize with defaults
    args->model_path = NULL;
    args->dump_level = DUMP_NONE;
    args->dump_only  = false;
    args->quiet      = false;
    args->log_level  = LOG_LEVEL_NORMAL;  // Default to normal
    args->log_file   = NULL;
    args->show_help  = false;
    args->show_version = false;
    
    // No arguments = show help
    if (argc < 2) {
        args->show_help = true;
        return 0;
    }
    
    // Parse arguments
    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];
        
        // Version flag
        if (strcmp(arg, "--version") == 0 || strcmp(arg, "-v") == 0) {
            args->show_version = true;
            return 0;
        }
        // Help flags
        else if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0) {
            args->show_help = true;
            return 0;
        }
        // Dump flags
        else if (strcmp(arg, "--dump") == 0 || strcmp(arg, "-d") == 0) {
            args->dump_level = DUMP_BASIC;
        }
        else if (strcmp(arg, "--dump-ex") == 0 || strcmp(arg, "-dx") == 0) {
            args->dump_level = DUMP_EXTENDED;
        }
        else if (strcmp(arg, "--dump-only") == 0) {
            args->dump_only = true;
        }
        // Logging flags
        else if (strcmp(arg, "--quiet") == 0 || strcmp(arg, "-q") == 0) {
            args->quiet = true;
            args->log_level = LOG_LEVEL_QUIET;
        }
        else if (strcmp(arg, "--verbose") == 0 || strcmp(arg, "-vv") == 0) {
            args->log_level = LOG_LEVEL_VERBOSE;
        }
        else if (strcmp(arg, "--trace") == 0) {
            args->log_level = LOG_LEVEL_TRACE;
        }
        else if (strcmp(arg, "--log-file") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "ERROR: --log-file requires a path argument\n");
                return -1;
            }
            args->log_file = argv[++i];
        }
        // Model path (doesn't start with -)
        else if (arg[0] != '-') {
            if (args->model_path == NULL) {
                args->model_path = arg;
            } else {
                fprintf(stderr, "ERROR: Multiple model files specified\n");
                fprintf(stderr, "       Already have: %s\n", args->model_path);
                fprintf(stderr, "       Cannot use: %s\n", arg);
                return -1;
            }
        }
        // Unknown flag
        else {
            fprintf(stderr, "ERROR: Unknown option '%s'\n", arg);
            fprintf(stderr, "       Use --help for usage information\n");
            return -1;
        }
    }
    
    // Validate: must have model path if not showing help or version
    if (!args->show_help && !args->show_version && args->model_path == NULL) {
        fprintf(stderr, "ERROR: No model file specified\n");
        fprintf(stderr, "       Use --help for usage information\n");
        return -1;
    }
    
    return 0;
}
