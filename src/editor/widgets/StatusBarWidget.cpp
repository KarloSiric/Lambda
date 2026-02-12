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
#include <QResizeEvent>
#include <QtCore/qobject.h>
#include <QtGui/qicon.h>

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
	setSizeGripEnabled( false );
	setupUI();
	applyStyles();
	// Must be called AFTER applyStyles() — CSS padding would override it if called before.
	// 12px left is the original working value; positive top/bottom replace the original -2/-1.
	setContentsMargins( 12, 0, 4, 0 );
}

StatusBarWidget::~StatusBarWidget() {
}

void StatusBarWidget::setupUI() {
	createModelInfoWidgets();
	createViewInfoWidgets();
	createToggleButtons();
}

void StatusBarWidget::createModelInfoWidgets() {
	// File path with smart truncation
	m_filePathLabel = new QLabel( this );
	m_filePathLabel->setMinimumWidth( 200 );
	m_filePathLabel->setMaximumWidth( 500 );
	addWidget( m_filePathLabel );
	addWidget( createSeparator() );

	// Essential model statistics
	m_vertexCountLabel = new QLabel( this );
	m_vertexCountLabel->setToolTip( "Model/Object Vertex Count" );
	m_vertexCountLabel->setFixedWidth( 90 );
	addWidget( m_vertexCountLabel );
	addWidget( createSeparator() );

	m_triangleCountLabel = new QLabel( this );
	m_triangleCountLabel->setToolTip( "Model/Object Polygon count" );
	m_triangleCountLabel->setFixedWidth( 90 );
	addWidget( m_triangleCountLabel );
	addWidget( createSeparator() );

	m_boneCountLabel = new QLabel( this );
	m_boneCountLabel->setToolTip( "Model/Object Bone count" );
	m_boneCountLabel->setFixedWidth( 85 );
	addWidget( m_boneCountLabel );
	addWidget( createSeparator() );

	m_fileSizeLabel = new QLabel( this );
	m_fileSizeLabel->setToolTip( "File size" );
	m_fileSizeLabel->setFixedWidth( 120 );
	addWidget( m_fileSizeLabel );
	addWidget( createSeparator() );

	// Placeholder widgets kept for API compatibility (never shown in status bar)
	m_sequenceCountLabel = new QLabel( this );
	m_textureCountLabel = new QLabel( this );
	m_sequenceInfoLabel = new QLabel( this );
	m_bodygroupLabel = new QLabel( this );
	m_skinLabel = new QLabel( this );
	m_attachmentLabel = new QLabel( this );
	m_eventLabel = new QLabel( this );
	m_boneNameLabel = new QLabel( this );
	m_controllerNameLabel = new QLabel( this );

	m_sequenceCountLabel->hide();
	m_textureCountLabel->hide();
	m_sequenceInfoLabel->hide();
	m_bodygroupLabel->hide();
	m_skinLabel->hide();
	m_attachmentLabel->hide();
	m_eventLabel->hide();
	m_boneNameLabel->hide();
	m_controllerNameLabel->hide();

	// Stretch spacer between normal (left) and permanent (right) sections.
	// MUST be addWidget (not addPermanentWidget) so it sits in the normal section
	// and acts as the natural divider. Using addPermanentWidget here puts the spacer
	// inside the right section, causing the two sections to overlap when their
	// combined minimum width exceeds the status bar width.
	QWidget *spacer = new QWidget( this );
	addWidget( spacer, 1 );
}

