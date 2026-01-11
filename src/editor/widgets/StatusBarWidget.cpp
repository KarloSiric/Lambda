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
 *   Purpose: Professional status bar widget implementation
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include "StatusBarWidget.h"
#include <QMenu>
#include <QFileInfo>
#include <QClipboard>
#include <QApplication>

StatusBarWidget::StatusBarWidget( QWidget *parent )
	: QStatusBar( parent ),
	  m_vertexCount( 0 ),
	  m_triangleCount( 0 ),
	  m_boneCount( 0 ),
	  m_sequenceCount( 0 ),
	  m_textureCount( 0 ),
	  m_gridSize( 10.0f ),
	  m_cameraDistance( 50.0f ),
	  m_viewportWidth( 1920 ),
	  m_viewportHeight( 1080 ),
	  m_currentFPS( 60 ),
	  m_frameTime( 16.7f ),
	  m_cameraX( 0.0f ),
	  m_cameraY( 0.0f ),
	  m_cameraZ( 0.0f ),
	  m_fov( 75.0f ),
	  m_zoom( 100.0f ),
	  m_playbackSpeed( 1.0f ),
	  m_viewMode( "Textured" ),
	  m_currentBodygroup( 0 ),
	  m_totalBodygroups( 0 ),
	  m_currentSkin( 0 ),
	  m_totalSkins( 0 ),
	  m_attachmentCount( 0 ),
	  m_eventCount( 0 ) {
	setSizeGripEnabled( false ); // Disable size grip to prevent cutoff
	setContentsMargins( 4, 0, 4, 0 ); // Add padding on left/right edges
	setupUI();
	applyStyles();
}

StatusBarWidget::~StatusBarWidget() {
}

void StatusBarWidget::setupUI() {
	createModelInfoWidgets();
	createViewInfoWidgets();
	createToggleButtons();
}

void StatusBarWidget::createModelInfoWidgets() {
	m_filePathLabel = new QLabel( "No model" );
	m_filePathLabel->setMinimumWidth( 150 );
	m_filePathLabel->setMaximumWidth( 300 ); // Reduced from 600
	addWidget( m_filePathLabel );

	// Copy path button
	m_copyPathButton = new QPushButton();
	m_copyPathButton->setText( "📋" );
	m_copyPathButton->setFixedSize( 20, 20 );
	m_copyPathButton->setToolTip( "Copy path" );
	connect( m_copyPathButton, &QPushButton::clicked, this, &StatusBarWidget::onCopyFilePathClicked );
	addWidget( m_copyPathButton );

	m_vertexCountLabel = new QLabel( "Verts: --" );
	m_vertexCountLabel->setToolTip( "Vertex count" );
	addWidget( m_vertexCountLabel );

	m_triangleCountLabel = new QLabel( "Tris: --" );
	m_triangleCountLabel->setToolTip( "Triangle count" );
	addWidget( m_triangleCountLabel );

	m_boneCountLabel = new QLabel( "Bones: --" );
	m_boneCountLabel->setToolTip( "Bone count" );
	addWidget( m_boneCountLabel );

	m_sequenceCountLabel = new QLabel( "Seqs: --" );
	m_sequenceCountLabel->setToolTip( "Sequence count" );
	addWidget( m_sequenceCountLabel );

	m_textureCountLabel = new QLabel( "Tex: --" );
	m_textureCountLabel->setToolTip( "Texture count" );
	addWidget( m_textureCountLabel );

	m_fileSizeLabel = new QLabel( "Size: --" );
	m_fileSizeLabel->setToolTip( "File size" );
	addWidget( m_fileSizeLabel );

	m_sequenceInfoLabel = new QLabel( "Seq: --" );
	m_sequenceInfoLabel->setToolTip( "Current sequence" );
	addWidget( m_sequenceInfoLabel );

	m_bodygroupLabel = new QLabel( "Body: --" );
	m_bodygroupLabel->setToolTip( "Bodygroup" );
	addWidget( m_bodygroupLabel );

	m_skinLabel = new QLabel( "Skin: --" );
	m_skinLabel->setToolTip( "Skin" );
	addWidget( m_skinLabel );

	m_attachmentLabel = new QLabel( "Attach: --" );
	m_attachmentLabel->setToolTip( "Attachment count" );
	addWidget( m_attachmentLabel );

	m_eventLabel = new QLabel( "Events: --" );
	m_eventLabel->setToolTip( "Event count" );
	addWidget( m_eventLabel );

	m_boneNameLabel = new QLabel( "Bone: --" );
	m_boneNameLabel->setToolTip( "Selected bone" );
	addWidget( m_boneNameLabel );

	m_controllerNameLabel = new QLabel( "Ctrl: --" );
	m_controllerNameLabel->setToolTip( "Controller" );
	addWidget( m_controllerNameLabel );

	// Stretch spacer to push everything else to the right
	addPermanentWidget( new QWidget(), 1 );
}

