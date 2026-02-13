/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   TransformPanel.cpp  -  Model transform control panel implementation
 *
 * ===========================================================================
 */

#include "TransformPanel.h"
#include "../widgets/ModelViewport.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QGroupBox>
#include <QGridLayout>
#include <cmath>

TransformPanel::TransformPanel( QWidget *parent )
    : InspectorPanel( parent )
    , m_posXLabel( nullptr )
    , m_posYLabel( nullptr )
    , m_posZLabel( nullptr )
    , m_rotationYSlider( nullptr )
    , m_rotationYLabel( nullptr )
    , m_resetPositionBtn( nullptr )
    , m_resetRotationBtn( nullptr )
    , m_resetAllBtn( nullptr ) {
	setupUI();
}

void TransformPanel::setupUI() {
	QVBoxLayout *contentLayout = new QVBoxLayout( m_contentWidget );
	contentLayout->setContentsMargins( 4, 4, 4, 4 );
	contentLayout->setSpacing( 8 );

	QString groupStyle =
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
	    "}";

	QString labelStyle =
	    "color: #000000;"
	    "background-color: #ffffff;"
	    "border: 1px inset;"
	    "border-color: #808080 #ffffff #ffffff #808080;"
	    "padding: 2px 4px;"
	    "font-family: monospace;";

	// === Position Display Section ===
	QGroupBox *posGroup = new QGroupBox( "Model Position", m_contentWidget );
	posGroup->setStyleSheet( groupStyle );

	QGridLayout *posLayout = new QGridLayout( posGroup );
	posLayout->setSpacing( 4 );

	QLabel *xLabel = new QLabel( "X:", posGroup );
	QLabel *yLabel = new QLabel( "Y:", posGroup );
	QLabel *zLabel = new QLabel( "Z:", posGroup );
	xLabel->setStyleSheet( "color: #000000;" );
	yLabel->setStyleSheet( "color: #000000;" );
	zLabel->setStyleSheet( "color: #000000;" );

	m_posXLabel = new QLabel( "0.00", posGroup );
	m_posYLabel = new QLabel( "0.00", posGroup );
	m_posZLabel = new QLabel( "0.00", posGroup );
	m_posXLabel->setStyleSheet( labelStyle );
	m_posYLabel->setStyleSheet( labelStyle );
	m_posZLabel->setStyleSheet( labelStyle );
	m_posXLabel->setMinimumWidth( 60 );
	m_posYLabel->setMinimumWidth( 60 );
	m_posZLabel->setMinimumWidth( 60 );
	m_posXLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
	m_posYLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
	m_posZLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

	posLayout->addWidget( xLabel, 0, 0 );
	posLayout->addWidget( m_posXLabel, 0, 1 );
	posLayout->addWidget( yLabel, 1, 0 );
	posLayout->addWidget( m_posYLabel, 1, 1 );
	posLayout->addWidget( zLabel, 2, 0 );
	posLayout->addWidget( m_posZLabel, 2, 1 );

	contentLayout->addWidget( posGroup );

	// === Rotation Control Section ===
	QGroupBox *rotGroup = new QGroupBox( "Model Rotation", m_contentWidget );
	rotGroup->setStyleSheet( groupStyle );

	QVBoxLayout *rotLayout = new QVBoxLayout( rotGroup );
	rotLayout->setSpacing( 8 );

	// Y rotation (horizontal spin)
	QHBoxLayout *rotYLayout = new QHBoxLayout();
	QLabel *rotYText = new QLabel( "Y Axis:", rotGroup );
	rotYText->setStyleSheet( "color: #000000;" );

	m_rotationYSlider = new QSlider( Qt::Horizontal, rotGroup );
	m_rotationYSlider->setRange( -180, 180 );
	m_rotationYSlider->setValue( 0 );
	m_rotationYSlider->setTickPosition( QSlider::TicksBelow );
	m_rotationYSlider->setTickInterval( 45 );
	m_rotationYSlider->setToolTip( "Rotate model around Y axis (-180 to 180 degrees)" );
	connect( m_rotationYSlider, &QSlider::valueChanged, this, &TransformPanel::onRotationYChanged );

	m_rotationYLabel = new QLabel( "0\u00B0", rotGroup );
	m_rotationYLabel->setMinimumWidth( 45 );
	m_rotationYLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
	m_rotationYLabel->setStyleSheet( "color: #000000; font-weight: bold;" );

	rotYLayout->addWidget( rotYText );
	rotYLayout->addWidget( m_rotationYSlider, 1 );
	rotYLayout->addWidget( m_rotationYLabel );

	rotLayout->addLayout( rotYLayout );

	contentLayout->addWidget( rotGroup );

	// Separator
	QFrame *separator = new QFrame( m_contentWidget );
	separator->setFrameShape( QFrame::HLine );
	separator->setStyleSheet( "background-color: #808080;" );
	contentLayout->addWidget( separator );

	// === Action Buttons Section ===
	QString buttonStyle =
	    "QPushButton {"
	    "    background-color: #c0c0c0;"
	    "    color: #000000;"
	    "    border: 2px solid;"
	    "    border-top-color: #ffffff;"
	    "    border-left-color: #ffffff;"
	    "    border-right-color: #808080;"
	    "    border-bottom-color: #808080;"
	    "    padding: 4px 8px;"
	    "}"
	    "QPushButton:hover {"
	    "    background-color: #d0d0d0;"
	    "}"
	    "QPushButton:pressed {"
	    "    border-top-color: #808080;"
	    "    border-left-color: #808080;"
	    "    border-right-color: #ffffff;"
	    "    border-bottom-color: #ffffff;"
	    "}";

	QHBoxLayout *buttonRow1 = new QHBoxLayout();
	m_resetPositionBtn = new QPushButton( "Reset Position", m_contentWidget );
	m_resetPositionBtn->setToolTip( "Reset model position to origin" );
	m_resetPositionBtn->setStyleSheet( buttonStyle );
	connect( m_resetPositionBtn, &QPushButton::clicked, this, &TransformPanel::onResetPositionClicked );

	m_resetRotationBtn = new QPushButton( "Reset Rotation", m_contentWidget );
	m_resetRotationBtn->setToolTip( "Reset model rotation" );
	m_resetRotationBtn->setStyleSheet( buttonStyle );
	connect( m_resetRotationBtn, &QPushButton::clicked, this, &TransformPanel::onResetRotationClicked );

	buttonRow1->addWidget( m_resetPositionBtn );
	buttonRow1->addWidget( m_resetRotationBtn );
	contentLayout->addLayout( buttonRow1 );

	m_resetAllBtn = new QPushButton( "Reset All Transforms", m_contentWidget );
	m_resetAllBtn->setToolTip( "Reset both position and rotation" );
	m_resetAllBtn->setStyleSheet( buttonStyle );
	connect( m_resetAllBtn, &QPushButton::clicked, this, &TransformPanel::onResetAllClicked );
	contentLayout->addWidget( m_resetAllBtn );

	// Spacer at bottom
	contentLayout->addStretch();
}

