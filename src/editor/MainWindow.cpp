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
#include "widgets/ModelViewport.h"
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QToolBar>
#include <QWidget>
#include <QWindow>
#include <QActionGroup>
#include <QDockWidget>
#include <QOpenGLWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QTabBar>
#include <QPushButton>
#include <QLineEdit>
#include <exception>
#include <QtCore/qcontainerfwd.h>
#include <QtCore/qnamespace.h>
#include <QtCore/qobject.h>
#include <QtGui/qaction.h>
#include <QtGui/qactiongroup.h>
#include <QtGui/qicon.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qmainwindow.h>
#include <QtWidgets/qmenu.h>
#include <QtWidgets/qmessagebox.h>
#include <QtWidgets/qtoolbar.h>

MainWindow::MainWindow( QWidget *parent )
	: QMainWindow( parent ) {
	setWindowTitle( "Lambda MDL Editor" );
	resize( MW_WIDTH, MW_HEIGHT );

	// Apply classic window styling
	setStyleSheet(
		"QMainWindow { "
		"    background-color: #c0c0c0; "
		"} "
		"QMenuBar { "
		"    background-color: #d0d0d0; "
		"    color: #000000; "
		"    border-bottom: 1px solid #808080; "
		"} "
		"QMenuBar::item { "
		"    background-color: transparent; "
		"    padding: 4px 8px; "
		"} "
		"QMenuBar::item:selected { "
		"    background-color: #000080; "
		"    color: #ffffff; "
		"} "
		"QToolBar { "
		"    background-color: #d0d0d0; "
		"    border: 1px solid #808080; "
		"    spacing: 3px; "
		"    padding: 2px; "
		"} "
		"QToolButton { "
		"    background-color: #c0c0c0; "
		"    border: 2px solid; "
		"    border-top-color: #ffffff; "
		"    border-left-color: #ffffff; "
		"    border-right-color: #808080; "
		"    border-bottom-color: #808080; "
		"    padding: 3px; "
		"    margin: 1px; "
		"} "
		"QToolButton:hover { "
		"    background-color: #d0d0d0; "
		"} "
		"QToolButton:pressed { "
		"    border-top-color: #808080; "
		"    border-left-color: #808080; "
		"    border-right-color: #ffffff; "
		"    border-bottom-color: #ffffff; "
		"    background-color: #a0a0a0; "
		"}" );

	// Setting up all the major componenents
	setupMenus();
	setupToolbars();

	// Setting up the docks and the main window for viewing things
	setupDocks();
	setupViewports();

	// Set up real-time status bar update timer (60 Hz)
	m_statusUpdateTimer = new QTimer( this );
	// Setting the timer update interval time to be 100 ms and not 16.7 like before
	// @Note: Improvement on performance for re-rendering the status bar text update
	m_statusUpdateTimer->setInterval( 100 ); // ~10 FPS
	connect( m_statusUpdateTimer, &QTimer::timeout, this, &MainWindow::onStatusBarUpdate );
	m_statusUpdateTimer->start();

	// Initialize FPS tracking
	m_fpsTimer.start();
	m_frameCount = 0;
	m_lastFps = 0.0f;
}
void MainWindow::setupMenus() {
	createFileMenu();
	createEditMenu();
	createViewMenu();
	createToolsMenu();
	createModelMenu();
	createBodypartMenu();
	createBonesMenu();
	createSequencesMenu();
	createTexturesMenu();
	createDebugMenu();
	createWindowMenu();
	createHelpMenu();
}
void MainWindow::createToolbarUpper() {
	QToolBar *toolBar = addToolBar( "Main Toolbar" );
	toolBar->setMovable( false );
	toolBar->setIconSize( QSize( 18, 18 ) );

	QAction *actionOpen = toolBar->addAction( QIcon( ":/icons/open_folder.png" ), "Open Folder" );

	// FILE
	QAction *actionNew = toolBar->addAction( QIcon( ":/icons/new_file.png" ), "New File" );
	// toolBar->addAction( "New" );
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

	// @Note: Adding second toolbar
	QToolBar *secondary = addToolBar( "Secondary" );
	secondary->setMovable( false );
	secondary->setIconSize( QSize( 20, 20 ) );

	secondary->setObjectName( "Secondary" );
}

void MainWindow::createModelMenu() {
	QMenu *modelMenu = menuBar()->addMenu( tr( "&Model" ) );

	// ═══════════════════════════════════════════════════════
	// MODEL INFO
	// ═══════════════════════════════════════════════════════

	modelMenu->addAction( "Model Properties..." ); // Opens dialog
	modelMenu->addAction( "Model Statistics..." ); // Vertex/tri count, etc.

	modelMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// MODEL OPERATIONS
	// ═══════════════════════════════════════════════════════

	modelMenu->addAction( "Validate Model..." ); // Check for errors
	modelMenu->addAction( "Optimize Model..." ); // Reduce poly count
	modelMenu->addAction( "Fix Issues..." ); // Auto-fix common problems

	modelMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// MODEL METADATA
	// ═══════════════════════════════════════════════════════

	modelMenu->addAction( "Set Eye Position..." );
	modelMenu->addAction( "Set Bounding Box..." );
	modelMenu->addAction( "Set Collision Hull..." );

	modelMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// LIST SUBMENUS (Browse model contents)
	// ═══════════════════════════════════════════════════════

	QMenu *listMenu = modelMenu->addMenu( "List" );
	listMenu->addAction( "List Bones..." ); // Opens dialog with all bones
	listMenu->addAction( "List Sequences..." );
	listMenu->addAction( "List Bodyparts..." );
	listMenu->addAction( "List Textures..." );
	listMenu->addAction( "List Hitboxes..." );
	listMenu->addAction( "List Attachments..." );
	listMenu->addAction( "List Events..." );

	modelMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// ADVANCED
	// ═══════════════════════════════════════════════════════

	modelMenu->addAction( "Generate LODs..." ); // Level of Detail models
	modelMenu->addAction( "Bake Vertex Colors..." );
	modelMenu->addAction( "Convert to Source 2..." ); // Future feature
}

void MainWindow::createBodypartMenu() {
	QMenu *bodypartMenu = menuBar()->addMenu( tr( "&Bodyparts" ) );

	// ═══════════════════════════════════════════════════════
	// SELECT BODYPART (Will be populated dynamically from model)
	// ═══════════════════════════════════════════════════════

	QMenu *selectBodypartMenu = bodypartMenu->addMenu( "Select Bodypart" );
	// NOTE: In a real implementation, you'll populate this from the loaded model's bodyparts“
	// For now, we'll add placeholders

	selectBodypartMenu->addAction( "Body (3 submdoels)" );
	selectBodypartMenu->addAction( "Head (2 submodels)" );
	selectBodypartMenu->addAction( "Weapon (4 submodels)" );
	selectBodypartMenu->addSeparator();
	selectBodypartMenu->addAction( "Refresh List" );

	bodypartMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// SUBMODEL SELECTION (Changes variant of selected bodypart)
	// ═══════════════════════════════════════════════════════

	QMenu *submodelMenu = bodypartMenu->addMenu( "Select Submodel" );
	// this is the radio button behavior
	QActionGroup *submodelGroup = new QActionGroup( this );

	QAction *submodel0 = submodelMenu->addAction( "Submodel 0 (Default)" );
	submodel0->setCheckable( true );
	submodel0->setChecked( true );
	submodelGroup->addAction( submodel0 );

	QAction *submodel1 = submodelMenu->addAction( "Submodel 1" );
	submodel1->setCheckable( true );
	submodelGroup->addAction( submodel1 );

	QAction *submodel2 = submodelMenu->addAction( "Submodel 2" );
	submodel2->setCheckable( true );
	submodelGroup->addAction( submodel2 );

	submodelMenu->addSeparator();
	submodelMenu->addAction( "Randomize" );
	submodelMenu->addAction( "Reset to Default" );

	bodypartMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// VISIBILITY OPTIONS
	// ═══════════════════════════════════════════════════════

	QAction *showBodyparts = bodypartMenu->addAction( "Show All Bodyparts" );
	showBodyparts->setCheckable( true );
	showBodyparts->setChecked( true );

	QAction *showBodypartNames = bodypartMenu->addAction( "Show Bodypart Names" );
	showBodypartNames->setCheckable( true );

	QAction *showSubmodelBounds = bodypartMenu->addAction( "Show Submodel Bounds" );
	showSubmodelBounds->setCheckable( true );

	QAction *highlightSelected = bodypartMenu->addAction( "Highlight Selected" );
	highlightSelected->setCheckable( true );

	bodypartMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// BODYGROUP PRESETS (Common combinations)
	// ═══════════════════════════════════════════════════════

	QMenu *presetsMenu = bodypartMenu->addMenu( "Bodygroup Presets" );
	presetsMenu->addAction( "Default Loadout" );
	presetsMenu->addAction( "All Variants Visible" );
	presetsMenu->addAction( "Custom Preset 1..." );
	presetsMenu->addAction( "Custom Preset 2..." );
	presetsMenu->addSeparator();
	presetsMenu->addAction( "Save Current as Preset..." );

	bodypartMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// IMPORT/EXPORT BODYPARTS
	// ═══════════════════════════════════════════════════════

	QMenu *importBodypartMenu = bodypartMenu->addMenu( "Import Bodypart" );
	importBodypartMenu->addAction( "Import from SMD..." );
	importBodypartMenu->addAction( "Import from OBJ..." );
	importBodypartMenu->addAction( "Import from FBX..." );

	QMenu *exportBodypartMenu = bodypartMenu->addMenu( "Export Bodypart" );
	exportBodypartMenu->addAction( "Export Selected to SMD..." );
	exportBodypartMenu->addAction( "Export Selected to OBJ..." );
	exportBodypartMenu->addAction( "Export All Bodyparts..." );

	bodypartMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// BODYPART INFORMATION
	// ═══════════════════════════════════════════════════════

	bodypartMenu->addAction( "List All Bodyparts..." ); // Opens dialog
	bodypartMenu->addAction( "Bodypart Properties..." ); // Shows selected bodypart info
}