void StatusBarWidget::createViewInfoWidgets() {
	// Performance
	m_fpsLabel = new QLabel( "FPS: 60" );
	m_fpsLabel->setToolTip( "Frames per second" );
	addPermanentWidget( m_fpsLabel );

	m_frameTimeLabel = new QLabel( "Frame: 16ms" );
	m_frameTimeLabel->setToolTip( "Frame time" );
	addPermanentWidget( m_frameTimeLabel );

	// System stats
	m_cpuUsageLabel = new QLabel( "CPU: 0%" );
	m_cpuUsageLabel->setToolTip( "CPU usage" );
	addPermanentWidget( m_cpuUsageLabel );

	m_gpuUsageLabel = new QLabel( "GPU: 0%" );
	m_gpuUsageLabel->setToolTip( "GPU usage" );
	addPermanentWidget( m_gpuUsageLabel );

	m_ramUsageLabel = new QLabel( "RAM: 0MB" );
	m_ramUsageLabel->setToolTip( "RAM usage" );
	addPermanentWidget( m_ramUsageLabel );

	// Viewport
	m_resolutionLabel = new QLabel( "Res: 1920x1080" );
	m_resolutionLabel->setToolTip( "Viewport resolution" );
	addPermanentWidget( m_resolutionLabel );

	m_viewModeLabel = new QLabel( "Mode: Textured" );
	m_viewModeLabel->setToolTip( "Render mode" );
	addPermanentWidget( m_viewModeLabel );

	m_fovLabel = new QLabel( "FOV: 75°" );
	m_fovLabel->setToolTip( "Field of view" );
	addPermanentWidget( m_fovLabel );

	m_zoomLabel = new QLabel( "Zoom: 100%" );
	m_zoomLabel->setToolTip( "Zoom level" );
	addPermanentWidget( m_zoomLabel );

	m_playbackSpeedLabel = new QLabel( "Speed: 1.0x" );
	m_playbackSpeedLabel->setToolTip( "Playback speed" );
	addPermanentWidget( m_playbackSpeedLabel );

	m_cameraPosLabel = new QLabel( "Cam: 0,0,0" );
	m_cameraPosLabel->setToolTip( "Camera position XYZ" );
	addPermanentWidget( m_cameraPosLabel );

	m_cameraDistLabel = new QLabel( "Dist: 50" );
	m_cameraDistLabel->setToolTip( "Camera distance" );
	addPermanentWidget( m_cameraDistLabel );

	m_gridSizeLabel = new QLabel( "Grid: 10" );
	m_gridSizeLabel->setToolTip( "Grid size" );
	addPermanentWidget( m_gridSizeLabel );
}

