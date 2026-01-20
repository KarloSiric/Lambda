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
 *   Purpose: Factory implementation for creating application toolbars
 * ===============================================================================
 */

#include "ToolbarFactory.h"
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QIcon>

namespace ToolbarFactory {

void createToolbars( QMainWindow *mainWindow ) {
	createMainToolbar( mainWindow );
	createSecondaryToolbar( mainWindow );
}

void createMainToolbar( QMainWindow *mainWindow ) {
	QToolBar *toolBar = mainWindow->addToolBar( "Main Toolbar" );
	toolBar->setMovable( false );
	toolBar->setIconSize( QSize( 18, 18 ) );

	QAction *actionOpen = toolBar->addAction( QIcon( ":/icons/open_folder.png" ), "Open Folder" );

	// FILE
	QAction *actionNew = toolBar->addAction( QIcon( ":/icons/new_file.png" ), "New File" );
	toolBar->addAction( "Open" );
	toolBar->addAction( "Save" );
	toolBar->addSeparator();

	// UNDO / REDO
	toolBar->addAction( "Undo" );
	toolBar->addAction( "Redo" );
	toolBar->addSeparator();

	// CAMERA
	toolBar->addAction( "Orbit" );
	toolBar->addAction( "Pan" );
	toolBar->addAction( "Zoom" );
	toolBar->addAction( "Reset Camera" );
	toolBar->addSeparator();

	// VIEW
	toolBar->addAction( "Wireframe" );
	toolBar->addAction( "Flat" );
	toolBar->addAction( "Textured" );
	toolBar->addAction( "Fullbright" );
	toolBar->addAction( "Bones" );
	toolBar->addAction( "Hitboxes" );
	toolBar->addAction( "Grid" );
	toolBar->addSeparator();

	// MODEL EDIT
	toolBar->addAction( "Scale" );
	toolBar->addAction( "Rotate" );
	toolBar->addAction( "Translate" );
	toolBar->addAction( "Mirror" );
	toolBar->addAction( "Reset Pose" );
	toolBar->addSeparator();

	// ANIMATION
	toolBar->addAction( "Play" );
	toolBar->addAction( "Pause" );
	toolBar->addAction( "Stop" );
	toolBar->addAction( "Prev Frame" );
	toolBar->addAction( "Next Frame" );
	toolBar->addAction( "Loop" );
	toolBar->addSeparator();

	// COMPILER
	toolBar->addAction( "Compile" );
	toolBar->addAction( "Decompile" );

	// Suppress unused variable warnings
	Q_UNUSED( actionOpen );
	Q_UNUSED( actionNew );
}

void createSecondaryToolbar( QMainWindow *mainWindow ) {
	QToolBar *secondary = mainWindow->addToolBar( "Secondary" );
	secondary->setMovable( false );
	secondary->setIconSize( QSize( 20, 20 ) );
	secondary->setObjectName( "Secondary" );
}

} // namespace ToolbarFactory
