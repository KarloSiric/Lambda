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
#include "ThemeManager.h"
#include "ConsoleWidget.h"
#include "LogWidget.h"
#include "ModelViewport.h"
#include "ConsoleBridge.h"
#include "LoggerBridge.h"
#include "util_console.h"

// Inspector panels
#include "../panels/BrowserPanel.h"
#include "../panels/ModelInfoPanel.h"
#include "../panels/SequencesPanel.h"
#include "../panels/TexturesPanel.h"
#include "../panels/BodypartsPanel.h"
#include "../panels/BonesPanel.h"
#include "../panels/BoneControllersPanel.h"
#include "../panels/ModelDisplayPanel.h"
#include "../panels/AttachmentsPanel.h"
#include "../panels/LightingPanel.h"
#include "../widgets/TextureViewWidget.h"
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
#include <QColorDialog>
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

	// Initialize toolbar action pointers
	m_actionWireframe = nullptr;
	m_actionTextured = nullptr;
	m_actionBones = nullptr;
	m_actionHitboxes = nullptr;
	m_actionAttachments = nullptr;
	m_actionNormals = nullptr;
	m_actionGrid = nullptr;
	m_actionAxes = nullptr;
	m_actionGround = nullptr;
	m_actionLighting = nullptr;

	m_sequenceSelector = new QComboBox( this );
	m_sequenceSelector->setObjectName( "SequenceSelector" );
	m_sequenceSelector->setMinimumWidth( 150 );
	m_sequenceSelector->setToolTip( "Select animation sequence" );

	setWindowTitle( "Lambda MDL Editor" );
	setWindowIcon( QIcon( ":/icons/logo-icon-finalized.png" ) );
	resize( MW_WIDTH, MW_HEIGHT );

	// Apply classic window styling (Windows 95/2000)
	setStyleSheet( ThemeManager::getClassicMainWindowStyle() );

	// Initialize animation state
	m_loopAnimation = true;

	// Setting up all the major componenents
	setupMenus();
    
    QToolBar *secondaryToolbar = findChild<QToolBar*>( "Secondary" );
    if ( secondaryToolbar ) 
    {
        secondaryToolbar->addWidget( new QLabel( " Sequence: ", this ) );
        secondaryToolbar->addWidget( m_sequenceSelector );
    }
    
	setupToolbars();
	connectToolbarActions();

	// Setting up the docks and the main window for viewing things
	setupDocks();
	setupViewports();

	// Set up real-time status bar update timer (60 Hz)
	m_statusUpdateTimer = new QTimer( this );
	// Setting the timer update interval time to be 100 ms and not 16.7 like before
	// @Note: Improvement on performance for re-rendering the status bar text update
	m_statusUpdateTimer->setInterval( 250 ); // 4 FPS - status bar doesn't need fast updates
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

MainWindow::~MainWindow() {
    ConsoleBridge::shutdown();
    LoggerBridge::shutdown();

    // Stop status timer
    if ( m_statusUpdateTimer && m_statusUpdateTimer->isActive() ) {
        m_statusUpdateTimer->stop();
    }

    // Delete all viewport tabs
    if ( tabWidget ) {
        while ( tabWidget->count() > 0 ) {
            QWidget *widget = tabWidget->widget( 0 );
            tabWidget->removeTab( 0 );
            delete widget;
        }
    }	
}

// ═══════════════════════════════════════════════════════════════════════════
// Menu creation: see menus/MenuFactory.cpp
// Toolbar creation: see toolbars/ToolbarFactory.cpp
// ═══════════════════════════════════════════════════════════════════════════