void StatusBarWidget::createToggleButtons() {
	// Add small spacer before buttons
	QWidget *leftSpacer = new QWidget();
	leftSpacer->setFixedWidth( 4 );
	addPermanentWidget( leftSpacer );

	m_inspectorToggle = new QPushButton();
	m_inspectorToggle->setText( "I" );
	m_inspectorToggle->setFixedSize( 28, 24 );
	m_inspectorToggle->setCheckable( true );
	m_inspectorToggle->setChecked( true );
	m_inspectorToggle->setToolTip( "Inspector Panel (Right-click for options)" );
	m_inspectorToggle->setContextMenuPolicy( Qt::CustomContextMenu );
	connect( m_inspectorToggle, &QPushButton::clicked, this, &StatusBarWidget::onInspectorButtonClicked );
	connect( m_inspectorToggle, &QPushButton::customContextMenuRequested, this, &StatusBarWidget::showInspectorContextMenu );
	addPermanentWidget( m_inspectorToggle );

	m_consoleToggle = new QPushButton();
	m_consoleToggle->setText( "C" );
	m_consoleToggle->setFixedSize( 28, 24 );
	m_consoleToggle->setCheckable( true );
	m_consoleToggle->setChecked( true );
	m_consoleToggle->setToolTip( "Console Panel (Right-click for options)" );
	m_consoleToggle->setContextMenuPolicy( Qt::CustomContextMenu );
	connect( m_consoleToggle, &QPushButton::clicked, this, &StatusBarWidget::onConsoleButtonClicked );
	connect( m_consoleToggle, &QPushButton::customContextMenuRequested, this, &StatusBarWidget::showConsoleContextMenu );
	addPermanentWidget( m_consoleToggle );

	m_memoryToggle = new QPushButton();
	m_memoryToggle->setText( "M" );
	m_memoryToggle->setFixedSize( 28, 24 );
	m_memoryToggle->setCheckable( true );
	m_memoryToggle->setChecked( false );
	m_memoryToggle->setToolTip( "Memory Panel (Right-click for options)" );
	m_memoryToggle->setContextMenuPolicy( Qt::CustomContextMenu );
	connect( m_memoryToggle, &QPushButton::clicked, this, &StatusBarWidget::onMemoryButtonClicked );
	connect( m_memoryToggle, &QPushButton::customContextMenuRequested, this, &StatusBarWidget::showMemoryContextMenu );
	addPermanentWidget( m_memoryToggle );

	// Add small spacer after buttons to prevent cutoff at right edge
	QWidget *rightSpacer = new QWidget();
	rightSpacer->setFixedWidth( 4 );
	addPermanentWidget( rightSpacer );
}

