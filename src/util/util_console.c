/*
 * ═══════════════════════════════════════════════════════════════════════════
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ═══════════════════════════════════════════════════════════════════════════
 *
 *   Copyright (c) 1996-2002, Valve LLC. All rights reserved.
 *
 * ───────────────────────────────────────────────────────────────────────────
 *   Author: Karlo Siric
 *   Purpose: Console output system implementation with ANSI colors
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include "util_console.h"
#include <string.h>
#include <stdlib.h>

// ANSI color codes
#define ANSI_RESET   "\033[0m"
#define ANSI_GREEN   "\033[32m"
#define ANSI_YELLOW  "\033[33m"
#define ANSI_RED     "\033[31m"
#define ANSI_CYAN    "\033[36m"
#define ANSI_WHITE   "\033[37m"
#define ANSI_BOLD    "\033[1m"

// Callback for GUI integration
static console_callback_t g_console_callback = NULL;

// Buffer for formatted messages
#define CONSOLE_BUFFER_SIZE 2048
static char g_console_buffer[CONSOLE_BUFFER_SIZE];

// Enable/disable colors (can be toggled)
static int g_colors_enabled = 1;

void console_init(void) {
    g_console_callback = NULL;
    g_colors_enabled = 1;
}

void console_shutdown(void) {
    g_console_callback = NULL;
}

void console_set_callback(console_callback_t callback) {
    g_console_callback = callback;
}

void console_set_colors(int enabled) {
    g_colors_enabled = enabled;
}

void console_print(console_msg_type_t type, const char *fmt, ...) {
    if (!fmt) return;

    va_list args;
    va_start(args, fmt);
    vsnprintf(g_console_buffer, CONSOLE_BUFFER_SIZE, fmt, args);
    va_end(args);

    // Print with colors if enabled
    if (g_colors_enabled) {
        switch (type) {
            case CONSOLE_INFO:
                printf("%s\n", g_console_buffer);
                break;
            case CONSOLE_SUCCESS:
                printf("%s%s[OK]%s %s\n", ANSI_BOLD, ANSI_GREEN, ANSI_RESET, g_console_buffer);
                break;
            case CONSOLE_WARNING:
                printf("%s%s[!]%s %s\n", ANSI_BOLD, ANSI_YELLOW, ANSI_RESET, g_console_buffer);
                break;
            case CONSOLE_ERROR:
                printf("%s%s[ERROR]%s %s\n", ANSI_BOLD, ANSI_RED, ANSI_RESET, g_console_buffer);
                break;
        }
    } else {
        // No colors
        const char *prefix = "";
        switch (type) {
            case CONSOLE_INFO:    prefix = ""; break;
            case CONSOLE_SUCCESS: prefix = "[OK] "; break;
            case CONSOLE_WARNING: prefix = "[!] "; break;
            case CONSOLE_ERROR:   prefix = "[ERROR] "; break;
        }
        printf("%s%s\n", prefix, g_console_buffer);
    }

    fflush(stdout);

    // Also send to GUI callback if registered
    if (g_console_callback) {
        g_console_callback(type, g_console_buffer);
    }
}

void console_print_raw(const char *fmt, ...) {
    if (!fmt) return;

    va_list args;
    va_start(args, fmt);
    vsnprintf(g_console_buffer, CONSOLE_BUFFER_SIZE, fmt, args);
    va_end(args);

    printf("%s", g_console_buffer);
    fflush(stdout);

    if (g_console_callback) {
        g_console_callback(CONSOLE_INFO, g_console_buffer);
    }
}

void console_newline(void) {
    printf("\n");
    fflush(stdout);
}
