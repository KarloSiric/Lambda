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
#include "theme/ThemeManager.h"
#include "util_console.h"
#include "util_logger.h"
#include <QApplication>
#include <QWidget>
#include <QSurfaceFormat>
#include <QSysInfo>
#include <QDateTime>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qmainwindow.h>

#define LAMBDA_VERSION "0.70 BETA"
#define LAMBDA_BUILD_DATE __DATE__
#define LAMBDA_BUILD_TIME __TIME__

int main(int argc, char *argv[])
{
    // Initialize console system
    console_init();

    // Initialize logger with console output disabled (only file/GUI)
    t_log_options log_opts = {0};
    log_opts.console_level = LOG_FATAL;  // Suppress LOG_* from terminal
    log_opts.use_colors = true;
    logger_init( &log_opts );

    // Print startup banner
    console_print_raw( "\n" );
    console_print_raw( "Lambda - Half-Life Model Viewer/Editor\n" );
    console_print_raw( "Version %s\n", LAMBDA_VERSION );
    console_print_raw( "\n" );

    // Print build info
    QDateTime now = QDateTime::currentDateTime();
    console_print( CONSOLE_INFO, "Started: %s", now.toString( "dddd, MMMM d yyyy - hh:mm:ss" ).toUtf8().constData() );
    console_print( CONSOLE_INFO, "Build: %s %s", LAMBDA_BUILD_DATE, LAMBDA_BUILD_TIME );

    // Print OS info
    console_print( CONSOLE_INFO, "OS: %s %s (%s)",
        QSysInfo::productType().toUtf8().constData(),
        QSysInfo::productVersion().toUtf8().constData(),
        QSysInfo::currentCpuArchitecture().toUtf8().constData() );
    console_print( CONSOLE_INFO, "Kernel: %s %s",
        QSysInfo::kernelType().toUtf8().constData(),
        QSysInfo::kernelVersion().toUtf8().constData() );

    console_print_raw( "\n" );

    // Request OpenGL 4.1 Core Profile BEFORE creating QApplication
    QSurfaceFormat format;
    format.setVersion( 4, 1 );                    // OpenGL 4.1
    format.setProfile( QSurfaceFormat::CoreProfile );  // Core profile (modern OpenGL)
    format.setDepthBufferSize( 24 );              // 24-bit depth buffer
    format.setStencilBufferSize( 8 );             // 8-bit stencil buffer
    format.setSamples( 4 );                       // 4x MSAA (anti-aliasing)
    QSurfaceFormat::setDefaultFormat( format );   // Apply to all OpenGL widgets

    QApplication app( argc, argv );

    MainWindow window;

    ThemeManager::applyThemeLight( app );

    window.show();

    int result = app.exec();

    // Cleanup
    logger_shutdown();
    console_shutdown();

    return result;
}