void StatusBarWidget::applyStyles() {
	// Classic tool aesthetic with lighter grays and traditional sunken appearance
	setStyleSheet(
		"QStatusBar { "
		"    background-color: #d0d0d0; "
		"    color: #202020; "
		"    border-top: 1px solid #a0a0a0; "
		"    border-bottom: 1px solid #ffffff; "
		"} "
		"QStatusBar::item { border: none; }" );

	// File path with sunken panel look
	m_filePathLabel->setStyleSheet(
		"QLabel { "
		"    padding: 2px 4px; "
		"    color: #101010; "
		"    background-color: #e8e8e8; "
		"    border: 1px solid #808080; "
		"    border-top-color: #808080; "
		"    border-left-color: #808080; "
		"    border-right-color: #f0f0f0; "
		"    border-bottom-color: #f0f0f0; "
		"}" );

	// Model info with classic sunken panels
	QString modelInfoStyle =
		"QLabel { "
		"    padding: 2px 4px; "
		"    color: #202020; "
		"    background-color: #e8e8e8; "
		"    border: 1px solid #808080; "
		"    border-top-color: #808080; "
		"    border-left-color: #808080; "
		"    border-right-color: #f0f0f0; "
		"    border-bottom-color: #f0f0f0; "
		"}";
	m_vertexCountLabel->setStyleSheet( modelInfoStyle );
	m_triangleCountLabel->setStyleSheet( modelInfoStyle );
	m_boneCountLabel->setStyleSheet( modelInfoStyle );
	m_sequenceCountLabel->setStyleSheet( modelInfoStyle );
	m_textureCountLabel->setStyleSheet( modelInfoStyle );

	QString viewInfoStyle =
		"QLabel { "
		"    padding: 2px 4px; "
		"    color: #202020; "
		"    background-color: #e8e8e8; "
		"    border: 1px solid #808080; "
		"    border-top-color: #808080; "
		"    border-left-color: #808080; "
		"    border-right-color: #f0f0f0; "
		"    border-bottom-color: #f0f0f0; "
		"}";
	m_fileSizeLabel->setStyleSheet( modelInfoStyle );
	m_sequenceInfoLabel->setStyleSheet( modelInfoStyle );
	m_boneNameLabel->setStyleSheet( modelInfoStyle );
	m_controllerNameLabel->setStyleSheet( modelInfoStyle );
	m_bodygroupLabel->setStyleSheet( modelInfoStyle );
	m_skinLabel->setStyleSheet( modelInfoStyle );
	m_attachmentLabel->setStyleSheet( modelInfoStyle );
	m_eventLabel->setStyleSheet( modelInfoStyle );
	m_cpuUsageLabel->setStyleSheet( viewInfoStyle );
	m_gpuUsageLabel->setStyleSheet( viewInfoStyle );
	m_ramUsageLabel->setStyleSheet( viewInfoStyle );
	m_resolutionLabel->setStyleSheet( viewInfoStyle );
	m_gridSizeLabel->setStyleSheet( viewInfoStyle );
	m_cameraDistLabel->setStyleSheet( viewInfoStyle );
	m_fpsLabel->setStyleSheet( viewInfoStyle );
	m_frameTimeLabel->setStyleSheet( viewInfoStyle );
	m_cameraPosLabel->setStyleSheet( viewInfoStyle );
	m_fovLabel->setStyleSheet( viewInfoStyle );
	m_zoomLabel->setStyleSheet( viewInfoStyle );
	m_viewModeLabel->setStyleSheet( viewInfoStyle );
	m_playbackSpeedLabel->setStyleSheet( viewInfoStyle );

	// Classic raised buttons (like Windows 95/2000 style)
	QString buttonStyle =
		"QPushButton { "
		"    border: 2px solid; "
		"    border-top-color: #ffffff; "
		"    border-left-color: #ffffff; "
		"    border-right-color: #808080; "
		"    border-bottom-color: #808080; "
		"    background-color: #c0c0c0; "
		"    padding: 3px 8px; "
		"    color: #000000; "
		"    font-size: 11px; "
		"    font-weight: bold; "
		"} "
		"QPushButton:checked { "
		"    border-top-color: #808080; "
		"    border-left-color: #808080; "
		"    border-right-color: #ffffff; "
		"    border-bottom-color: #ffffff; "
		"    background-color: #a0a0a0; "
		"    color: #000000; "
		"} "
		"QPushButton:hover { "
		"    background-color: #d0d0d0; "
		"} "
		"QPushButton:pressed { "
		"    border-top-color: #808080; "
		"    border-left-color: #808080; "
		"    border-right-color: #ffffff; "
		"    border-bottom-color: #ffffff; "
		"    background-color: #b0b0b0; "
		"}";

	m_inspectorToggle->setStyleSheet( buttonStyle );
	m_consoleToggle->setStyleSheet( buttonStyle );
	m_memoryToggle->setStyleSheet( buttonStyle );

	// Copy button style (smaller, compact)
	QString copyButtonStyle =
		"QPushButton { "
		"    border: 2px solid; "
		"    border-top-color: #ffffff; "
		"    border-left-color: #ffffff; "
		"    border-right-color: #808080; "
		"    border-bottom-color: #808080; "
		"    background-color: #c0c0c0; "
		"    padding: 1px; "
		"    color: #000000; "
		"    font-size: 10px; "
		"} "
		"QPushButton:hover { "
		"    background-color: #d0d0d0; "
		"} "
		"QPushButton:pressed { "
		"    border-top-color: #808080; "
		"    border-left-color: #808080; "
		"    border-right-color: #ffffff; "
		"    border-bottom-color: #ffffff; "
		"    background-color: #b0b0b0; "
		"}";
	m_copyPathButton->setStyleSheet( copyButtonStyle );
}