void MainWindow::createBonesMenu() {
	QMenu *bonesMenu = menuBar()->addMenu( tr( "&Bones" ) );

	// ═══════════════════════════════════════════════════════
	// BONE VISIBILITY
	// ═══════════════════════════════════════════════════════

	QAction *showBones = bonesMenu->addAction( "Show Bones" );
	showBones->setCheckable( true );
	showBones->setChecked( true ); // Usually want to see bones by default

	QAction *showBoneNames = bonesMenu->addAction( "Show Bone Names" );
	showBoneNames->setCheckable( true );

	QAction *showBoneWeights = bonesMenu->addAction( "Show Bone Weights" );
	showBoneWeights->setCheckable( true );
	// NOTE: Shows color-coded vertex weight painting

	QAction *highlightSelectedBone = bonesMenu->addAction( "Highlight Selected Bone" );
	highlightSelectedBone->setCheckable( true );

	bonesMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// SELECT BONE (Hierarchical bone list)
	// ═══════════════════════════════════════════════════════

	QMenu *selectBoneMenu = bonesMenu->addMenu( "Select Bone" );
	// @NOTE: This will be populated from the model's bone hierarchy
	// @NOTE(Karlo): This is being hardcoded at the moment will be populated later
	// Showing example hierarchy with indentation
	selectBoneMenu->addAction( "Root" );
	selectBoneMenu->addAction( "  └─ Pelvis" );
	selectBoneMenu->addAction( "      ├─ Spine1" );
	selectBoneMenu->addAction( "      │   └─ Spine2" );
	selectBoneMenu->addAction( "      │       └─ Neck" );
	selectBoneMenu->addAction( "      │           └─ Head" );
	selectBoneMenu->addAction( "      ├─ L_Thigh" );
	selectBoneMenu->addAction( "      │   └─ L_Calf" );
	selectBoneMenu->addAction( "      │       └─ L_Foot" );
	selectBoneMenu->addAction( "      └─ R_Thigh" );
	selectBoneMenu->addAction( "          └─ R_Calf" );
	selectBoneMenu->addAction( "              └─ R_Foot" );
	selectBoneMenu->addSeparator();
	selectBoneMenu->addAction( "Expand All" );
	selectBoneMenu->addAction( "Collapse All" );
	selectBoneMenu->addAction( "Refresh Hierarchy" );

	bonesMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// BONE DISPLAY OPTIONS
	// ═══════════════════════════════════════════════════════

	QMenu *displayModeMenu = bonesMenu->addMenu( "Bone Display Mode" );
	QActionGroup *displayModeGroup = new QActionGroup( this );

	QAction *displayLines = displayModeMenu->addAction( "Lines" );
	displayLines->setCheckable( true );
	displayLines->setChecked( true ); // Default: simple line skeleton
	displayModeGroup->addAction( displayLines );

	QAction *displayBones = displayModeMenu->addAction( "Bones (3D)" );
	displayBones->setCheckable( true );
	displayModeGroup->addAction( displayBones );

	QAction *displayWireframe = displayModeMenu->addAction( "Wireframe with Vertices" );
	displayWireframe->setCheckable( true );
	displayModeGroup->addAction( displayWireframe );

	displayModeMenu->addSeparator();

	QAction *showBoneAxes = displayModeMenu->addAction( "Show Bone Axes" );
	showBoneAxes->setCheckable( true );
	// Shows X, Y, Z axes at each bone

	QAction *showBoneOrigins = displayModeMenu->addAction( "Show Bone Origins" );
	showBoneOrigins->setCheckable( true );
	// Shows pivot point of each bone

	displayModeMenu->addSeparator();
	displayModeMenu->addAction( "Bone Size..." ); // Opens slider/input dialog

	bonesMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// HITBOX MANAGEMENT
	// ═══════════════════════════════════════════════════════

	QMenu *hitboxMenu = bonesMenu->addMenu( "Hitboxes" );

	QAction *showHitboxes = hitboxMenu->addAction( "Show Hitboxes" );
	showHitboxes->setCheckable( true );

	QAction *showHitboxNames = hitboxMenu->addAction( "Show Hitbox Names" );
	showHitboxNames->setCheckable( true );

	hitboxMenu->addSeparator();

	// Hitbox sets (for different damage groups - head, chest, legs)
	QMenu *hitboxSetMenu = hitboxMenu->addMenu( "Select Hitbox Set" );
	QActionGroup *hitboxSetGroup = new QActionGroup( this );

	QAction *hitboxDefault = hitboxSetMenu->addAction( "Default (Set 0)" );
	hitboxDefault->setCheckable( true );
	hitboxDefault->setChecked( true );
	hitboxSetGroup->addAction( hitboxDefault );

	QAction *hitboxCustom1 = hitboxSetMenu->addAction( "Custom Set 1" );
	hitboxCustom1->setCheckable( true );
	hitboxSetGroup->addAction( hitboxCustom1 );

	QAction *hitboxCustom2 = hitboxSetMenu->addAction( "Custom Set 2" );
	hitboxCustom2->setCheckable( true );
	hitboxSetGroup->addAction( hitboxCustom2 );

	hitboxMenu->addSeparator();
	hitboxMenu->addAction( "Edit Hitbox..." ); // Opens hitbox editor dialog
	hitboxMenu->addAction( "Add New Hitbox..." );
	hitboxMenu->addAction( "Delete Selected Hitbox" );
	hitboxMenu->addSeparator();
	hitboxMenu->addAction( "Import Hitboxes from QC..." );
	hitboxMenu->addAction( "Export Hitboxes to QC..." );

	bonesMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// BONE CONTROLLERS (For facial animations, mouth movement, etc.)
	// ═══════════════════════════════════════════════════════

	QMenu *controllersMenu = bonesMenu->addMenu( "Bone Controllers" );

	QAction *showControllers = controllersMenu->addAction( "Show Controllers" );
	showControllers->setCheckable( true );

	controllersMenu->addSeparator();

	QMenu *selectControllerMenu = controllersMenu->addMenu( "Select Controller" );
	selectControllerMenu->addAction( "Mouth (Controller 0)" );
	selectControllerMenu->addAction( "Eyes Vertical (Controller 1)" );
	selectControllerMenu->addAction( "Eyes Horizontal (Controller 2)" );
	selectControllerMenu->addAction( "Body Turn (Controller 3)" );

	controllersMenu->addSeparator();
	controllersMenu->addAction( "Reset Controller Values" );
	controllersMenu->addAction( "Edit Controller Range..." );

	bonesMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// CONSTRAINTS & IK (Inverse Kinematics)
	// ═══════════════════════════════════════════════════════

	QMenu *constraintsMenu = bonesMenu->addMenu( "Constraints & IK" );

	QAction *showIKChains = constraintsMenu->addAction( "Show IK Chains" );
	showIKChains->setCheckable( true );

	QAction *showConstraints = constraintsMenu->addAction( "Show Constraints" );
	showConstraints->setCheckable( true );

	constraintsMenu->addSeparator();
	constraintsMenu->addAction( "Add IK Chain..." );
	constraintsMenu->addAction( "Add Constraint..." );
	constraintsMenu->addAction( "Edit Selected IK/Constraint..." );
	constraintsMenu->addAction( "Remove Selected" );

	bonesMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// BONE INFORMATION & TOOLS
	// ═══════════════════════════════════════════════════════

	bonesMenu->addAction( "List All Bones..." ); // Opens dialog with full bone list
	bonesMenu->addAction( "Show Bone Properties..." ); // Shows selected bone data
	bonesMenu->addAction( "Show Bone Hierarchy..." ); // Tree view diagram

	bonesMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// EXPORT BONES
	// ═══════════════════════════════════════════════════════

	QMenu *exportBonesMenu = bonesMenu->addMenu( "Export" );
	exportBonesMenu->addAction( "Export Skeleton to SMD..." );
	exportBonesMenu->addAction( "Export Bone Weights..." );
	exportBonesMenu->addAction( "Export Hitboxes to QC..." );
}