void StatusBarWidget::createViewInfoWidgets() {
	// Performance metrics - Empty by default
	m_fpsLabel = new QLabel( this );
	m_fpsLabel->setToolTip( "Frames per second" );
	m_fpsLabel->setFixedWidth( 65 );
	addPermanentWidget( m_fpsLabel );
	addPermanentWidget( createSeparator() );

	m_cpuUsageLabel = new QLabel( this );
	m_gpuUsageLabel = new QLabel( this );
	m_ramUsageLabel = new QLabel( this );

	// Viewport essentials - Empty by default, FIXED size so they don't shrink
	m_gridSizeLabel = new QLabel( this );
	m_gridSizeLabel->setToolTip( "Grid size" );
	m_gridSizeLabel->setFixedSize( 65, 20 );
	addPermanentWidget( m_gridSizeLabel );
	addPermanentWidget( createSeparator() );

	m_zoomLabel = new QLabel( this );
	m_zoomLabel->setToolTip( "Zoom level" );
	m_zoomLabel->setFixedSize( 90, 20 );
	addPermanentWidget( m_zoomLabel );
	addPermanentWidget( createSeparator() );

	// Create placeholder widgets for API compatibility (hidden - belong in Inspector/overlays)
	m_frameTimeLabel = new QLabel( this );
	m_resolutionLabel = new QLabel( this );
	m_viewModeLabel = new QLabel( this );
	m_fovLabel = new QLabel( this );
	m_playbackSpeedLabel = new QLabel( this );

	// Mouse/cursor 3D position - Empty by default, FIXED size
	m_cameraPosLabel = new QLabel( this );
	m_cameraPosLabel->setToolTip( "Cursor position (3D)" );
	m_cameraPosLabel->setFixedSize( 200, 20 );
	addPermanentWidget( m_cameraPosLabel );
	addPermanentWidget( createSeparator() );

	// Viewport size - Empty by default, FIXED size
	m_modelViewportWidthHeight = new QLabel( this );
	m_modelViewportWidthHeight->setToolTip( "Model Viewport Window Resolution" );
	m_modelViewportWidthHeight->setFixedSize( 140, 20 );
	addPermanentWidget( m_modelViewportWidthHeight );
	addPermanentWidget( createSeparator() );

	// Dynamic help text section (like J.A.C.K's "Press F1 for Help")
	m_helpLabel = new QLabel( "Press F1 for Help", this );
	m_helpLabel->setToolTip( "Context-sensitive help (press F1)" );
	m_helpLabel->setMinimumWidth( 120 );
	addPermanentWidget( m_helpLabel );

	m_cameraDistLabel = new QLabel( this );

	// Hide non-essential viewport info
	m_frameTimeLabel->hide();
	m_cpuUsageLabel->hide();
	m_gpuUsageLabel->hide();
	m_ramUsageLabel->hide();
	m_resolutionLabel->hide();
	m_viewModeLabel->hide();
	m_fovLabel->hide();
	m_playbackSpeedLabel->hide();
	m_cameraDistLabel->hide();
}

void StatusBarWidget::createToggleButtons() {
	// Separator before toggle buttons section
	addPermanentWidget( createSeparator() );

	// Small spacer after separator
	QWidget *leftSpacer = new QWidget();
	leftSpacer->setFixedWidth( 4 );
	addPermanentWidget( leftSpacer );

	// Console toggle FIRST (before inspector)
	m_consoleToggle = new QPushButton();
	m_consoleToggle->setIcon( QIcon( ":/icons/toggle-console-memory-bottom-dock-icon.png" ) );
	m_consoleToggle->setIconSize( QSize( 22, 22 ) );  // Bigger icons for better visibility
	m_consoleToggle->setFixedSize( 32, 26 );  // Larger button size
	m_consoleToggle->setCheckable( true );
	m_consoleToggle->setChecked( true );
	m_consoleToggle->setToolTip( "Console Panel (Right-click for options)" );
	m_consoleToggle->setContextMenuPolicy( Qt::CustomContextMenu );
	connect( m_consoleToggle, &QPushButton::clicked, this, &StatusBarWidget::onConsoleButtonClicked );
	connect( m_consoleToggle, &QPushButton::customContextMenuRequested, this, &StatusBarWidget::showConsoleContextMenu );
	addPermanentWidget( m_consoleToggle );

	// Inspector toggle SECOND
	m_inspectorToggle = new QPushButton();
	m_inspectorToggle->setIcon( QIcon( ":/icons/toggle-inspector-icon.png" ) );
	m_inspectorToggle->setIconSize( QSize( 22, 22 ) );  // Bigger icons for better visibility
	m_inspectorToggle->setFixedSize( 32, 26 );  // Larger button size
	m_inspectorToggle->setCheckable( true );
	m_inspectorToggle->setChecked( true );
	m_inspectorToggle->setToolTip( "Inspector Panel (Right-click for options)" );
	m_inspectorToggle->setContextMenuPolicy( Qt::CustomContextMenu );
	connect( m_inspectorToggle, &QPushButton::clicked, this, &StatusBarWidget::onInspectorButtonClicked );
	connect( m_inspectorToggle, &QPushButton::customContextMenuRequested, this, &StatusBarWidget::showInspectorContextMenu );
	addPermanentWidget( m_inspectorToggle );

	// Memory toggle REMOVED - memory is part of console now

	// Add small spacer after buttons to prevent cutoff at right edge
	QWidget *rightSpacer = new QWidget();
	rightSpacer->setFixedWidth( 4 );
	addPermanentWidget( rightSpacer );
}

