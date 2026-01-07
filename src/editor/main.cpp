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
#include <QApplication>
#include <QWidget>
#include <QSurfaceFormat>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qmainwindow.h>



int main(int argc, char *argv[])
{
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

    return ( app.exec() );
}










