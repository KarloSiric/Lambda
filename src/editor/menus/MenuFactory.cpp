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
 *   Purpose: Factory functions for creating application menus
 * ===============================================================================
 */

#include "MenuFactory.h"
#include "MainWindow.h"

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QApplication>

namespace MenuFactory {

void createMenus( QMainWindow *mainWindow ) {
	createFileMenu( mainWindow );
	createEditMenu( mainWindow );
	createViewMenu( mainWindow );
	createToolsMenu( mainWindow );
	createModelMenu( mainWindow );
	createBodypartMenu( mainWindow );
	createBonesMenu( mainWindow );
	createSequencesMenu( mainWindow );
	createTexturesMenu( mainWindow );
	createDebugMenu( mainWindow );
	createWindowMenu( mainWindow );
	createHelpMenu( mainWindow );
}

void createFileMenu( QMainWindow *mainWindow ) {
	MainWindow *mw = qobject_cast<MainWindow *>( mainWindow );
	QMenu *fileMenu = mainWindow->menuBar()->addMenu( QObject::tr( "&File" ) );

	// ==================================
	// OPEN SECTION
	// ==================================

	// New Tab - Opens a new empty tab for loading another model
	QAction *newTabAction = fileMenu->addAction( "New Tab" );
	newTabAction->setShortcut( QKeySequence( "Ctrl+T" ) );
	if ( mw ) {
		QObject::connect( newTabAction, &QAction::triggered, mw, &MainWindow::onNewTab );
	}
    
    
    
	fileMenu->addSeparator();

	QAction *openAction = fileMenu->addAction( "Open Model" );
	openAction->setShortcut( QKeySequence::Open ); // Ctrl+O
	if ( mw ) {
		QObject::connect( openAction, &QAction::triggered, mw, &MainWindow::onOpenModel );
	}
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
	QObject::connect( actionExit, &QAction::triggered, qApp, &QApplication::exit );
}

void createEditMenu( QMainWindow *mainWindow ) {
	QMenu *editMenu = mainWindow->menuBar()->addMenu( QObject::tr( "&Edit" ) );

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
	transformMenu->addAction( "Rotate 90 CW" );
	transformMenu->addAction( "Rotate 90 CCW" );
	transformMenu->addAction( "Rotate 180" );
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

	// ==================================
	// SELECTION
	// ==================================

	editMenu->addAction( "Select All" );
	editMenu->addAction( "Select None" );
	editMenu->addAction( "Invert Selection" );
}

void createViewMenu( QMainWindow *mainWindow ) {
	QMenu *viewMenu = mainWindow->menuBar()->addMenu( QObject::tr( "&View" ) );

	// ==================================
	// RENDER MODE
	// ==================================

	QMenu *renderModeMenu = viewMenu->addMenu( "Render Mode" );
	QActionGroup *renderModeGroup = new QActionGroup( mainWindow );

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

	// ==================================
	// SHADING OPTIONS
	// ==================================

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

	// ==================================
	// SHOW SUBMENU ( Visibility Toggling )
	// ==================================

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

	// ==================================
	// VIEWPORT LAYOUT
	// ==================================

	QMenu *layoutMenu = viewMenu->addMenu( "Viewport Layout" );
	QActionGroup *layoutGroup = new QActionGroup( mainWindow );

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

	// ==================================
	// CAMERA
	// ==================================

	QMenu *cameraMenu = viewMenu->addMenu( "Camera" );
	QActionGroup *cameraProjGroup = new QActionGroup( mainWindow );

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

	// ==================================
	// GRID SIZE
	// ==================================

	QMenu *gridSizeMenu = viewMenu->addMenu( "Grid Size" );
	QActionGroup *gridSizeGroup = new QActionGroup( mainWindow );

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

	// ==================================
	// COLORS
	// ==================================

	viewMenu->addAction( "Background Color..." );
	viewMenu->addAction( "Grid Color..." );
	viewMenu->addAction( "Ground Color..." );

	viewMenu->addSeparator();

	// ==================================
	// SCREENSHOTS
	// ==================================

	QMenu *screenshotMenu = viewMenu->addMenu( "Screenshot" );
	screenshotMenu->addAction( "Capture Viewport" );
	screenshotMenu->addAction( "Capture Viewport (4K)" );
	screenshotMenu->addAction( "Capture All Sequences" );
	screenshotMenu->addAction( "Capture Thumbnail..." );

	viewMenu->addSeparator();

	// ==================================
	// FULLSCREEN
	// ==================================

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

void createModelMenu( QMainWindow *mainWindow ) {
	QMenu *modelMenu = mainWindow->menuBar()->addMenu( QObject::tr( "&Model" ) );

	// ==================================
	// MODEL INFO
	// ==================================

	modelMenu->addAction( "Model Properties..." ); // Opens dialog
	modelMenu->addAction( "Model Statistics..." ); // Vertex/tri count, etc.

	modelMenu->addSeparator();

	// ==================================
	// MODEL OPERATIONS
	// ==================================

	modelMenu->addAction( "Validate Model..." ); // Check for errors
	modelMenu->addAction( "Optimize Model..." ); // Reduce poly count
	modelMenu->addAction( "Fix Issues..." ); // Auto-fix common problems

	modelMenu->addSeparator();

	// ==================================
	// MODEL METADATA
	// ==================================

	modelMenu->addAction( "Set Eye Position..." );
	modelMenu->addAction( "Set Bounding Box..." );
	modelMenu->addAction( "Set Collision Hull..." );

	modelMenu->addSeparator();

	// ==================================
	// LIST SUBMENUS (Browse model contents)
	// ==================================

	QMenu *listMenu = modelMenu->addMenu( "List" );
	listMenu->addAction( "List Bones..." ); // Opens dialog with all bones
	listMenu->addAction( "List Sequences..." );
	listMenu->addAction( "List Bodyparts..." );
	listMenu->addAction( "List Textures..." );
	listMenu->addAction( "List Hitboxes..." );
	listMenu->addAction( "List Attachments..." );
	listMenu->addAction( "List Events..." );

	modelMenu->addSeparator();

	// ==================================
	// ADVANCED
	// ==================================

	modelMenu->addAction( "Generate LODs..." ); // Level of Detail models
	modelMenu->addAction( "Bake Vertex Colors..." );
	modelMenu->addAction( "Convert to Source 2..." ); // Future feature
}

void createBodypartMenu( QMainWindow *mainWindow ) {
	QMenu *bodypartMenu = mainWindow->menuBar()->addMenu( QObject::tr( "&Bodyparts" ) );

	// ==================================
	// SELECT BODYPART (Will be populated dynamically from model)
	// ==================================

	QMenu *selectBodypartMenu = bodypartMenu->addMenu( "Select Bodypart" );
	// NOTE: In a real implementation, you'll populate this from the loaded model's bodyparts"
	// For now, we'll add placeholders

	selectBodypartMenu->addAction( "Body (3 submdoels)" );
	selectBodypartMenu->addAction( "Head (2 submodels)" );
	selectBodypartMenu->addAction( "Weapon (4 submodels)" );
	selectBodypartMenu->addSeparator();
	selectBodypartMenu->addAction( "Refresh List" );

	bodypartMenu->addSeparator();

	// ==================================
	// SUBMODEL SELECTION (Changes variant of selected bodypart)
	// ==================================

	QMenu *submodelMenu = bodypartMenu->addMenu( "Select Submodel" );
	// this is the radio button behavior
	QActionGroup *submodelGroup = new QActionGroup( mainWindow );

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

	// ==================================
	// VISIBILITY OPTIONS
	// ==================================

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

	// ==================================
	// BODYGROUP PRESETS (Common combinations)
	// ==================================

	QMenu *presetsMenu= bodypartMenu->addMenu( "Bodygroup Presets" );
	presetsMenu->addAction( "Default Loadout" );
	presetsMenu->addAction( "All Variants Visible" );
	presetsMenu->addAction( "Custom Preset 1..." );
	presetsMenu->addAction( "Custom Preset 2..." );
	presetsMenu->addSeparator();
	presetsMenu->addAction( "Save Current as Preset..." );

	bodypartMenu->addSeparator();

	// ==================================
	// IMPORT/EXPORT BODYPARTS
	// ==================================

	QMenu *importBodypartMenu = bodypartMenu->addMenu( "Import Bodypart" );
	importBodypartMenu->addAction( "Import from SMD..." );
	importBodypartMenu->addAction( "Import from OBJ..." );
	importBodypartMenu->addAction( "Import from FBX..." );

	QMenu *exportBodypartMenu = bodypartMenu->addMenu( "Export Bodypart" );
	exportBodypartMenu->addAction( "Export Selected to SMD..." );
	exportBodypartMenu->addAction( "Export Selected to OBJ..." );
	exportBodypartMenu->addAction( "Export All Bodyparts..." );

	bodypartMenu->addSeparator();

	// ==================================
	// BODYPART INFORMATION
	// ==================================

	bodypartMenu->addAction( "List All Bodyparts..." ); // Opens dialog
	bodypartMenu->addAction( "Bodypart Properties..." ); // Shows selected bodypart info
}

void createBonesMenu( QMainWindow *mainWindow ) {
	QMenu *bonesMenu = mainWindow->menuBar()->addMenu( QObject::tr( "&Bones" ) );

	// ==================================
	// BONE VISIBILITY
	// ==================================

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

	// ==================================
	// SELECT BONE (Hierarchical bone list)
	// ==================================

	QMenu *selectBoneMenu = bonesMenu->addMenu( "Select Bone" );
	// @NOTE: This will be populated from the model's bone hierarchy
	// @NOTE(Karlo): This is being hardcoded at the moment will be populated later
	// Showing example hierarchy with indentation
	selectBoneMenu->addAction( "Root" );
	selectBoneMenu->addAction( "  L Pelvis" );
	selectBoneMenu->addAction( "      | Spine1" );
	selectBoneMenu->addAction( "      |   L Spine2" );
	selectBoneMenu->addAction( "      |       L Neck" );
	selectBoneMenu->addAction( "      |           L Head" );
	selectBoneMenu->addAction( "      | L_Thigh" );
	selectBoneMenu->addAction( "      |   L L_Calf" );
	selectBoneMenu->addAction( "      |       L L_Foot" );
	selectBoneMenu->addAction( "      L R_Thigh" );
	selectBoneMenu->addAction( "          L R_Calf" );
	selectBoneMenu->addAction( "              L R_Foot" );
	selectBoneMenu->addSeparator();
	selectBoneMenu->addAction( "Expand All" );
	selectBoneMenu->addAction( "Collapse All" );
	selectBoneMenu->addAction( "Refresh Hierarchy" );

	bonesMenu->addSeparator();

	// ==================================
	// BONE DISPLAY OPTIONS
	// ==================================

	QMenu *displayModeMenu = bonesMenu->addMenu( "Bone Display Mode" );
	QActionGroup *displayModeGroup = new QActionGroup( mainWindow );

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

	// ==================================
	// HITBOX MANAGEMENT
	// ==================================

	QMenu *hitboxMenu = bonesMenu->addMenu( "Hitboxes" );

	QAction *showHitboxes = hitboxMenu->addAction( "Show Hitboxes" );
	showHitboxes->setCheckable( true );

	QAction *showHitboxNames = hitboxMenu->addAction( "Show Hitbox Names" );
	showHitboxNames->setCheckable( true );

	hitboxMenu->addSeparator();

	// Hitbox sets (for different damage groups - head, chest, legs)
	QMenu *hitboxSetMenu = hitboxMenu->addMenu( "Select Hitbox Set" );
	QActionGroup *hitboxSetGroup = new QActionGroup( mainWindow );

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

	// ==================================
	// BONE CONTROLLERS (For facial animations, mouth movement, etc.)
	// ==================================

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

	// ==================================
	// CONSTRAINTS & IK (Inverse Kinematics)
	// ==================================

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

	// ==================================
	// BONE INFORMATION & TOOLS
	// ==================================



	bonesMenu->addAction( "List All Bones..." ); // Opens dialog with full bone list
	bonesMenu->addAction( "Show Bone Properties..." ); // Shows selected bone data
	bonesMenu->addAction( "Show Bone Hierarchy..." ); // Tree view diagram

	bonesMenu->addSeparator();

	// ==================================
	// EXPORT BONES
	// ==================================

	QMenu *exportBonesMenu = bonesMenu->addMenu( "Export" );
	exportBonesMenu->addAction( "Export Skeleton to SMD..." );
	exportBonesMenu->addAction( "Export Bone Weights..." );
	exportBonesMenu->addAction( "Export Hitboxes to QC..." );
}

void createSequencesMenu( QMainWindow *mainWindow ) {
	QMenu *sequencesMenu = mainWindow->menuBar()->addMenu( QObject::tr( "&Sequences" ) );

	// ==================================
	// SELECT SEQUENCE (Populated from model)
	// ==================================

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

	// ==================================
	// PLAYBACK CONTROLS
	// ==================================

	sequencesMenu->addAction( "Play" );
	sequencesMenu->addAction( "Pause" );
	sequencesMenu->addAction( "Stop" );
	sequencesMenu->addAction( "Restart" );

	sequencesMenu->addSeparator();

	// ==================================
	// FRAME NAVIGATION
	// ==================================

	sequencesMenu->addAction( "Next Frame" );
	sequencesMenu->addAction( "Previous Frame" );
	sequencesMenu->addAction( "Jump to Frame..." ); // Input dialog
	sequencesMenu->addAction( "Jump to Start" );
	sequencesMenu->addAction( "Jump to End" );

    sequencesMenu->addSeparator();

	// ==================================
	// PLAYBACK OPTIONS
	// ==================================

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
	QActionGroup *speedGroup = new QActionGroup( mainWindow );

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

	// ==================================
	// SEQUENCE BLENDING (Mix two animations)
	// ==================================

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
	QActionGroup *blendFactorGroup = new QActionGroup( mainWindow );

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
	QActionGroup *blendTypeGroup = new QActionGroup( mainWindow );

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

	// ==================================
	// SEQUENCE INFORMATION
	// ==================================

	QAction *showSeqInfo = sequencesMenu->addAction( "Show Sequence Info" );
	showSeqInfo->setCheckable( true );
	// Displays: frame count, FPS, duration, blend count

	sequencesMenu->addAction( "List All Sequences..." );
	sequencesMenu->addAction( "Sequence Properties..." );

	sequencesMenu->addSeparator();

	// ==================================
	// SEQUENCE EVENTS (Sound effects, particle effects, etc.)
	// ==================================

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

	// ==================================
	// POSE EDITING
	// ==================================

	sequencesMenu->addAction( "Reset Pose" ); // T-pose
	sequencesMenu->addAction( "Apply Current Frame Pose" );
	sequencesMenu->addAction( "Edit Bone Orientation..." );

	sequencesMenu->addSeparator();

	// ==================================
	// SEQUENCE FILTERING
	// ==================================

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

	// ==================================
	// IMPORT/EXPORT
	// ==================================

	QMenu *importSeqMenu = sequencesMenu->addMenu( "Import Sequence" );
	importSeqMenu->addAction( "Import from SMD..." );
	importSeqMenu->addAction( "Import from FBX..." );
	importSeqMenu->addAction( "Import from BVH..." );

	QMenu *exportSeqMenu = sequencesMenu->addMenu( "Export Sequence" );
	exportSeqMenu->addAction( "Export Selected to SMD..." );
	exportSeqMenu->addAction( "Export Selected to FBX..." );
	exportSeqMenu->addAction( "Export All Sequences..." );
}

void createTexturesMenu( QMainWindow *mainWindow ) {
	QMenu *texturesMenu = mainWindow->menuBar()->addMenu( QObject::tr( "&Textures" ) );

	// ==================================
	// SELECT TEXTURE
	// ==================================

	QMenu *selectTexMenu = texturesMenu->addMenu( "Select Texture" );
	// NOTE: Will be populated from model's textures
	selectTexMenu->addAction( "body.bmp (512x512)" );
	selectTexMenu->addAction( "head.bmp (256x256)" );
	selectTexMenu->addAction( "weapon.bmp (128x128)" );
	selectTexMenu->addAction( "eyes.bmp (64x64)" );
	selectTexMenu->addSeparator();
	selectTexMenu->addAction( "Refresh List" );

	texturesMenu->addSeparator();

	// ==================================
	// TEXTURE DISPLAY
	// ==================================

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

	// ==================================
	// TEXTURE PROPERTIES (Half-Life specific flags)
	// ==================================

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

	// ==================================
	// TEXTURE FORMAT
	// ==================================

	QMenu *formatMenu = texturesMenu->addMenu( "Texture Format" );
	QActionGroup *formatGroup = new QActionGroup( mainWindow );

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

	// ==================================
	// MATERIAL/SHADER OPTIONS (For advanced rendering)
	// ==================================

	QMenu *materialMenu = texturesMenu->addMenu( "Material/Shader" );

	QMenu *shaderTypeMenu = materialMenu->addMenu( "Shader Type" );
	QActionGroup *shaderGroup = new QActionGroup( mainWindow );

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

	// ==================================
	// TEXTURE EDITING
	// ==================================

	texturesMenu->addAction( "Import Texture..." );
	texturesMenu->addAction( "Export Texture..." );
	texturesMenu->addAction( "Replace Texture..." );
	texturesMenu->addAction( "Edit in External Program..." );

	texturesMenu->addSeparator();

	// ==================================
	// UV MAPPING
	// ==================================

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

	// ==================================
	// TEXTURE FILTERING
	// ==================================

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

	// ==================================
	// MULTI-TEXTURE MANAGEMENT
	// ==================================

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

	// ==================================
	// TEXTURE PREVIEW
	// ==================================

	QMenu *previewMenu = texturesMenu->addMenu( "Preview" );

	QMenu *previewSizeMenu = previewMenu->addMenu( "Preview Size" );
	QActionGroup *previewSizeGroup = new QActionGroup( mainWindow );

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
	QActionGroup *previewModeGroup = new QActionGroup( mainWindow );

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

	// ==================================
	// ADVANCED OPTIONS
	// ==================================

	QMenu *advancedMenu = texturesMenu->addMenu( "Advanced" );

	QMenu *compressionMenu = advancedMenu->addMenu( "Texture Compression" );
	QActionGroup *compressionGroup = new QActionGroup( mainWindow );

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

	// ==================================
	// TEXTURE INFORMATION
	// ==================================

	texturesMenu->addAction( "List All Textures..." );
	texturesMenu->addAction( "Texture Info..." );
	texturesMenu->addAction( "Show Memory Usage..." );
}

void createToolsMenu( QMainWindow *mainWindow ) {
	QMenu *toolsMenu = mainWindow->menuBar()->addMenu( "&Tools" );

	toolsMenu->addAction( "Compile Model..." );
	toolsMenu->addAction( "Decompile Model..." );
	toolsMenu->addAction( "Generate Thumbnails..." );
	toolsMenu->addAction( "Validate Model..." );
}

void createDebugMenu( QMainWindow *mainWindow ) {
	QMenu *debugMenu = mainWindow->menuBar()->addMenu( QObject::tr( "&Debug" ) );

	// ==================================
	// PERFORMANCE MONITORING
	// ==================================

	QAction *showFPS = debugMenu->addAction( "Show FPS" );
	showFPS->setCheckable( true );

	QAction *showMemory = debugMenu->addAction( "Show Memory" );
	showMemory->setCheckable( true );

	QAction *showRenderStats = debugMenu->addAction( "Show Render Stats" );
	showRenderStats->setCheckable( true );

	debugMenu->addSeparator();

	// ==================================
	// DEBUGGING TOOLS
    // ==================================

	debugMenu->addAction( "Print Model Info to Console..." );
	debugMenu->addAction( "Dump Vertices to File..." );
	debugMenu->addAction( "Dump Bones to File..." );
	debugMenu->addAction( "Dump Sequence to File..." );
	debugMenu->addAction( "Export Debug Log..." );
    
    debugMenu->addSeparator();

	// ==================================
	// OPENGL DEBUG
	// ==================================

	QMenu *glDebugMenu = debugMenu->addMenu( "OpenGL Debug" );
	QAction *showGLInfo = glDebugMenu->addAction( "Show GL Info" );
	showGLInfo->setCheckable( true );

	QAction*wireframeOverlay = glDebugMenu->addAction( "Wireframe Overlay" );
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

	// ==================================
	// MODEL VALIDATION & TESTING
	// ==================================

	QMenu *validateMenu = debugMenu->addMenu( "Validation" );

	validateMenu->addAction( "Validate Bone Hierarchy" );
	validateMenu->addAction( "Validate Sequences" );
	validateMenu->addAction( "Validate Textures" );
	validateMenu->addAction( "Validate Hitboxes" );
	validateMenu->addAction( "Validate Attachments" );
	validateMenu->addSeparator();
	validateMenu->addAction( "Run All Validation Checks..." );

	debugMenu->addSeparator();

	// ==================================
	// LOGGER SETTINGS (IMPORTANT FOR USER NEEDS!)
	// ==================================

	QMenu *logLevelMenu = debugMenu->addMenu( "Log Level" );
	QActionGroup *logLevelGroup = new QActionGroup( mainWindow );

	QAction *logError = logLevelMenu->addAction( "Error Only" );
	logError->setCheckable( true );
	logLevelGroup->addAction( logError );

	QAction *logWarning= logLevelMenu->addAction( "Warning" );
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

	// ==================================
	// CONSOLE ACTIONS
	// ==================================

	debugMenu->addAction( "Clear Console" );
	debugMenu->addAction( "Copy Console to Clipboard" );
	debugMenu->addAction( "Save Console to File..." );
	debugMenu->addAction( "Open Log File Location..." );

	debugMenu->addSeparator();

	// ==================================
	// PROFILING
	// ==================================

	QMenu *profilingMenu = debugMenu->addMenu( "Profiling" );

	QAction *enableProfiling = profilingMenu->addAction( "Profiling" );

	enableProfiling->setCheckable( true );

	profilingMenu->addSeparator();
	profilingMenu->addAction( "Show Profiling Results..." );
	profilingMenu->addAction( "Reset Profiling Results..." );
	profilingMenu->addAction( "Export Profiling Report..." );

	debugMenu->addSeparator();

	// ==================================
	// CRASH & ERROR HANDLING
	// ==================================

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

	// ==================================
	// ADVANCED DEBUG
	// ==================================

	QAction *showMemAddresses = debugMenu->addAction( "Show Memory Addresses" );
	showMemAddresses->setCheckable( true );

	QAction *enableAsserts = debugMenu->addAction( "Enable Runtime Assertions" );
	enableAsserts->setCheckable( true );
	enableAsserts->setChecked( true );

	debugMenu->addSeparator();

	// ==================================
	// TESTING
	// ==================================

	debugMenu->addAction( "Run Unit Tests..." );
	debugMenu->addAction( "Stress Test Renderer..." );
	debugMenu->addAction( "Test All Sequences..." );
}

void createWindowMenu( QMainWindow *mainWindow ) {
	MainWindow *mw = qobject_cast<MainWindow *>( mainWindow );
	QMenu *winMenu = mainWindow->menuBar()->addMenu( "&Window" );

	// Tab Management
	QAction *newTabAction = winMenu->addAction( "New Tab" );
	newTabAction->setShortcut( QKeySequence( "Ctrl+T" ) );
	if ( mw ) {
		QObject::connect( newTabAction, &QAction::triggered, mw, &MainWindow::onNewTab );
	}

    winMenu->addSeparator();

	// Panels
	winMenu->addAction( "Texture Browser" );
	winMenu->addAction( "Model Explorer" );
	winMenu->addAction( "Sequence Viewer" );
	winMenu->addAction( "Inspector" );

	winMenu->addSeparator();
	winMenu->addAction( "Reset Layout" );
}

void createHelpMenu( QMainWindow *mainWindow ) {
	QMenu *helpMenu = mainWindow->menuBar()->addMenu( "&Help" );

	helpMenu->addAction( "Documentation" );
	helpMenu->addAction( "Report Issue" );
	helpMenu->addAction( "About Lambda MDL Editor" );
}

} // namespace MenuFactory
