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
 *   Purpose: Qt Theme Main Manager for the Application
 * ═══════════════════════════════════════════════════════════════════════════
 */


#include "ThemeManager.h"
#include <QPalette>
#include <QtCore/qnamespace.h>
#include <QtGui/qcolor.h>
#include <QtGui/qpalette.h>
#include <QtWidgets/qapplication.h>
#include <QStyleFactory>

void ThemeManager::applyThemeLight( QApplication &app ) 
{
    
QString style = R"(
        QMainWindow {
            background: #dcdcdc;
        }

        QDockWidget {
            titlebar-close-icon: url(none);
            titlebar-normal-icon: url(none);
            padding: 0;
            border: 1px solid #a0a0a0;
            background: #f0f0f0;
        }

        QDockWidget::title {
            background: #e5e5e5;
            padding: 4px;
            font-weight: bold;
            color: #333;
            border-bottom: 1px solid #a0a0a0;
        }

        QWidget {
            background: #f8f8f8;
        }

        /* Separators between docks */
        QSplitter::handle {
            background: #c0c0c0;
        }

        QMenuBar {
            background: #ececec;
        }
        QMenuBar::item {
            padding: 4px 10px;
            background: transparent;
        }
        QMenuBar::item:selected {
            background: #d0d0d0;
        }

        QMenu {
            background: #ffffff;
            border: 1px solid #a0a0a0;
        }
        QMenu::item:selected {
            background: #c8d8ff;
        }

        QToolBar {
            background: #e0e0e0;
            border-bottom: 1px solid #a0a0a0;
            spacing: 5px;
        }

        QStatusBar {
            background: #e5e5e5;
            border-top: 1px solid #a0a0a0;
        }
    )";

    app.setStyle(QStyleFactory::create("Fusion"));
    app.setStyleSheet(style);    
}
