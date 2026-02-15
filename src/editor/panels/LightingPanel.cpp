/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   LightingPanel.cpp  -  Model lighting control panel implementation
 *
 * ===========================================================================
 */

#include "LightingPanel.h"
#include "../widgets/ModelViewport.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QGroupBox>
#include <QGridLayout>
#include <QColorDialog>

LightingPanel::LightingPanel( QWidget *parent )
    : InspectorPanel( parent )
    , m_lightingEnabled( nullptr )
    , m_ambientSlider( nullptr )
    , m_ambientLabel( nullptr )
    , m_lightDirXSlider( nullptr )
    , m_lightDirYSlider( nullptr )
    , m_lightDirZSlider( nullptr )
    , m_lightDirXLabel( nullptr )
    , m_lightDirYLabel( nullptr )
    , m_lightDirZLabel( nullptr )
    , m_lightColorBtn( nullptr )
    , m_lightColor( 255, 255, 255 )
    , m_chromeEnabled( nullptr )
    , m_resetBtn( nullptr ) {
	setupUI();
}

void LightingPanel::setupUI() {
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

	// === Global Lighting Toggle ===
	m_lightingEnabled = new QCheckBox( "Enable Lighting", m_contentWidget );
	m_lightingEnabled->setChecked( true );
	m_lightingEnabled->setStyleSheet( "color: #000000; font-weight: bold;" );
	m_lightingEnabled->setToolTip( "Enable or disable all lighting calculations" );
	connect( m_lightingEnabled, &QCheckBox::toggled, this, &LightingPanel::onLightingToggled );
	contentLayout->addWidget( m_lightingEnabled );

	// === Ambient Light Section ===
	QGroupBox *ambientGroup = new QGroupBox( "Ambient Light", m_contentWidget );
	ambientGroup->setStyleSheet( groupStyle );

	QVBoxLayout *ambientLayout = new QVBoxLayout( ambientGroup );
	ambientLayout->setSpacing( 4 );

	QHBoxLayout *ambientRow = new QHBoxLayout();
	QLabel *ambientText = new QLabel( "Intensity:", ambientGroup );
	ambientText->setStyleSheet( "color: #000000;" );

	m_ambientSlider = new QSlider( Qt::Horizontal, ambientGroup );
	m_ambientSlider->setRange( 0, 100 );
	m_ambientSlider->setValue( 20 );
	m_ambientSlider->setToolTip( "Ambient light intensity (0-100%)" );
	connect( m_ambientSlider, &QSlider::valueChanged, this, &LightingPanel::onAmbientChanged );

	m_ambientLabel = new QLabel( "20%", ambientGroup );
	m_ambientLabel->setMinimumWidth( 40 );
	m_ambientLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
	m_ambientLabel->setStyleSheet( "color: #000000; font-weight: bold;" );

	ambientRow->addWidget( ambientText );
	ambientRow->addWidget( m_ambientSlider, 1 );
	ambientRow->addWidget( m_ambientLabel );
	ambientLayout->addLayout( ambientRow );

	contentLayout->addWidget( ambientGroup );

	// === Light Direction Section ===
	QGroupBox *dirGroup = new QGroupBox( "Light Direction", m_contentWidget );
	dirGroup->setStyleSheet( groupStyle );

	QGridLayout *dirLayout = new QGridLayout( dirGroup );
	dirLayout->setSpacing( 4 );

	// X Direction
	QLabel *xLabel = new QLabel( "X:", dirGroup );
	xLabel->setStyleSheet( "color: #000000;" );
	m_lightDirXSlider = new QSlider( Qt::Horizontal, dirGroup );
	m_lightDirXSlider->setRange( -100, 100 );
	m_lightDirXSlider->setValue( 0 );
	m_lightDirXSlider->setToolTip( "Light direction X component" );
	connect( m_lightDirXSlider, &QSlider::valueChanged, this, &LightingPanel::onLightDirXChanged );
	m_lightDirXLabel = new QLabel( "0.00", dirGroup );
	m_lightDirXLabel->setMinimumWidth( 40 );
	m_lightDirXLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
	m_lightDirXLabel->setStyleSheet( "color: #000000;" );

	dirLayout->addWidget( xLabel, 0, 0 );
	dirLayout->addWidget( m_lightDirXSlider, 0, 1 );
	dirLayout->addWidget( m_lightDirXLabel, 0, 2 );

	// Y Direction
	QLabel *yLabel = new QLabel( "Y:", dirGroup );
	yLabel->setStyleSheet( "color: #000000;" );
	m_lightDirYSlider = new QSlider( Qt::Horizontal, dirGroup );
	m_lightDirYSlider->setRange( -100, 100 );
	m_lightDirYSlider->setValue( 0 );
	m_lightDirYSlider->setToolTip( "Light direction Y component" );
	connect( m_lightDirYSlider, &QSlider::valueChanged, this, &LightingPanel::onLightDirYChanged );
	m_lightDirYLabel = new QLabel( "0.00", dirGroup );
	m_lightDirYLabel->setMinimumWidth( 40 );
	m_lightDirYLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
	m_lightDirYLabel->setStyleSheet( "color: #000000;" );

	dirLayout->addWidget( yLabel, 1, 0 );
	dirLayout->addWidget( m_lightDirYSlider, 1, 1 );
	dirLayout->addWidget( m_lightDirYLabel, 1, 2 );

	// Z Direction
	QLabel *zLabel = new QLabel( "Z:", dirGroup );
	zLabel->setStyleSheet( "color: #000000;" );
	m_lightDirZSlider = new QSlider( Qt::Horizontal, dirGroup );
	m_lightDirZSlider->setRange( -100, 100 );
	m_lightDirZSlider->setValue( -100 );  // Default light from above
	m_lightDirZSlider->setToolTip( "Light direction Z component" );
	connect( m_lightDirZSlider, &QSlider::valueChanged, this, &LightingPanel::onLightDirZChanged );
	m_lightDirZLabel = new QLabel( "-1.00", dirGroup );
	m_lightDirZLabel->setMinimumWidth( 40 );
	m_lightDirZLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
	m_lightDirZLabel->setStyleSheet( "color: #000000;" );

	dirLayout->addWidget( zLabel, 2, 0 );
	dirLayout->addWidget( m_lightDirZSlider, 2, 1 );
	dirLayout->addWidget( m_lightDirZLabel, 2, 2 );

	dirLayout->setColumnStretch( 1, 1 );

	contentLayout->addWidget( dirGroup );

	// === Light Color Section ===
	QGroupBox *colorGroup = new QGroupBox( "Light Color", m_contentWidget );
	colorGroup->setStyleSheet( groupStyle );

	QHBoxLayout *colorLayout = new QHBoxLayout( colorGroup );
	colorLayout->setSpacing( 8 );

	QLabel *colorLabel = new QLabel( "Color:", colorGroup );
	colorLabel->setStyleSheet( "color: #000000;" );

	m_lightColorBtn = new QPushButton( colorGroup );
	m_lightColorBtn->setMinimumSize( 60, 24 );
	m_lightColorBtn->setToolTip( "Click to select light color" );
	updateLightColorButton();
	connect( m_lightColorBtn, &QPushButton::clicked, this, &LightingPanel::onLightColorClicked );

	colorLayout->addWidget( colorLabel );
	colorLayout->addWidget( m_lightColorBtn );
	colorLayout->addStretch();

	contentLayout->addWidget( colorGroup );

	// === Effects Section ===
	QGroupBox *effectsGroup = new QGroupBox( "Effects", m_contentWidget );
	effectsGroup->setStyleSheet( groupStyle );

	QVBoxLayout *effectsLayout = new QVBoxLayout( effectsGroup );

	m_chromeEnabled = new QCheckBox( "Chrome/Additive Effect", effectsGroup );
	m_chromeEnabled->setStyleSheet( "color: #000000;" );
	m_chromeEnabled->setToolTip( "Enable chrome shell effect for models with chrome textures" );
	connect( m_chromeEnabled, &QCheckBox::toggled, this, &LightingPanel::onChromeToggled );

	effectsLayout->addWidget( m_chromeEnabled );

	contentLayout->addWidget( effectsGroup );

	// Separator
	QFrame *separator = new QFrame( m_contentWidget );
	separator->setFrameShape( QFrame::HLine );
	separator->setStyleSheet( "background-color: #808080;" );
	contentLayout->addWidget( separator );

	// === Reset Button ===
	m_resetBtn = new QPushButton( "Reset Lighting", m_contentWidget );
	m_resetBtn->setToolTip( "Reset all lighting settings to defaults" );
	m_resetBtn->setStyleSheet( buttonStyle );
	connect( m_resetBtn, &QPushButton::clicked, this, &LightingPanel::onResetLightingClicked );
	contentLayout->addWidget( m_resetBtn );

	// Spacer at bottom
	contentLayout->addStretch();
}

