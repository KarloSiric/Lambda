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
#include <QtCore/qnamespace.h>
#include <QtGui/qaction.h>
#include <QtGui/qicon.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qmainwindow.h>
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
    
    // ═══════════════════════════════════════════════════════
    // SHADING OPTIONS
    // ═══════════════════════════════════════════════════════
    
    QMenu *shadingMenu = menuBar()->addMenu( "Shading" );
    QAction *actionSmoothShading = shadingMenu->addAction( "Smooth Shading" );
    
    actionSmoothShading->setCheckable( true );
    actionSmoothShading->setChecked( true );
    
    
    
    
    
    
    
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
