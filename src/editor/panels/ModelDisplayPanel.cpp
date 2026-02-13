/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   ModelDisplayPanel.cpp  -  Render toggle and transform controls panel
 *
 * ===========================================================================
 */

#include "ModelDisplayPanel.h"
#include "../widgets/ModelViewport.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QLabel>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

ModelDisplayPanel::ModelDisplayPanel( QWidget *parent )
    : InspectorPanel( parent )
    , m_wireframeCheck( nullptr )
    , m_showGridCheck( nullptr )
    , m_showGroundCheck( nullptr )
    , m_showAxesCheck( nullptr )
    , m_showBonesCheck( nullptr )
    , m_showHitboxesCheck( nullptr )
    , m_rotationXSlider( nullptr )
    , m_rotationYSlider( nullptr )
    , m_rotationZSlider( nullptr )
    , m_meshScaleSpin( nullptr )
    , m_boneScaleSpin( nullptr )
    , m_mirrorXBtn( nullptr )
    , m_mirrorYBtn( nullptr )
    , m_mirrorZBtn( nullptr )
    , m_fovSlider( nullptr )
    , m_weaponOriginBtn( nullptr )
    , m_resetAllBtn( nullptr ) {
	setupUI();

	// Display panel doesn't need a model to be useful
	showPlaceholder( false );
}

