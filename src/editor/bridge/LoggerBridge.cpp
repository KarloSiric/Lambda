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
 *   Purpose: Bridge implementation between C backend logging and Qt LogWidget
 * ===============================================================================
 */

#include "LoggerBridge.h"
#include <cstdarg>
#include <cstdio>
#include <QString>

// Global widget pointer
static LogWidget *g_logWidget = nullptr;

namespace LoggerBridge {

void init( LogWidget *widget ) {
	g_logWidget = widget;
}

LogWidget *getWidget() {
	return g_logWidget;
}

void shutdown() {
	g_logWidget = nullptr;
}

} // namespace LoggerBridge

// Helper function to convert level int to LogLevel enum
static LogLevel intToLogLevel( int level ) {
	switch ( level ) {
		case LOG_LEVEL_TRACE:   return LogLevel::Trace;
		case LOG_LEVEL_DEBUG:   return LogLevel::Debug;
		case LOG_LEVEL_INFO:    return LogLevel::Info;
		case LOG_LEVEL_WARNING: return LogLevel::Warning;
		case LOG_LEVEL_ERROR:   return LogLevel::Error;
		case LOG_LEVEL_FATAL:   return LogLevel::Fatal;
		default:                return LogLevel::Info;
	}
}

// Internal logging implementation
static void log_impl( int level, const char *category, const char *format, va_list args ) {
	char buffer[2048];
	vsnprintf( buffer, sizeof( buffer ), format, args );

	// Also print to stdout/stderr for debugging
	FILE *output = ( level >= LOG_LEVEL_WARNING ) ? stderr : stdout;
	const char *levelStr = "???";
	switch ( level ) {
		case LOG_LEVEL_TRACE:   levelStr = "TRACE"; break;
		case LOG_LEVEL_DEBUG:   levelStr = "DEBUG"; break;
		case LOG_LEVEL_INFO:    levelStr = "INFO"; break;
		case LOG_LEVEL_WARNING: levelStr = "WARN"; break;
		case LOG_LEVEL_ERROR:   levelStr = "ERROR"; break;
		case LOG_LEVEL_FATAL:   levelStr = "FATAL"; break;
	}

	if ( category && category[0] ) {
		fprintf( output, "[%s][%s] %s\n", levelStr, category, buffer );
	} else {
		fprintf( output, "[%s] %s\n", levelStr, buffer );
	}
	fflush( output );

	// Send to LogWidget if available
	if ( g_logWidget ) {
		QString msg = QString::fromUtf8( buffer );
		QString cat = category ? QString::fromUtf8( category ) : QString();
		g_logWidget->log( intToLogLevel( level ), msg, cat );
	}
}

// C-callable logging functions
extern "C" {

void log_trace( const char *category, const char *format, ... ) {
	va_list args;
	va_start( args, format );
	log_impl( LOG_LEVEL_TRACE, category, format, args );
	va_end( args );
}

void log_debug( const char *category, const char *format, ... ) {
	va_list args;
	va_start( args, format );
	log_impl( LOG_LEVEL_DEBUG, category, format, args );
	va_end( args );
}

void log_info( const char *category, const char *format, ... ) {
	va_list args;
	va_start( args, format );
	log_impl( LOG_LEVEL_INFO, category, format, args );
	va_end( args );
}

void log_warning( const char *category, const char *format, ... ) {
	va_list args;
	va_start( args, format );
	log_impl( LOG_LEVEL_WARNING, category, format, args );
	va_end( args );
}

void log_error( const char *category, const char *format, ... ) {
	va_list args;
	va_start( args, format );
	log_impl( LOG_LEVEL_ERROR, category, format, args );
	va_end( args );
}

void log_fatal( const char *category, const char *format, ... ) {
	va_list args;
	va_start( args, format );
	log_impl( LOG_LEVEL_FATAL, category, format, args );
	va_end( args );
}

void log_message( int level, const char *category, const char *format, ... ) {
	va_list args;
	va_start( args, format );
	log_impl( level, category, format, args );
	va_end( args );
}

} // extern "C"
