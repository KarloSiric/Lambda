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
#include <QWindow>
#include <QDockWidget>
#include <QOpenGLWidget>
#include <QtCore/qnamespace.h>
#include <QtGui/qicon.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qtoolbar.h>

MainWindow::MainWindow( QWidget *parent )
	: QMainWindow( parent ) {
	setWindowTitle( "Lambda MDL Editor" );
	resize( MW_WIDTH, MW_HEIGHT );

	// Setting up all the major componenents
	setupMenus();
	setupToolbars();

	// Setting up the docks and the main window for viewing things
	createDocks();
	createViewportContainer();
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
void MainWindow::setupToolbars() {
	QToolBar *toolBar = addToolBar( "Main Toolbar" );
    toolBar->setMovable( false );
    // FILE
    toolBar->addAction("New");
    toolBar->addAction("Open");
    toolBar->addAction("Save");
    toolBar->addSeparator();

    // UNDO / REDO
    toolBar->addAction("Undo");
    toolBar->addAction("Redo");
    toolBar->addSeparator();

    // CAMERA 
    toolBar->addAction("Orbit");
    toolBar->addAction("Pan");
    toolBar->addAction("Zoom");
    toolBar->addAction("Reset Camera");
    toolBar->addSeparator();

    // VIEW
    toolBar->addAction("Wireframe");
    toolBar->addAction("Flat");
    toolBar->addAction("Textured");
    toolBar->addAction("Fullbright");
    toolBar->addAction("Bones");
    toolBar->addAction("Hitboxes");
    toolBar->addAction("Grid");
    toolBar->addSeparator();

    // MODEL EDIT
    toolBar->addAction("Scale");
    toolBar->addAction("Rotate");
    toolBar->addAction("Translate");
    toolBar->addAction("Mirror");
    toolBar->addAction("Reset Pose");
    toolBar->addSeparator();

    // ANIMATION
    toolBar->addAction("Play");
    toolBar->addAction("Pause");
    toolBar->addAction("Stop");
    toolBar->addAction("Prev Frame");
    toolBar->addAction("Next Frame");
    toolBar->addAction("Loop");
    toolBar->addSeparator();

    // COMPILER
    toolBar->addAction("Compile");
    toolBar->addAction("Decompile");

}
void MainWindow::createFileMenu() {
	QMenu *fileMenu = menuBar()->addMenu( tr( "&File" ) );

	fileMenu->addAction( "New Project" );
	fileMenu->addAction( "Open Model..." );
	fileMenu->addAction( "Open Recent" );

	fileMenu->addSeparator();

	fileMenu->addAction( "Save Project" );
	fileMenu->addAction( "Save Project As..." );

	fileMenu->addSeparator();

	QMenu *exportMenu = fileMenu->addMenu( "Export" );

	exportMenu->addAction( "Export QC Script" );
	exportMenu->addAction( "Export SMD" );
	exportMenu->addAction( "Export OBJ Mesh" );
	exportMenu->addAction( "Export Textures" );

	fileMenu->addSeparator();

	fileMenu->addAction( "Exit" );
}

void MainWindow::createEditMenu() {
	QMenu *editMenu = menuBar()->addMenu( tr( "&Edit" ) );

	editMenu->addAction( "Undo" );
	editMenu->addAction( "Redo" );

	editMenu->addSeparator();

	editMenu->addAction( "Scale Model" );
	editMenu->addAction( "Scale Mesh" );
	editMenu->addAction( "Translate Model" );

	editMenu->addSeparator();

	editMenu->addAction( "Reset Pose" );
	editMenu->addAction( "Reset Model Camera" );

	editMenu->addSeparator();

	editMenu->addAction( "Preferences" );
}
void MainWindow::createViewMenu() {
	QMenu *viewMenu = menuBar()->addMenu( tr( "&View" ) );

	QMenu *renderMode = viewMenu->addMenu( "Render Mode" );
	renderMode->addAction( "Wireframe" );
	renderMode->addAction( "Flat Shaded" );
	renderMode->addAction( "Solid" );
	renderMode->addAction( "Textured" );
	renderMode->addAction( "Fullbright" );

	viewMenu->addSeparator();

	viewMenu->addAction( "Show Bones" );
	viewMenu->addAction( "Show Hitboxes" );
	viewMenu->addAction( "Show Attachments" );
	viewMenu->addAction( "Show Grid" );
	viewMenu->addAction( "Show Ground" );

	viewMenu->addSeparator();

	QMenu *layoutMenu = viewMenu->addMenu( "Layout" );
	layoutMenu->addAction( "Single View" );
	layoutMenu->addAction( "Split View" );
	layoutMenu->addAction( "Quad View" );

	viewMenu->addSeparator();
	viewMenu->addAction( "Reset View" );
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

	setCentralWidget( viewportContainer );
}

void MainWindow::createDocks() {
	// @Note: Added the left panel, going to style it with icons like Hammer Editor
	//        For that we need to add the icons, resize it to some amount that later can
	//        be adjusted in the settings menu right and also make it resizeable
	leftDock = new QDockWidget( "Tools", this );
	leftDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

	leftDock->setMinimumWidth( 60 );
	leftDock->setMaximumWidth( 200 );
	leftDock->resize( 80, leftDock->height() );

	QWidget *toolsPanel = new QWidget();
	QVBoxLayout *toolsLayout = new QVBoxLayout( toolsPanel );

	toolsLayout->addWidget( new QLabel( "Tools Panel Placeholder" ) );
	toolsPanel->setLayout( toolsLayout );

	leftDock->setWidget( toolsPanel );
	addDockWidget( Qt::LeftDockWidgetArea, leftDock );

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
	QWidget *consolePanel = new QWidget();
	QVBoxLayout *consoleLayout = new QVBoxLayout( new QLabel( "Console Panel TODO" ) );

	consolePanel->setLayout( consoleLayout );

	bottomDock->setWidget( consolePanel );
	addDockWidget( Qt::BottomDockWidgetArea, bottomDock );
}


void MainWindow::setTheme()
{
    
    
       
}
