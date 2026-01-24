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
	toolBar->setIconSize( QSize( 20, 20 ) );
	toolBar->setObjectName( "Main Toolbar" );

	// ═══════════════════════════════════════════════════════════════════════
	// FILE SECTION (1 icon)
	// ═══════════════════════════════════════════════════════════════════════
	QAction *actionOpen = toolBar->addAction( "Open" );
	actionOpen->setToolTip( "Open Model (Ctrl+O)" );
	actionOpen->setShortcut( QKeySequence( "Ctrl+O" ) );

	toolBar->addSeparator();

	// ═══════════════════════════════════════════════════════════════════════
	// VIEW TOGGLES SECTION (9 icons)
	// ═══════════════════════════════════════════════════════════════════════
	QAction *actionWireframe = toolBar->addAction( "Wireframe" );
	actionWireframe->setCheckable( true );
	actionWireframe->setToolTip( "Toggle Wireframe (W)" );
	actionWireframe->setShortcut( QKeySequence( "W" ) );

	QAction *actionTextured = toolBar->addAction( "Textured" );
	actionTextured->setCheckable( true );
	actionTextured->setChecked( true ); // Default on
	actionTextured->setToolTip( "Toggle Textured View (T)" );
	actionTextured->setShortcut( QKeySequence( "T" ) );

	QAction *actionBones = toolBar->addAction( "Bones" );
	actionBones->setCheckable( true );
	actionBones->setToolTip( "Toggle Bones/Skeleton (B)" );
	actionBones->setShortcut( QKeySequence( "B" ) );

	QAction *actionHitboxes = toolBar->addAction( "Hitboxes" );
	actionHitboxes->setCheckable( true );
	actionHitboxes->setToolTip( "Toggle Hitboxes (H)" );
	actionHitboxes->setShortcut( QKeySequence( "H" ) );

	QAction *actionAttachments = toolBar->addAction( "Attachments" );
	actionAttachments->setCheckable( true );
	actionAttachments->setToolTip( "Toggle Attachment Points (A)" );
	actionAttachments->setShortcut( QKeySequence( "A" ) );

	QAction *actionNormals = toolBar->addAction( "Normals" );
	actionNormals->setCheckable( true );
	actionNormals->setToolTip( "Toggle Normals (N)" );
	actionNormals->setShortcut( QKeySequence( "N" ) );

	QAction *actionGrid = toolBar->addAction( "Grid" );
	actionGrid->setCheckable( true );
	actionGrid->setChecked( true ); // Default on
	actionGrid->setToolTip( "Toggle Grid (G)" );
	actionGrid->setShortcut( QKeySequence( "G" ) );

	QAction *actionAxes = toolBar->addAction( "Axes" );
	actionAxes->setCheckable( true );
	actionAxes->setChecked( true ); // Default on
	actionAxes->setToolTip( "Toggle Axes (X)" );
	actionAxes->setShortcut( QKeySequence( "X" ) );

	QAction *actionGround = toolBar->addAction( "Ground" );
	actionGround->setCheckable( true );
	actionGround->setToolTip( "Toggle Ground Plane (Shift+G)" );
	actionGround->setShortcut( QKeySequence( "Shift+G" ) );

	toolBar->addSeparator();

	// ═══════════════════════════════════════════════════════════════════════
	// CAMERA SECTION (2 icons)
	// ═══════════════════════════════════════════════════════════════════════
	QAction *actionResetCamera = toolBar->addAction( "Reset Camera" );
	actionResetCamera->setToolTip( "Reset Camera to Default (Home)" );
	actionResetCamera->setShortcut( QKeySequence( "Home" ) );

	QAction *actionCenterModel = toolBar->addAction( "Center Model" );
	actionCenterModel->setToolTip( "Center Model in View (C)" );
	actionCenterModel->setShortcut( QKeySequence( "C" ) );

	toolBar->addSeparator();

	// ═══════════════════════════════════════════════════════════════════════
	// ANIMATION SECTION (6 icons)
	// ═══════════════════════════════════════════════════════════════════════
	QAction *actionPlay = toolBar->addAction( "Play" );
	actionPlay->setToolTip( "Play Animation (Space)" );
	actionPlay->setShortcut( QKeySequence( "Space" ) );

	QAction *actionPause = toolBar->addAction( "Pause" );
	actionPause->setToolTip( "Pause Animation (P)" );
	actionPause->setShortcut( QKeySequence( "P" ) );

	QAction *actionStop = toolBar->addAction( "Stop" );
	actionStop->setToolTip( "Stop Animation (S)" );
	actionStop->setShortcut( QKeySequence( "S" ) );

	QAction *actionPrevFrame = toolBar->addAction( "Prev Frame" );
	actionPrevFrame->setToolTip( "Previous Frame (Left)" );
	actionPrevFrame->setShortcut( QKeySequence( "Left" ) );

	QAction *actionNextFrame = toolBar->addAction( "Next Frame" );
	actionNextFrame->setToolTip( "Next Frame (Right)" );
	actionNextFrame->setShortcut( QKeySequence( "Right" ) );

	QAction *actionLoop = toolBar->addAction( "Loop" );
	actionLoop->setCheckable( true );
	actionLoop->setChecked( true ); // Default on
	actionLoop->setToolTip( "Toggle Loop (L)" );
	actionLoop->setShortcut( QKeySequence( "L" ) );

	toolBar->addSeparator();

	// ═══════════════════════════════════════════════════════════════════════
	// UTILITY SECTION (3 icons)
	// ═══════════════════════════════════════════════════════════════════════
	QAction *actionScreenshot = toolBar->addAction( "Screenshot" );
	actionScreenshot->setToolTip( "Take Screenshot (F12)" );
	actionScreenshot->setShortcut( QKeySequence( "F12" ) );

	QAction *actionBackground = toolBar->addAction( "Background" );
	actionBackground->setToolTip( "Change Background Color" );

	QAction *actionLighting = toolBar->addAction( "Lighting" );
	actionLighting->setCheckable( true );
	actionLighting->setChecked( true ); // Default on
	actionLighting->setToolTip( "Toggle Lighting (Shift+L)" );
	actionLighting->setShortcut( QKeySequence( "Shift+L" ) );

	// Suppress unused variable warnings
	Q_UNUSED( actionOpen );
	Q_UNUSED( actionWireframe );
	Q_UNUSED( actionTextured );
	Q_UNUSED( actionBones );
	Q_UNUSED( actionHitboxes );
	Q_UNUSED( actionAttachments );
	Q_UNUSED( actionNormals );
	Q_UNUSED( actionGrid );
	Q_UNUSED( actionAxes );
	Q_UNUSED( actionGround );
	Q_UNUSED( actionResetCamera );
	Q_UNUSED( actionCenterModel );
	Q_UNUSED( actionPlay );
	Q_UNUSED( actionPause );
	Q_UNUSED( actionStop );
	Q_UNUSED( actionPrevFrame );
	Q_UNUSED( actionNextFrame );
	Q_UNUSED( actionLoop );
	Q_UNUSED( actionScreenshot );
	Q_UNUSED( actionBackground );
	Q_UNUSED( actionLighting );
}

void createSecondaryToolbar( QMainWindow *mainWindow ) {
	QToolBar *secondary = mainWindow->addToolBar( "Secondary" );
	secondary->setMovable( false );
	secondary->setIconSize( QSize( 20, 20 ) );
	secondary->setObjectName( "Secondary" );

	// Secondary toolbar reserved for future use (sequence selector, etc.)
}

} // namespace ToolbarFactory