void LightingPanel::refresh() {
	if ( !m_viewport ) {
		return;
	}

	// Sync UI with viewport lighting state
	m_lightingEnabled->blockSignals( true );
	m_lightingEnabled->setChecked( m_viewport->isLightingEnabled() );
	m_lightingEnabled->blockSignals( false );

	// Update ambient
	float ambient = m_viewport->getAmbientIntensity();
	int ambientPercent = static_cast<int>( ambient * 100.0f );
	m_ambientSlider->blockSignals( true );
	m_ambientSlider->setValue( ambientPercent );
	m_ambientSlider->blockSignals( false );
	m_ambientLabel->setText( QString( "%1%" ).arg( ambientPercent ) );

	// Update light direction
	float dirX, dirY, dirZ;
	m_viewport->getLightDirection( dirX, dirY, dirZ );

	m_lightDirXSlider->blockSignals( true );
	m_lightDirXSlider->setValue( static_cast<int>( dirX * 100.0f ) );
	m_lightDirXSlider->blockSignals( false );
	m_lightDirXLabel->setText( QString::number( dirX, 'f', 2 ) );

	m_lightDirYSlider->blockSignals( true );
	m_lightDirYSlider->setValue( static_cast<int>( dirY * 100.0f ) );
	m_lightDirYSlider->blockSignals( false );
	m_lightDirYLabel->setText( QString::number( dirY, 'f', 2 ) );

	m_lightDirZSlider->blockSignals( true );
	m_lightDirZSlider->setValue( static_cast<int>( dirZ * 100.0f ) );
	m_lightDirZSlider->blockSignals( false );
	m_lightDirZLabel->setText( QString::number( dirZ, 'f', 2 ) );

	// Update light color
	m_lightColor = m_viewport->getLightColor();
	updateLightColorButton();

	// Update chrome
	m_chromeEnabled->blockSignals( true );
	m_chromeEnabled->setChecked( m_viewport->isChromeEnabled() );
	m_chromeEnabled->blockSignals( false );
}

