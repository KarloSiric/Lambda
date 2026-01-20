/*
 * ===============================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===============================================================================
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
 * -------------------------------------------------------------------------------
 *   Author: Karlo Siric
 *   Purpose: Bridge between C backend logging and Qt LogWidget
 * ===============================================================================
 */

#ifndef LOGGERBRIDGE_H
#define LOGGERBRIDGE_H

#include "LogWidget.h"

// C-compatible log levels (must match util_log.h if it exists)
#ifdef __cplusplus
extern "C" {
#endif

#define LOG_LEVEL_TRACE   0
#define LOG_LEVEL_DEBUG   1
#define LOG_LEVEL_INFO    2
#define LOG_LEVEL_WARNING 3
#define LOG_LEVEL_ERROR   4
#define LOG_LEVEL_FATAL   5

// C-callable logging functions
void log_trace( const char *category, const char *format, ... );
void log_debug( const char *category, const char *format, ... );
void log_info( const char *category, const char *format, ... );
void log_warning( const char *category, const char *format, ... );
void log_error( const char *category, const char *format, ... );
void log_fatal( const char *category, const char *format, ... );

// Generic log function
void log_message( int level, const char *category, const char *format, ... );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

namespace LoggerBridge {

// Initialize the bridge with a LogWidget instance
void init( LogWidget *widget );

// Get current widget (for direct C++ access)
LogWidget *getWidget();

// Shutdown and cleanup
void shutdown();

} // namespace LoggerBridge

#endif // __cplusplus

#endif // LOGGERBRIDGE_H
