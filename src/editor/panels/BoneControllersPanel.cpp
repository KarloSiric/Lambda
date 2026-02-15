/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   BoneControllersPanel.cpp  -  Bone controller sliders implementation
 *
 * ===========================================================================
 */

#include "BoneControllersPanel.h"
#include "../widgets/ModelViewport.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QScrollArea>

BoneControllersPanel::BoneControllersPanel( QWidget *parent )
    : InspectorPanel( parent )
    , m_controllersContainer( nullptr )
    , m_controllersLayout( nullptr )
    , m_noControllersLabel( nullptr )
    , m_resetAllBtn( nullptr ) {
	setupUI();
}

void BoneControllersPanel::setupUI() {
	QVBoxLayout *contentLayout = new QVBoxLayout( m_contentWidget );
	contentLayout->setContentsMargins( 4, 4, 4, 4 );
	contentLayout->setSpacing( 6 );

	// 90s Windows 95 style
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

	QString buttonStyle =
	    "QPushButton {"
	    "    background-color: #c0c0c0;"
	    "    border: 2px outset;"
	    "    border-top-color: #ffffff;"
	    "    border-left-color: #ffffff;"
	    "    border-right-color: #808080;"
	    "    border-bottom-color: #808080;"
	    "    padding: 4px 12px;"
	    "    color: #000000;"
	    "    font-weight: bold;"
	    "}"
	    "QPushButton:pressed {"
	    "    border: 2px inset;"
	    "    border-top-color: #808080;"
	    "    border-left-color: #808080;"
	    "    border-right-color: #ffffff;"
	    "    border-bottom-color: #ffffff;"
	    "}"
	    "QPushButton:hover {"
	    "    background-color: #d4d4d4;"
	    "}";

	// Scroll area for controllers
	QScrollArea *scrollArea = new QScrollArea( m_contentWidget );
	scrollArea->setWidgetResizable( true );
	scrollArea->setFrameStyle( QFrame::NoFrame );
	scrollArea->setStyleSheet( "QScrollArea { background-color: #c0c0c0; border: none; }" );

	// Container for all controller widgets
	m_controllersContainer = new QWidget( scrollArea );
	m_controllersContainer->setStyleSheet( "background-color: #c0c0c0;" );
	m_controllersLayout = new QVBoxLayout( m_controllersContainer );
	m_controllersLayout->setContentsMargins( 0, 0, 0, 0 );
	m_controllersLayout->setSpacing( 8 );

	// Placeholder when no controllers
	m_noControllersLabel = new QLabel( "No bone controllers in this model", m_controllersContainer );
	m_noControllersLabel->setAlignment( Qt::AlignCenter );
	m_noControllersLabel->setStyleSheet( "QLabel { color: #808080; font-style: italic; padding: 20px; }" );
	m_controllersLayout->addWidget( m_noControllersLabel );

	m_controllersLayout->addStretch();
	scrollArea->setWidget( m_controllersContainer );
	contentLayout->addWidget( scrollArea, 1 );

	// Reset all button at bottom
	m_resetAllBtn = new QPushButton( "Reset All", m_contentWidget );
	m_resetAllBtn->setStyleSheet( buttonStyle );
	m_resetAllBtn->setToolTip( "Reset all controllers to their rest positions" );
	connect( m_resetAllBtn, &QPushButton::clicked, this, &BoneControllersPanel::onResetAll );
	contentLayout->addWidget( m_resetAllBtn );

	// Store styles for later use
	m_controllersContainer->setProperty( "sliderStyle", sliderStyle );
	m_controllersContainer->setProperty( "spinStyle", spinStyle );
	m_controllersContainer->setProperty( "groupBoxStyle", groupBoxStyle );
}

void BoneControllersPanel::refresh() {
	clearControllers();
	createControllerWidgets();
}