void MainWindow::createSequencesMenu() {
	QMenu *sequencesMenu = menuBar()->addMenu( tr( "&Sequences" ) );

	// ═══════════════════════════════════════════════════════
	// SELECT SEQUENCE (Populated from model)
	// ═══════════════════════════════════════════════════════

	QMenu *selectSeqMenu = sequencesMenu->addMenu( "Select Sequence" );
	// NOTE: Will be populated from loaded model's sequences
	selectSeqMenu->addAction( "idle" );
	selectSeqMenu->addAction( "walk" );
	selectSeqMenu->addAction( "run" );
	selectSeqMenu->addAction( "jump" );
	selectSeqMenu->addAction( "crouch_idle" );
	selectSeqMenu->addAction( "crouch_walk" );
	selectSeqMenu->addAction( "die1" );
	selectSeqMenu->addAction( "die2" );
	selectSeqMenu->addSeparator();
	selectSeqMenu->addAction( "Refresh List" );
	selectSeqMenu->addAction( "Search Sequences..." );

	sequencesMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// PLAYBACK CONTROLS
	// ═══════════════════════════════════════════════════════

	sequencesMenu->addAction( "Play" );
	sequencesMenu->addAction( "Pause" );
	sequencesMenu->addAction( "Stop" );
	sequencesMenu->addAction( "Restart" );

	sequencesMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// FRAME NAVIGATION
	// ═══════════════════════════════════════════════════════

	sequencesMenu->addAction( "Next Frame" );
	sequencesMenu->addAction( "Previous Frame" );
	sequencesMenu->addAction( "Jump to Frame..." ); // Input dialog
	sequencesMenu->addAction( "Jump to Start" );
	sequencesMenu->addAction( "Jump to End" );

	sequencesMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// PLAYBACK OPTIONS
	// ═══════════════════════════════════════════════════════

	QMenu *playbackMenu = sequencesMenu->addMenu( "Playback Options" );

	QAction *loopAnim = playbackMenu->addAction( "Loop" );
	loopAnim->setCheckable( true );
	loopAnim->setChecked( true ); // Usually want looping

	QAction *reverseAnim = playbackMenu->addAction( "Reverse" );
	reverseAnim->setCheckable( true );

	QAction *pingPong = playbackMenu->addAction( "Ping-Pong" );
	pingPong->setCheckable( true );
	// Plays forward, then backward, then repeats

	playbackMenu->addSeparator();

	QMenu *speedMenu = playbackMenu->addMenu( "Playback Speed" );
	QActionGroup *speedGroup = new QActionGroup( this );

	QAction *speed025 = speedMenu->addAction( "0.25x" );
	speed025->setCheckable( true );
	speedGroup->addAction( speed025 );

	QAction *speed050 = speedMenu->addAction( "0.5x" );
	speed050->setCheckable( true );
	speedGroup->addAction( speed050 );

	QAction *speed100 = speedMenu->addAction( "1.0x (Normal)" );
	speed100->setCheckable( true );
	speed100->setChecked( true );
	speedGroup->addAction( speed100 );

	QAction *speed150 = speedMenu->addAction( "1.5x" );
	speed150->setCheckable( true );
	speedGroup->addAction( speed150 );

	QAction *speed200 = speedMenu->addAction( "2.0x" );
	speed200->setCheckable( true );
	speedGroup->addAction( speed200 );

	speedMenu->addSeparator();
	speedMenu->addAction( "Custom Speed..." );

	playbackMenu->addSeparator();
	playbackMenu->addAction( "Override Frame Rate..." ); // Manually set FPS

	sequencesMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// SEQUENCE BLENDING (Mix two animations)
	// ═══════════════════════════════════════════════════════

	QMenu *blendMenu = sequencesMenu->addMenu( "Sequence Blending" );

	QAction *enableBlend = blendMenu->addAction( "Enable Blending" );
	enableBlend->setCheckable( true );

	blendMenu->addSeparator();

	QMenu *blendSeqMenu = blendMenu->addMenu( "Select Blend Sequence" );
	blendSeqMenu->addAction( "idle" );
	blendSeqMenu->addAction( "walk" );
	blendSeqMenu->addAction( "run" );
	// NOTE: Same list as main sequences

	blendMenu->addSeparator();

	QMenu *blendFactorMenu = blendMenu->addMenu( "Blend Factor" );
	QActionGroup *blendFactorGroup = new QActionGroup( this );

	QStringList blendValues = { "0%", "25%", "50%", "75%", "100%" };
	for ( const QString &val : blendValues ) {
		QAction *blendAction = blendFactorMenu->addAction( val );
		blendAction->setCheckable( true );
		if ( val == "50%" ) {
			blendAction->setChecked( true );
		}
		blendFactorGroup->addAction( blendAction );
	}
	blendFactorMenu->addSeparator();
	blendFactorMenu->addAction( "Custom..." );

	blendMenu->addSeparator();

	QMenu *blendTypeMenu = blendMenu->addMenu( "Blend Type" );
	QActionGroup *blendTypeGroup = new QActionGroup( this );

	QAction *blendLinear = blendTypeMenu->addAction( "Linear" );
	blendLinear->setCheckable( true );
	blendLinear->setChecked( true );
	blendTypeGroup->addAction( blendLinear );

	QAction *blendEaseIn = blendTypeMenu->addAction( "Ease In" );
	blendEaseIn->setCheckable( true );
	blendTypeGroup->addAction( blendEaseIn );

	QAction *blendEaseOut = blendTypeMenu->addAction( "Ease Out" );
	blendEaseOut->setCheckable( true );
	blendTypeGroup->addAction( blendEaseOut );

	QAction *blendCustom = blendTypeMenu->addAction( "Custom Curve..." );
	blendCustom->setCheckable( true );
	blendTypeGroup->addAction( blendCustom );

	sequencesMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// SEQUENCE INFORMATION
	// ═══════════════════════════════════════════════════════

	QAction *showSeqInfo = sequencesMenu->addAction( "Show Sequence Info" );
	showSeqInfo->setCheckable( true );
	// Displays: frame count, FPS, duration, blend count

	sequencesMenu->addAction( "List All Sequences..." );
	sequencesMenu->addAction( "Sequence Properties..." );

	sequencesMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// SEQUENCE EVENTS (Sound effects, particle effects, etc.)
	// ═══════════════════════════════════════════════════════

	QMenu *eventsMenu = sequencesMenu->addMenu( "Sequence Events" );

	QAction *showEvents = eventsMenu->addAction( "Show Events" );
	showEvents->setCheckable( true );
	// Highlights event markers on timeline

	eventsMenu->addSeparator();
	eventsMenu->addAction( "Edit Events..." ); // Opens event editor
	eventsMenu->addAction( "Add Event..." );
	eventsMenu->addAction( "Delete Selected Event" );
	eventsMenu->addSeparator();
	eventsMenu->addAction( "Import Events from QC..." );
	eventsMenu->addAction( "Export Events to QC..." );

	sequencesMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// POSE EDITING
	// ═══════════════════════════════════════════════════════

	sequencesMenu->addAction( "Reset Pose" ); // T-pose
	sequencesMenu->addAction( "Apply Current Frame Pose" );
	sequencesMenu->addAction( "Edit Bone Orientation..." );

	sequencesMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// SEQUENCE FILTERING
	// ═══════════════════════════════════════════════════════

	QMenu *filterMenu = sequencesMenu->addMenu( "Filter Sequences" );

	QAction *sortAlpha = filterMenu->addAction( "Sort Alphabetically" );
	sortAlpha->setCheckable( true );

	filterMenu->addSeparator();

	QMenu *filterTypeMenu = filterMenu->addMenu( "Filter by Type" );
	QAction *filterAll = filterTypeMenu->addAction( "All Sequences" );
	filterAll->setCheckable( true );
	filterAll->setChecked( true );

	QAction *filterIdle = filterTypeMenu->addAction( "Idle Animations" );
	filterIdle->setCheckable( true );

	QAction *filterMove = filterTypeMenu->addAction( "Movement" );
	filterMove->setCheckable( true );

	QAction *filterAttack = filterTypeMenu->addAction( "Attack/Action" );
	filterAttack->setCheckable( true );

	QAction *filterCustom = filterTypeMenu->addAction( "Custom Filter..." );
	filterCustom->setCheckable( true );

	sequencesMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// IMPORT/EXPORT
	// ═══════════════════════════════════════════════════════

	QMenu *importSeqMenu = sequencesMenu->addMenu( "Import Sequence" );
	importSeqMenu->addAction( "Import from SMD..." );
	importSeqMenu->addAction( "Import from FBX..." );
	importSeqMenu->addAction( "Import from BVH..." );

	QMenu *exportSeqMenu = sequencesMenu->addMenu( "Export Sequence" );
	exportSeqMenu->addAction( "Export Selected to SMD..." );
	exportSeqMenu->addAction( "Export Selected to FBX..." );
	exportSeqMenu->addAction( "Export All Sequences..." );
}

