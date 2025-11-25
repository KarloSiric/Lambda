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
 *   Purpose: QtApplication MainWindow Point 
 * ═══════════════════════════════════════════════════════════════════════════
 */




#include "MainWindow.h"
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QToolBar>
#include <QWidget>
#include <QDockWidget>
#include <QOpenGLWidget>



MainWindow::MainWindow( QWidget *parent ) 
    : QMainWindow( parent ) 
{
    
    setWindowTitle( "Lambda MDL Editor" );
    resize( MW_WIDTH, MW_HEIGHT );
    
    setupMenus( );
    setupToolbars( );
    setupLeftToolbox( );
    setupRightInspector( );
    setupViewports( ); 
    
}


void MainWindow::setupMenus( ) 
{
    
    QMenu *fileMenu = menuBar()->addMenu("File");
    
    fileMenu->addAction( tr( "New" ) );
    fileMenu->addAction( tr( "Open..." ) );
    fileMenu->addAction( tr( "Save" ) );
    fileMenu->addAction( tr( "Save as" ) );
    fileMenu->addSeparator();
    fileMenu->addAction( tr( "Exit" ) );
    
    
    QMenu *editMenu = menuBar()->addMenu("Edit");
    editMenu->addAction( tr( "" ) );
    
    
    
    
    
    
}