void MainWindow::createViewportContainer() {
	// Create container widget with frame for viewport panel
	QWidget *viewportContainer = new QWidget( this );
	viewportContainer->setObjectName( "ViewportContainer" ); // Required for #ViewportContainer CSS selector
	QVBoxLayout *containerLayout = new QVBoxLayout( viewportContainer );
	containerLayout->setContentsMargins( 4, 4, 4, 4 );
	containerLayout->setSpacing( 0 );

	// Apply classic frame style to container (raised 3D border)
	viewportContainer->setStyleSheet( ThemeManager::getClassicViewportContainerStyle() );

	// Create tab widget for multi-model support
	tabWidget = new QTabWidget( viewportContainer );
	tabWidget->setTabsClosable( true ); // Enable close buttons
	tabWidget->setMovable( true ); // Allow tab reordering
	tabWidget->setDocumentMode( false ); // Keep traditional tabs for classic look

	// Apply classic tab styling (compact tabs like BSP editor)
	tabWidget->setStyleSheet( ThemeManager::getClassicTabWidgetStyle() );

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
	// @Note: Adding the right panel, this is the main inspector panel with tabs for:
	//        Browser, Model Info, Sequences, Textures, Bodyparts, Bones

	rightDock = new QDockWidget( "Inspector", this );
	rightDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

	// Create container widget for two-row tab layout
	m_inspectorContainer = new QWidget( this );
	m_inspectorContainer->setMinimumWidth( MW_RIGHT_DOCK_MIN_WIDTH );
	m_inspectorContainer->setMaximumWidth( MW_RIGHT_DOCK_MAX_WIDTH );

	QVBoxLayout *inspectorLayout = new QVBoxLayout( m_inspectorContainer );
	inspectorLayout->setContentsMargins( 0, 0, 0, 0 );
	inspectorLayout->setSpacing( 0 );

	// Row 1: Model Info, Display, Animation, Textures
	m_inspectorTabsRow1 = new QTabWidget( m_inspectorContainer );
	m_inspectorTabsRow1->setDocumentMode( false );
	m_inspectorTabsRow1->tabBar()->setExpanding( true );
	m_inspectorTabsRow1->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

	// Row 2: Bodyparts, Bones, Attachments, Lighting
	m_inspectorTabsRow2 = new QTabWidget( m_inspectorContainer );
	m_inspectorTabsRow2->setDocumentMode( false );
	m_inspectorTabsRow2->tabBar()->setExpanding( true );
	m_inspectorTabsRow2->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

	// Row 3: File browser
	m_inspectorTabsRow3 = new QTabWidget( m_inspectorContainer );
	m_inspectorTabsRow3->setDocumentMode( false );
	m_inspectorTabsRow3->tabBar()->setExpanding( true );
	m_inspectorTabsRow3->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

	// Create all inspector panels
	m_browserPanel = new BrowserPanel( this );
	m_modelInfoPanel = new ModelInfoPanel( this );
	m_sequencesPanel = new SequencesPanel( this );
	m_texturesPanel = new TexturesPanel( this );
	m_bodypartsPanel = new BodypartsPanel( this );
	m_bonesPanel = new BonesPanel( this );
	m_boneControllersPanel = new BoneControllersPanel( this );
	m_modelDisplayPanel = new ModelDisplayPanel( this );
	m_attachmentsPanel = new AttachmentsPanel( this );
	m_lightingPanel = new LightingPanel( this );

	// Row 1 tabs: Full names
	m_inspectorTabsRow1->addTab( m_modelInfoPanel, "Model Info" );
	m_inspectorTabsRow1->addTab( m_modelDisplayPanel, "Display" );
	m_inspectorTabsRow1->addTab( m_sequencesPanel, "Animation" );
	m_inspectorTabsRow1->addTab( m_texturesPanel, "Textures" );

	// Row 2 tabs: Full names
	m_inspectorTabsRow2->addTab( m_bodypartsPanel, "Bodyparts" );
	m_inspectorTabsRow2->addTab( m_bonesPanel, "Bones" );
	m_inspectorTabsRow2->addTab( m_boneControllersPanel, "Controllers" );
	m_inspectorTabsRow2->addTab( m_attachmentsPanel, "Attachments" );
	m_inspectorTabsRow2->addTab( m_lightingPanel, "Lighting" );

	// Row 3 tabs: File browser
	m_inspectorTabsRow3->addTab( m_browserPanel, "File Browser" );

	// Add all three rows - model panels share space, files at bottom
	inspectorLayout->addWidget( m_inspectorTabsRow1, 1 );
	inspectorLayout->addWidget( m_inspectorTabsRow2, 1 );
	inspectorLayout->addWidget( m_inspectorTabsRow3, 1 );

	// Connect browser panel's model selection signal
	connect( m_browserPanel, &BrowserPanel::modelFileSelected,
	         this, &MainWindow::onBrowserModelSelected );

	// Connect textures panel's request to open texture in new tab
	connect( m_texturesPanel, &TexturesPanel::requestTextureTab,
	         this, &MainWindow::onTextureTabRequested );

	// Connect ModelDisplayPanel state changes to sync toolbar buttons
	connect( m_modelDisplayPanel, &ModelDisplayPanel::viewportStateChanged, this, [this]() {
		ModelViewport *viewport = getCurrentViewport();
		if ( viewport ) {
			syncToolbarWithViewport( viewport );
		}
	} );

	// Connect LightingPanel state changes to sync toolbar buttons
	connect( m_lightingPanel, &LightingPanel::viewportStateChanged, this, [this]() {
		ModelViewport *viewport = getCurrentViewport();
		if ( viewport ) {
			syncToolbarWithViewport( viewport );
		}
	} );

	// Apply classic inspector panel styling to all three tab rows
	QString inspectorStyle = ThemeManager::getClassicInspectorStyle();
	m_inspectorTabsRow1->setStyleSheet( inspectorStyle );
	m_inspectorTabsRow2->setStyleSheet( inspectorStyle );
	m_inspectorTabsRow3->setStyleSheet( inspectorStyle );

	// Set container background to match gray theme
	m_inspectorContainer->setStyleSheet( "QWidget { background-color: #c0c0c0; }" );

	rightDock->setWidget( m_inspectorContainer );
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
	QTabWidget *bottomTabs = new QTabWidget( this );
	bottomTabs->setDocumentMode( false ); // Classic tabs

	// Console panel - simple output area (no log filtering)
	m_consoleWidget = new ConsoleWidget( this );

	// Log panel - detailed logging with filtering
	m_logWidget = new LogWidget( this );

	// Memory panel
	QWidget *memoryPanel = new QWidget( this );
	QVBoxLayout *memoryLayout = new QVBoxLayout( memoryPanel );
	QLabel *memoryLabel = new QLabel( "Memory Panel - Hex Dump and Structure View", memoryPanel );
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

	// Apply classic bottom tab styling (Console/Log/Memory)
	bottomTabs->setStyleSheet( ThemeManager::getClassicBottomTabsStyle() );

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
	rightDock->setTitleBarWidget( new QWidget( this ) );
	bottomDock->setTitleBarWidget( new QWidget( this ) );

	// Apply classic dock styling
	rightDock->setStyleSheet( ThemeManager::getClassicDockStyle() );
	bottomDock->setStyleSheet( ThemeManager::getClassicDockStyle() );

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
}