void MainWindow::createTexturesMenu() {
	QMenu *texturesMenu = menuBar()->addMenu( tr( "&Textures" ) );

	// ═══════════════════════════════════════════════════════
	// SELECT TEXTURE
	// ═══════════════════════════════════════════════════════

	QMenu *selectTexMenu = texturesMenu->addMenu( "Select Texture" );
	// NOTE: Will be populated from model's textures
	selectTexMenu->addAction( "body.bmp (512x512)" );
	selectTexMenu->addAction( "head.bmp (256x256)" );
	selectTexMenu->addAction( "weapon.bmp (128x128)" );
	selectTexMenu->addAction( "eyes.bmp (64x64)" );
	selectTexMenu->addSeparator();
	selectTexMenu->addAction( "Refresh List" );

	texturesMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// TEXTURE DISPLAY
	// ═══════════════════════════════════════════════════════

	QAction *showTexture = texturesMenu->addAction( "Show Texture" );
	showTexture->setCheckable( true );
	showTexture->setChecked( true );

	QAction *showUVMap = texturesMenu->addAction( "Show UV Map" );
	showUVMap->setCheckable( true );
	// Overlays UV layout on model

	QAction *showUVSeams = texturesMenu->addAction( "Show UV Seams" );
	showUVSeams->setCheckable( true );
	// Highlights where UV islands are cut

	QAction *showUVChecker = texturesMenu->addAction( "Show UV Checker" );
	showUVChecker->setCheckable( true );
	// Checkerboard pattern to check UV distortion

	texturesMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// TEXTURE PROPERTIES (Half-Life specific flags)
	// ═══════════════════════════════════════════════════════

	QMenu *propertiesMenu = texturesMenu->addMenu( "Texture Properties" );

	// Half-Life texture flags
	QAction *flagChrome = propertiesMenu->addAction( "Chrome/Reflective" );
	flagChrome->setCheckable( true );

	QAction *flagAdditive = propertiesMenu->addAction( "Additive Blending" );
	flagAdditive->setCheckable( true );

	QAction *flagTransparent = propertiesMenu->addAction( "Transparent" );
	flagTransparent->setCheckable( true );

	QAction *flagFlat = propertiesMenu->addAction( "Flat Shade" );
	flagFlat->setCheckable( true );

	QAction *flagFullbright = propertiesMenu->addAction( "Fullbright" );
	flagFullbright->setCheckable( true );

	propertiesMenu->addSeparator();
	propertiesMenu->addAction( "Edit All Flags..." );

	texturesMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// TEXTURE FORMAT
	// ═══════════════════════════════════════════════════════

	QMenu *formatMenu = texturesMenu->addMenu( "Texture Format" );
	QActionGroup *formatGroup = new QActionGroup( this );

	QAction *formatBMP = formatMenu->addAction( "BMP (8-bit indexed)" );
	formatBMP->setCheckable( true );
	formatBMP->setChecked( true ); // Half-Life default
	formatGroup->addAction( formatBMP );

	QAction *formatTGA = formatMenu->addAction( "TGA" );
	formatTGA->setCheckable( true );
	formatGroup->addAction( formatTGA );

	QAction *formatPNG = formatMenu->addAction( "PNG" );
	formatPNG->setCheckable( true );
	formatGroup->addAction( formatPNG );

	QAction *formatDDS = formatMenu->addAction( "DDS (Compressed)" );
	formatDDS->setCheckable( true );
	formatGroup->addAction( formatDDS );

	texturesMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// MATERIAL/SHADER OPTIONS (For advanced rendering)
	// ═══════════════════════════════════════════════════════

	QMenu *materialMenu = texturesMenu->addMenu( "Material/Shader" );

	QMenu *shaderTypeMenu = materialMenu->addMenu( "Shader Type" );
	QActionGroup *shaderGroup = new QActionGroup( this );

	QAction *shaderStandard = shaderTypeMenu->addAction( "Standard" );
	shaderStandard->setCheckable( true );
	shaderStandard->setChecked( true );
	shaderGroup->addAction( shaderStandard );

	QAction *shaderMetallic = shaderTypeMenu->addAction( "Metallic" );
	shaderMetallic->setCheckable( true );
	shaderGroup->addAction( shaderMetallic );

	QAction *shaderGlass = shaderTypeMenu->addAction( "Glass/Transparent" );
	shaderGlass->setCheckable( true );
	shaderGroup->addAction( shaderGlass );

	QAction *shaderGlow = shaderTypeMenu->addAction( "Glow/Emissive" );
	shaderGlow->setCheckable( true );
	shaderGroup->addAction( shaderGlow );

	QAction *shaderCustom = shaderTypeMenu->addAction( "Custom Shader..." );
	shaderCustom->setCheckable( true );
	shaderGroup->addAction( shaderCustom );

	materialMenu->addSeparator();

	QAction *showMaterialProps = materialMenu->addAction( "Show Material Properties" );
	showMaterialProps->setCheckable( true );

	materialMenu->addAction( "Edit Material..." );

	texturesMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// TEXTURE EDITING
	// ═══════════════════════════════════════════════════════

	texturesMenu->addAction( "Import Texture..." );
	texturesMenu->addAction( "Export Texture..." );
	texturesMenu->addAction( "Replace Texture..." );
	texturesMenu->addAction( "Edit in External Program..." );

	texturesMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// UV MAPPING
	// ═══════════════════════════════════════════════════════

	QMenu *uvMenu = texturesMenu->addMenu( "UV Mapping" );

	QAction *showUVEditor = uvMenu->addAction( "Show UV Editor" );
	showUVEditor->setCheckable( true );

	uvMenu->addSeparator();
	uvMenu->addAction( "View UV Layout..." );
	uvMenu->addAction( "Edit UV Coordinates..." );

	uvMenu->addSeparator();

	QMenu *uvTransformMenu = uvMenu->addMenu( "UV Transform" );
	uvTransformMenu->addAction( "Offset U/V..." );
	uvTransformMenu->addAction( "Scale U/V..." );
	uvTransformMenu->addAction( "Rotate UV..." );
	uvTransformMenu->addSeparator();
	uvTransformMenu->addAction( "Reset UV Transform" );

	uvMenu->addSeparator();
	uvMenu->addAction( "Optimize UVs" );
	uvMenu->addAction( "Auto-Unwrap..." );
	uvMenu->addAction( "Export UV Map Image..." );

	texturesMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// TEXTURE FILTERING
	// ═══════════════════════════════════════════════════════

	QMenu *filterTexMenu = texturesMenu->addMenu( "Filter Textures" );

	QAction *sortTexAlpha = filterTexMenu->addAction( "Sort Alphabetically" );
	sortTexAlpha->setCheckable( true );

	filterTexMenu->addSeparator();

	QMenu *filterTexTypeMenu = filterTexMenu->addMenu( "Filter by Type" );
	QAction *filterTexAll = filterTexTypeMenu->addAction( "All Textures" );
	filterTexAll->setCheckable( true );
	filterTexAll->setChecked( true );

	QAction *filterDiffuse = filterTexTypeMenu->addAction( "Diffuse Maps" );
	filterDiffuse->setCheckable( true );

	QAction *filterNormal = filterTexTypeMenu->addAction( "Normal Maps" );
	filterNormal->setCheckable( true );

	QAction *filterSpecular = filterTexTypeMenu->addAction( "Specular Maps" );
	filterSpecular->setCheckable( true );

	QAction *filterGlow = filterTexTypeMenu->addAction( "Glow Maps" );
	filterGlow->setCheckable( true );

	filterTexMenu->addSeparator();

	QAction *showTexStats = filterTexMenu->addAction( "Show Texture Statistics" );
	showTexStats->setCheckable( true );
	// Shows total memory usage, texture count, etc.

	texturesMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// MULTI-TEXTURE MANAGEMENT
	// ═══════════════════════════════════════════════════════

	QMenu *multiTexMenu = texturesMenu->addMenu( "Batch Operations" );

	QMenu *importAllMenu = multiTexMenu->addMenu( "Import All Textures" );
	importAllMenu->addAction( "From Folder..." );
	importAllMenu->addAction( "From Texture Pack..." );
	importAllMenu->addAction( "Auto-Match to Slots" );

	QMenu *exportAllMenu = multiTexMenu->addMenu( "Export All Textures" );
	exportAllMenu->addAction( "To Folder..." );
	exportAllMenu->addAction( "To Texture Pack..." );
	exportAllMenu->addAction( "Preserve Directory Structure" );

	multiTexMenu->addSeparator();
	multiTexMenu->addAction( "Replace All Textures..." );
	multiTexMenu->addAction( "Batch Resize..." );
	multiTexMenu->addAction( "Batch Convert Format..." );

	texturesMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// TEXTURE PREVIEW
	// ═══════════════════════════════════════════════════════

	QMenu *previewMenu = texturesMenu->addMenu( "Preview" );

	QMenu *previewSizeMenu = previewMenu->addMenu( "Preview Size" );
	QActionGroup *previewSizeGroup = new QActionGroup( this );

	QAction *previewSmall = previewSizeMenu->addAction( "Small (64x64)" );
	previewSmall->setCheckable( true );
	previewSizeGroup->addAction( previewSmall );

	QAction *previewMedium = previewSizeMenu->addAction( "Medium (256x256)" );
	previewMedium->setCheckable( true );
	previewMedium->setChecked( true );
	previewSizeGroup->addAction( previewMedium );

	QAction *previewLarge = previewSizeMenu->addAction( "Large (512x512)" );
	previewLarge->setCheckable( true );
	previewSizeGroup->addAction( previewLarge );

	previewMenu->addSeparator();

	QMenu *previewModeMenu = previewMenu->addMenu( "Preview Mode" );
	QActionGroup *previewModeGroup = new QActionGroup( this );

	QAction *previewTexOnly = previewModeMenu->addAction( "Texture Only" );
	previewTexOnly->setCheckable( true );
	previewModeGroup->addAction( previewTexOnly );

	QAction *previewModel = previewModeMenu->addAction( "Model with Texture" );
	previewModel->setCheckable( true );
	previewModel->setChecked( true );
	previewModeGroup->addAction( previewModel );

	QAction *previewWireframe = previewModeMenu->addAction( "Wireframe + Texture" );
	previewWireframe->setCheckable( true );
	previewModeGroup->addAction( previewWireframe );

	QAction *previewSideBySide = previewModeMenu->addAction( "Side-by-Side" );
	previewSideBySide->setCheckable( true );
	previewModeGroup->addAction( previewSideBySide );

	texturesMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// ADVANCED OPTIONS
	// ═══════════════════════════════════════════════════════

	QMenu *advancedMenu = texturesMenu->addMenu( "Advanced" );

	QMenu *compressionMenu = advancedMenu->addMenu( "Texture Compression" );
	QActionGroup *compressionGroup = new QActionGroup( this );

	QAction *compressNone = compressionMenu->addAction( "No Compression" );
	compressNone->setCheckable( true );
	compressNone->setChecked( true );
	compressionGroup->addAction( compressNone );

	QAction *compressDXT1 = compressionMenu->addAction( "DXT1 (BC1)" );
	compressDXT1->setCheckable( true );
	compressionGroup->addAction( compressDXT1 );

	QAction *compressDXT5 = compressionMenu->addAction( "DXT5 (BC3)" );
	compressDXT5->setCheckable( true );
	compressionGroup->addAction( compressDXT5 );

	QAction *compressAuto = compressionMenu->addAction( "Auto-Select" );
	compressAuto->setCheckable( true );
	compressionGroup->addAction( compressAuto );

	advancedMenu->addSeparator();

	QMenu *mipmapMenu = advancedMenu->addMenu( "Mipmap Settings" );
	QAction *genMipmaps = mipmapMenu->addAction( "Generate Mipmaps" );
	genMipmaps->setCheckable( true );

	mipmapMenu->addSeparator();
	mipmapMenu->addAction( "Mipmap Count..." );
	mipmapMenu->addAction( "Mipmap Filter Type..." );

	advancedMenu->addSeparator();

	QMenu *scaleMenu = advancedMenu->addMenu( "Texture Scaling" );
	scaleMenu->addAction( "Scale 50%" );
	scaleMenu->addAction( "Scale 100% (Original)" );
	scaleMenu->addAction( "Scale 200%" );
	scaleMenu->addSeparator();
	scaleMenu->addAction( "Custom Resolution..." );

	texturesMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// TEXTURE INFORMATION
	// ═══════════════════════════════════════════════════════

	texturesMenu->addAction( "List All Textures..." );
	texturesMenu->addAction( "Texture Info..." );
	texturesMenu->addAction( "Show Memory Usage..." );
}