void StatusBarWidget::setModelInfo( const QString &filePath, int vertexCount,
									int triangleCount, int boneCount,
									int sequenceCount, int textureCount ) {
	m_currentFilePath = filePath;
	m_vertexCount = vertexCount;
	m_triangleCount = triangleCount;
	m_boneCount = boneCount;
	m_sequenceCount = sequenceCount;
	m_textureCount = textureCount;

	// Show full path with smart truncation if too long
	QString displayPath = filePath;
	QFontMetrics metrics( m_filePathLabel->font() );
	int maxWidth = m_filePathLabel->maximumWidth() - 12; // Account for padding

	if ( metrics.horizontalAdvance( displayPath ) > maxWidth ) {
		// Truncate in the middle, keeping start and end visible
		QFileInfo fileInfo( filePath );
		QString fileName = fileInfo.fileName();
		QString dirPath = fileInfo.absolutePath();

		// Try to fit ".../<last_dir>/<filename>"
		QString shortPath = ".../" + dirPath.section( '/', -1 ) + "/" + fileName;
		if ( metrics.horizontalAdvance( shortPath ) > maxWidth ) {
			// If still too long, just show ".../<filename>"
			shortPath = ".../" + fileName;
		}
		displayPath = shortPath;
	}

	m_filePathLabel->setText( displayPath );
	m_filePathLabel->setToolTip( filePath ); // Full path always in tooltip

	m_vertexCountLabel->setText( QString( "Verts: %L1" ).arg( vertexCount ) );
	m_triangleCountLabel->setText( QString( "Tris: %L1" ).arg( triangleCount ) );
	m_boneCountLabel->setText( QString( "Bones: %1" ).arg( boneCount ) );
	m_sequenceCountLabel->setText( QString( "Seqs: %1" ).arg( sequenceCount ) );
	m_textureCountLabel->setText( QString( "Tex: %1" ).arg( textureCount ) );
}

void StatusBarWidget::setFileSize( qint64 sizeBytes ) {
	if ( sizeBytes < 1024 ) {
		m_fileSizeLabel->setText( QString( "Size: %1 B" ).arg( sizeBytes ) );
	} else if ( sizeBytes < 1024 * 1024 ) {
		m_fileSizeLabel->setText( QString( "Size: %1 KB" ).arg( sizeBytes / 1024.0, 0, 'f', 1 ) );
	} else {
		m_fileSizeLabel->setText( QString( "Size: %1 MB" ).arg( sizeBytes / (1024.0 * 1024.0), 0, 'f', 2 ) );
	}
}

void StatusBarWidget::setSequenceInfo( const QString &sequenceName, int currentFrame, int totalFrames ) {
	if ( sequenceName.isEmpty() || totalFrames == 0 ) {
		m_sequenceInfoLabel->setText( "Seq: --" );
	} else {
		m_sequenceInfoLabel->setText( QString( "Seq: %1 %2/%3" ).arg( sequenceName ).arg( currentFrame ).arg( totalFrames ) );
	}
}

void StatusBarWidget::setBoneName( const QString &boneName ) {
	if ( boneName.isEmpty() ) {
		m_boneNameLabel->setText( "Bone: --" );
	} else {
		m_boneNameLabel->setText( QString( "Bone: %1" ).arg( boneName ) );
	}
}

void StatusBarWidget::setControllerName( const QString &controllerName ) {
	if ( controllerName.isEmpty() ) {
		m_controllerNameLabel->setText( "Ctrl: --" );
	} else {
		m_controllerNameLabel->setText( QString( "Ctrl: %1" ).arg( controllerName ) );
	}
}

