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

// ═══════════════════════════════════════════════════════════════════════════
// CLASSIC THEME (Windows 95/2000 Style)
// ═══════════════════════════════════════════════════════════════════════════

void ThemeManager::applyClassicTheme( QApplication *app ) {
    if ( !app ) return;

    // Use Windows style as base
    app->setStyle( QStyleFactory::create( "Windows" ) );

    // Set classic Windows font (Tahoma 14pt - readable size)
    // Fallback chain: Tahoma → MS Sans Serif → System Default
    QFont classicFont;
    classicFont.setFamily( "Tahoma" );
    classicFont.setPointSize( 14 );
    classicFont.setStyleHint( QFont::SansSerif );
    app->setFont( classicFont );

    // Note: Individual component styles are applied per-widget
    // Main window calls getClassicMainWindowStyle(), etc.
}

QString ThemeManager::getClassicMainWindowStyle() {
    return R"(
        QMainWindow {
            background-color: #c0c0c0;
        }
        QMenuBar {
            background-color: #d0d0d0;
            color: #000000;
            border-bottom: 1px solid #808080;
        }
        QMenuBar::item {
            background-color: transparent;
            padding: 4px 8px;
        }
        QMenuBar::item:selected {
            background-color: #000080;
            color: #ffffff;
        }
        QToolBar {
            background-color: #d0d0d0;
            border: 1px solid #808080;
            spacing: 4px;
            padding: 4px;
        }
        QToolButton {
            background-color: #c0c0c0;
            border: 2px solid;
            border-top-color: #ffffff;
            border-left-color: #ffffff;
            border-right-color: #808080;
            border-bottom-color: #808080;
            padding: 5px;
            margin: 2px;
        }
        QToolButton:hover {
            background-color: #d0d0d0;
        }
        QToolButton:pressed {
            border-top-color: #808080;
            border-left-color: #808080;
            border-right-color: #ffffff;
            border-bottom-color: #ffffff;
            background-color: #a0a0a0;
        }
        QToolButton:checked {
            border-top-color: #808080;
            border-left-color: #808080;
            border-right-color: #ffffff;
            border-bottom-color: #ffffff;
            background-color: #a0a0a0;
        }
    )";
}

QString ThemeManager::getClassicViewportContainerStyle() {
    // Use an explicit class selector so the border only applies to the
    // container QWidget itself, not every QWidget child (ModelViewport, tabs,
    // tab bar, etc.) which a bare "QWidget { }" rule would also match.
    return R"(
        QWidget#ViewportContainer {
            background-color: #c0c0c0;
            border: 2px solid;
            border-top-color: #ffffff;
            border-left-color: #ffffff;
            border-right-color: #808080;
            border-bottom-color: #808080;
        }
    )";
}

QString ThemeManager::getClassicTabWidgetStyle() {
    return R"(
        QTabWidget::pane {
            border: 1px solid;
            border-top-color: #808080;
            border-left-color: #808080;
            border-right-color: #ffffff;
            border-bottom-color: #ffffff;
            background-color: #c0c0c0;
            margin: 0px;
        }
        QTabBar::tab {
            background-color: #c0c0c0;
            border: 1px solid;
            border-top-color: #ffffff;
            border-left-color: #ffffff;
            border-right-color: #808080;
            border-bottom: none;
            padding: 2px 14px 2px 6px;
            margin-right: 1px;
            color: #000000;
            font-size: 10px;
            min-width: 50px;
            max-width: 150px;
            min-height: 16px;
            max-height: 16px;
        }
        QTabBar::tab:first {
            padding: 2px 14px 2px 6px;
            min-width: 50px;
            min-height: 16px;
            max-height: 16px;
        }
        QTabBar::tab:selected {
            background-color: #0a246a;
            border-top-color: #0d2d85;
            border-left-color: #0d2d85;
            border-right-color: #081d55;
            border-bottom: 1px solid #0a246a;
            color: #ffffff;
            font-weight: bold;
        }
        QTabBar::tab:hover {
            background-color: #a8a8a8;
        }
        QTabBar::close-button {
            subcontrol-position: center right;
            width: 10px;
            height: 10px;
            margin-right: 2px;
            background-color: transparent;
            border: none;
            image: url(data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMTAiIGhlaWdodD0iMTAiPjxwYXRoIGQ9Ik0yLDIgTDgsOCBNMiw4IEw4LDIiIHN0cm9rZT0iIzYwNjA2MCIgc3Ryb2tlLXdpZHRoPSIxLjUiIGZpbGw9Im5vbmUiLz48L3N2Zz4=);
        }
        QTabBar::close-button:hover {
            background-color: #d04040;
            border: 1px solid #a03030;
            image: url(data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMTAiIGhlaWdodD0iMTAiPjxwYXRoIGQ9Ik0yLDIgTDgsOCBNMiw4IEw4LDIiIHN0cm9rZT0iI2ZmZmZmZiIgc3Ryb2tlLXdpZHRoPSIxLjUiIGZpbGw9Im5vbmUiLz48L3N2Zz4=);
        }
    )";
}

