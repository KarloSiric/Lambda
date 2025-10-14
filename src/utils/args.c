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
#include <stdio.h>
#include <string.h>

// Styling constants
#define RULER_DOUBLE "═══════════════════════════════════════════════════════════════"
#define RULER_SINGLE "───────────────────────────────────────────────────────────────"

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
    printf("  Version: 1.0.0\n");
    printf("%s\n", RULER_DOUBLE);
    printf("\n");
}

/*
 * Print usage information
 */
void print_usage(const char *program_name)
{
    print_banner();
    
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
    printf("      Suppress all output except errors\n\n");
    
    printf("  --help, -h\n");
    printf("      Show this help message\n\n");
    
    printf("EXAMPLES:\n");
    printf("  # Run viewer silently\n");
    printf("  %s scientist.mdl\n\n", program_name);
    
    printf("  # Print basic structure + run viewer\n");
    printf("  %s scientist.mdl --dump\n\n", program_name);
    
    printf("  # Print detailed structure and exit\n");
    printf("  %s scientist.mdl --dump-ex --dump-only\n\n", program_name);
    
    printf("  # Dump to file\n");
    printf("  %s scientist.mdl --dump-only > report.txt\n\n", program_name);
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
    args->show_help  = false;
    
    // No arguments = show help
    if (argc < 2) {
        args->show_help = true;
        return 0;
    }
    
    // Parse arguments
    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];
        
        // Help flags
        if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0) {
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
        // Quiet flag
        else if (strcmp(arg, "--quiet") == 0 || strcmp(arg, "-q") == 0) {
            args->quiet = true;
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
    
    // Validate: must have model path if not showing help
    if (!args->show_help && args->model_path == NULL) {
        fprintf(stderr, "ERROR: No model file specified\n");
        fprintf(stderr, "       Use --help for usage information\n");
        return -1;
    }
    
    return 0;
}