void ModelDisplayPanel::setupUI() {
	QVBoxLayout *contentLayout = new QVBoxLayout( m_contentWidget );
	contentLayout->setContentsMargins( 4, 4, 4, 4 );
	contentLayout->setSpacing( 6 );

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

	// 90s style for sliders
	QString sliderStyle =
	    "QSlider::groove:horizontal {"
	    "    background: #ffffff;"
	    "    border: 2px inset;"
	    "    border-top-color: #808080;"
	    "    border-left-color: #808080;"
	    "    border-right-color: #ffffff;"
	    "    border-bottom-color: #ffffff;"
	    "    height: 8px;"
	    "}"
	    "QSlider::handle:horizontal {"
	    "    background: #c0c0c0;"
	    "    border: 2px outset;"
	    "    border-top-color: #ffffff;"
	    "    border-left-color: #ffffff;"
	    "    border-right-color: #808080;"
	    "    border-bottom-color: #808080;"
	    "    width: 12px;"
	    "    margin: -4px 0;"
	    "}";

	// 90s style for spinboxes
	QString spinStyle =
	    "QDoubleSpinBox {"
	    "    background-color: #ffffff;"
	    "    border: 2px inset;"
	    "    border-top-color: #808080;"
	    "    border-left-color: #808080;"
	    "    border-right-color: #ffffff;"
	    "    border-bottom-color: #ffffff;"
	    "    color: #000000;"
	    "    padding: 2px;"
	    "}";

	// 90s style for toggle buttons
	QString toggleBtnStyle =
	    "QPushButton {"
	    "    background-color: #c0c0c0;"
	    "    border: 2px outset;"
	    "    border-top-color: #ffffff;"
	    "    border-left-color: #ffffff;"
	    "    border-right-color: #808080;"
	    "    border-bottom-color: #808080;"
	    "    padding: 4px 8px;"
	    "    color: #000000;"
	    "    font-weight: bold;"
	    "}"
	    "QPushButton:checked {"
	    "    background-color: #a0a0a0;"
	    "    border: 2px inset;"
	    "    border-top-color: #808080;"
	    "    border-left-color: #808080;"
	    "    border-right-color: #ffffff;"
	    "    border-bottom-color: #ffffff;"
	    "}"
	    "QPushButton:hover {"
	    "    background-color: #d4d4d4;"
	    "}";

	QString labelStyle = "QLabel { color: #000000; }";

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

	// ===== Rotation Group =====
	QGroupBox *rotationGroup = new QGroupBox( "Model Rotation", m_contentWidget );
	rotationGroup->setStyleSheet( groupBoxStyle );

	QGridLayout *rotLayout = new QGridLayout( rotationGroup );
	rotLayout->setContentsMargins( 8, 8, 8, 8 );
	rotLayout->setSpacing( 4 );

	// X Rotation
	QLabel *rotXLabel = new QLabel( "X:", rotationGroup );
	rotXLabel->setStyleSheet( labelStyle );
	rotLayout->addWidget( rotXLabel, 0, 0 );

	m_rotationXSlider = new QSlider( Qt::Horizontal, rotationGroup );
	m_rotationXSlider->setRange( -180, 180 );
	m_rotationXSlider->setValue( 0 );
	m_rotationXSlider->setStyleSheet( sliderStyle );
	m_rotationXSlider->setToolTip( "Tilt model forward/back" );
	connect( m_rotationXSlider, &QSlider::valueChanged, this, &ModelDisplayPanel::onRotationXChanged );
	rotLayout->addWidget( m_rotationXSlider, 0, 1 );

	// Y Rotation
	QLabel *rotYLabel = new QLabel( "Y:", rotationGroup );
	rotYLabel->setStyleSheet( labelStyle );
	rotLayout->addWidget( rotYLabel, 1, 0 );

	m_rotationYSlider = new QSlider( Qt::Horizontal, rotationGroup );
	m_rotationYSlider->setRange( -180, 180 );
	m_rotationYSlider->setValue( 0 );
	m_rotationYSlider->setStyleSheet( sliderStyle );
	m_rotationYSlider->setToolTip( "Spin model left/right" );
	connect( m_rotationYSlider, &QSlider::valueChanged, this, &ModelDisplayPanel::onRotationYChanged );
	rotLayout->addWidget( m_rotationYSlider, 1, 1 );

	// Z Rotation
	QLabel *rotZLabel = new QLabel( "Z:", rotationGroup );
	rotZLabel->setStyleSheet( labelStyle );
	rotLayout->addWidget( rotZLabel, 2, 0 );

	m_rotationZSlider = new QSlider( Qt::Horizontal, rotationGroup );
	m_rotationZSlider->setRange( -180, 180 );
	m_rotationZSlider->setValue( 0 );
	m_rotationZSlider->setStyleSheet( sliderStyle );
	m_rotationZSlider->setToolTip( "Tilt model sideways" );
	connect( m_rotationZSlider, &QSlider::valueChanged, this, &ModelDisplayPanel::onRotationZChanged );
	rotLayout->addWidget( m_rotationZSlider, 2, 1 );

	contentLayout->addWidget( rotationGroup );

	// ===== Transform Options Group =====
	QGroupBox *transformGroup = new QGroupBox( "Transform Options", m_contentWidget );
	transformGroup->setStyleSheet( groupBoxStyle );

	QGridLayout *transLayout = new QGridLayout( transformGroup );
	transLayout->setContentsMargins( 8, 8, 8, 8 );
	transLayout->setSpacing( 4 );

	// Mesh Scale
	QLabel *meshScaleLabel = new QLabel( "Mesh Scale:", transformGroup );
	meshScaleLabel->setStyleSheet( labelStyle );
	transLayout->addWidget( meshScaleLabel, 0, 0 );

	m_meshScaleSpin = new QDoubleSpinBox( transformGroup );
	m_meshScaleSpin->setRange( 0.1, 10.0 );
	m_meshScaleSpin->setSingleStep( 0.1 );
	m_meshScaleSpin->setValue( 1.0 );
	m_meshScaleSpin->setDecimals( 1 );
	m_meshScaleSpin->setSuffix( "x" );
	m_meshScaleSpin->setStyleSheet( spinStyle );
	m_meshScaleSpin->setToolTip( "Scale the entire model (0.1x - 10.0x)" );
	connect( m_meshScaleSpin, QOverload<double>::of( &QDoubleSpinBox::valueChanged ),
	         this, &ModelDisplayPanel::onMeshScaleChanged );
	transLayout->addWidget( m_meshScaleSpin, 0, 1 );

	// Bone Scale (for bone overlay visualization)
	QLabel *boneScaleLabel = new QLabel( "Bone Scale:", transformGroup );
	boneScaleLabel->setStyleSheet( labelStyle );
	transLayout->addWidget( boneScaleLabel, 1, 0 );

	m_boneScaleSpin = new QDoubleSpinBox( transformGroup );
	m_boneScaleSpin->setRange( 0.1, 10.0 );
	m_boneScaleSpin->setSingleStep( 0.1 );
	m_boneScaleSpin->setValue( 1.0 );
	m_boneScaleSpin->setDecimals( 1 );
	m_boneScaleSpin->setSuffix( "x" );
	m_boneScaleSpin->setStyleSheet( spinStyle );
	m_boneScaleSpin->setToolTip( "Scale bone overlay visualization (0.1x - 10.0x)" );
	connect( m_boneScaleSpin, QOverload<double>::of( &QDoubleSpinBox::valueChanged ),
	         this, &ModelDisplayPanel::onBoneScaleChanged );
	transLayout->addWidget( m_boneScaleSpin, 1, 1 );

	// Mirror buttons row
	QLabel *mirrorLabel = new QLabel( "Mirror:", transformGroup );
	mirrorLabel->setStyleSheet( labelStyle );
	transLayout->addWidget( mirrorLabel, 2, 0 );

	QHBoxLayout *mirrorLayout = new QHBoxLayout();
	mirrorLayout->setSpacing( 4 );

	m_mirrorXBtn = new QPushButton( "X", transformGroup );
	m_mirrorXBtn->setCheckable( true );
	m_mirrorXBtn->setStyleSheet( toggleBtnStyle );
	m_mirrorXBtn->setToolTip( "Mirror model on X axis" );
	m_mirrorXBtn->setFixedWidth( 32 );
	connect( m_mirrorXBtn, &QPushButton::toggled, this, &ModelDisplayPanel::onMirrorXToggled );
	mirrorLayout->addWidget( m_mirrorXBtn );

	m_mirrorYBtn = new QPushButton( "Y", transformGroup );
	m_mirrorYBtn->setCheckable( true );
	m_mirrorYBtn->setStyleSheet( toggleBtnStyle );
	m_mirrorYBtn->setToolTip( "Mirror model on Y axis" );
	m_mirrorYBtn->setFixedWidth( 32 );
	connect( m_mirrorYBtn, &QPushButton::toggled, this, &ModelDisplayPanel::onMirrorYToggled );
	mirrorLayout->addWidget( m_mirrorYBtn );

	m_mirrorZBtn = new QPushButton( "Z", transformGroup );
	m_mirrorZBtn->setCheckable( true );
	m_mirrorZBtn->setStyleSheet( toggleBtnStyle );
	m_mirrorZBtn->setToolTip( "Mirror model on Z axis" );
	m_mirrorZBtn->setFixedWidth( 32 );
	connect( m_mirrorZBtn, &QPushButton::toggled, this, &ModelDisplayPanel::onMirrorZToggled );
	mirrorLayout->addWidget( m_mirrorZBtn );

	mirrorLayout->addStretch();
	transLayout->addLayout( mirrorLayout, 2, 1 );

	contentLayout->addWidget( transformGroup );

	// ===== Camera Options Group =====
	QGroupBox *cameraGroup = new QGroupBox( "Camera Options", m_contentWidget );
	cameraGroup->setStyleSheet( groupBoxStyle );

	QGridLayout *camLayout = new QGridLayout( cameraGroup );
	camLayout->setContentsMargins( 8, 8, 8, 8 );
	camLayout->setSpacing( 4 );

	// FOV slider
	QLabel *fovLabel = new QLabel( "FOV:", cameraGroup );
	fovLabel->setStyleSheet( labelStyle );
	camLayout->addWidget( fovLabel, 0, 0 );

	m_fovSlider = new QSlider( Qt::Horizontal, cameraGroup );
	m_fovSlider->setRange( 30, 120 );
	m_fovSlider->setValue( 65 );
	m_fovSlider->setStyleSheet( sliderStyle );
	m_fovSlider->setToolTip( "Field of View (30° - 120°)" );
	connect( m_fovSlider, &QSlider::valueChanged, this, &ModelDisplayPanel::onFovChanged );
	camLayout->addWidget( m_fovSlider, 0, 1 );

	// Weapon origin toggle
	m_weaponOriginBtn = new QPushButton( "Weapon Origin View", cameraGroup );
	m_weaponOriginBtn->setCheckable( true );
	m_weaponOriginBtn->setStyleSheet( toggleBtnStyle );
	m_weaponOriginBtn->setToolTip( "Position camera for first-person weapon view" );
	connect( m_weaponOriginBtn, &QPushButton::toggled, this, &ModelDisplayPanel::onWeaponOriginToggled );
	camLayout->addWidget( m_weaponOriginBtn, 1, 0, 1, 2 );

	contentLayout->addWidget( cameraGroup );

	// ===== Reset All Button =====
	m_resetAllBtn = new QPushButton( "Reset All", m_contentWidget );
	m_resetAllBtn->setStyleSheet( toggleBtnStyle );
	m_resetAllBtn->setToolTip( "Reset all display, rotation, and transform settings to defaults" );
	connect( m_resetAllBtn, &QPushButton::clicked, this, &ModelDisplayPanel::onResetAll );
	contentLayout->addWidget( m_resetAllBtn );

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
	m_rotationXSlider->blockSignals( true );
	m_rotationYSlider->blockSignals( true );
	m_rotationZSlider->blockSignals( true );
	m_meshScaleSpin->blockSignals( true );
	m_boneScaleSpin->blockSignals( true );
	m_mirrorXBtn->blockSignals( true );
	m_mirrorYBtn->blockSignals( true );
	m_mirrorZBtn->blockSignals( true );
	m_fovSlider->blockSignals( true );
	m_weaponOriginBtn->blockSignals( true );

	// Sync render toggles
	m_wireframeCheck->setChecked( m_viewport->isWireframeMode() );
	m_showGridCheck->setChecked( m_viewport->isShowGrid() );
	m_showGroundCheck->setChecked( m_viewport->isShowGround() );
	m_showAxesCheck->setChecked( m_viewport->isShowAxes() );
	m_showBonesCheck->setChecked( m_viewport->isShowBones() );
	m_showHitboxesCheck->setChecked( m_viewport->isShowHitboxes() );

	// Sync rotation (convert radians to degrees)
	m_rotationXSlider->setValue( static_cast<int>( m_viewport->getModelRotationX() * 180.0f / M_PI ) );
	m_rotationYSlider->setValue( static_cast<int>( m_viewport->getModelRotationY() * 180.0f / M_PI ) );
	m_rotationZSlider->setValue( static_cast<int>( m_viewport->getModelRotationZ() * 180.0f / M_PI ) );

	// Sync scale
	m_meshScaleSpin->setValue( m_viewport->getMeshScale() );
	m_boneScaleSpin->setValue( m_viewport->getBoneScale() );

	// Sync mirror
	m_mirrorXBtn->setChecked( m_viewport->isMirrorX() );
	m_mirrorYBtn->setChecked( m_viewport->isMirrorY() );
	m_mirrorZBtn->setChecked( m_viewport->isMirrorZ() );

	// Sync camera
	m_fovSlider->setValue( static_cast<int>( m_viewport->getFov() ) );
	m_weaponOriginBtn->setChecked( m_viewport->isWeaponOriginMode() );

	// Unblock signals
	m_wireframeCheck->blockSignals( false );
	m_showGridCheck->blockSignals( false );
	m_showGroundCheck->blockSignals( false );
	m_showAxesCheck->blockSignals( false );
	m_showBonesCheck->blockSignals( false );
	m_showHitboxesCheck->blockSignals( false );
	m_rotationXSlider->blockSignals( false );
	m_rotationYSlider->blockSignals( false );
	m_rotationZSlider->blockSignals( false );
	m_meshScaleSpin->blockSignals( false );
	m_boneScaleSpin->blockSignals( false );
	m_mirrorXBtn->blockSignals( false );
	m_mirrorYBtn->blockSignals( false );
	m_mirrorZBtn->blockSignals( false );
	m_fovSlider->blockSignals( false );
	m_weaponOriginBtn->blockSignals( false );
}