void MainWindow::createFileMenu() {
	QMenu *fileMenu = menuBar()->addMenu( tr( "&File" ) );

	// ==================================
	// OPEN SECTION
	// ==================================

	// New Tab - Opens a new empty tab for loading another model
	QAction *newTabAction = fileMenu->addAction( "New Tab" );
	newTabAction->setShortcut( QKeySequence( "Ctrl+T" ) );
	connect( newTabAction, &QAction::triggered, this, &MainWindow::onNewTab );

	fileMenu->addSeparator();

	QAction *openAction = fileMenu->addAction( "Open Model" );
	openAction->setShortcut( QKeySequence::Open ); // Ctrl+O
	connect( openAction, &QAction::triggered, this, &MainWindow::onOpenModel );
	QMenu *recentMenu = fileMenu->addMenu( "Open recent" );
	recentMenu->addAction( "barney.mdl" );
	recentMenu->addAction( "player.mdl" );
	recentMenu->addAction( "scientist.mdl" );

	fileMenu->addAction( "Open project (.pak)" );

	fileMenu->addSeparator();

	// ==================================
	// CLOSE SECTION
	// ==================================

	fileMenu->addAction( "Close Model" );
	fileMenu->addAction( "Close Window" );
	fileMenu->addAction( "New Window" );

	fileMenu->addSeparator();

	// ==================================
	// SAVE SECTION
	// ==================================
	fileMenu->addAction( "Save Model" );
	fileMenu->addAction( "Save Copy..." );

	fileMenu->addSeparator();

	// ==================================
	// RELOAD SECTION
	// ==================================

	fileMenu->addAction( "Reload Model" );
	fileMenu->addAction( "Reload Textures" );

	fileMenu->addSeparator();

	// ==================================
	// IMPORT/EXPORT SECTION
	// ==================================

	QMenu *importMenu = fileMenu->addMenu( "Import" );
	importMenu->addAction( "Import SMD Reference" );
	importMenu->addAction( "Import SMD Sequence" );
	importMenu->addAction( "Import OBJ Mesh..." );
	importMenu->addAction( "Import Textures..." );
	importMenu->addAction( "Import QC Script..." );

	QMenu *exportMenu = fileMenu->addMenu( "Export" );
	exportMenu->addAction( "Export QC Script..." );
	exportMenu->addAction( "Export SMD (Reference)" );
	exportMenu->addAction( "Export SMD (Sequence)" );
	exportMenu->addAction( "Export SMD (All Sequences)" );
	exportMenu->addAction( "Export OBJ Mesh..." );
	exportMenu->addAction( "Export All Textures" );

	fileMenu->addSeparator();

	// ==================================
	// PROPERTIES SECTION
	// ==================================

	// @Note(Karlo): this will go to the ModelMenuBar when I add it, because it makes more sense there
	fileMenu->addAction( "Model Properties..." );

	fileMenu->addSeparator();

	// ==================================
	// EXIT SECTION
	// ==================================

	QAction *actionExit = fileMenu->addAction( "Exit" );
	connect( actionExit, &QAction::triggered, qApp, &QApplication::exit );
}
void MainWindow::createEditMenu() {
	QMenu *editMenu = menuBar()->addMenu( tr( "&Edit" ) );

	// @Note: We will grey it out for starters
	QAction *actionUndo = editMenu->addAction( "Undo" );
	actionUndo->setEnabled( false );

	QAction *actionRedo = editMenu->addAction( "Redo" );
	actionRedo->setEnabled( false );

	editMenu->addAction( "History..." );
	editMenu->addSeparator();

	// ==================================
	// CLIPOARD
	// ==================================

	editMenu->addAction( "Cut" );
	editMenu->addAction( "Copy" );
	editMenu->addAction( "Paste" );
	editMenu->addAction( "Duplicate" );
	editMenu->addAction( "Delete" );

	editMenu->addSeparator();

	// ==================================
	// TRANSFORM
	// ==================================

	QMenu *transformMenu = editMenu->addMenu( "Transform" );
	transformMenu->addAction( "Scale Model..." );
	transformMenu->addAction( "Scale Uniformly..." );
	transformMenu->addAction( "Scale Bones..." );
	transformMenu->addSeparator();
	transformMenu->addAction( "Rotate Model..." );
	transformMenu->addAction( "Rotate 90° CW" );
	transformMenu->addAction( "Rotate 90° CCW" );
	transformMenu->addAction( "Rotate 180°" );
	transformMenu->addSeparator();
	transformMenu->addAction( "Flip Horizontally" );
	transformMenu->addAction( "Flip Vertically" );
	transformMenu->addSeparator();
	transformMenu->addAction( "Translate Model..." );

	// ==================================
	// MESH OPERATIONS
	// ==================================

	QMenu *meshMenu = editMenu->addMenu( "Mesh Operations" );
	meshMenu->addAction( "Optimize Mesh..." );
	meshMenu->addAction( "Smooth Normals..." );
	meshMenu->addAction( "Recalculate Normals" );
	meshMenu->addAction( "Triangulate" );
	meshMenu->addAction( "Merge Vertices..." );
	meshMenu->addAction( "Remove Duplicates" );

	// ==================================
	// RESET
	// ==================================

	editMenu->addAction( "Reset Pose" );
	editMenu->addAction( "Reset Camera" );
	editMenu->addAction( "Reset All Transforms" );

	editMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// SELECTION
	// ═══════════════════════════════════════════════════════

	editMenu->addAction( "Select All" );
	editMenu->addAction( "Select None" );
	editMenu->addAction( "Invert Selection" );
}
void MainWindow::createViewMenu() {
	QMenu *viewMenu = menuBar()->addMenu( tr( "&View" ) );

	// ═══════════════════════════════════════════════════════
	// RENDER MODE
	// ═══════════════════════════════════════════════════════

	QMenu *renderModeMenu = viewMenu->addMenu( "Render Mode" );
	QActionGroup *renderModeGroup = new QActionGroup( this );

	QAction *actionWireframe = renderModeMenu->addAction( "Wireframe" );
	actionWireframe->setCheckable( true );
	renderModeGroup->addAction( actionWireframe );

	QAction *actionFlat = renderModeMenu->addAction( "Flat Shaded" );
	actionFlat->setCheckable( true );
	renderModeGroup->addAction( actionFlat );

	QAction *actionSmooth = renderModeMenu->addAction( "Smooth Shaded" );
	actionSmooth->setCheckable( true );
	renderModeGroup->addAction( actionSmooth );

	QAction *actionTextured = renderModeMenu->addAction( "Textured" );
	actionTextured->setCheckable( true );
	actionTextured->setChecked( true );
	renderModeGroup->addAction( actionTextured );

	QAction *actionTexturedWireframe = renderModeMenu->addAction( "Textured & Wireframe" );
	actionTexturedWireframe->setCheckable( true );
	renderModeGroup->addAction( actionTexturedWireframe );

	QAction *actionFullbright = renderModeMenu->addAction( "Fullbright" );
	actionFullbright->setCheckable( true );
	renderModeGroup->addAction( actionFullbright );

	viewMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// SHADING OPTIONS
	// ═══════════════════════════════════════════════════════

	QMenu *shadingMenu = viewMenu->addMenu( "Shading" );
	QAction *actionSmoothShading = shadingMenu->addAction( "Smooth Shading" );

	actionSmoothShading->setCheckable( true );
	actionSmoothShading->setChecked( true );

	QAction *actionBackfaceCull = shadingMenu->addAction( "Backface Culling" );
	actionBackfaceCull->setCheckable( true );
	actionBackfaceCull->setChecked( true );

	QAction *actionTwoSidedLight = shadingMenu->addAction( "Two-Sided Lighting" );
	actionTwoSidedLight->setCheckable( true );

	QAction *actionVertexColors = shadingMenu->addAction( "Vertex Colors" );
	actionVertexColors->setCheckable( true );

	viewMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// SHOW SUBMENU ( Visibility Toggling )
	// ═══════════════════════════════════════════════════════

	QMenu *showMenu = viewMenu->addMenu( "Show" );

	QAction *actionBones = showMenu->addAction( "Show Bones" );
	actionBones->setCheckable( true );

	QAction *actionBoneNames = showMenu->addAction( "Bone Names" );
	actionBoneNames->setCheckable( true );

	QAction *actionHitboxes = showMenu->addAction( "Show Hitboxes" );
	actionHitboxes->setCheckable( true );

	QAction *actionHitboxesNames = showMenu->addAction( "Hitboxes Names" );
	actionHitboxesNames->setCheckable( true );

	QAction *actionAttachments = showMenu->addAction( "Show Attachments" );
	actionAttachments->setCheckable( true );

	QAction *actionAttachmentNames = showMenu->addAction( "Attachment Names" );
	actionAttachmentNames->setCheckable( true );

	showMenu->addSeparator();

	QAction *actionNormals = showMenu->addAction( "Normals" );
	actionNormals->setCheckable( true );

	QAction *actionTangents = showMenu->addAction( "Tangents" );
	actionTangents->setCheckable( true );

	QAction *actionUV = showMenu->addAction( "UV Coordinates" );
	actionUV->setCheckable( true );

	QAction *actionVertexID = showMenu->addAction( "Vertex IDs" );
	actionVertexID->setCheckable( true );

	showMenu->addSeparator();

	QAction *actionGround = showMenu->addAction( "Ground" );
	actionGround->setCheckable( true );
	actionGround->setChecked( true );

	QAction *actionGroundReflect = showMenu->addAction( "Ground Reflection" );
	actionGroundReflect->setCheckable( true );

	QAction *actionGrid = showMenu->addAction( "Grid" );
	actionGrid->setCheckable( true );
	actionGrid->setChecked( true );

	QAction *actionAxis = showMenu->addAction( "Axis" );
	actionAxis->setCheckable( true );

	QAction *actionOrigin = showMenu->addAction( "Origin" );
	actionOrigin->setCheckable( true );

	showMenu->addSeparator();

	QAction *actionBBox = showMenu->addAction( "Bounding Box" );
	actionBBox->setCheckable( true );

	QAction *actionCollisionHull = showMenu->addAction( "Collision Hull" );
	actionCollisionHull->setCheckable( true );

	QAction *showSeqBBox = showMenu->addAction( "Sequence Bounding Box" );
	showSeqBBox->setCheckable( true );

	QAction *showEyePos = showMenu->addAction( "Eye Position" );
	showEyePos->setCheckable( true );

	viewMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// VIEWPORT LAYOUT
	// ═══════════════════════════════════════════════════════

	QMenu *layoutMenu = viewMenu->addMenu( "Viewport Layout" );
	QActionGroup *layoutGroup = new QActionGroup( this );

	QAction *actionLayoutSingle = layoutMenu->addAction( "Single View" );
	actionLayoutSingle->setCheckable( true );
	actionLayoutSingle->setChecked( true );
	layoutGroup->addAction( actionLayoutSingle );

	QAction *actionLayoutDualH = layoutMenu->addAction( "Dual View (Horizontally)" );
	actionLayoutDualH->setCheckable( true );
	layoutGroup->addAction( actionLayoutDualH );

	QAction *layoutDualV = layoutMenu->addAction( "Dual View (Vertical)" );
	layoutDualV->setCheckable( true );
	layoutGroup->addAction( layoutDualV );

	QAction *layoutQuad = layoutMenu->addAction( "Quad View" );
	layoutQuad->setCheckable( true );
	layoutGroup->addAction( layoutQuad );

	viewMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// CAMERA
	// ═══════════════════════════════════════════════════════

	QMenu *cameraMenu = viewMenu->addMenu( "Camera" );
	QActionGroup *cameraProjGroup = new QActionGroup( this );

	QAction *actionCameraPerspective = cameraMenu->addAction( "Perspective" );
	actionCameraPerspective->setCheckable( true );
	actionCameraPerspective->setChecked( true );
	cameraProjGroup->addAction( actionCameraPerspective );

	QAction *actionCameraOrtho = cameraMenu->addAction( "Orthographic" );
	actionCameraOrtho->setCheckable( true );
	cameraProjGroup->addAction( actionCameraOrtho );

	cameraMenu->addSeparator();

	cameraMenu->addAction( "Front View" );
	cameraMenu->addAction( "Back View" );
	cameraMenu->addAction( "Right View" );
	cameraMenu->addAction( "Left View" );
	cameraMenu->addAction( "Top View" );
	cameraMenu->addAction( "Bottom View" );

	cameraMenu->addSeparator();

	cameraMenu->addAction( "Frame Selected" );
	cameraMenu->addAction( "Frame All" );
	cameraMenu->addAction( "Reset View" );

	viewMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// GRID SIZE
	// ═══════════════════════════════════════════════════════

	QMenu *gridSizeMenu = viewMenu->addMenu( "Grid Size" );
	QActionGroup *gridSizeGroup = new QActionGroup( this );

	QStringList gridSizes = { "1", "2", "4", "8", "16", "32", "64", "128", "256", "512" };
	for ( const QString &size : gridSizes ) {
		QAction *gridAction = gridSizeMenu->addAction( size + " units" );
		gridAction->setCheckable( true );
		if ( size == "64" ) {
			gridAction->setChecked( true );
		}
		gridSizeGroup->addAction( gridAction );
	}

	gridSizeMenu->addSeparator();
	gridSizeMenu->addAction( "Custom..." );

	viewMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// COLORS
	// ═══════════════════════════════════════════════════════

	viewMenu->addAction( "Background Color..." );
	viewMenu->addAction( "Grid Color..." );
	viewMenu->addAction( "Ground Color..." );

	viewMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// SCREENSHOTS
	// ═══════════════════════════════════════════════════════

	QMenu *screenshotMenu = viewMenu->addMenu( "Screenshot" );
	screenshotMenu->addAction( "Capture Viewport" );
	screenshotMenu->addAction( "Capture Viewport (4K)" );
	screenshotMenu->addAction( "Capture All Sequences" );
	screenshotMenu->addAction( "Capture Thumbnail..." );

	viewMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// FULLSCREEN
	// ═══════════════════════════════════════════════════════

	QMenu *screenMenu = viewMenu->addMenu( "Screen" );
	QAction *actionFullscreen = screenMenu->addAction( "Fullscreen" );
	actionFullscreen->setCheckable( true );

	QAction *actionToggleUI = screenMenu->addAction( "Toggle UI" );
	actionToggleUI->setCheckable( true );
	actionToggleUI->setChecked( true );

	QAction *actionToggleConsole = screenMenu->addAction( "Toggle Console" );
	actionToggleConsole->setCheckable( true );

	QAction *actionToggleSidePanel = screenMenu->addAction( "Toggle Inspector Panel" );
	actionToggleSidePanel->setCheckable( true );

	QAction *actionToggleStatusBar = screenMenu->addAction( "Toggle Status Bar" );
	actionToggleStatusBar->setCheckable( true );
}
void MainWindow::createToolsMenu() {
	QMenu *toolsMenu = menuBar()->addMenu( "&Tools" );

	toolsMenu->addAction( "Compile Model..." );
	toolsMenu->addAction( "Decompile Model..." );
	toolsMenu->addAction( "Generate Thumbnails..." );
	toolsMenu->addAction( "Validate Model..." );
}
void MainWindow::createDebugMenu() {
	// TODO: Need to be filled properly, for now empty

	QMenu *debugMenu = menuBar()->addMenu( tr( "&Debug" ) );

	// ═══════════════════════════════════════════════════════
	// PERFORMANCE MONITORING
	// ═══════════════════════════════════════════════════════

	QAction *showFPS = debugMenu->addAction( "Show FPS" );
	showFPS->setCheckable( true );

	QAction *showMemory = debugMenu->addAction( "Show Memory" );
	showMemory->setCheckable( true );

	QAction *showRenderStats = debugMenu->addAction( "Show Render Stats" );
	showRenderStats->setCheckable( true );

	debugMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// DEBUGGING TOOLS
	// ═══════════════════════════════════════════════════════

	debugMenu->addAction( "Print Model Info to Console..." );
	debugMenu->addAction( "Dump Vertices to File..." );
	debugMenu->addAction( "Dump Bones to File..." );
	debugMenu->addAction( "Dump Sequence to File..." );
	debugMenu->addAction( "Export Debug Log..." );

	debugMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// OPENGL DEBUG
	// ═══════════════════════════════════════════════════════

	QMenu *glDebugMenu = debugMenu->addMenu( "OpenGL Debug" );

	QAction *showGLInfo = glDebugMenu->addAction( "Show GL Info" );
	showGLInfo->setCheckable( true );

	QAction *wireframeOverlay = glDebugMenu->addAction( "Wireframe Overlay" );
	wireframeOverlay->setCheckable( true );

	QAction *showDrawCalls = glDebugMenu->addAction( "Show Draw Calls" );
	showDrawCalls->setCheckable( true );

	QAction *showBufferStats = glDebugMenu->addAction( "Show Buffer Stats" );
	showBufferStats->setCheckable( true );

	glDebugMenu->addSeparator();
	glDebugMenu->addAction( "Clear GL Errors" );
	glDebugMenu->addAction( "Force GL Sync" );
	glDebugMenu->addAction( "Recreate GL Context" );

	debugMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// MODEL VALIDATION & TESTING
	// ═══════════════════════════════════════════════════════

	QMenu *validateMenu = debugMenu->addMenu( "Validation" );

	validateMenu->addAction( "Validate Bone Hierarchy" );
	validateMenu->addAction( "Validate Sequences" );
	validateMenu->addAction( "Validate Textures" );
	validateMenu->addAction( "Validate Hitboxes" );
	validateMenu->addAction( "Validate Attachments" );
	validateMenu->addSeparator();
	validateMenu->addAction( "Run All Validation Checks..." );

	debugMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// LOGGER SETTINGS (IMPORTANT FOR USER NEEDS!)
	// ═══════════════════════════════════════════════════════

	QMenu *logLevelMenu = debugMenu->addMenu( "Log Level" );
	QActionGroup *logLevelGroup = new QActionGroup( this );

	QAction *logError = logLevelMenu->addAction( "Error Only" );
	logError->setCheckable( true );
	logLevelGroup->addAction( logError );

	QAction *logWarning = logLevelMenu->addAction( "Warning" );
	logWarning->setCheckable( true );
	logLevelGroup->addAction( logWarning );

	QAction *logInfo = logLevelMenu->addAction( "Info" );
	logInfo->setCheckable( true );
	logLevelGroup->addAction( logInfo );

	QAction *logDebug = logLevelMenu->addAction( "Debug" );
	logDebug->setCheckable( true );
	logLevelGroup->addAction( logDebug );

	QAction *logVerbose = logLevelMenu->addAction( "Verbose" );
	logVerbose->setCheckable( true );
	logLevelGroup->addAction( logVerbose );

	// ═══════════════════════════════════════════════════════
	// CONSOLE ACTIONS
	// ═══════════════════════════════════════════════════════

	debugMenu->addAction( "Clear Console" );
	debugMenu->addAction( "Copy Console to Clipboard" );
	debugMenu->addAction( "Save Console to File..." );
	debugMenu->addAction( "Open Log File Location..." );

	debugMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// PROFILING
	// ═══════════════════════════════════════════════════════

	QMenu *profilingMenu = debugMenu->addMenu( "Profiling" );

	QAction *enableProfiling = profilingMenu->addAction( "Profiling" );

	enableProfiling->setCheckable( true );

	profilingMenu->addSeparator();
	profilingMenu->addAction( "Show Profiling Results..." );
	profilingMenu->addAction( "Reset Profiling Results..." );
	profilingMenu->addAction( "Export Profiling Report..." );

	debugMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// CRASH & ERROR HANDLING
	// ═══════════════════════════════════════════════════════

	QMenu *crashMenu = debugMenu->addMenu( "Crash Handling" );

	QAction *enableCrashDump = crashMenu->addAction( "Enable Crash Dumps" );
	enableCrashDump->setCheckable( true );
	enableCrashDump->setChecked( true );

	QAction *enableAutoSave = crashMenu->addAction( "Enable Auto-Save" );
	enableAutoSave->setCheckable( true );
	enableAutoSave->setChecked( true );

	crashMenu->addSeparator();
	crashMenu->addAction( "Open Crash Logs Folder..." );
	crashMenu->addAction( "Send Crash Report..." );

	debugMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// ADVANCED DEBUG
	// ═══════════════════════════════════════════════════════

	QAction *showMemAddresses = debugMenu->addAction( "Show Memory Addresses" );
	showMemAddresses->setCheckable( true );

	QAction *enableAsserts = debugMenu->addAction( "Enable Runtime Assertions" );
	enableAsserts->setCheckable( true );
	enableAsserts->setChecked( true );

	debugMenu->addSeparator();

	// ═══════════════════════════════════════════════════════
	// TESTING
	// ═══════════════════════════════════════════════════════

	debugMenu->addAction( "Run Unit Tests..." );
	debugMenu->addAction( "Stress Test Renderer..." );
	debugMenu->addAction( "Test All Sequences..." );
}

