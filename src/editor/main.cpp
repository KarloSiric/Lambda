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

// Function to print startup banner - can be called anytime
  void printStartupBanner() {
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

  int main( int argc, char *argv[] ) {
      // Initialize systems
      console_init();

      t_log_options log_opts = {0};
      log_opts.console_level = LOG_FATAL;
      log_opts.use_colors = true;
      logger_init( &log_opts );

      // OpenGL setup
      QSurfaceFormat format;
      format.setVersion( 4, 1 );
      format.setProfile( QSurfaceFormat::CoreProfile );
      format.setDepthBufferSize( 24 );
      format.setStencilBufferSize( 8 );
      format.setSamples( 4 );
      QSurfaceFormat::setDefaultFormat( format );

      QApplication app( argc, argv );

      MainWindow window;
      
      ThemeManager::applyThemeLight( app );

      // Print startup banner AFTER window exists (so console widget receives it)
      printStartupBanner();

      window.show();

      int result = app.exec();

      logger_shutdown();
      console_shutdown();

      return result;
  }