void StatusBarWidget::setSystemStats( float cpuUsage, float gpuUsage, float ramUsage ) {
	m_cpuUsageLabel->setText( QString( "CPU: %1%" ).arg( cpuUsage, 0, 'f', 1 ) );
	m_gpuUsageLabel->setText( QString( "GPU: %1%" ).arg( gpuUsage, 0, 'f', 1 ) );
	m_ramUsageLabel->setText( QString( "RAM: %1MB" ).arg( ramUsage, 0, 'f', 0 ) );
}

void StatusBarWidget::setCameraDistance( float distance ) {
	m_cameraDistance = distance;
	m_cameraDistLabel->setText( QString( "Dist: %1" ).arg( distance, 0, 'f', 1 ) );
}

void StatusBarWidget::setGridSize( float size ) {
	m_gridSize = size;
	m_gridSizeLabel->setText( QString( "Grid: %1" ).arg( size, 0, 'f', 1 ) );
}

void StatusBarWidget::setResolution( int width, int height ) {
	m_viewportWidth = width;
	m_viewportHeight = height;
	m_resolutionLabel->setText( QString( "Res: %1x%2" ).arg( width ).arg( height ) );
}

void StatusBarWidget::setFPS( int fps ) {
	m_currentFPS = fps;
	m_fpsLabel->setText( QString( "FPS: %1" ).arg( fps ) );

	// Classic style with color coding for performance
	QString baseStyle =
		"QLabel { "
		"    padding: 3px 6px; "
		"    background-color: #e8e8e8; "
		"    border: 1px solid #808080; "
		"    border-top-color: #808080; "
		"    border-left-color: #808080; "
		"    border-right-color: #f0f0f0; "
		"    border-bottom-color: #f0f0f0; "
		"    color: ";

	if ( fps >= 50 ) {
		m_fpsLabel->setStyleSheet( baseStyle + "#006000; }" ); // Dark green for good FPS
	} else if ( fps >= 30 ) {
		m_fpsLabel->setStyleSheet( baseStyle + "#806000; }" ); // Dark yellow for OK FPS
	} else {
		m_fpsLabel->setStyleSheet( baseStyle + "#800000; }" ); // Dark red for poor FPS
	}
}

void StatusBarWidget::setFrameTime( float milliseconds ) {
	m_frameTime = milliseconds;
	m_frameTimeLabel->setText( QString( "Frame: %1ms" ).arg( milliseconds, 0, 'f', 1 ) );

	// Color code frame time (16.7ms = 60fps threshold)
	QString baseStyle =
		"QLabel { "
		"    padding: 3px 6px; "
		"    background-color: #e8e8e8; "
		"    border: 1px solid #808080; "
		"    border-top-color: #808080; "
		"    border-left-color: #808080; "
		"    border-right-color: #f0f0f0; "
		"    border-bottom-color: #f0f0f0; "
		"    color: ";

	if ( milliseconds <= 20.0f ) { // Under 20ms (50+ fps)
		m_frameTimeLabel->setStyleSheet( baseStyle + "#006000; }" );
	} else if ( milliseconds <= 33.0f ) { // Under 33ms (30+ fps)
		m_frameTimeLabel->setStyleSheet( baseStyle + "#806000; }" );
	} else {
		m_frameTimeLabel->setStyleSheet( baseStyle + "#800000; }" );
	}
}

void StatusBarWidget::setCameraPosition( float x, float y, float z ) {
	m_cameraX = x;
	m_cameraY = y;
	m_cameraZ = z;
	m_cameraPosLabel->setText( QString( "Cam: %1,%2,%3" ).arg( x, 0, 'f', 1 ).arg( y, 0, 'f', 1 ).arg( z, 0, 'f', 1 ) );
}

void StatusBarWidget::setFOV( float fov ) {
	m_fov = fov;
	m_fovLabel->setText( QString( "FOV: %1°" ).arg( fov, 0, 'f', 0 ) );
}