QString ThemeManager::getClassicBottomTabsStyle() {
    return R"(
        QTabWidget::pane {
            border: 2px solid;
            border-top-color: #808080;
            border-left-color: #808080;
            border-right-color: #ffffff;
            border-bottom-color: #ffffff;
            background-color: #353535;
        }
        QTabBar::tab {
            background-color: #c0c0c0;
            border: 2px solid;
            border-top-color: #ffffff;
            border-left-color: #ffffff;
            border-right-color: #808080;
            border-bottom: none;
            padding: 4px 12px;
            margin-right: 2px;
            color: #000000;
            min-width: 80px;
        }
        QTabBar::tab:selected {
            background-color: #0a246a;
            border-top-color: #0d2d85;
            border-left-color: #0d2d85;
            border-right-color: #081d55;
            border-bottom: 2px solid #0a246a;
            color: #ffffff;
            font-weight: bold;
        }
        QTabBar::tab:hover {
            background-color: #a8a8a8;
        }
    )";
}

QString ThemeManager::getClassicDockStyle() {
    return R"(
        QDockWidget {
            background-color: #c0c0c0;
            border: 3px solid;
            border-top-color: #ffffff;
            border-left-color: #ffffff;
            border-right-color: #808080;
            border-bottom-color: #808080;
        }
        QDockWidget::title {
            background-color: #0a246a;
            color: #ffffff;
            padding: 4px 6px;
            text-align: left;
            font-weight: bold;
        }
        QDockWidget > QWidget {
            border: 2px solid;
            border-top-color: #808080;
            border-left-color: #808080;
            border-right-color: #f0f0f0;
            border-bottom-color: #f0f0f0;
            background-color: #353535;
        }
    )";
}

