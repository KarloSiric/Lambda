/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   ModelDisplayPanel.cpp  -  Render toggle controls panel implementation
 *
 * ===========================================================================
 */

#include "ModelDisplayPanel.h"
#include "../widgets/ModelViewport.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QLabel>

ModelDisplayPanel::ModelDisplayPanel( QWidget *parent )
    : InspectorPanel( parent )
    , m_wireframeCheck( nullptr )
    , m_showGridCheck( nullptr )
    , m_showGroundCheck( nullptr )
    , m_showAxesCheck( nullptr )
    , m_showBonesCheck( nullptr )
    , m_showHitboxesCheck( nullptr ) {
	setupUI();

	// Display panel doesn't need a model to be useful
	showPlaceholder( false );
}

void ModelDisplayPanel::setupUI() {
	QVBoxLayout *contentLayout = new QVBoxLayout( m_contentWidget );
	contentLayout->setContentsMargins( 4, 4, 4, 4 );
	contentLayout->setSpacing( 8 );

	// 90s Windows 95 style for checkboxes
	QString checkStyle =
	    "QCheckBox {"
	    "    color: #000000;"
	    "    spacing: 6px;"
	    "}"
	    "QCheckBox::indicator {"
	    "    width: 13px;"
	    "    height: 13px;"
	    "    border: 2px inset;"
	    "    border-top-color: #808080;"
	    "    border-left-color: #808080;"
	    "    border-right-color: #ffffff;"
	    "    border-bottom-color: #ffffff;"
	    "    background-color: #ffffff;"
	    "}"
	    "QCheckBox::indicator:checked {"
	    "    background-color: #ffffff;"
	    "}";

	// 90s Windows 95 style for group boxes
	QString groupBoxStyle =
	    "QGroupBox {"
	    "    background-color: #c0c0c0;"
	    "    border: 2px groove #ffffff;"
	    "    border-top-color: #808080;"
	    "    border-left-color: #808080;"
	    "    border-right-color: #ffffff;"
	    "    border-bottom-color: #ffffff;"
	    "    margin-top: 14px;"
	    "    padding: 8px 4px 4px 4px;"
	    "    font-weight: bold;"
	    "    color: #000000;"
	    "}"
	    "QGroupBox::title {"
	    "    subcontrol-origin: margin;"
	    "    subcontrol-position: top left;"
	    "    padding: 2px 6px;"
	    "    background-color: #c0c0c0;"
	    "    color: #000000;"
	    "}";

	// ===== Render Options Group =====
	QGroupBox *renderGroup = new QGroupBox( "Render Options", m_contentWidget );
	renderGroup->setStyleSheet( groupBoxStyle );

	QGridLayout *renderLayout = new QGridLayout( renderGroup );
	renderLayout->setContentsMargins( 8, 8, 8, 8 );
	renderLayout->setSpacing( 6 );

	// Row 0
	m_wireframeCheck = new QCheckBox( "Wireframe", renderGroup );
	m_wireframeCheck->setStyleSheet( checkStyle );
	m_wireframeCheck->setToolTip( "Render model as wireframe mesh" );
	connect( m_wireframeCheck, &QCheckBox::toggled, this, &ModelDisplayPanel::onWireframeToggled );
	renderLayout->addWidget( m_wireframeCheck, 0, 0 );

	m_showGridCheck = new QCheckBox( "Show Grid", renderGroup );
	m_showGridCheck->setStyleSheet( checkStyle );
	m_showGridCheck->setToolTip( "Display reference grid on the floor" );
	connect( m_showGridCheck, &QCheckBox::toggled, this, &ModelDisplayPanel::onShowGridToggled );
	renderLayout->addWidget( m_showGridCheck, 0, 1 );

	// Row 1
	m_showGroundCheck = new QCheckBox( "Show Ground", renderGroup );
	m_showGroundCheck->setStyleSheet( checkStyle );
	m_showGroundCheck->setToolTip( "Display solid ground plane" );
	connect( m_showGroundCheck, &QCheckBox::toggled, this, &ModelDisplayPanel::onShowGroundToggled );
	renderLayout->addWidget( m_showGroundCheck, 1, 0 );

	m_showAxesCheck = new QCheckBox( "Show Axes", renderGroup );
	m_showAxesCheck->setStyleSheet( checkStyle );
	m_showAxesCheck->setToolTip( "Display XYZ coordinate axes at origin" );
	connect( m_showAxesCheck, &QCheckBox::toggled, this, &ModelDisplayPanel::onShowAxesToggled );
	renderLayout->addWidget( m_showAxesCheck, 1, 1 );

	// Row 2
	m_showBonesCheck = new QCheckBox( "Show Bones", renderGroup );
	m_showBonesCheck->setStyleSheet( checkStyle );
	m_showBonesCheck->setToolTip( "Display bone skeleton overlay" );
	connect( m_showBonesCheck, &QCheckBox::toggled, this, &ModelDisplayPanel::onShowBonesToggled );
	renderLayout->addWidget( m_showBonesCheck, 2, 0 );

	m_showHitboxesCheck = new QCheckBox( "Show Hitboxes", renderGroup );
	m_showHitboxesCheck->setStyleSheet( checkStyle );
	m_showHitboxesCheck->setToolTip( "Display hitbox collision volumes" );
	connect( m_showHitboxesCheck, &QCheckBox::toggled, this, &ModelDisplayPanel::onShowHitboxesToggled );
	renderLayout->addWidget( m_showHitboxesCheck, 2, 1 );

	contentLayout->addWidget( renderGroup );

	// Separator
	QFrame *separator = new QFrame( m_contentWidget );
	separator->setFrameShape( QFrame::HLine );
	separator->setStyleSheet( "background-color: #808080;" );
	contentLayout->addWidget( separator );

	// Info label
	QLabel *infoLabel = new QLabel( m_contentWidget );
	infoLabel->setText( "These options affect viewport rendering only.\nHotkeys: G=Grid, F=Ground, B=Bones, H=Hitboxes" );
	infoLabel->setWordWrap( true );
	infoLabel->setStyleSheet(
	    "QLabel {"
	    "    color: #000000;"
	    "    background-color: #ffffff;"
	    "    padding: 6px;"
	    "    border: 2px inset;"
	    "    border-top-color: #808080;"
	    "    border-left-color: #808080;"
	    "    border-right-color: #ffffff;"
	    "    border-bottom-color: #ffffff;"
	    "}" );
	contentLayout->addWidget( infoLabel );

	// Add stretch at the end
	contentLayout->addStretch();
}