// ============================================================================
// Render toggle slots
// ============================================================================

void ModelDisplayPanel::onWireframeToggled( bool checked ) {
	if ( m_viewport ) {
		m_viewport->setWireframeMode( checked );
		emit viewportStateChanged();
	}
}

void ModelDisplayPanel::onShowGridToggled( bool checked ) {
	if ( m_viewport ) {
		m_viewport->setShowGrid( checked );
		emit viewportStateChanged();
	}
}

void ModelDisplayPanel::onShowGroundToggled( bool checked ) {
	if ( m_viewport ) {
		m_viewport->setShowGround( checked );
		emit viewportStateChanged();
	}
}

void ModelDisplayPanel::onShowAxesToggled( bool checked ) {
	if ( m_viewport ) {
		m_viewport->setShowAxes( checked );
		emit viewportStateChanged();
	}
}

void ModelDisplayPanel::onShowBonesToggled( bool checked ) {
	if ( m_viewport ) {
		m_viewport->setShowBones( checked );
		emit viewportStateChanged();
	}
}

void ModelDisplayPanel::onShowHitboxesToggled( bool checked ) {
	if ( m_viewport ) {
		m_viewport->setShowHitboxes( checked );
		emit viewportStateChanged();
	}
}

// ============================================================================
// Rotation slots
// ============================================================================