QString ThemeManager::getClassicInspectorStyle() {
    return R"(
        /* === 90s WINDOWS 95 STYLE === */

        /* Main panel background - classic gray */
        QWidget {
            background-color: #c0c0c0;
            color: #000000;
        }

        /* Tab widget styling - clean gray theme */
        QTabWidget {
            background-color: #c0c0c0;
        }
        QTabWidget::pane {
            border: 2px solid;
            border-top-color: #808080;
            border-left-color: #808080;
            border-right-color: #ffffff;
            border-bottom-color: #ffffff;
            background-color: #c0c0c0;
        }
        QTabWidget::tab-bar {
            left: 0px;
            background-color: #c0c0c0;
        }

        /* Tab bar background - gray to match theme */
        QTabBar {
            background-color: #c0c0c0;
        }

        /* Tab buttons - 3D raised style */
        QTabBar::tab {
            background-color: #c0c0c0;
            border: 2px solid;
            border-top-color: #ffffff;
            border-left-color: #ffffff;
            border-right-color: #808080;
            border-bottom-color: #808080;
            padding: 4px 12px;
            margin-right: 2px;
            color: #000000;
            font-size: 11px;
        }
        QTabBar::tab:selected {
            background-color: #0a246a;
            border-top-color: #0d2d85;
            border-left-color: #0d2d85;
            border-right-color: #081d55;
            border-bottom-color: #081d55;
            color: #ffffff;
            font-weight: bold;
        }
        QTabBar::tab:!selected {
            margin-top: 2px;
            background-color: #c0c0c0;
        }
        QTabBar::tab:hover:!selected {
            background-color: #d4d4d4;
        }

        /* Group boxes with 3D beveled borders */
        QGroupBox {
            background-color: #c0c0c0;
            border: 2px groove;
            border-top-color: #808080;
            border-left-color: #808080;
            border-right-color: #ffffff;
            border-bottom-color: #ffffff;
            margin-top: 14px;
            padding: 8px 4px 4px 4px;
            font-weight: bold;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            padding: 2px 6px;
            background-color: #c0c0c0;
            color: #000000;
        }

        /* Tables with sunken 3D border */
        QTableWidget, QTreeWidget, QListWidget {
            background-color: #ffffff;
            alternate-background-color: #f0f0f0;
            color: #000000;
            border: 2px solid;
            border-top-color: #808080;
            border-left-color: #808080;
            border-right-color: #ffffff;
            border-bottom-color: #ffffff;
            selection-background-color: #0a246a;
            selection-color: #ffffff;
            gridline-color: #c0c0c0;
        }
        QHeaderView::section {
            background-color: #c0c0c0;
            border: 1px solid;
            border-top-color: #ffffff;
            border-left-color: #ffffff;
            border-right-color: #808080;
            border-bottom-color: #808080;
            padding: 2px 4px;
            color: #000000;
        }

        /* Raised 3D buttons */
        QPushButton {
            background-color: #c0c0c0;
            border: 2px solid;
            border-top-color: #ffffff;
            border-left-color: #ffffff;
            border-right-color: #808080;
            border-bottom-color: #808080;
            padding: 3px 12px;
            min-height: 18px;
            color: #000000;
        }
        QPushButton:pressed {
            border-top-color: #808080;
            border-left-color: #808080;
            border-right-color: #ffffff;
            border-bottom-color: #ffffff;
            background-color: #b0b0b0;
        }
        QPushButton:hover {
            background-color: #d0d0d0;
        }
        QPushButton:disabled {
            color: #808080;
        }

        /* Checkboxes with visible checkmark - Windows 95 style */
        QCheckBox {
            spacing: 6px;
            color: #000000;
        }
        QCheckBox::indicator {
            width: 13px;
            height: 13px;
            border: 2px solid;
            border-top-color: #808080;
            border-left-color: #808080;
            border-right-color: #ffffff;
            border-bottom-color: #ffffff;
            background-color: #ffffff;
        }
        QCheckBox::indicator:checked {
            /* Checkmark icon for checked state */
            background-color: #ffffff;
            image: url(:/icons/checkmark.png);
        }
        QCheckBox::indicator:unchecked {
            background-color: #ffffff;
        }
        QCheckBox::indicator:hover {
            border-top-color: #505050;
            border-left-color: #505050;
        }
        QCheckBox::indicator:disabled {
            background-color: #c0c0c0;
            border-top-color: #a0a0a0;
            border-left-color: #a0a0a0;
        }
        QCheckBox::indicator:checked:disabled {
            background-color: #909090;
        }

        /* Radio buttons */
        QRadioButton {
            spacing: 5px;
            color: #000000;
        }

        /* Sliders with classic look */
        QSlider::groove:horizontal {
            border: 1px solid;
            border-top-color: #808080;
            border-left-color: #808080;
            border-right-color: #ffffff;
            border-bottom-color: #ffffff;
            height: 6px;
            background: #c0c0c0;
        }
        QSlider::handle:horizontal {
            background-color: #c0c0c0;
            border: 2px solid;
            border-top-color: #ffffff;
            border-left-color: #ffffff;
            border-right-color: #808080;
            border-bottom-color: #808080;
            width: 11px;
            margin: -5px 0;
        }

        /* Combo boxes */
        QComboBox {
            background-color: #ffffff;
            border: 2px solid;
            border-top-color: #808080;
            border-left-color: #808080;
            border-right-color: #ffffff;
            border-bottom-color: #ffffff;
            padding: 2px 4px;
            color: #000000;
        }
        QComboBox::drop-down {
            border: none;
            width: 16px;
            background-color: #c0c0c0;
        }
        QComboBox QAbstractItemView {
            background-color: #ffffff;
            color: #000000;
            selection-background-color: #0a246a;
            selection-color: #ffffff;
        }

        /* Spin boxes */
        QSpinBox, QDoubleSpinBox {
            background-color: #ffffff;
            border: 2px solid;
            border-top-color: #808080;
            border-left-color: #808080;
            border-right-color: #ffffff;
            border-bottom-color: #ffffff;
            padding: 1px 3px;
            color: #000000;
        }

        /* Labels */
        QLabel {
            background-color: transparent;
            color: #000000;
            border: none;
        }

        /* Scroll areas */
        QScrollArea {
            border: none;
            background-color: #c0c0c0;
        }

        /* Scroll bars - classic Windows style */
        QScrollBar:vertical {
            background: #c0c0c0;
            width: 16px;
            border: 1px solid #808080;
        }
        QScrollBar::handle:vertical {
            background-color: #c0c0c0;
            border: 2px solid;
            border-top-color: #ffffff;
            border-left-color: #ffffff;
            border-right-color: #808080;
            border-bottom-color: #808080;
            min-height: 20px;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            background-color: #c0c0c0;
            border: 2px solid;
            border-top-color: #ffffff;
            border-left-color: #ffffff;
            border-right-color: #808080;
            border-bottom-color: #808080;
            height: 16px;
        }
        QScrollBar:horizontal {
            background: #c0c0c0;
            height: 16px;
            border: 1px solid #808080;
        }
        QScrollBar::handle:horizontal {
            background-color: #c0c0c0;
            border: 2px solid;
            border-top-color: #ffffff;
            border-left-color: #ffffff;
            border-right-color: #808080;
            border-bottom-color: #808080;
            min-width: 20px;
        }

        /* Line edits */
        QLineEdit {
            background-color: #ffffff;
            border: 2px solid;
            border-top-color: #808080;
            border-left-color: #808080;
            border-right-color: #ffffff;
            border-bottom-color: #ffffff;
            padding: 2px;
            color: #000000;
        }

        /* Text edits */
        QTextEdit, QPlainTextEdit {
            background-color: #ffffff;
            border: 2px solid;
            border-top-color: #808080;
            border-left-color: #808080;
            border-right-color: #ffffff;
            border-bottom-color: #ffffff;
            color: #000000;
        }

        /* Frame separators */
        QFrame[frameShape="4"], QFrame[frameShape="5"] {
            color: #808080;
        }
    )";
}

// ═══════════════════════════════════════════════════════════════════════════
// MODERN LIGHT THEME
// ═══════════════════════════════════════════════════════════════════════════

void ThemeManager::applyModernLightTheme( QApplication *app ) {
    if ( !app ) return;

    app->setStyle( QStyleFactory::create( "Fusion" ) );

    // Use system default font - works on all platforms
    app->setFont( QFont( app->font().family(), 11 ) );

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
    app->setPalette( pal );

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


        /* TOOLBAR SECONDARY */
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

    app->setStyleSheet( style );
}

// ═══════════════════════════════════════════════════════════════════════════
// MODERN DARK THEME (TODO)
// ═══════════════════════════════════════════════════════════════════════════

void ThemeManager::applyModernDarkTheme( QApplication *app ) {
    // TODO: Implement dark theme
    Q_UNUSED( app );
}
