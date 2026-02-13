/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   CameraPanel.cpp  -  Camera control panel implementation
 *
 * ===========================================================================
 */

#include "CameraPanel.h"
#include "../widgets/ModelViewport.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QGroupBox>

CameraPanel::CameraPanel( QWidget *parent )
    : InspectorPanel( parent )
    , m_fovSlider( nullptr )
    , m_fovLabel( nullptr )
    , m_distanceSpinBox( nullptr )
    , m_positionLabel( nullptr )
    , m_resetCameraBtn( nullptr )
    , m_centerModelBtn( nullptr ) {
	setupUI();
}

void CameraPanel::setupUI() {
	QVBoxLayout *contentLayout = new QVBoxLayout( m_contentWidget );
	contentLayout->setContentsMargins( 4, 4, 4, 4 );
	contentLayout->setSpacing( 8 );

	// === FOV Control Section ===
	QGroupBox *fovGroup = new QGroupBox( "Field of View", m_contentWidget );
	fovGroup->setStyleSheet(
	    "QGroupBox {"
	    "    font-weight: bold;"
	    "    color: #000000;"
	    "    border: 2px groove;"
	    "    border-color: #ffffff #808080 #808080 #ffffff;"
	    "    margin-top: 8px;"
	    "    padding: 4px;"
	    "}"
	    "QGroupBox::title {"
	    "    subcontrol-origin: margin;"
	    "    left: 8px;"
	    "    padding: 0 4px;"
	    "}" );

	QHBoxLayout *fovLayout = new QHBoxLayout( fovGroup );
	fovLayout->setSpacing( 8 );

	m_fovSlider = new QSlider( Qt::Horizontal, fovGroup );
	m_fovSlider->setRange( 10, 120 );
	m_fovSlider->setValue( 65 );  // Default FOV
	m_fovSlider->setTickPosition( QSlider::TicksBelow );
	m_fovSlider->setTickInterval( 10 );
	m_fovSlider->setToolTip( "Adjust camera field of view (10-120 degrees)" );
	connect( m_fovSlider, &QSlider::valueChanged, this, &CameraPanel::onFovChanged );

	m_fovLabel = new QLabel( "65\u00B0", fovGroup );  // degree symbol
	m_fovLabel->setMinimumWidth( 40 );
	m_fovLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
	m_fovLabel->setStyleSheet( "color: #000000; font-weight: bold;" );

	fovLayout->addWidget( m_fovSlider, 1 );
	fovLayout->addWidget( m_fovLabel );

	contentLayout->addWidget( fovGroup );

	// === Distance Control Section ===
	QGroupBox *distGroup = new QGroupBox( "Camera Distance", m_contentWidget );
	distGroup->setStyleSheet( fovGroup->styleSheet() );

	QHBoxLayout *distLayout = new QHBoxLayout( distGroup );
	distLayout->setSpacing( 8 );

	QLabel *distLabel = new QLabel( "Distance:", distGroup );
	distLabel->setStyleSheet( "color: #000000;" );

	m_distanceSpinBox = new QDoubleSpinBox( distGroup );
	m_distanceSpinBox->setRange( 1.0, 500.0 );
	m_distanceSpinBox->setValue( 35.0 );  // Default distance
	m_distanceSpinBox->setSingleStep( 5.0 );
	m_distanceSpinBox->setSuffix( " units" );
	m_distanceSpinBox->setToolTip( "Camera distance from model origin" );
	connect( m_distanceSpinBox, QOverload<double>::of( &QDoubleSpinBox::valueChanged ),
	         this, &CameraPanel::onDistanceChanged );

	distLayout->addWidget( distLabel );
	distLayout->addWidget( m_distanceSpinBox, 1 );

	contentLayout->addWidget( distGroup );

	// === Position Display Section ===
	QGroupBox *posGroup = new QGroupBox( "Camera Position", m_contentWidget );
	posGroup->setStyleSheet( fovGroup->styleSheet() );

	QVBoxLayout *posLayout = new QVBoxLayout( posGroup );

	m_positionLabel = new QLabel( "X: 0.00  Y: 0.00  Z: 0.00", posGroup );
	m_positionLabel->setStyleSheet(
	    "color: #000000;"
	    "background-color: #ffffff;"
	    "border: 1px inset;"
	    "border-color: #808080 #ffffff #ffffff #808080;"
	    "padding: 4px;"
	    "font-family: monospace;" );
	m_positionLabel->setAlignment( Qt::AlignCenter );

	posLayout->addWidget( m_positionLabel );

	contentLayout->addWidget( posGroup );

	// Separator
	QFrame *separator = new QFrame( m_contentWidget );
	separator->setFrameShape( QFrame::HLine );
	separator->setStyleSheet( "background-color: #808080;" );
	contentLayout->addWidget( separator );

	// === Action Buttons Section ===
	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->setSpacing( 8 );

	m_resetCameraBtn = new QPushButton( "Reset Camera", m_contentWidget );
	m_resetCameraBtn->setToolTip( "Reset camera to default position and orientation" );
	m_resetCameraBtn->setStyleSheet(
	    "QPushButton {"
	    "    background-color: #c0c0c0;"
	    "    color: #000000;"
	    "    border: 2px solid;"
	    "    border-top-color: #ffffff;"
	    "    border-left-color: #ffffff;"
	    "    border-right-color: #808080;"
	    "    border-bottom-color: #808080;"
	    "    padding: 4px 12px;"
	    "    min-width: 80px;"
	    "}"
	    "QPushButton:hover {"
	    "    background-color: #d0d0d0;"
	    "}"
	    "QPushButton:pressed {"
	    "    border-top-color: #808080;"
	    "    border-left-color: #808080;"
	    "    border-right-color: #ffffff;"
	    "    border-bottom-color: #ffffff;"
	    "}" );
	connect( m_resetCameraBtn, &QPushButton::clicked, this, &CameraPanel::onResetCameraClicked );

	m_centerModelBtn = new QPushButton( "Center Model", m_contentWidget );
	m_centerModelBtn->setToolTip( "Center camera on loaded model" );
	m_centerModelBtn->setStyleSheet( m_resetCameraBtn->styleSheet() );
	connect( m_centerModelBtn, &QPushButton::clicked, this, &CameraPanel::onCenterModelClicked );

	buttonLayout->addWidget( m_resetCameraBtn );
	buttonLayout->addWidget( m_centerModelBtn );

	contentLayout->addLayout( buttonLayout );

	// Spacer at bottom
	contentLayout->addStretch();
}