void StatusBarWidget::setZoomLevel( float zoom ) {
	m_zoom = zoom;
	m_zoomLabel->setText( QString( "Zoom: %1%" ).arg( zoom, 0, 'f', 0 ) );
}

void StatusBarWidget::setViewMode( const QString &mode ) {
	m_viewMode = mode;
	m_viewModeLabel->setText( QString( "Mode: %1" ).arg( mode ) );
}

void StatusBarWidget::setPlaybackSpeed( float speed ) {
	m_playbackSpeed = speed;
	m_playbackSpeedLabel->setText( QString( "Speed: %1x" ).arg( speed, 0, 'f', 1 ) );
}

void StatusBarWidget::setBodygroupInfo( int current, int total ) {
	m_currentBodygroup = current;
	m_totalBodygroups = total;
	if ( total == 0 ) {
		m_bodygroupLabel->setText( "Body: --" );
	} else {
		m_bodygroupLabel->setText( QString( "Body: %1/%2" ).arg( current ).arg( total ) );
	}
}

void StatusBarWidget::setSkinInfo( int current, int total ) {
	m_currentSkin = current;
	m_totalSkins = total;
	if ( total == 0 ) {
		m_skinLabel->setText( "Skin: --" );
	} else {
		m_skinLabel->setText( QString( "Skin: %1/%2" ).arg( current ).arg( total ) );
	}
}

void StatusBarWidget::setAttachmentCount( int count ) {
	m_attachmentCount = count;
	if ( count == 0 ) {
		m_attachmentLabel->setText( "Attach: --" );
	} else {
		m_attachmentLabel->setText( QString( "Attach: %1" ).arg( count ) );
	}
}

void StatusBarWidget::setEventCount( int count ) {
	m_eventCount = count;
	if ( count == 0 ) {
		m_eventLabel->setText( "Events: --" );
	} else {
		m_eventLabel->setText( QString( "Events: %1" ).arg( count ) );
	}
}

void StatusBarWidget::clearModelInfo() {
	m_currentFilePath.clear();
	m_vertexCount = 0;
	m_triangleCount = 0;
	m_boneCount = 0;
	m_sequenceCount = 0;
	m_textureCount = 0;

	m_filePathLabel->setText( "No model" );
	m_filePathLabel->setToolTip( "" );
	m_vertexCountLabel->setText( "Verts: --" );
	m_triangleCountLabel->setText( "Tris: --" );
	m_boneCountLabel->setText( "Bones: --" );
	m_sequenceCountLabel->setText( "Seqs: --" );
	m_textureCountLabel->setText( "Tex: --" );
	m_fileSizeLabel->setText( "Size: --" );
	m_sequenceInfoLabel->setText( "Seq: --" );
	m_boneNameLabel->setText( "Bone: --" );
	m_controllerNameLabel->setText( "Ctrl: --" );
	m_bodygroupLabel->setText( "Body: --" );
	m_skinLabel->setText( "Skin: --" );
	m_attachmentLabel->setText( "Attach: --" );
	m_eventLabel->setText( "Events: --" );
}

void StatusBarWidget::setInspectorVisible( bool visible ) {
	m_inspectorToggle->setChecked( visible );
}

void StatusBarWidget::setConsoleVisible( bool visible ) {
	m_consoleToggle->setChecked( visible );
}

void StatusBarWidget::setMemoryVisible( bool visible ) {
	m_memoryToggle->setChecked( visible );
}

void StatusBarWidget::onInspectorButtonClicked() {
	emit inspectorToggleRequested();
}

void StatusBarWidget::onConsoleButtonClicked() {
	emit consoleToggleRequested();
}

void StatusBarWidget::onMemoryButtonClicked() {
	emit memoryToggleRequested();
}

void StatusBarWidget::onCopyFilePathClicked() {
	if ( !m_currentFilePath.isEmpty() ) {
		QClipboard *clipboard = QApplication::clipboard();
		clipboard->setText( m_currentFilePath );
	}
}