void ModelDisplayPanel::onRotationXChanged( int value ) {
	if ( m_viewport ) {
		float radians = static_cast<float>( value ) * M_PI / 180.0f;
		m_viewport->setModelRotationX( radians );
	}
}

void ModelDisplayPanel::onRotationYChanged( int value ) {
	if ( m_viewport ) {
		float radians = static_cast<float>( value ) * M_PI / 180.0f;
		m_viewport->setModelRotationY( radians );
	}
}

void ModelDisplayPanel::onRotationZChanged( int value ) {
	if ( m_viewport ) {
		float radians = static_cast<float>( value ) * M_PI / 180.0f;
		m_viewport->setModelRotationZ( radians );
	}
}

// ============================================================================
// Scale slots
// ============================================================================

void ModelDisplayPanel::onMeshScaleChanged( double value ) {
	if ( m_viewport ) {
		m_viewport->setMeshScale( static_cast<float>( value ) );
	}
}

void ModelDisplayPanel::onBoneScaleChanged( double value ) {
	if ( m_viewport ) {
		m_viewport->setBoneScale( static_cast<float>( value ) );
	}
}

// ============================================================================
// Mirror slots
// ============================================================================

void ModelDisplayPanel::onMirrorXToggled( bool checked ) {
	if ( m_viewport ) {
		m_viewport->setMirrorX( checked );
	}
}