void MainWindow::createWindowMenu() {
	QMenu *winMenu = menuBar()->addMenu( "&Window" );

	// Tab Management
	QAction *newTabAction = winMenu->addAction( "New Tab" );
	newTabAction->setShortcut( QKeySequence( "Ctrl+T" ) );
	connect( newTabAction, &QAction::triggered, this, &MainWindow::onNewTab );

	winMenu->addSeparator();

	// Panels
	winMenu->addAction( "Texture Browser" );
	winMenu->addAction( "Model Explorer" );
	winMenu->addAction( "Sequence Viewer" );
	winMenu->addAction( "Inspector" );

	winMenu->addSeparator();
	winMenu->addAction( "Reset Layout" );
}
void MainWindow::createHelpMenu() {
	QMenu *helpMenu = menuBar()->addMenu( "&Help" );

	helpMenu->addAction( "Documentation" );
	helpMenu->addAction( "Report Issue" );
	helpMenu->addAction( "About Lambda MDL Editor" );
}

void MainWindow::createViewportContainer() {
	// Create container widget with frame for viewport panel
	QWidget *viewportContainer = new QWidget( this );
	QVBoxLayout *containerLayout = new QVBoxLayout( viewportContainer );
	containerLayout->setContentsMargins( 4, 4, 4, 4 );
	containerLayout->setSpacing( 0 );

	// Apply frame style to container (raised 3D border)
	viewportContainer->setStyleSheet(
		"QWidget { "
		"    background-color: #c0c0c0; "
		"    border: 2px solid; "
		"    border-top-color: #ffffff; "
		"    border-left-color: #ffffff; "
		"    border-right-color: #808080; "
		"    border-bottom-color: #808080; "
		"}" );

	// Create tab widget for multi-model support
	tabWidget = new QTabWidget( viewportContainer );
	tabWidget->setTabsClosable( true ); // Enable close buttons
	tabWidget->setMovable( true ); // Allow tab reordering
	tabWidget->setDocumentMode( false ); // Keep traditional tabs for classic look

	// Apply classic tab styling with better X close buttons
	// Compact tabs like BSP editor
	QString tabStyle =
		"QTabWidget::pane { "
		"    border: 1px solid; "
		"    border-top-color: #808080; "
		"    border-left-color: #808080; "
		"    border-right-color: #ffffff; "
		"    border-bottom-color: #ffffff; "
		"    background-color: #c0c0c0; "
		"    margin: 0px; "
		"} "
		"QTabBar::tab { "
		"    background-color: #c0c0c0; "
		"    border: 1px solid; "
		"    border-top-color: #ffffff; "
		"    border-left-color: #ffffff; "
		"    border-right-color: #808080; "
		"    border-bottom: none; "
		"    padding: 2px 14px 2px 6px; "
		"    margin-right: 1px; "
		"    color: #000000; "
		"    font-size: 10px; "
		"    min-width: 50px; "
		"    max-width: 150px; "
		"    min-height: 16px; "
		"    max-height: 16px; "
		"} "
		"QTabBar::tab:first { "
		"    padding: 2px 14px 2px 6px; "
		"    min-width: 50px; "
		"    min-height: 16px; "
		"    max-height: 16px; "
		"} "
		"QTabBar::tab:selected { "
		"    background-color: #0a246a; "
		"    border-top-color: #0d2d85; "
		"    border-left-color: #0d2d85; "
		"    border-right-color: #081d55; "
		"    border-bottom: 1px solid #0a246a; "
		"    color: #ffffff; "
		"    font-weight: bold; "
		"} "
		"QTabBar::tab:hover { "
		"    background-color: #a8a8a8; "
		"} "
		"QTabBar::close-button { "
		"    subcontrol-position: center right; "
		"    width: 10px; "
		"    height: 10px; "
		"    margin-right: 2px; "
		"    background-color: transparent; "
		"    border: none; "
		"    image: url(data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMTAiIGhlaWdodD0iMTAiPjxwYXRoIGQ9Ik0yLDIgTDgsOCBNMiw4IEw4LDIiIHN0cm9rZT0iIzYwNjA2MCIgc3Ryb2tlLXdpZHRoPSIxLjUiIGZpbGw9Im5vbmUiLz48L3N2Zz4=); "
		"} "
		"QTabBar::close-button:hover { "
		"    background-color: #d04040; "
		"    border: 1px solid #a03030; "
		"    image: url(data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMTAiIGhlaWdodD0iMTAiPjxwYXRoIGQ9Ik0yLDIgTDgsOCBNMiw4IEw4LDIiIHN0cm9rZT0iI2ZmZmZmZiIgc3Ryb2tlLXdpZHRoPSIxLjUiIGZpbGw9Im5vbmUiLz48L3N2Zz4=); "
		"}";

	tabWidget->setStyleSheet( tabStyle );

	// Connect tab close signal
	connect( tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::onCloseTab );

	// Connect tab switch signal (fixes multi-tab rendering bug)
	connect( tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged );

	// Add tab widget to container layout
	containerLayout->addWidget( tabWidget );

	// Set viewport container as central widget (not just tab widget)
	setCentralWidget( viewportContainer );

	// Create initial tab with empty viewport
	addViewportTab( "Untitled" );

	// Remove close button from first tab completely (first tab always stays open)
	tabWidget->tabBar()->setTabButton( 0, QTabBar::RightSide, nullptr );
	tabWidget->tabBar()->setTabButton( 0, QTabBar::LeftSide, nullptr );
}

