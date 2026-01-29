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
 *   Purpose: Application Main Entry Point
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include "MainWindow.h"
#include "ModelViewport.h"
#include "theme/ThemeManager.h"
#include "util_console.h"
#include "util_logger.h"
#include <QApplication>
#include <QWidget>
#include <QTimer>
#include <QSurfaceFormat>
#include <QSysInfo>
#include <QDateTime>
#include <QDir>
#include <QStandardPaths>
#include <QIcon>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qmainwindow.h>

#define LAMBDA_VERSION "0.70 BETA"
#define LAMBDA_BUILD_DATE __DATE__
#define LAMBDA_BUILD_TIME __TIME__

// Function to print startup banner - can be called anytime
void printStartupBanner( void ) {
	console_print_raw( "\n" );
	console_print_raw( "Lambda - Half-Life Model Viewer/Editor\n" );
	console_print_raw( "Version %s\n", LAMBDA_VERSION );
	console_print_raw( "\n" );

	QDateTime now = QDateTime::currentDateTime();
	CONSOLE_INFO( "Started: %s", now.toString( "dddd, MMMM d yyyy - hh:mm:ss" ).toUtf8().constData() );
	CONSOLE_INFO( "Build: %s %s", LAMBDA_BUILD_DATE, LAMBDA_BUILD_TIME );
	CONSOLE_INFO( "OS: %s %s (%s)",
				  QSysInfo::productType().toUtf8().constData(),
				  QSysInfo::productVersion().toUtf8().constData(),
				  QSysInfo::currentCpuArchitecture().toUtf8().constData() );
	CONSOLE_INFO( "Kernel: %s %s",
				  QSysInfo::kernelType().toUtf8().constData(),
				  QSysInfo::kernelVersion().toUtf8().constData() );
	console_print_raw( "\n" );
}

void printOpenGLInfo() {
	if ( ModelViewport::s_glInfoReady ) {
		CONSOLE_INFO( "GPU: %s", ModelViewport::s_gpuRenderer.toUtf8().constData() );
		CONSOLE_INFO( "Vendor: %s", ModelViewport::s_gpuVendor.toUtf8().constData() );
		CONSOLE_INFO( "OpenGL: %s", ModelViewport::s_glVersion.toUtf8().constData() );
		CONSOLE_INFO( "GLSL: %s", ModelViewport::s_glslVersion.toUtf8().constData() );
		console_print_raw( "\n" );
	}
}

void printLoadingSuccess( void ) {
	CONSOLE_SUCCESS( "Initialization Complete" );
	console_print_raw( "\n" );
}

int main( int argc, char *argv[] ) {
	QApplication app( argc, argv );
	app.setApplicationName( "Lambda" );
	app.setOrganizationName( "kSoftware" );
	app.setWindowIcon( QIcon( ":/icons/logo-icon-finalized.png" ) );

	QString appDataPath = QStandardPaths::writableLocation( QStandardPaths::AppDataLocation );
	QString logDir = appDataPath + "/logs";
	QDir().mkpath( logDir );
	QString logFilePath = logDir + "/lambda.log";

	console_init();

	// IMPORTANT @NOTE: logFilePathBytes must stay alive while logger is active
	QByteArray logFilePathBytes = logFilePath.toUtf8();

	t_log_options log_opts = { 0 };
	log_opts.file_path = logFilePathBytes.constData();
	log_opts.console_level = LOG_ERROR;
	log_opts.use_colors = true;
	log_opts.max_bytes = 5 * 1024 * 1024; // 5 MB
	log_opts.max_files = 3;
	logger_init( &log_opts );

	// 5. OpenGL format setup
	QSurfaceFormat format;
	format.setVersion( 4, 1 );
	format.setProfile( QSurfaceFormat::CoreProfile );
	format.setDepthBufferSize( 24 );
	format.setStencilBufferSize( 8 );
	format.setSamples( 4 );
	QSurfaceFormat::setDefaultFormat( format );

	// 6. Apply Classic Windows 95/2000 theme
	ThemeManager::applyClassicTheme( &app );

	// 7. Create main window
	MainWindow window;
 
	QTimer::singleShot( 1000, [&]() {
		printStartupBanner();

		QTimer::singleShot( 700, [&]() {
			printOpenGLInfo();
            
            
            
#ifdef _WIN32 
            // For Windows OS 
            QString displayPath = logFilePath;
#else
            // for MacOS/Linux or UNIX like OS we need different slashes format
            QString displayPath = logFilePath;
            displayPath.replace( " ", "\\ " );
#endif
            CONSOLE_INFO( "Log file: %s", displayPath.toUtf8().constData() );
            
            
            
            
            
            

			QTimer::singleShot( 400, [&]() {
				printLoadingSuccess();
			} );
		} );
	} );

	window.show();
    
	int result = app.exec();

	logger_shutdown();
	console_shutdown();

	return result;
}