void StatusBarWidget::applyStyles() {
	// Enhanced status bar with prominent 3D raised border to match panel aesthetic
	// Note: QStatusBar::item has no padding to allow full-height separators
	setStyleSheet(
		"QStatusBar { "
		"    background-color: #c0c0c0; "
		"    color: #202020; "
		"    border: 1px solid; "
		"    border-top-color: #ffffff; "
		"    border-left-color: #ffffff; "
		"    border-right-color: #808080; "
		"    border-bottom-color: #808080; "
		"    padding: 0px 2px; "
		"} "
		"QStatusBar::item { border: none; padding: 0px; margin: 0px; }" );

	// Adding style for the items in the status bar
	const QString cellStyle =
		"QLabel { "
		"   padding: 2px 6px; "  // Added vertical padding for text labels
		"   color: #101010; "
		"   background-color: #c0c0c0; "
		"}";

	m_filePathLabel->setStyleSheet( cellStyle );

	// // File path with sunken panel look
	// m_filePathLabel->setStyleSheet(
	// 	"QLabel { "
	// 	"    padding: 2px 4px; "
	// 	"    color: #101010; "
	// 	"    background-color: #e8e8e8; "
	// 	"    border: 1px solid #808080; "
	// 	"    border-top-color: #808080; "
	// 	"    border-left-color: #808080; "
	// 	"    border-right-color: #f0f0f0; "
	// 	"    border-bottom-color: #f0f0f0; "
	// 	"}" );

	// Classic sunken panel style for visible info labels
	// QString infoLabelStyle =
	// 	"QLabel { "
	// 	"    padding: 2px 6px; "
	// 	"    color: #202020; "
	// 	"    background-color: #e8e8e8; "
	// 	"    border: 2px solid; "
	// 	"    border-top-color: #808080; "
	// 	"    border-left-color: #808080; "
	// 	"    border-right-color: #f0f0f0; "
	// 	"    border-bottom-color: #f0f0f0; "
	// 	"}";

	// Apply to all visible model info widgets

	m_filePathLabel->setStyleSheet( cellStyle );
    m_vertexCountLabel->setStyleSheet( cellStyle );
	m_triangleCountLabel->setStyleSheet( cellStyle );
	m_boneCountLabel->setStyleSheet( cellStyle );
	m_fileSizeLabel->setStyleSheet( cellStyle );

	m_fpsLabel->setStyleSheet( cellStyle );
	m_cpuUsageLabel->setStyleSheet( cellStyle );
	m_gpuUsageLabel->setStyleSheet( cellStyle );
	m_ramUsageLabel->setStyleSheet( cellStyle );
	m_gridSizeLabel->setStyleSheet( cellStyle );
	m_zoomLabel->setStyleSheet( cellStyle );
	m_cameraPosLabel->setStyleSheet( cellStyle );
	m_modelViewportWidthHeight->setStyleSheet( cellStyle );

	// Help label with bold AND italic font to stand out when showing dynamic tooltips
	QString helpStyle =
		"QLabel { "
		"   padding: 2px 6px; "
		"   color: #202020; "
		"   background-color: #c0c0c0; "
		"   font-weight: bold; "
		"   font-style: italic; "
		"}";
	if ( m_helpLabel ) {
		m_helpLabel->setStyleSheet( helpStyle );
	}

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
	// m_copyPathButton->setStyleSheet( copyButtonStyle );
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
	int maxWidth = m_filePathLabel->width() - 12; // Account for padding

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
	m_triangleCountLabel->setText( QString( "Polys: %L1" ).arg( triangleCount ) );
	m_boneCountLabel->setText( QString( "Bones: %1" ).arg( boneCount ) );
	m_sequenceCountLabel->setText( QString( "Seqs: %1" ).arg( sequenceCount ) );
	m_textureCountLabel->setText( QString( "Tex: %1" ).arg( textureCount ) );
}

