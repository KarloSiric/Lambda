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
#include "MenuFactory.h"
#include "ToolbarFactory.h"
#include "ConsoleWidget.h"
#include "LogWidget.h"
#include "ModelViewport.h"
#include "ConsoleBridge.h"
#include "LoggerBridge.h"
#include "util_console.h"
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
#include <QFileInfo>
#include <QTabBar>
#include <QPushButton>
#include <QLineEdit>
#include <QtWidgets/qtextedit.h>
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
#include <QtWidgets/qtoolbar.h>

MainWindow::MainWindow( QWidget *parent )
	: QMainWindow( parent ) {
        
        
    // initializing the pointer to nullptr
    m_consoleWidget = nullptr;
    m_sequenceSelector = nullptr;

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

	// Initialize animation state
	m_loopAnimation = true;

	// Setting up all the major componenents
	setupMenus();
	setupToolbars();
	connectToolbarActions();

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
    
    ConsoleBridge::init( m_consoleWidget );
    LoggerBridge::init( m_logWidget );

	// Log startup messages
	log_info( "App", "Lambda MDL Editor started" );
	log_info( "App", "Initializing subsystems..." );
	log_debug( "OpenGL", "Setting up rendering context" );

	// Initialize FPS tracking
	m_fpsTimer.start();
	m_frameCount = 0;
	m_lastFps = 0.0f;
}
void MainWindow::setupMenus() {
	MenuFactory::createMenus( this );
}

// ═══════════════════════════════════════════════════════════════════════════
// Menu creation: see menus/MenuFactory.cpp
// Toolbar creation: see toolbars/ToolbarFactory.cpp
// ═══════════════════════════════════════════════════════════════════════════

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
    m_consoleWidget = new ConsoleWidget();

	// Log panel - detailed logging with filtering
	m_logWidget = new LogWidget();

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
	bottomTabs->addTab( m_consoleWidget, "Console" );
	bottomTabs->addTab( m_logWidget, "Log" );
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
	ToolbarFactory::createToolbars( this );
}