void TransformPanel::refresh() {
	updatePositionDisplay();

	if ( m_viewport ) {
		// Update rotation slider (convert radians to degrees)
		float rotY = m_viewport->getModelRotationY();
		int degrees = static_cast<int>( rotY * 180.0f / M_PI );
		m_rotationYSlider->blockSignals( true );
		m_rotationYSlider->setValue( degrees );
		m_rotationYSlider->blockSignals( false );
		m_rotationYLabel->setText( QString( "%1\u00B0" ).arg( degrees ) );
	}
}

void TransformPanel::onRotationYChanged( int value ) {
	m_rotationYLabel->setText( QString( "%1\u00B0" ).arg( value ) );

	if ( m_viewport ) {
		// Convert degrees to radians
		float radians = static_cast<float>( value ) * M_PI / 180.0f;
		m_viewport->setModelRotationY( radians );
	}
}

void TransformPanel::onResetPositionClicked() {
	if ( m_viewport ) {
		m_viewport->resetModelPosition();
		updatePositionDisplay();
	}
}

void TransformPanel::onResetRotationClicked() {
	if ( m_viewport ) {
		m_viewport->resetModelRotation();
		m_rotationYSlider->setValue( 0 );
		m_rotationYLabel->setText( "0\u00B0" );
	}
}

void TransformPanel::onResetAllClicked() {
	onResetPositionClicked();
	onResetRotationClicked();
}

void TransformPanel::updatePositionDisplay() {
	if ( !m_viewport ) {
		m_posXLabel->setText( "---" );
		m_posYLabel->setText( "---" );
		m_posZLabel->setText( "---" );
		return;
	}

	float x, y, z;
	m_viewport->getModelOffset( x, y, z );

	m_posXLabel->setText( QString::number( x, 'f', 2 ) );
	m_posYLabel->setText( QString::number( y, 'f', 2 ) );
	m_posZLabel->setText( QString::number( z, 'f', 2 ) );
}