void MainWindow::setupTheme( void ) 
{
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
		CONSOLE_ERROR( "File not found: %s", filePath.toUtf8().constData() );
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

	// Update sequence selector dropdown with model's sequences
	updateSequenceList( viewport );
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
		// This is expected when TextureViewWidget or other non-viewport tabs are active
		// No warning needed - just return null silently
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

	// Get the viewport for the new tab (may be null for texture tabs)
	ModelViewport *viewport = qobject_cast<ModelViewport *>( tabWidget->widget( index ) );

	if ( !viewport ) {
		// This is a non-viewport tab (e.g., TextureViewWidget)
		// Clear model-specific UI elements
		m_statusBar->clearModelInfo();
		updateInspector( nullptr );
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

	// Update sequence selector for the new tab's model
	updateSequenceList( viewport );

	// Sync toolbar toggle buttons with viewport state
	syncToolbarWithViewport( viewport );

	// Update inspector panels for the new viewport
	updateInspector( viewport );

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

	// Viewport size ALWAYS visible (not cursor-dependent)
	m_statusBar->setViewportSize( viewport->width(), viewport->height() );

	// Check if mouse is inside the viewport
	QPoint globalPos = QCursor::pos();
	QPoint localPos = viewport->mapFromGlobal( globalPos );
	bool mouseInViewport = viewport->rect().contains( localPos );

	// Only update cursor-dependent info when mouse is inside AND position changed
	if ( mouseInViewport ) {
		// Only do expensive raycast if mouse moved since last update
		if ( localPos != m_lastMousePos ) {
			m_lastMousePos = localPos;

			// Raycast mouse position to 3D world coordinates
			float worldX = 0.0f, worldY = 0.0f, worldZ = 0.0f;
			if ( viewport->raycastToGround( localPos.x(), localPos.y(), worldX, worldY, worldZ ) ) {
				m_statusBar->setCameraPosition( worldX, worldY, worldZ );
			} else {
				m_statusBar->setCameraPosition( 0.0f, 0.0f, 0.0f );
			}
		}

		// Update zoom level based on camera distance (cheap operation)
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
    
	// Connect all toolbar actions
	for ( QAction *action : mainToolbar->actions() ) {
		QString text = action->text();

		// ─────────────────────────────────────────────────────────────────────
		// FILE SECTION
		// ─────────────────────────────────────────────────────────────────────
		if ( text == "Open" ) {
			connect( action, &QAction::triggered, this, &MainWindow::onOpenModel );
		}

		// ─────────────────────────────────────────────────────────────────────
		// VIEW TOGGLES SECTION
		// ─────────────────────────────────────────────────────────────────────
		else if ( text == "Wireframe" ) {
			m_actionWireframe = action;
			connect( action, &QAction::toggled, this, &MainWindow::onToggleWireframe );
		} else if ( text == "Textured" ) {
			m_actionTextured = action;
			connect( action, &QAction::toggled, this, &MainWindow::onToggleTextured );
		} else if ( text == "Bones" ) {
			m_actionBones = action;
			connect( action, &QAction::toggled, this, &MainWindow::onToggleBones );
		} else if ( text == "Hitboxes" ) {
			m_actionHitboxes = action;
			connect( action, &QAction::toggled, this, &MainWindow::onToggleHitboxes );
		} else if ( text == "Attachments" ) {
			m_actionAttachments = action;
			connect( action, &QAction::toggled, this, &MainWindow::onToggleAttachments );
		} else if ( text == "Normals" ) {
			m_actionNormals = action;
			connect( action, &QAction::toggled, this, &MainWindow::onToggleNormals );
		} else if ( text == "Grid" ) {
			m_actionGrid = action;
			connect( action, &QAction::toggled, this, &MainWindow::onToggleGrid );
		} else if ( text == "Axes" ) {
			m_actionAxes = action;
			connect( action, &QAction::toggled, this, &MainWindow::onToggleAxes );
		} else if ( text == "Ground" ) {
			m_actionGround = action;
			connect( action, &QAction::toggled, this, &MainWindow::onToggleGround );
		}

		// ─────────────────────────────────────────────────────────────────────
		// CAMERA SECTION
		// ─────────────────────────────────────────────────────────────────────
		else if ( text == "Reset Camera" ) {
			connect( action, &QAction::triggered, this, &MainWindow::onResetCamera );
		} else if ( text == "Center Model" ) {
			connect( action, &QAction::triggered, this, &MainWindow::onCenterModel );
		}

		// ─────────────────────────────────────────────────────────────────────
		// ANIMATION SECTION
		// ─────────────────────────────────────────────────────────────────────
		else if ( text == "Play" ) {
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
			connect( action, &QAction::toggled, this, &MainWindow::onToggleLoop );
		}

		// ─────────────────────────────────────────────────────────────────────
		// UTILITY SECTION
		// ─────────────────────────────────────────────────────────────────────
		else if ( text == "Screenshot" ) {
			connect( action, &QAction::triggered, this, &MainWindow::onScreenshot );
		} else if ( text == "Background" ) {
			connect( action, &QAction::triggered, this, &MainWindow::onBackgroundColor );
		} else if ( text == "Lighting" ) {
			m_actionLighting = action;
			connect( action, &QAction::toggled, this, &MainWindow::onToggleLighting );
		}
	}

	log_info( "UI", "Toolbar actions connected (21 actions)" );
}

// ═══════════════════════════════════════════════════════════════════════════
// Toolbar Sync
// ═══════════════════════════════════════════════════════════════════════════

void MainWindow::syncToolbarWithViewport( ModelViewport *viewport ) {
	if ( !viewport ) return;

	// Block signals to prevent feedback loops when updating
	auto blockAction = []( QAction *action, bool block ) {
		if ( action ) action->blockSignals( block );
	};

	blockAction( m_actionWireframe, true );
	blockAction( m_actionTextured, true );
	blockAction( m_actionBones, true );
	blockAction( m_actionHitboxes, true );
	blockAction( m_actionAttachments, true );
	blockAction( m_actionNormals, true );
	blockAction( m_actionGrid, true );
	blockAction( m_actionAxes, true );
	blockAction( m_actionGround, true );
	blockAction( m_actionLighting, true );

	// Sync checked states from viewport
	if ( m_actionWireframe ) m_actionWireframe->setChecked( viewport->isWireframeMode() );
	// Note: Textured mode not yet implemented in ModelViewport
	// if ( m_actionTextured ) m_actionTextured->setChecked( viewport->isTexturedMode() );
	if ( m_actionBones ) m_actionBones->setChecked( viewport->isShowBones() );
	if ( m_actionHitboxes ) m_actionHitboxes->setChecked( viewport->isShowHitboxes() );
	if ( m_actionAttachments ) m_actionAttachments->setChecked( viewport->isShowAttachments() );
	// Note: Show normals not yet implemented in ModelViewport
	// if ( m_actionNormals ) m_actionNormals->setChecked( viewport->isShowNormals() );
	if ( m_actionGrid ) m_actionGrid->setChecked( viewport->isShowGrid() );
	if ( m_actionAxes ) m_actionAxes->setChecked( viewport->isShowAxes() );
	if ( m_actionGround ) m_actionGround->setChecked( viewport->isShowGround() );
	if ( m_actionLighting ) m_actionLighting->setChecked( viewport->isLightingEnabled() );

	// Unblock signals
	blockAction( m_actionWireframe, false );
	blockAction( m_actionTextured, false );
	blockAction( m_actionBones, false );
	blockAction( m_actionHitboxes, false );
	blockAction( m_actionAttachments, false );
	blockAction( m_actionNormals, false );
	blockAction( m_actionGrid, false );
	blockAction( m_actionAxes, false );
	blockAction( m_actionGround, false );
	blockAction( m_actionLighting, false );
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

void MainWindow::onToggleLoop( bool checked ) {
	m_loopAnimation = checked;

	// TODO: Pass loop setting to viewport animation state when mdl_animation supports looping
	log_info( "Animation", "Loop %s", m_loopAnimation ? "enabled" : "disabled" );
}

void MainWindow::onSequenceChanged( int index ) {
	if ( index < 0 ) return;

	ModelViewport *viewport = getCurrentViewport();
	if ( !viewport || !viewport->hasModelLoaded() ) return;

	// Set the sequence on the viewport
	viewport->setSequence( index );
}

// ═══════════════════════════════════════════════════════════════════════════
// Sequence List Management
// ═══════════════════════════════════════════════════════════════════════════

void MainWindow::updateSequenceList( ModelViewport *viewport ) {
	if ( !m_sequenceSelector ) return;

	// Block signals while updating to prevent triggering onSequenceChanged
	m_sequenceSelector->blockSignals( true );
	m_sequenceSelector->clear();

	if ( !viewport || !viewport->hasModelLoaded() ) {
		m_sequenceSelector->addItem( "(No model loaded)" );
		m_sequenceSelector->setEnabled( false );
		m_sequenceSelector->blockSignals( false );
		return;
	}

	// Get sequence count from viewport
	int seqCount = viewport->getSequenceCount();
	if ( seqCount <= 0 ) {
		m_sequenceSelector->addItem( "(No sequences)" );
		m_sequenceSelector->setEnabled( false );
		m_sequenceSelector->blockSignals( false );
		return;
	}

	// Populate the dropdown with actual sequence names from the model
	for ( int i = 0; i < seqCount; i++ ) {
		QString seqName = viewport->getSequenceName( i );
		if ( seqName.isEmpty() ) {
			seqName = QString( "Sequence %1" ).arg( i );
		}
		m_sequenceSelector->addItem( QString( "%1: %2" ).arg( i ).arg( seqName ) );
	}

	m_sequenceSelector->setEnabled( true );

	// Set current selection to match viewport's current sequence
	int currentSeq = viewport->getCurrentSequence();
	if ( currentSeq >= 0 && currentSeq < seqCount ) {
		m_sequenceSelector->setCurrentIndex( currentSeq );
	}

	m_sequenceSelector->blockSignals( false );

	log_debug( "UI", "Sequence list updated: %d sequences", seqCount );
}

// ═══════════════════════════════════════════════════════════════════════════
// View Toggle Slots
// ═══════════════════════════════════════════════════════════════════════════

void MainWindow::onToggleWireframe( bool checked ) {
	ModelViewport *viewport = getCurrentViewport();
	if ( viewport ) {
		viewport->setWireframeMode( checked );
		log_info( "View", "Wireframe %s", checked ? "enabled" : "disabled" );
	}
}

void MainWindow::onToggleTextured( bool checked ) {
	// TODO: Implement textured toggle in viewport
	log_info( "View", "Textured %s", checked ? "enabled" : "disabled" );
}

void MainWindow::onToggleBones( bool checked ) {
	ModelViewport *viewport = getCurrentViewport();
	if ( viewport ) {
		viewport->setShowBones( checked );
		log_info( "View", "Bones %s", checked ? "enabled" : "disabled" );
	}
}

void MainWindow::onToggleHitboxes( bool checked ) {
	ModelViewport *viewport = getCurrentViewport();
	if ( viewport ) {
		viewport->setShowHitboxes( checked );
		log_info( "View", "Hitboxes %s", checked ? "enabled" : "disabled" );
	}
}

void MainWindow::onToggleAttachments( bool checked ) {
	ModelViewport *viewport = getCurrentViewport();
	if ( viewport ) {
		viewport->setShowAttachments( checked );
		log_info( "View", "Attachments %s", checked ? "enabled" : "disabled" );
	}
}

void MainWindow::onToggleNormals( bool checked ) {
	// TODO: Implement normals toggle in viewport
	log_info( "View", "Normals %s", checked ? "enabled" : "disabled" );
}

void MainWindow::onToggleGrid( bool checked ) {
	ModelViewport *viewport = getCurrentViewport();
	if ( viewport ) {
		viewport->setShowGrid( checked );
		log_info( "View", "Grid %s", checked ? "enabled" : "disabled" );
	}
}

void MainWindow::onToggleAxes( bool checked ) {
	ModelViewport *viewport = getCurrentViewport();
	if ( viewport ) {
		viewport->setShowAxes( checked );
		log_info( "View", "Axes %s", checked ? "enabled" : "disabled" );
	}
}

void MainWindow::onToggleGround( bool checked ) {
	ModelViewport *viewport = getCurrentViewport();
	if ( viewport ) {
		viewport->setShowGround( checked );
		log_info( "View", "Ground %s", checked ? "enabled" : "disabled" );
	}
}

void MainWindow::onToggleLighting( bool checked ) {
	// TODO: Implement lighting toggle in viewport
	log_info( "View", "Lighting %s", checked ? "enabled" : "disabled" );
}

// ═══════════════════════════════════════════════════════════════════════════
// Camera Control Slots
// ═══════════════════════════════════════════════════════════════════════════

void MainWindow::onResetCamera() {
	ModelViewport *viewport = getCurrentViewport();
	if ( viewport ) {
		viewport->resetCamera();
		log_info( "Camera", "Camera reset to default position" );
	}
}

void MainWindow::onCenterModel() {
	ModelViewport *viewport = getCurrentViewport();
	if ( viewport && viewport->hasModelLoaded() ) {
		viewport->frameModel();
		log_info( "Camera", "Camera centered on model" );
	} else {
		log_warning( "Camera", "No model loaded to center on" );
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// Utility Slots
// ═══════════════════════════════════════════════════════════════════════════

void MainWindow::onScreenshot() {
	ModelViewport *viewport = getCurrentViewport();
	if ( !viewport ) {
		log_warning( "Screenshot", "No viewport available" );
		return;
	}

	// Get save file path from user
	QString filePath = QFileDialog::getSaveFileName(
		this,
		"Save Screenshot",
		"screenshot.png",
		"PNG Images (*.png);;JPEG Images (*.jpg);;All Files (*)" );

	if ( filePath.isEmpty() ) {
		return;
	}

	// Grab the viewport framebuffer
	QImage screenshot = viewport->grabFramebuffer();
	if ( screenshot.save( filePath ) ) {
		log_info( "Screenshot", "Saved to: %s", filePath.toUtf8().constData() );
	} else {
		log_error( "Screenshot", "Failed to save screenshot" );
	}
}

void MainWindow::onBackgroundColor() {
	// TODO: Implement background color picker
	// For now, show a color dialog and log the selection
	QColor color = QColorDialog::getColor( Qt::gray, this, "Select Background Color" );
	if ( color.isValid() ) {
		log_info( "View", "Background color changed to: %s", color.name().toUtf8().constData() );
		// TODO: Apply to viewport - need to add setBackgroundColor method
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// Inspector Panel Management
// ═══════════════════════════════════════════════════════════════════════════

void MainWindow::updateInspector( ModelViewport *viewport ) {
	// Update all inspector panels with the current viewport
	// Browser panel is independent and doesn't need viewport updates

	m_modelInfoPanel->setViewport( viewport );
	m_sequencesPanel->setViewport( viewport );
	m_texturesPanel->setViewport( viewport );
	m_bodypartsPanel->setViewport( viewport );
	m_bonesPanel->setViewport( viewport );
	m_boneControllersPanel->setViewport( viewport );
	m_modelDisplayPanel->setViewport( viewport );
	m_attachmentsPanel->setViewport( viewport );
	m_lightingPanel->setViewport( viewport );
}

void MainWindow::onBrowserModelSelected( const QString &path ) {
	// Load the selected model in the current viewport tab
	ModelViewport *viewport = getCurrentViewport();
	if ( viewport ) {
		if ( viewport->loadModel( path ) ) {
			// Update tab title with model name
			int index = tabWidget->currentIndex();
			QFileInfo fileInfo( path );
			tabWidget->setTabText( index, fileInfo.fileName() );

			// Store model info for status bar
			TabModelInfo info;
			info.filePath = path;
			info.fileSize = fileInfo.size();
			info.vertexCount = viewport->getVertexCount();
			info.triangleCount = viewport->getTriangleCount();
			info.boneCount = viewport->getBoneCount();
			info.sequenceCount = viewport->getSequenceCount();
			info.textureCount = viewport->getTextureCount();
			m_tabModelInfo[index] = info;

			// Update status bar
			m_statusBar->setModelInfo( path, info.vertexCount, info.triangleCount,
			                           info.boneCount, info.sequenceCount, info.textureCount );
			m_statusBar->setFileSize( info.fileSize );

			// Update sequence list
			updateSequenceList( viewport );

			// Update inspector panels
			updateInspector( viewport );

			log_info( "Browser", "Loaded model: %s", path.toUtf8().constData() );
		} else {
			log_error( "Browser", "Failed to load model: %s", path.toUtf8().constData() );
		}
	}
}

void MainWindow::onTextureTabRequested( const QImage &image, const QString &name,
                                         int width, int height, int flags ) {
	// Create a TextureViewWidget and add it as a new tab
	TextureViewWidget *textureView = new TextureViewWidget( image, name, width, height, flags, this );

	// Add to tab widget
	int index = tabWidget->addTab( textureView, QString( "Tex: %1" ).arg( name ) );
	tabWidget->setCurrentIndex( index );

	log_info( "Textures", "Opened texture '%s' (%dx%d) in new tab", name.toUtf8().constData(), width, height );
}