void CameraPanel::refresh() {
	if ( !m_viewport ) return;

	// Update distance spin box (without triggering the changed signal)
	m_distanceSpinBox->blockSignals( true );
	m_distanceSpinBox->setValue( m_viewport->getCameraDistance() );
	m_distanceSpinBox->blockSignals( false );

	updatePositionDisplay();
}

void CameraPanel::onFovChanged( int value ) {
	m_fovLabel->setText( QString( "%1\u00B0" ).arg( value ) );

	// Note: FOV would need to be implemented in ModelViewport
	// For now, this is a placeholder for future FOV control
	// if ( m_viewport ) {
	//     m_viewport->setFov( (float)value );
	// }
}

void CameraPanel::onDistanceChanged( double value ) {
	if ( m_viewport ) {
		m_viewport->setCameraDistance( (float)value );
		updatePositionDisplay();
	}
}

void CameraPanel::onResetCameraClicked() {
	if ( m_viewport ) {
		m_viewport->resetCamera();
		refresh();
	}
}

void CameraPanel::onCenterModelClicked() {
	if ( m_viewport ) {
		m_viewport->frameModel();
		refresh();
	}
}

void CameraPanel::updatePositionDisplay() {
	if ( !m_viewport ) {
		m_positionLabel->setText( "X: ---  Y: ---  Z: ---" );
		return;
	}

	float x, y, z;
	m_viewport->getCameraPosition( x, y, z );

	m_positionLabel->setText( QString( "X: %1  Y: %2  Z: %3" )
	                              .arg( x, 0, 'f', 2 )
	                              .arg( y, 0, 'f', 2 )
	                              .arg( z, 0, 'f', 2 ) );
}