void MainWindow::createDocks() {
	// @Note: Adding the right panel, this is teh main inspector panel, loads anything
	//        from tree view of the proejct/.pak, to sequences, animations, model info, model
	//        and so forth. It has to be broken into multiple different panels

	rightDock = new QDockWidget( "Inspector", this );
	rightDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

	QWidget *inspectorPanel = new QWidget();
	// TODO: Here we can add more panels they will all be active and necessary
	//       Will decide going on how and what is necessary to add

	QVBoxLayout *inspLayout = new QVBoxLayout( inspectorPanel );

	inspLayout->addWidget( new QLabel( "Inspector Panel Placeholder, will fill more panels" ) );
	inspectorPanel->setLayout( inspLayout );
	inspectorPanel->setMinimumWidth( MW_RIGHT_DOCK_MIN_WIDTH );
	inspectorPanel->setMaximumWidth( MW_RIGHT_DOCK_MAX_WIDTH );

	// Add inner frame to inspector panel
	inspectorPanel->setStyleSheet(
		"QWidget { "
		"    background-color: #505050; "
		"    border: 2px solid; "
		"    border-top-color: #808080; "
		"    border-left-color: #808080; "
		"    border-right-color: #f0f0f0; "
		"    border-bottom-color: #f0f0f0; "
		"} "
		"QLabel { "
		"    color: #c0c0c0; "
		"    border: none; "
		"}" );

	rightDock->setWidget( inspectorPanel );
	addDockWidget( Qt::RightDockWidgetArea, rightDock );

	// @Note: Adding now the bottom panel dock, main console for logger messages and so forth
	//        Also need to add for future the dump files so seperate panels for animations
	//        bones, vertices, hitboxes, everything basically right, and of course adding issues or something else

	// TODO: In the future will decide what else to add to he bottom dock as panels
	//       For now just the console, but would be good to add memory dump or something,
	//       or hex dump to be able to see the HEX view of the MDL file and whatnot

	// @Note: Create bottom dock with Console and Memory tabs
	bottomDock = new QDockWidget( "Output", this );
	bottomDock->setAllowedAreas( Qt::BottomDockWidgetArea );
	bottomDock->setObjectName( "BottomDock" );
	bottomDock->setMinimumHeight( MW_CONSOLE_MIN_HEIGHT );
	bottomDock->setMaximumHeight( MW_CONSOLE_MAX_HEIGHT );

	// Create tab widget for Console and Memory
	QTabWidget *bottomTabs = new QTabWidget();
	bottomTabs->setDocumentMode( false ); // Classic tabs

	// Console panel - simple output area (no log filtering)
	QWidget *consolePanel = new QWidget();
	QVBoxLayout *consoleLayout = new QVBoxLayout( consolePanel );
	consoleLayout->setContentsMargins( 4, 4, 4, 4 );
	consoleLayout->setSpacing( 0 );

	// Console content area - placeholder for now
	QLabel *consoleLabel = new QLabel( "Console output will appear here..." );
	consoleLabel->setAlignment( Qt::AlignTop | Qt::AlignLeft );

	consoleLayout->addWidget( consoleLabel, 1 );
	consolePanel->setStyleSheet(
		"QWidget { "
		"    background-color: #353535; "
		"} "
		"QLabel { "
		"    color: #c0c0c0; "
		"    border: none; "
		"    padding: 4px; "
		"}" );

	// Memory panel
	QWidget *memoryPanel = new QWidget();
	QVBoxLayout *memoryLayout = new QVBoxLayout( memoryPanel );
	QLabel *memoryLabel = new QLabel( "Memory Panel - Hex Dump and Structure View" );
	memoryLabel->setStyleSheet( "QLabel { color: #c0c0c0; }" );
	memoryLayout->addWidget( memoryLabel );
	memoryPanel->setStyleSheet(
		"QWidget { "
		"    background-color: #353535; "
		"} "
		"QLabel { "
		"    color: #c0c0c0; "
		"    border: none; "
		"}" );

	// Add tabs
	bottomTabs->addTab( consolePanel, "Console" );
	bottomTabs->addTab( memoryPanel, "Memory" );

	// Style the tabs to match window tabs (classic with blue active)
	QString bottomTabStyle =
		"QTabWidget::pane { "
		"    border: 2px solid; "
		"    border-top-color: #808080; "
		"    border-left-color: #808080; "
		"    border-right-color: #ffffff; "
		"    border-bottom-color: #ffffff; "
		"    background-color: #353535; "
		"} "
		"QTabBar::tab { "
		"    background-color: #c0c0c0; "
		"    border: 2px solid; "
		"    border-top-color: #ffffff; "
		"    border-left-color: #ffffff; "
		"    border-right-color: #808080; "
		"    border-bottom: none; "
		"    padding: 4px 12px; "
		"    margin-right: 2px; "
		"    color: #000000; "
		"    min-width: 80px; "
		"} "
		"QTabBar::tab:selected { "
		"    background-color: #0a246a; " // Classic blue for active tab
		"    border-top-color: #0d2d85; "
		"    border-left-color: #0d2d85; "
		"    border-right-color: #081d55; "
		"    border-bottom: 2px solid #0a246a; "
		"    color: #ffffff; "
		"    font-weight: bold; "
		"} "
		"QTabBar::tab:hover { "
		"    background-color: #a8a8a8; "
		"}";

	bottomTabs->setStyleSheet( bottomTabStyle );

	bottomDock->setWidget( bottomTabs );
	addDockWidget( Qt::BottomDockWidgetArea, bottomDock );

	// Give right dock (Inspector) the corners so it spans full height
	// Console (bottom dock) will stop at the Inspector
	setCorner( Qt::TopRightCorner, Qt::RightDockWidgetArea );
	setCorner( Qt::BottomRightCorner, Qt::RightDockWidgetArea );

	// Make the docks non deattachable
	rightDock->setFeatures( QDockWidget::NoDockWidgetFeatures );
	bottomDock->setFeatures( QDockWidget::NoDockWidgetFeatures );

	// Remove title bar from inspector and console for cleaner look
	rightDock->setTitleBarWidget( new QWidget() );
	bottomDock->setTitleBarWidget( new QWidget() );

	// Apply classic dock styling with visible borders for separation
	QString dockStyle =
		"QDockWidget { "
		"    background-color: #c0c0c0; "
		"    border: 3px solid; "
		"    border-top-color: #ffffff; "
		"    border-left-color: #ffffff; "
		"    border-right-color: #808080; "
		"    border-bottom-color: #808080; "
		"} "
		"QDockWidget::title { "
		"    background-color: #0a246a; "
		"    color: #ffffff; "
		"    padding: 4px 6px; "
		"    text-align: left; "
		"    font-weight: bold; "
		"} "
		"QDockWidget > QWidget { "
		"    border: 2px solid; "
		"    border-top-color: #808080; "
		"    border-left-color: #808080; "
		"    border-right-color: #f0f0f0; "
		"    border-bottom-color: #f0f0f0; "
		"    background-color: #353535; "
		"}";

	// Inspector dock with prominent border
	QString inspectorStyle = dockStyle + "QDockWidget > QWidget { "
										 "    background-color: #505050; "
										 "}";

	rightDock->setStyleSheet( inspectorStyle );
	bottomDock->setStyleSheet( dockStyle );

	// Add spacing between docks and central widget
	setDockNestingEnabled( false ); // Cleaner separation

	// Set initial dock sizes - inspector wider for tab panels, console taller for logs
	resizeDocks( { rightDock }, { 600 }, Qt::Horizontal );
	resizeDocks( { bottomDock }, { 250 }, Qt::Vertical );

	// Create status bar widget
	m_statusBar = new StatusBarWidget( this );
	setStatusBar( m_statusBar );

	// Connect status bar signals to dock visibility
	connect( m_statusBar, &StatusBarWidget::inspectorToggleRequested, this, [this]() {
		bool visible = !rightDock->isVisible();
		rightDock->setVisible( visible );
		m_statusBar->setInspectorVisible( visible );
	} );

	connect( m_statusBar, &StatusBarWidget::consoleToggleRequested, this, [this, bottomTabs]() {
		bool visible = !bottomDock->isVisible();
		bottomDock->setVisible( visible );
		if ( visible ) {
			bottomTabs->setCurrentIndex( 0 ); // Switch to Console tab
		}
		m_statusBar->setConsoleVisible( visible );
	} );

	connect( m_statusBar, &StatusBarWidget::memoryToggleRequested, this, [this, bottomTabs]() {
		bool visible = !bottomDock->isVisible();
		bottomDock->setVisible( visible );
		if ( visible ) {
			bottomTabs->setCurrentIndex( 1 ); // Switch to Memory tab
		}
		m_statusBar->setMemoryVisible( visible );
	} );
}

