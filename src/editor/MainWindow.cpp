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
#include "theme/ThemeManager.h"
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
#include <QtWidgets/qtoolbar.h>

MainWindow::MainWindow( QWidget *parent )
	: QMainWindow( parent ) {
	setWindowTitle( "Lambda MDL Editor" );
	resize( MW_WIDTH, MW_HEIGHT );

	// Setting up all the major componenents
	setupMenus();
	setupToolbars();

	// Setting up the docks and the main window for viewing things
	setupDocks();
    setupViewports();
}
void MainWindow::setupMenus() {
	createFileMenu();
	createEditMenu();
	createViewMenu();
	createToolsMenu();
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
    
      
    
}


void MainWindow::createFileMenu() {
    
    QMenu *fileMenu = menuBar()->addMenu( tr( "&File" ) );
    
    // ==================================
    // OPEN SECTION   
    // ==================================
     
    fileMenu->addAction( "Open Model..." );
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
    transformMenu->addAction("Scale Model...");
    transformMenu->addAction("Scale Uniformly...");
    transformMenu->addAction("Scale Bones...");
    transformMenu->addSeparator();
    transformMenu->addAction("Rotate Model...");
    transformMenu->addAction("Rotate 90° CW");
    transformMenu->addAction("Rotate 90° CCW");
    transformMenu->addAction("Rotate 180°");
    transformMenu->addSeparator();
    transformMenu->addAction("Flip Horizontally");
    transformMenu->addAction("Flip Vertically");
    transformMenu->addSeparator();
    transformMenu->addAction("Translate Model...");
    
    // ==================================
    // MESH OPERATIONS
    // ==================================
    
    QMenu *meshMenu = editMenu->addMenu("Mesh Operations");
    meshMenu->addAction("Optimize Mesh...");
    meshMenu->addAction("Smooth Normals...");
    meshMenu->addAction("Recalculate Normals");
    meshMenu->addAction("Triangulate");
    meshMenu->addAction("Merge Vertices...");
    meshMenu->addAction("Remove Duplicates");
    
    // ==================================
    // RESET
    // ==================================
    
    editMenu->addAction("Reset Pose");
    editMenu->addAction("Reset Camera");
    editMenu->addAction("Reset All Transforms");

    editMenu->addSeparator();

    // ═══════════════════════════════════════════════════════
    // SELECTION
    // ═══════════════════════════════════════════════════════

    editMenu->addAction("Select All");
    editMenu->addAction("Select None");
    editMenu->addAction("Invert Selection");
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
    
    QAction* showSeqBBox = showMenu->addAction("Sequence Bounding Box");
    showSeqBBox->setCheckable(true);

    QAction* showEyePos = showMenu->addAction("Eye Position");
    showEyePos->setCheckable(true);

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
    
    QAction* layoutDualV = layoutMenu->addAction("Dual View (Vertical)");
    layoutDualV->setCheckable(true);
    layoutGroup->addAction(layoutDualV);

    QAction* layoutQuad = layoutMenu->addAction("Quad View");
    layoutQuad->setCheckable(true);
    layoutGroup->addAction(layoutQuad);

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
    
    cameraMenu->addAction("Front View");
    cameraMenu->addAction("Back View");
    cameraMenu->addAction("Right View");
    cameraMenu->addAction("Left View");
    cameraMenu->addAction("Top View");
    cameraMenu->addAction("Bottom View");

    cameraMenu->addSeparator();

    cameraMenu->addAction("Frame Selected");
    cameraMenu->addAction("Frame All");
    cameraMenu->addAction("Reset View");

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
}

void MainWindow::createWindowMenu() {
	QMenu *winMenu = menuBar()->addMenu( "&Window" );

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
	viewportContainer = new QWidget( this );
	viewportContainer->setStyleSheet( "background-color: #1e1e1e;" );

	viewportContainer->setObjectName( "ViewportContainer" );

	setCentralWidget( viewportContainer );
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

	rightDock->setWidget( inspectorPanel );
	addDockWidget( Qt::RightDockWidgetArea, rightDock );

	// @Note: Adding now the bottom panel dock, main console for logger messages and so forth
	//        Also need to add for future the dump files so seperate panels for animations
	//        bones, vertices, hitboxes, everything basically right, and of course adding issues or something else

	// TODO: In the future will decide what else to add to he bottom dock as panels
	//       For now just the console, but would be good to add memory dump or something,
	//       or hex dump to be able to see the HEX view of the MDL file and whatnot

	bottomDock = new QDockWidget( "Console", this );
	bottomDock->setAllowedAreas( Qt::BottomDockWidgetArea );
    bottomDock->setObjectName( "BottomDock" );
    
    
	QWidget *consolePanel = new QWidget();
	QVBoxLayout *consoleLayout = new QVBoxLayout( new QLabel( "Console Panel TODO" ) );

	consolePanel->setLayout( consoleLayout );
    consolePanel->setMinimumHeight( MW_CONSOLE_MIN_HEIGHT );
    consolePanel->setMaximumHeight( MW_CONSOLE_MAX_HEIGHT );

	consolePanel->setObjectName( "Console" );

	bottomDock->setWidget( consolePanel );
	addDockWidget( Qt::BottomDockWidgetArea, bottomDock );

	// Make the docks non deattachable
	rightDock->setFeatures( QDockWidget::NoDockWidgetFeatures );
	bottomDock->setFeatures( QDockWidget::NoDockWidgetFeatures );
    
    // removes height-reduction
    rightDock->setTitleBarWidget(new QWidget());
    
    
    // @Note: Adding status bar
    
    
    
}

void MainWindow::setupTheme( void ) {
    
    // here we will add the app reference instead of in the main
}

void MainWindow::setupViewports( void )
{
    createViewportContainer();
}


void MainWindow::setupDocks( void )
{
    createDocks();
}


void MainWindow::setupToolbars( void )
{
    
    createToolbarUpper();
}