void StatusBarWidget::showInspectorContextMenu( const QPoint &pos ) {
	QMenu menu( this );
	menu.setStyleSheet(
		"QMenu { "
		"    background-color: #d0d0d0; "
		"    color: #000000; "
		"    border: 2px solid; "
		"    border-top-color: #ffffff; "
		"    border-left-color: #ffffff; "
		"    border-right-color: #808080; "
		"    border-bottom-color: #808080; "
		"    padding: 2px; "
		"} "
		"QMenu::item { "
		"    padding: 4px 24px; "
		"    background-color: transparent; "
		"} "
		"QMenu::item:selected { "
		"    background-color: #000080; "
		"    color: #ffffff; "
		"} "
		"QMenu::separator { "
		"    height: 1px; "
		"    background-color: #808080; "
		"    margin: 2px 0px; "
		"}" );

	QAction *toggleAction = menu.addAction( "Toggle Inspector" );
	QAction *focusAction = menu.addAction( "Focus Inspector" );
	menu.addSeparator();
	QAction *resetAction = menu.addAction( "Reset Layout" );

	QAction *selected = menu.exec( m_inspectorToggle->mapToGlobal( pos ) );
	if ( selected == toggleAction || selected == focusAction ) {
		emit inspectorToggleRequested();
	}
}

void StatusBarWidget::showConsoleContextMenu( const QPoint &pos ) {
	QMenu menu( this );
	menu.setStyleSheet(
		"QMenu { "
		"    background-color: #d0d0d0; "
		"    color: #000000; "
		"    border: 2px solid; "
		"    border-top-color: #ffffff; "
		"    border-left-color: #ffffff; "
		"    border-right-color: #808080; "
		"    border-bottom-color: #808080; "
		"    padding: 2px; "
		"} "
		"QMenu::item { "
		"    padding: 4px 24px; "
		"    background-color: transparent; "
		"} "
		"QMenu::item:selected { "
		"    background-color: #000080; "
		"    color: #ffffff; "
		"} "
		"QMenu::separator { "
		"    height: 1px; "
		"    background-color: #808080; "
		"    margin: 2px 0px; "
		"}" );

	QAction *toggleAction = menu.addAction( "Toggle Console" );
	QAction *clearAction = menu.addAction( "Clear Console" );
	menu.addSeparator();
	QAction *logLevelAction = menu.addAction( "Set Log Level..." );

	QAction *selected = menu.exec( m_consoleToggle->mapToGlobal( pos ) );
	if ( selected == toggleAction ) {
		emit consoleToggleRequested();
	}
}

void StatusBarWidget::showMemoryContextMenu( const QPoint &pos ) {
	QMenu menu( this );
	menu.setStyleSheet(
		"QMenu { "
		"    background-color: #d0d0d0; "
		"    color: #000000; "
		"    border: 2px solid; "
		"    border-top-color: #ffffff; "
		"    border-left-color: #ffffff; "
		"    border-right-color: #808080; "
		"    border-bottom-color: #808080; "
		"    padding: 2px; "
		"} "
		"QMenu::item { "
		"    padding: 4px 24px; "
		"    background-color: transparent; "
		"} "
		"QMenu::item:selected { "
		"    background-color: #000080; "
		"    color: #ffffff; "
		"} "
		"QMenu::separator { "
		"    height: 1px; "
		"    background-color: #808080; "
		"    margin: 2px 0px; "
		"}" );

	QAction *toggleAction = menu.addAction( "Toggle Memory View" );
	menu.addSeparator();
	QAction *hexAction = menu.addAction( "Show Hex Dump" );
	QAction *structAction = menu.addAction( "Show Structure View" );

	QAction *selected = menu.exec( m_memoryToggle->mapToGlobal( pos ) );
	if ( selected == toggleAction ) {
		emit memoryToggleRequested();
	}
}