void BoneControllersPanel::clearControllers() {
	// Remove all existing controller widgets
	for ( auto &ctrl : m_controllers ) {
		if ( ctrl.slider ) ctrl.slider->deleteLater();
		if ( ctrl.spinBox ) ctrl.spinBox->deleteLater();
		if ( ctrl.nameLabel ) ctrl.nameLabel->deleteLater();
	}
	m_controllers.clear();

	// Clear layout except placeholder and stretch
	while ( m_controllersLayout->count() > 2 ) {
		QLayoutItem *item = m_controllersLayout->takeAt( 0 );
		if ( item->widget() && item->widget() != m_noControllersLabel ) {
			item->widget()->deleteLater();
		}
		delete item;
	}
}

void BoneControllersPanel::createControllerWidgets() {
	if ( !hasModel() || !m_viewport ) {
		m_noControllersLabel->setVisible( true );
		m_resetAllBtn->setEnabled( false );
		return;
	}

	int count = m_viewport->getControllerCount();
	if ( count == 0 ) {
		m_noControllersLabel->setVisible( true );
		m_resetAllBtn->setEnabled( false );
		return;
	}

	m_noControllersLabel->setVisible( false );
	m_resetAllBtn->setEnabled( true );

	QString sliderStyle = m_controllersContainer->property( "sliderStyle" ).toString();
	QString spinStyle = m_controllersContainer->property( "spinStyle" ).toString();
	QString groupBoxStyle = m_controllersContainer->property( "groupBoxStyle" ).toString();

	// Create a widget for each controller
	for ( int i = 0; i < count; i++ ) {
		int controllerIdx = m_viewport->getControllerIndex( i );
		int type = m_viewport->getControllerType( i );
		int bone = m_viewport->getControllerBone( i );
		float start = m_viewport->getControllerStart( i );
		float end = m_viewport->getControllerEnd( i );

		// Build name string
		QString boneName = ( bone >= 0 ) ? m_viewport->getBoneName( bone ) : "Unknown";
		QString typeName = getControllerTypeName( type );
		QString indexName = getControllerIndexName( controllerIdx );
		QString name = QString( "%1: %2 (%3)" ).arg( indexName ).arg( boneName ).arg( typeName );

		// Create group box for this controller
		QGroupBox *group = new QGroupBox( name, m_controllersContainer );
		group->setStyleSheet( groupBoxStyle );

		QVBoxLayout *groupLayout = new QVBoxLayout( group );
		groupLayout->setContentsMargins( 8, 8, 8, 8 );
		groupLayout->setSpacing( 4 );

		// Range label
		QLabel *rangeLabel = new QLabel( QString( "Range: %1 to %2" ).arg( start, 0, 'f', 1 ).arg( end, 0, 'f', 1 ), group );
		rangeLabel->setStyleSheet( "QLabel { color: #000000; font-size: 10px; }" );
		groupLayout->addWidget( rangeLabel );

		// Slider and spinbox row
		QHBoxLayout *sliderRow = new QHBoxLayout();
		sliderRow->setSpacing( 8 );

		QSlider *slider = new QSlider( Qt::Horizontal, group );
		slider->setRange( 0, 1000 );  // Use 0-1000 for precision, map to actual range
		slider->setValue( 500 );       // Default to middle (rest)
		slider->setStyleSheet( sliderStyle );
		slider->setProperty( "controllerArrayIndex", i );
		slider->setProperty( "controllerSlotIndex", controllerIdx );
		slider->setProperty( "minValue", start );
		slider->setProperty( "maxValue", end );
		connect( slider, &QSlider::valueChanged, this, &BoneControllersPanel::onControllerValueChanged );
		sliderRow->addWidget( slider, 1 );

		QDoubleSpinBox *spinBox = new QDoubleSpinBox( group );
		spinBox->setRange( start, end );
		spinBox->setSingleStep( ( end - start ) / 100.0 );
		spinBox->setValue( ( start + end ) / 2.0 );
		spinBox->setDecimals( 1 );
		spinBox->setFixedWidth( 70 );
		spinBox->setStyleSheet( spinStyle );
		spinBox->setProperty( "controllerSlotIndex", controllerIdx );
		spinBox->setProperty( "minValue", start );
		spinBox->setProperty( "maxValue", end );
		sliderRow->addWidget( spinBox );

		// Sync spinbox with slider AND update model
		connect( spinBox, QOverload<double>::of( &QDoubleSpinBox::valueChanged ), this, &BoneControllersPanel::onSpinBoxValueChanged );

		groupLayout->addLayout( sliderRow );

		// Insert before the stretch
		m_controllersLayout->insertWidget( m_controllersLayout->count() - 1, group );

		// Store reference
		ControllerWidget ctrl;
		ctrl.controllerIndex = controllerIdx;
		ctrl.slider = slider;
		ctrl.spinBox = spinBox;
		ctrl.nameLabel = rangeLabel;
		ctrl.minValue = start;
		ctrl.maxValue = end;
		m_controllers.append( ctrl );
	}
}