void MainWindow::onOpenModel() {
	// SAFETY: Ensure tab widget exists
	if ( !tabWidget ) {
		log_error( "UI", "Tab widget is null!" );
		return;
	}

	// Get current viewport (or create new tab if none exist)
	ModelViewport *viewport = getCurrentViewport();
	if ( !viewport ) {
		log_debug( "UI", "No viewport found, creating new tab..." );
		addViewportTab( "New Model" );
		viewport = getCurrentViewport();
	}

	// SAFETY: Double-check viewport creation succeeded
	if ( !viewport ) {
		log_error( "UI", "Failed to create viewport" );
        CONSOLE_ERROR( "Failed to create viewport. Please restart the application." );
		return;
	}

	// Open file dialog
	QString filePath = QFileDialog::getOpenFileName( this, "Open Half-Life Model", "", "Half-Life Models (*.mdl)" );

	if ( filePath.isEmpty() ) {
		log_debug( "UI", "Model loading cancelled by user" );
		return;
	}

	// SAFETY: Check if file actually exists
	QFileInfo fileInfo( filePath );
	if ( !fileInfo.exists() ) {
        CONSOLE_ERROR( "File not found: %s",  filePath.toUtf8().constData() );
		return;
	}

	// SAFETY: Check if it's actually a .mdl file
	if ( fileInfo.suffix().toLower() != "mdl" ) {
        CONSOLE_ERROR( "Invalid file type. Please select a .mdl file: %s", filePath.toUtf8().constData() );
		return;
	}

	// Load model in current viewport
	log_info( "MDL", "Loading model: %s", filePath.toUtf8().constData() );
	bool success = viewport->loadModel( filePath );

	if ( !success ) {
		log_error( "MDL", "Failed to load model: %s", filePath.toUtf8().constData() );
        CONSOLE_ERROR( "Failed to load model. File may be corrupted or invalid: %s",
                      filePath.toUtf8().constData() );
        m_statusBar->clearModelInfo();
		return;
	}

	log_info( "MDL", "Model loaded successfully: %s", fileInfo.fileName().toUtf8().constData() );

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

	// Log model statistics
	log_debug( "MDL", "  Vertices: %d, Triangles: %d, Bones: %d",
		info.vertexCount, info.triangleCount, info.boneCount );
	log_debug( "MDL", "  Sequences: %d, Textures: %d, Size: %lld bytes",
		info.sequenceCount, info.textureCount, (long long)info.fileSize );
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
        CONSOLE_ERROR( "Failed to create a new tab" );
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

    float fps = viewport->getCurrentFps();

	// Update FPS display (always shown)
	m_statusBar->setFPS( (int)fps );

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

// ═══════════════════════════════════════════════════════════════════════════
// Toolbar Action Connections
// ═══════════════════════════════════════════════════════════════════════════

void MainWindow::connectToolbarActions() {
	// Find the main toolbar
	QToolBar *mainToolbar = findChild<QToolBar *>( "Main Toolbar" );
	if ( !mainToolbar ) {
		// Try to find first toolbar if named lookup fails
		QList<QToolBar *> toolbars = findChildren<QToolBar *>();
		if ( !toolbars.isEmpty() ) {
			mainToolbar = toolbars.first();
		}
	}

	if ( !mainToolbar ) {
		log_warning( "UI", "Main toolbar not found for action connections" );
		return;
	}

	// Connect animation control actions
	for ( QAction *action : mainToolbar->actions() ) {
		QString text = action->text();

		if ( text == "Play" ) {
			connect( action, &QAction::triggered, this, &MainWindow::onPlayAnimation );
		} else if ( text == "Pause" ) {
			connect( action, &QAction::triggered, this, &MainWindow::onPauseAnimation );
		} else if ( text == "Stop" ) {
			connect( action, &QAction::triggered, this, &MainWindow::onStopAnimation );
		} else if ( text == "Prev Frame" ) {
			connect( action, &QAction::triggered, this, &MainWindow::onPrevFrame );
		} else if ( text == "Next Frame" ) {
			connect( action, &QAction::triggered, this, &MainWindow::onNextFrame );
		} else if ( text == "Loop" ) {
			action->setCheckable( true );
			action->setChecked( m_loopAnimation );
			connect( action, &QAction::triggered, this, &MainWindow::onToggleLoop );
		}
	}

	// Create and add sequence selector dropdown to the secondary toolbar
	QToolBar *secondaryToolbar = findChild<QToolBar *>( "Secondary" );
	if ( secondaryToolbar ) {
		// Add label
		QLabel *seqLabel = new QLabel( " Sequence: ", secondaryToolbar );
		seqLabel->setStyleSheet( "color: #000000; background: transparent;" );
		secondaryToolbar->addWidget( seqLabel );

		// Create sequence selector combobox
		m_sequenceSelector = new QComboBox( secondaryToolbar );
		m_sequenceSelector->setMinimumWidth( 180 );
		m_sequenceSelector->setMaximumWidth( 300 );
		m_sequenceSelector->addItem( "(No model loaded)" );
		m_sequenceSelector->setEnabled( false );

		// Apply classic Windows 95 style
		m_sequenceSelector->setStyleSheet(
			"QComboBox { "
			"    background-color: #ffffff; "
			"    color: #000000; "
			"    border: 2px solid; "
			"    border-top-color: #808080; "
			"    border-left-color: #808080; "
			"    border-right-color: #ffffff; "
			"    border-bottom-color: #ffffff; "
			"    padding: 1px 4px; "
			"    font-size: 11px; "
			"} "
			"QComboBox QAbstractItemView { "
			"    background-color: #ffffff; "
			"    color: #000000; "
			"    selection-background-color: #000080; "
			"    selection-color: #ffffff; "
			"}" );

		secondaryToolbar->addWidget( m_sequenceSelector );

		// Connect sequence selection signal
		connect( m_sequenceSelector, QOverload<int>::of( &QComboBox::currentIndexChanged ),
			this, &MainWindow::onSequenceChanged );
	}

	log_debug( "UI", "Toolbar actions connected" );
}

// ═══════════════════════════════════════════════════════════════════════════
// Animation Control Slots
// ═══════════════════════════════════════════════════════════════════════════

void MainWindow::onPlayAnimation() {
	ModelViewport *viewport = getCurrentViewport();
	if ( !viewport || !viewport->hasModelLoaded() ) {
		log_warning( "Animation", "No model loaded - cannot play animation" );
		return;
	}

	viewport->playAnimation( true );
	log_info( "Animation", "Animation playback started" );
}

void MainWindow::onPauseAnimation() {
	ModelViewport *viewport = getCurrentViewport();
	if ( !viewport ) return;

	viewport->playAnimation( false );
	log_info( "Animation", "Animation paused" );
}

void MainWindow::onStopAnimation() {
	ModelViewport *viewport = getCurrentViewport();
	if ( !viewport ) return;

	viewport->playAnimation( false );
	viewport->setAnimationFrame( 0.0f );
	log_info( "Animation", "Animation stopped and reset to frame 0" );
}

void MainWindow::onPrevFrame() {
	ModelViewport *viewport = getCurrentViewport();
	if ( !viewport || !viewport->hasModelLoaded() ) return;

	// Pause animation when stepping through frames manually
	viewport->playAnimation( false );

	float currentFrame = viewport->getCurrentFrame();
	float newFrame = currentFrame - 1.0f;
	if ( newFrame < 0 ) newFrame = 0;

	viewport->setAnimationFrame( newFrame );
	log_debug( "Animation", "Frame: %.1f", newFrame );
}

void MainWindow::onNextFrame() {
	ModelViewport *viewport = getCurrentViewport();
	if ( !viewport || !viewport->hasModelLoaded() ) return;

	// Pause animation when stepping through frames manually
	viewport->playAnimation( false );

	float currentFrame = viewport->getCurrentFrame();
	viewport->setAnimationFrame( currentFrame + 1.0f );
	log_debug( "Animation", "Frame: %.1f", currentFrame + 1.0f );
}

void MainWindow::onToggleLoop() {
	m_loopAnimation = !m_loopAnimation;

	// TODO: Pass loop setting to viewport animation state when mdl_animation supports looping
	log_info( "Animation", "Loop %s", m_loopAnimation ? "enabled" : "disabled" );
}
