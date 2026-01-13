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
 *   Purpose: Console output system for user-facing messages
 *            Separate from LOG system which is for developer/debug messages
 * ═══════════════════════════════════════════════════════════════════════════
 */

#ifndef UTIL_CONSOLE_H
#define UTIL_CONSOLE_H

#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Console Output System
 * ─────────────────────
 * Use these for USER-FACING messages that should appear in the Console panel.
 * These are regular information messages like:
 *   - Application startup info (driver, OpenGL version)
 *   - Model loading status
 *   - Save/export operations
 *   - General user notifications
 *
 * For DEVELOPER/DEBUG messages, use the LOG_* macros from util_logger.h
 * Those go to the Log panel (only visible in developer mode).
 */

// Console message types
typedef enum {
    CONSOLE_INFO,       // General information (white/default)
    CONSOLE_SUCCESS,    // Success messages (green)
    CONSOLE_WARNING,    // Warnings (yellow)
    CONSOLE_ERROR       // Errors (red)
} console_msg_type_t;

// Initialize console system
void console_init(void);

// Shutdown console system
void console_shutdown(void);

// Print to console with type
void console_print(console_msg_type_t type, const char *fmt, ...);

// Print raw message (no formatting)
void console_print_raw(const char *fmt, ...);

// Convenience macros for console output
#define CONSOLE_INFO(fmt, ...)    console_print(CONSOLE_INFO, fmt, ##__VA_ARGS__)
#define CONSOLE_SUCCESS(fmt, ...) console_print(CONSOLE_SUCCESS, fmt, ##__VA_ARGS__)
#define CONSOLE_WARN(fmt, ...)    console_print(CONSOLE_WARNING, fmt, ##__VA_ARGS__)
#define CONSOLE_ERROR(fmt, ...)   console_print(CONSOLE_ERROR, fmt, ##__VA_ARGS__)
#define CONSOLE_RAW(fmt, ...)     console_print_raw(fmt, ##__VA_ARGS__)

// Callback for GUI console widget to receive messages
typedef void (*console_callback_t)(console_msg_type_t type, const char *message);

// Set callback for GUI integration (NULL to disable)
void console_set_callback(console_callback_t callback);

// Enable/disable ANSI colors
void console_set_colors(int enabled);

// Print a newline
void console_newline(void);

#ifdef __cplusplus
}
#endif

#endif /* UTIL_CONSOLE_H */