void MainWindow::setupTheme( void ) {
	// here we will add the app reference instead of in the main
}

void MainWindow::setupViewports( void ) {
	createViewportContainer();
}

void MainWindow::setupDocks( void ) {
	createDocks();
}

void MainWindow::setupToolbars( void ) {
	createToolbarUpper();
}

void MainWindow::onOpenModel() {
	// SAFETY: Ensure tab widget exists
	if ( !tabWidget ) {
		qCritical() << "ERROR: Tab widget is null!";
		return;
	}

	// Get current viewport (or create new tab if none exist)
	ModelViewport *viewport = getCurrentViewport();
	if ( !viewport ) {
		qDebug() << "No viewport found, creating new tab...";
		addViewportTab( "New Model" );
		viewport = getCurrentViewport();
	}

	// SAFETY: Double-check viewport creation succeeded
	if ( !viewport ) {
		qCritical() << "ERROR: Failed to create viewport!";
		QMessageBox::critical( this, "Error", "Failed to create viewport. Please restart the application." );
		return;
	}

	// Open file dialog
	QString filePath = QFileDialog::getOpenFileName( this, "Open Half-Life Model", "", "Half-Life Models (*.mdl)" );

	if ( filePath.isEmpty() ) {
		qDebug() << "Model loading cancelled by user";
		return;
	}

	// SAFETY: Check if file actually exists
	QFileInfo fileInfo( filePath );
	if ( !fileInfo.exists() ) {
		qCritical() << "ERROR: File does not exist:" << filePath;
		QMessageBox::critical( this, "File Not Found", "The selected file does not exist:\n" + filePath );
		return;
	}

	// SAFETY: Check if it's actually a .mdl file
	if ( fileInfo.suffix().toLower() != "mdl" ) {
		qCritical() << "ERROR: Not a .mdl file:" << filePath;
		QMessageBox::critical( this, "Invalid File Type", "Please select a Half-Life model file (.mdl):\n" + filePath );
		return;
	}

	// Load model in current viewport
	bool success = viewport->loadModel( filePath );

	if ( !success ) {
		QMessageBox::critical( this, "Error Loading Model",
							   "Failed to load model. The file may be corrupted or invalid:\n\n" + filePath );
		return;
	}

	// Success! Update tab title to show model name
	int currentIndex = tabWidget->currentIndex();
	if ( currentIndex >= 0 ) {
		tabWidget->setTabText( currentIndex, fileInfo.fileName() );
	}

	// Store model info for this tab - get actual values from viewport
	int tabIndex = tabWidget->currentIndex();
	TabModelInfo &info = m_tabModelInfo[tabIndex];
	info.filePath = filePath;
	info.fileSize = fileInfo.size();
	info.vertexCount = viewport->getVertexCount();
	info.triangleCount = viewport->getTriangleCount();
	info.boneCount = viewport->getBoneCount();
	info.sequenceCount = viewport->getSequenceCount();
	info.textureCount = viewport->getTextureCount();
	info.currentSequence = "";
	info.currentFrame = 0;
	info.totalFrames = 0;
	info.selectedBone = "";
	info.activeController = "";

	// Update status bar with model info
	m_statusBar->setModelInfo( filePath, info.vertexCount, info.triangleCount,
							   info.boneCount, info.sequenceCount, info.textureCount );
	m_statusBar->setFileSize( info.fileSize );
}

// ═══════════════════════════════════════════════════════════════════════════
// Tab Management Helper Functions
// ═══════════════════════════════════════════════════════════════════════════

ModelViewport *MainWindow::createNewViewport() {
	try {
		ModelViewport *viewport = new ModelViewport( this );
		if ( !viewport ) {
			qCritical() << "ERROR: Failed to allocate ModelViewport!";
			return nullptr;
		}
		return viewport;
	} catch ( const std::exception &e ) {
		qCritical() << "EXCEPTION creating viewport:" << e.what();
		return nullptr;
	}
}

ModelViewport *MainWindow::getCurrentViewport() {
	if ( !tabWidget ) {
		qWarning() << "WARNING: tabWidget is null in getCurrentViewport()";
		return nullptr;
	}

	if ( tabWidget->count() == 0 ) {
		qDebug() << "No tabs available";
		return nullptr;
	}

	QWidget *widget = tabWidget->currentWidget();
	if ( !widget ) {
		qWarning() << "WARNING: currentWidget() returned null";
		return nullptr;
	}

	ModelViewport *viewport = qobject_cast<ModelViewport *>( widget );
	if ( !viewport ) {
		qWarning() << "WARNING: Current widget is not a ModelViewport!";
		return nullptr;
	}

	return viewport;
}

int MainWindow::addViewportTab( const QString &title ) {
	if ( !tabWidget ) {
		qCritical() << "ERROR: Cannot add tab - tabWidget is null!";
		return -1;
	}

	ModelViewport *viewport = createNewViewport();
	if ( !viewport ) {
		qCritical() << "ERROR: Failed to create viewport for new tab!";
		return -1;
	}

	// Connect viewport to status bar for hover updates
	viewport->setStatusBar( m_statusBar );

	int index = tabWidget->addTab( viewport, title );
	tabWidget->setCurrentIndex( index );
	return index;
}

void MainWindow::onNewTab() {
	int index = addViewportTab( "Untitled" );
	if ( index < 0 ) {
		QMessageBox::warning( this, "Error", "Failed to create new tab!" );
	}
}

void MainWindow::onCloseTab( int index ) {
	if ( !tabWidget ) {
		qWarning() << "WARNING: tabWidget is null in onCloseTab()";
		return;
	}

	if ( tabWidget->count() <= 1 ) {
		return;
	}

	if ( index < 0 || index >= tabWidget->count() ) {
		qWarning() << "WARNING: Invalid tab index:" << index;
		return;
	}

	QWidget *widget = tabWidget->widget( index );
	tabWidget->removeTab( index );

	// Remove this tab's model info
	m_tabModelInfo.remove( index );

	if ( widget ) {
		widget->deleteLater();
	}
}

void MainWindow::onTabChanged( int index ) {
	// WORKAROUND: Reload model data when switching tabs (fixes multi-tab rendering bug)
	// This is needed because the renderer uses global static variables for model data

	if ( !tabWidget ) {
		qWarning() << "WARNING: tabWidget is null in onTabChanged()";
		return;
	}

	if ( index < 0 || index >= tabWidget->count() ) {
		return;
	}

	// Get the viewport for the new tab
	ModelViewport *viewport = qobject_cast<ModelViewport *>( tabWidget->widget( index ) );

	if ( !viewport ) {
		qWarning() << "WARNING: Widget at index" << index << "is not a ModelViewport!";
		return;
	}

	// Update status bar with this tab's model info
	if ( m_tabModelInfo.contains( index ) ) {
		const TabModelInfo &info = m_tabModelInfo[index];
		m_statusBar->setModelInfo( info.filePath, info.vertexCount, info.triangleCount,
								   info.boneCount, info.sequenceCount, info.textureCount );
		m_statusBar->setFileSize( info.fileSize );
		m_statusBar->setSequenceInfo( info.currentSequence, info.currentFrame, info.totalFrames );
		m_statusBar->setBoneName( info.selectedBone );
		m_statusBar->setControllerName( info.activeController );
	} else {
		// No model loaded in this tab
		m_statusBar->clearModelInfo();
	}

	if ( viewport->hasModelLoaded() ) {
		viewport->update();
	}
}

void MainWindow::onStatusBarUpdate() {
	// Get current viewport
	ModelViewport *viewport = getCurrentViewport();
	if ( !viewport ) {
		return;
	}

	m_statusBar->setFPS( (int)viewport->getCurrentFps() );

	// Update FPS display (always shown)
	m_statusBar->setFPS( (int)m_lastFps );

	// Check if mouse is inside the viewport
	QPoint globalPos = QCursor::pos();
	QPoint localPos = viewport->mapFromGlobal( globalPos );
	bool mouseInViewport = viewport->rect().contains( localPos );

	// Viewport size ALWAYS visible (not cursor-dependent)
	m_statusBar->setViewportSize( viewport->width(), viewport->height() );

	// Only update cursor-dependent info when mouse is inside
	if ( mouseInViewport ) {
		// Update mouse position (screen coordinates for now)
		m_statusBar->setCameraPosition( (float)localPos.x(), (float)localPos.y(), 0.0f );

		// Update zoom level based on camera distance
		float defaultDistance = 50.0f;
		float currentDistance = viewport->getCameraDistance();
		float zoomPercent = ( currentDistance > 0.0f ) ? ( defaultDistance / currentDistance ) * 100.0f : 100.0f;
		m_statusBar->setZoomLevel( zoomPercent );

		// Update grid size
		m_statusBar->setGridSize( 10.0f );
	}
	// When mouse leaves viewport, clearViewportInfo() is called via leaveEvent
}