void ModelDisplayPanel::refresh() {
	syncWithViewport();
}

void ModelDisplayPanel::syncWithViewport() {
	if ( !m_viewport ) return;

	// Block signals to prevent feedback loop
	m_wireframeCheck->blockSignals( true );
	m_showGridCheck->blockSignals( true );
	m_showGroundCheck->blockSignals( true );
	m_showAxesCheck->blockSignals( true );
	m_showBonesCheck->blockSignals( true );
	m_showHitboxesCheck->blockSignals( true );

	m_wireframeCheck->setChecked( m_viewport->isWireframeMode() );
	m_showGridCheck->setChecked( m_viewport->isShowGrid() );
	m_showGroundCheck->setChecked( m_viewport->isShowGround() );
	m_showAxesCheck->setChecked( m_viewport->isShowAxes() );
	m_showBonesCheck->setChecked( m_viewport->isShowBones() );
	m_showHitboxesCheck->setChecked( m_viewport->isShowHitboxes() );

	m_wireframeCheck->blockSignals( false );
	m_showGridCheck->blockSignals( false );
	m_showGroundCheck->blockSignals( false );
	m_showAxesCheck->blockSignals( false );
	m_showBonesCheck->blockSignals( false );
	m_showHitboxesCheck->blockSignals( false );
}

void ModelDisplayPanel::onWireframeToggled( bool checked ) {
	if ( m_viewport ) {
		m_viewport->setWireframeMode( checked );
	}
}

void ModelDisplayPanel::onShowGridToggled( bool checked ) {
	if ( m_viewport ) {
		m_viewport->setShowGrid( checked );
	}
}

void ModelDisplayPanel::onShowGroundToggled( bool checked ) {
	if ( m_viewport ) {
		m_viewport->setShowGround( checked );
	}
}

void ModelDisplayPanel::onShowAxesToggled( bool checked ) {
	if ( m_viewport ) {
		m_viewport->setShowAxes( checked );
	}
}

void ModelDisplayPanel::onShowBonesToggled( bool checked ) {
	if ( m_viewport ) {
		m_viewport->setShowBones( checked );
	}
}

void ModelDisplayPanel::onShowHitboxesToggled( bool checked ) {
	if ( m_viewport ) {
		m_viewport->setShowHitboxes( checked );
	}
}