void BoneControllersPanel::onControllerValueChanged( int value ) {
	QSlider *slider = qobject_cast<QSlider *>( sender() );
	if ( !slider || !m_viewport ) return;

	int controllerIdx = slider->property( "controllerSlotIndex" ).toInt();
	float minVal = slider->property( "minValue" ).toFloat();
	float maxVal = slider->property( "maxValue" ).toFloat();

	// Normalized value 0-1 (slider is 0-1000)
	float normalizedValue = value / 1000.0f;

	// Calculate actual value for display
	float actualValue = minVal + normalizedValue * ( maxVal - minVal );

	// Update spinbox with actual value for display
	for ( auto &ctrl : m_controllers ) {
		if ( ctrl.slider == slider && ctrl.spinBox ) {
			ctrl.spinBox->blockSignals( true );
			ctrl.spinBox->setValue( actualValue );
			ctrl.spinBox->blockSignals( false );
			break;
		}
	}

	// Apply NORMALIZED value to viewport (renderer does interpolation)
	m_viewport->setControllerValue( controllerIdx, normalizedValue );
}

void BoneControllersPanel::onSpinBoxValueChanged( double value ) {
	QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox *>( sender() );
	if ( !spinBox || !m_viewport ) return;

	int controllerIdx = spinBox->property( "controllerSlotIndex" ).toInt();
	float minVal = spinBox->property( "minValue" ).toFloat();
	float maxVal = spinBox->property( "maxValue" ).toFloat();

	// Convert actual value to normalized 0-1
	float normalizedValue = ( value - minVal ) / ( maxVal - minVal );
	normalizedValue = qBound( 0.0f, normalizedValue, 1.0f );

	// Update slider to match (without retriggering)
	for ( auto &ctrl : m_controllers ) {
		if ( ctrl.spinBox == spinBox && ctrl.slider ) {
			ctrl.slider->blockSignals( true );
			ctrl.slider->setValue( ( int )( normalizedValue * 1000.0f ) );
			ctrl.slider->blockSignals( false );
			break;
		}
	}

	// Apply NORMALIZED value to viewport
	m_viewport->setControllerValue( controllerIdx, normalizedValue );
}

void BoneControllersPanel::onResetAll() {
	if ( !m_viewport ) return;

	// Reset all sliders to middle (rest position)
	for ( auto &ctrl : m_controllers ) {
		if ( ctrl.slider ) {
			ctrl.slider->setValue( 500 );  // Middle = rest
		}
	}
}

QString BoneControllersPanel::getControllerTypeName( int type ) const {
	// Type constants from studio.h / mdl_bonecontrollers.h
	// The high bit (0x80) can be STUDIO_RLOOP flag, so mask to get axis type
	int axisType = type & 0x07;
	switch ( axisType ) {
		case 0: return "X Pos";
		case 1: return "Y Pos";
		case 2: return "Z Pos";
		case 3: return "X Rot";
		case 4: return "Y Rot";
		case 5: return "Z Rot";
		default: return QString( "Type %1" ).arg( type );
	}
}

QString BoneControllersPanel::getControllerIndexName( int index ) const {
	// Controller index meanings
	switch ( index ) {
		case 0: return "Controller 0";
		case 1: return "Controller 1";
		case 2: return "Controller 2";
		case 3: return "Controller 3";
		case 4: return "Mouth";
		case 5: return "Eyes";
		default: return QString( "Controller %1" ).arg( index );
	}
}