void LightingPanel::onLightingToggled( bool enabled ) {
	if ( m_viewport ) {
		m_viewport->setLightingEnabled( enabled );
		emit viewportStateChanged();
	}
}

void LightingPanel::onAmbientChanged( int value ) {
	m_ambientLabel->setText( QString( "%1%" ).arg( value ) );

	if ( m_viewport ) {
		m_viewport->setAmbientIntensity( static_cast<float>( value ) / 100.0f );
	}
}

void LightingPanel::onLightDirXChanged( int value ) {
	float dirX = static_cast<float>( value ) / 100.0f;
	m_lightDirXLabel->setText( QString::number( dirX, 'f', 2 ) );

	if ( m_viewport ) {
		float x, y, z;
		m_viewport->getLightDirection( x, y, z );
		m_viewport->setLightDirection( dirX, y, z );
	}
}

void LightingPanel::onLightDirYChanged( int value ) {
	float dirY = static_cast<float>( value ) / 100.0f;
	m_lightDirYLabel->setText( QString::number( dirY, 'f', 2 ) );

	if ( m_viewport ) {
		float x, y, z;
		m_viewport->getLightDirection( x, y, z );
		m_viewport->setLightDirection( x, dirY, z );
	}
}

void LightingPanel::onLightDirZChanged( int value ) {
	float dirZ = static_cast<float>( value ) / 100.0f;
	m_lightDirZLabel->setText( QString::number( dirZ, 'f', 2 ) );

	if ( m_viewport ) {
		float x, y, z;
		m_viewport->getLightDirection( x, y, z );
		m_viewport->setLightDirection( x, y, dirZ );
	}
}

void LightingPanel::onLightColorClicked() {
	QColor newColor = QColorDialog::getColor( m_lightColor, this, "Select Light Color" );

	if ( newColor.isValid() ) {
		m_lightColor = newColor;
		updateLightColorButton();

		if ( m_viewport ) {
			m_viewport->setLightColor( m_lightColor );
		}
	}
}

void LightingPanel::onChromeToggled( bool enabled ) {
	if ( m_viewport ) {
		m_viewport->setChromeEnabled( enabled );
	}
}

void LightingPanel::onResetLightingClicked() {
	// Reset to defaults - full ambient for proper visibility (like HLMV)
	m_lightingEnabled->setChecked( true );
	m_ambientSlider->setValue( 100 );  // Full ambient
	m_lightDirXSlider->setValue( 0 );
	m_lightDirYSlider->setValue( 0 );
	m_lightDirZSlider->setValue( -100 );
	m_lightColor = QColor( 255, 255, 255 );
	updateLightColorButton();
	m_chromeEnabled->setChecked( false );

	if ( m_viewport ) {
		m_viewport->resetLighting();
	}
}

void LightingPanel::updateLightColorButton() {
	QString colorStyle = QString(
	    "QPushButton {"
	    "    background-color: %1;"
	    "    border: 2px solid;"
	    "    border-top-color: #808080;"
	    "    border-left-color: #808080;"
	    "    border-right-color: #ffffff;"
	    "    border-bottom-color: #ffffff;"
	    "}" ).arg( m_lightColor.name() );

	m_lightColorBtn->setStyleSheet( colorStyle );
}