void ModelDisplayPanel::onMirrorYToggled( bool checked ) {
	if ( m_viewport ) {
		m_viewport->setMirrorY( checked );
	}
}

void ModelDisplayPanel::onMirrorZToggled( bool checked ) {
	if ( m_viewport ) {
		m_viewport->setMirrorZ( checked );
	}
}

// ============================================================================
// Camera slots
// ============================================================================

void ModelDisplayPanel::onFovChanged( int value ) {
	if ( m_viewport ) {
		m_viewport->setFov( static_cast<float>( value ) );
	}
}

void ModelDisplayPanel::onWeaponOriginToggled( bool checked ) {
	if ( m_viewport ) {
		m_viewport->setWeaponOriginMode( checked );
	}
}

// ============================================================================
// Reset slot
// ============================================================================

void ModelDisplayPanel::onResetAll() {
	if ( !m_viewport ) return;

	// Reset rotation
	m_viewport->resetModelRotation();

	// Reset scale
	m_viewport->setMeshScale( 1.0f );
	m_viewport->setBoneScale( 1.0f );

	// Reset mirror
	m_viewport->setMirrorX( false );
	m_viewport->setMirrorY( false );
	m_viewport->setMirrorZ( false );

	// Reset FOV
	m_viewport->setFov( 65.0f );

	// Reset weapon origin mode
	m_viewport->setWeaponOriginMode( false );

	// Sync UI
	syncWithViewport();
}
