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
#include <QtGui/qfont.h>
#include <QtGui/qpalette.h>
#include <QtGui/qwindowdefs.h>
#include <QtWidgets/qapplication.h>
#include <QStyleFactory>

void ThemeManager::applyThemeLight( QApplication &app ) {
    
	app.setStyle( QStyleFactory::create( "Fusion" ) );

	app.setFont( QFont( "SF Pro", 16 ) );

	// ----------------------------
	// CUSTOM PALETTE
	// ----------------------------
	QPalette pal;
	pal.setColor( QPalette::Window, QColor( 245, 245, 245 ) ); // panels
	pal.setColor( QPalette::Base, QColor( 250, 250, 250 ) ); // text areas
	pal.setColor( QPalette::AlternateBase, QColor( 235, 235, 235 ) );
	pal.setColor( QPalette::ToolTipBase, Qt::white );
	pal.setColor( QPalette::ToolTipText, Qt::black );
	pal.setColor( QPalette::Text, Qt::black );
	pal.setColor( QPalette::Button, QColor( 240, 240, 240 ) );
	pal.setColor( QPalette::ButtonText, Qt::black );
	pal.setColor( QPalette::Highlight, QColor( 70, 120, 255 ) ); // blue selection
	pal.setColor( QPalette::HighlightedText, Qt::white );
	app.setPalette( pal );

	// ----------------------------
	// STYLESHEET (modern + hammered)
	// ----------------------------
	QString style = R"(
        QMainWindow {
            background: #f5f5f5;
        }

        QToolBar {
            background: #DEDEDE;
            border-bottom: 1px solid #000000;
            spacing: 6px;
        }

        QToolButton {
            padding: 5px 5px;
            border: 5px solid transparent;
            background: transparent;
        }

        QToolButton:hover {
            padding: 2px;
            background: #C0DCF3;
            border: 0.5px solid #BEBEBE;
        }

        QToolButton:checked {
            background: #D0E4FF;
            border: 1px solid #7AAFFF;
        }        
        
        QToolButton:pressed {
            background: #8FFFFF;
            border-radius: 4px;
        }
        
        QToolBar::separator {
            width: 1px;               /* thickness */
            height: 5px;
            background: #8A8A8A;      /* light gray */
            margin: 4px; 
        }


        /* MENUBAR ------------------------------------------------------- */
        QMenuBar {
            background: #ffffff;
            border-bottom: 1px solid #d0d0d0;
        }

        QMenuBar::item:selected {
            background: #e0e0e0;
        }

        QDockWidget {
            titlebar-close-icon: none;
            titlebar-normal-icon: none;
            border: 1px solid #C0C0C0;
            background: #F2F2F2;
        }

        QDockWidget::title {
            background: #E6E6E6;
            padding: 3px;
            margin: 0;
            border-bottom: 1px solid #BEBEBE;
            font-size: 11px;
            color: #555;
        }

        QDockWidget::close-button, QDockWidget::float-button {
            width: 0;
            height: 0;
            margin: 0;
            padding: 0;
        }
        /* SPLITTER ------------------------------------------------------ */
        QSplitter::handle {
            background: #cccccc;
        }

        /* STATUSBAR ----------------------------------------------------- */
        QStatusBar {
            background: #efefef;
            border-top: 1px solid #c8c8c8;
        }
        
        /* Global */
        QWidget {
            background-color: #f5f5f5;
            color: #111;
        }

        /* Dark central viewport */
        #viewport {
            background-color: #202020;
        }

        /* Dock widget backgrounds + borders */
        QDockWidget {
            border: 1px solid #b5b5b5;
            background: #f0f0f0;
        }

        QDockWidget::title {
            padding: 5px;
            background: #e8e8e8;
            border-bottom: 1px solid #b5b5b5;
            font-weight: bold;
        }
        
        
        QWidget#ViewportContainer {
            border-left: 1px solid #C0C0C0;
            border-right: 1px solid #C0C0C0;
            border-top: 1px solid #C0C0C0;
            background: #1E1E1E;
        }
        
        
        // TOOLBAR SECONDARY // 
        QToolBar#Secondary {
            border-top: 1px solid #C0C0C0;
        }
        
        QMainWindow::separator {
            width: 4px;
            height: 10px;
            background: #C6C6C6;
        }

        QDockWidget {
            border: 1px solid #C6C6C6;
            background: #F8F8F8;
        }

        #ViewportContainer {
            border: 1px solid #C6C6C6;
            background: #1E1E1E; /* dark viewport */
        }
        
        
        
        

        QSplitter::handle {
            background: #C6C6C6;       /* separator color */
        }

        QSplitter::handle:horizontal {
            width: 3px;
        }

        QSplitter::handle:vertical {
            height: 3px;
        }
        
        
        #ConsoleDock {
            border-top: 1px solid #C6C6C6;
            background: #FFFFFF;
        }
        
        
        /* Bottom console separator line */
        #Console {
            border-top: 1px solid #b5b5b5;
        }
        
        #InspectorPanel {
            background: #efefef;
            border-left: 1px solid #b5b5b5;
        }
        
        
        
    )";

	app.setStyleSheet( style );
}

void ThemeManager::applyThemeDark( QApplication &app ) {
	// TODO: need to implement it
}
