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
#include "IconFactory.h"
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
	toolBar->setIconSize( QSize( 32, 32 ) );  // Larger icons for better visibility
	toolBar->setToolButtonStyle( Qt::ToolButtonIconOnly );
	toolBar->setObjectName( "Main Toolbar" );

	// ═══════════════════════════════════════════════════════════════════════
	// FILE OPERATIONS - Removed open folder (use File menu instead)
	// ═══════════════════════════════════════════════════════════════════════

	// ═══════════════════════════════════════════════════════════════════════
	// RENDER MODES
	// ═══════════════════════════════════════════════════════════════════════
	QAction *actionWireframe = toolBar->addAction( IconFactory::wireframe(), "" );
	actionWireframe->setCheckable( true );
	actionWireframe->setToolTip( "Wireframe" );

	QAction *actionTextured = toolBar->addAction( QIcon( ":/icons/textured-icon.png" ), "" );
	actionTextured->setCheckable( true );
	actionTextured->setChecked( true );
	actionTextured->setToolTip( "Textured (T)" );
	actionTextured->setShortcut( QKeySequence( "T" ) );

	toolBar->addSeparator();

	// ═══════════════════════════════════════════════════════════════════════
	// OVERLAY TOGGLES
	// ═══════════════════════════════════════════════════════════════════════
	QAction *actionBones = toolBar->addAction( QIcon( ":/icons/draw-bones-icon.png" ), "" );
	actionBones->setCheckable( true );
	actionBones->setToolTip( "Bones (B)" );
	actionBones->setShortcut( QKeySequence( "B" ) );

	QAction *actionHitboxes = toolBar->addAction( QIcon( ":/icons/draw-hitboxes-icon.png" ), "" );
	actionHitboxes->setCheckable( true );
	actionHitboxes->setToolTip( "Hitboxes (H)" );
	actionHitboxes->setShortcut( QKeySequence( "H" ) );

	QAction *actionAttachments = toolBar->addAction( QIcon( ":/icons/draw-attachments-icon.png" ), "" );
	actionAttachments->setCheckable( true );
	actionAttachments->setToolTip( "Attachments" );

	toolBar->addSeparator();

	// ═══════════════════════════════════════════════════════════════════════
	// SCENE HELPERS - Grid group together
	// ═══════════════════════════════════════════════════════════════════════
	QAction *actionGrid = toolBar->addAction( IconFactory::grid(), "" );
	actionGrid->setCheckable( true );
	actionGrid->setChecked( true );
	actionGrid->setToolTip( "Grid (G)" );
	actionGrid->setShortcut( QKeySequence( "G" ) );

	QAction *actionGridIncrease = toolBar->addAction( IconFactory::gridIncrease(), "" );
	actionGridIncrease->setToolTip( "Increase Grid Size (])" );
	actionGridIncrease->setShortcut( QKeySequence( "]" ) );

	QAction *actionGridDecrease = toolBar->addAction( IconFactory::gridDecrease(), "" );
	actionGridDecrease->setToolTip( "Decrease Grid Size ([)" );
	actionGridDecrease->setShortcut( QKeySequence( "[" ) );

	toolBar->addSeparator();

	QAction *actionAxes = toolBar->addAction( QIcon( ":/icons/draw-axis-icon.png" ), "" );
	actionAxes->setCheckable( true );
	actionAxes->setChecked( true );
	actionAxes->setToolTip( "Axes (X)" );
	actionAxes->setShortcut( QKeySequence( "X" ) );

	QAction *actionGround = toolBar->addAction( QIcon( ":/icons/draw-ground-icon.png" ), "" );
	actionGround->setCheckable( true );
	actionGround->setToolTip( "Ground (Shift+G)" );
	actionGround->setShortcut( QKeySequence( "Shift+G" ) );

	toolBar->addSeparator();

	// ═══════════════════════════════════════════════════════════════════════
	// CAMERA / VIEWS
	// ═══════════════════════════════════════════════════════════════════════
	QAction *actionResetCamera = toolBar->addAction( QIcon( ":/icons/camera-reset-icon.png" ), "" );
	actionResetCamera->setToolTip( "Reset Camera (Home)" );
	actionResetCamera->setShortcut( QKeySequence( "Home" ) );

	QAction *actionViewXY = toolBar->addAction( IconFactory::viewFront(), "" );
	actionViewXY->setToolTip( "Front View (Ctrl+1)" );
	actionViewXY->setShortcut( QKeySequence( "Ctrl+1" ) );

	QAction *actionViewXZ = toolBar->addAction( IconFactory::viewTop(), "" );
	actionViewXZ->setToolTip( "Top View (Ctrl+2)" );
	actionViewXZ->setShortcut( QKeySequence( "Ctrl+2" ) );

	QAction *actionViewZY = toolBar->addAction( IconFactory::viewSide(), "" );
	actionViewZY->setToolTip( "Side View (Ctrl+3)" );
	actionViewZY->setShortcut( QKeySequence( "Ctrl+3" ) );

	// Suppress unused variable warnings
	Q_UNUSED( actionWireframe );
	Q_UNUSED( actionTextured );
	Q_UNUSED( actionBones );
	Q_UNUSED( actionHitboxes );
	Q_UNUSED( actionAttachments );
	Q_UNUSED( actionGrid );
	Q_UNUSED( actionAxes );
	Q_UNUSED( actionGround );
	Q_UNUSED( actionGridIncrease );
	Q_UNUSED( actionGridDecrease );
	Q_UNUSED( actionResetCamera );
	Q_UNUSED( actionViewXY );
	Q_UNUSED( actionViewXZ );
	Q_UNUSED( actionViewZY );
}

void createSecondaryToolbar( QMainWindow *mainWindow ) {
	QToolBar *secondary = mainWindow->addToolBar( "Secondary" );
	secondary->setMovable( false );
	secondary->setIconSize( QSize( 20, 20 ) );
	secondary->setObjectName( "Secondary" );

	// Secondary toolbar reserved for future use (sequence selector, etc.)
}

} // namespace ToolbarFactory