void StatusBarWidget::setFileSize( qint64 sizeBytes ) {
	if ( sizeBytes < 1024 ) {
		m_fileSizeLabel->setText( QString( "Size: %1 B" ).arg( sizeBytes ) );
	} else if ( sizeBytes < 1024 * 1024 ) {
		m_fileSizeLabel->setText( QString( "Size: %1 KB" ).arg( sizeBytes / 1024.0, 0, 'f', 1 ) );
	} else if ( sizeBytes < 1024 * 1024 * 1024 ){
		m_fileSizeLabel->setText( QString( "Size: %1 MB" ).arg( sizeBytes / ( 1024.0 * 1024.0 ), 0, 'f', 2 ) );
	} else {
        m_fileSizeLabel->setText( QString( "Size: %1 GB" ).arg( sizeBytes / ( 1024.0 * 1024.0 * 1024.0), 0, 'f', 2 ) ); 
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
	// Only show if size is valid (non-zero)
	if ( size > 0.0f ) {
		m_gridSizeLabel->setText( QString( "Grid: %1" ).arg( size, 0, 'f', 1 ) );
	} else {
		m_gridSizeLabel->setText( "" );
	}
}


void StatusBarWidget::setResolution( int width, int height ) {
	m_viewportWidth = width;
	m_viewportHeight = height;
	m_resolutionLabel->setText( QString( "Res: %1x%2" ).arg( width ).arg( height ) );
}

void StatusBarWidget::setViewportSize( int width, int height ) {
	m_viewportWidth = width;
	m_viewportHeight = height;
	// Only show if viewport has valid dimensions (mouse is in viewport)
	if ( width > 0 && height > 0 ) {
		m_modelViewportWidthHeight->setText( QString( "@ W: %1 H:%2" ).arg( width ).arg( height ) );
	} else {
		m_modelViewportWidthHeight->setText( "" );
	}
}


void StatusBarWidget::setFPS( int fps ) {
	m_currentFPS = fps;
	m_fpsLabel->setText( QString( "FPS: %1" ).arg( fps ) );

	// Classic style with color coding for performance
	QString baseStyle =
		"QLabel { "
		"    padding: 3px 6px; "
		// "    background-color: #e8e8e8; "
		"    background-color: #212121; "
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
	// Only show position if at least one value is non-zero (mouse is in viewport)
	if ( x != 0.0f || y != 0.0f || z != 0.0f ) {
		m_cameraPosLabel->setText( QString( "x: %1, y: %2, z: %3" )
			.arg( x, 0, 'f', 1 ).arg( y, 0, 'f', 1 ).arg( z, 0, 'f', 1 ) );
	} else {
		m_cameraPosLabel->setText( "" );
	}
}


void StatusBarWidget::setFOV( float fov ) {
	m_fov = fov;
	m_fovLabel->setText( QString( "FOV: %1°" ).arg( fov, 0, 'f', 0 ) );
}

void StatusBarWidget::setZoomLevel( float zoom ) {
	m_zoom = zoom;
	// Only show if zoom is valid (non-zero)
	// Display as multiplier (e.g., 1.23 instead of 123)
	if ( zoom > 0.0f ) {
		float multiplier = zoom / 100.0f;  // Convert to multiplier
		m_zoomLabel->setText( QString( "Zoom: %1" ).arg( multiplier, 0, 'f', 2 ) );
	} else {
		m_zoomLabel->setText( "" );
	}
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

	// Clear all labels (empty string, not "--" - like J.A.C.K editor)
	m_filePathLabel->setText( "" );
	m_filePathLabel->setToolTip( "" );
	m_vertexCountLabel->setText( "" );
	m_triangleCountLabel->setText( "" );
	m_boneCountLabel->setText( "" );
	m_sequenceCountLabel->setText( "" );
	m_textureCountLabel->setText( "" );
	m_fileSizeLabel->setText( "" );
	m_sequenceInfoLabel->setText( "" );
	m_boneNameLabel->setText( "" );
	m_controllerNameLabel->setText( "" );
	m_bodygroupLabel->setText( "" );
	m_skinLabel->setText( "" );
	m_attachmentLabel->setText( "" );
	m_eventLabel->setText( "" );

	// Also clear position and viewport labels when no model
	m_cameraPosLabel->setText( "" );
	m_modelViewportWidthHeight->setText( "" );
}

void StatusBarWidget::setInspectorVisible( bool visible ) {
	m_inspectorToggle->setChecked( visible );
}

void StatusBarWidget::setConsoleVisible( bool visible ) {
	m_consoleToggle->setChecked( visible );
}

void StatusBarWidget::onInspectorButtonClicked() {
	emit inspectorToggleRequested();
}

void StatusBarWidget::onConsoleButtonClicked() {
	emit consoleToggleRequested();
}

void StatusBarWidget::onCopyFilePathClicked() {
	if ( !m_currentFilePath.isEmpty() ) {
		QClipboard *clipboard = QApplication::clipboard();
		clipboard->setText( m_currentFilePath );
	}
}

void StatusBarWidget::resizeEvent( QResizeEvent *event ) {
	QStatusBar::resizeEvent( event );
	updateWidgetVisibility();
}

void StatusBarWidget::updateWidgetVisibility() {
	int availableWidth = width();

	// Responsive visibility optimized for half-screen (960px at 1920px resolution)
	// Level 1 (always): File path, FPS, toggles
	// Level 2 (700px+): Essential model stats + viewport controls (Verts, Tris, Bones, Size, Grid, Zoom)
	// Level 3 (1100px+): System stats (CPU, GPU, RAM)

	bool showLevel2 = availableWidth >= 700; // Essential model stats + viewport controls
	bool showLevel3 = availableWidth >= 1100; // System stats (nice to have)

	// Level 2: Essential model stats (always visible at half-screen and above)
	m_vertexCountLabel->setVisible( showLevel2 );
	m_triangleCountLabel->setVisible( showLevel2 );
	m_boneCountLabel->setVisible( showLevel2 );
	m_fileSizeLabel->setVisible( showLevel2 );

	// Level 2: Critical viewport controls (Grid and Zoom are essential!)
	m_gridSizeLabel->setVisible( showLevel2 );
	m_zoomLabel->setVisible( showLevel2 );

	// Level 3: System stats - CPU/GPU/RAM labels are not in the layout,
	// so they must NOT be shown via setVisible here (they'd appear at 0,0).
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

	QAction *selected = menu.exec( m_consoleToggle->mapToGlobal( pos ) );
	if ( selected == toggleAction ) {
		emit consoleToggleRequested();
	}
}

QWidget* StatusBarWidget::createSeparator( QWidget *parent ) {
	QFrame* separator = new QFrame( parent ? parent : this );
	separator->setFrameShape( QFrame::NoFrame );  // Let stylesheet handle everything
	separator->setFrameShadow( QFrame::Plain );
	separator->setFixedWidth( 3 );  // 3px total width for the groove
	separator->setContentsMargins( 0, 0, 0, 0 );
	// Classic Windows groove: dark line | light line | middle (creates depth)
	separator->setStyleSheet(
		"QFrame { "
		"    background-color: #c0c0c0; "
		"    border-left: 1px solid #404040; "    // Dark shadow
		"    border-right: 1px solid #ffffff; "   // Light highlight
		"    margin: 0px; "
		"    padding: 0px; "
		"}" );
	return separator;
}

void StatusBarWidget::setHelpText( const QString &text ) {
	if ( m_helpLabel ) {
		m_helpLabel->setText( text );
	}
}

void StatusBarWidget::clearHelpText() {
	if ( m_helpLabel ) {
		m_helpLabel->setText( "Press F1 for Help" );
	}
}

void StatusBarWidget::clearViewportInfo() {
    // @Note( Karlo )
	//         Clear all viewport-related labels when mouse leaves viewport
	//         They should be EMPTY, not showing "0" or default values

	m_gridSizeLabel->setText( "" );
	m_zoomLabel->setText( "" );
	m_cameraPosLabel->setText( "" );
}
