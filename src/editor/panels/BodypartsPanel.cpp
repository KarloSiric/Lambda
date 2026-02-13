/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   BodypartsPanel.cpp  -  Bodypart/Submodel selector implementation
 *
 * ===========================================================================
 */

#include "BodypartsPanel.h"
#include "../widgets/ModelViewport.h"
#include <QVBoxLayout>
#include <QLabel>

BodypartsPanel::BodypartsPanel( QWidget *parent )
    : InspectorPanel( parent )
    , m_scrollArea( nullptr )
    , m_bodypartContainer( nullptr )
    , m_bodypartLayout( nullptr ) {
	setupUI();
}

void BodypartsPanel::setupUI() {
	QVBoxLayout *contentLayout = new QVBoxLayout( m_contentWidget );
	contentLayout->setContentsMargins( 0, 0, 0, 0 );
	contentLayout->setSpacing( 0 );

	// Scrollable area for bodypart groups
	m_scrollArea = new QScrollArea( m_contentWidget );
	m_scrollArea->setWidgetResizable( true );
	m_scrollArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	m_scrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
	m_scrollArea->setToolTip( "Select different model variations for each bodypart" );
	m_scrollArea->setStyleSheet(
	    "QScrollArea {"
	    "    background-color: #c0c0c0;"
	    "    border: none;"
	    "}" );

	m_bodypartContainer = new QWidget( m_scrollArea );
	m_bodypartLayout = new QVBoxLayout( m_bodypartContainer );
	m_bodypartLayout->setContentsMargins( 4, 4, 4, 4 );
	m_bodypartLayout->setSpacing( 8 );
	m_bodypartLayout->setAlignment( Qt::AlignTop );

	m_scrollArea->setWidget( m_bodypartContainer );
	contentLayout->addWidget( m_scrollArea );
}

void BodypartsPanel::refresh() {
	clearBodyparts();

	if ( !hasModel() ) {
		return;
	}

	populateBodyparts();
}

void BodypartsPanel::onSubmodelSelected( int bodypartIndex, int submodelIndex ) {
	if ( m_viewport ) {
		m_viewport->setBodypart( bodypartIndex, submodelIndex );
	}
}

void BodypartsPanel::clearBodyparts() {
	// Delete all button groups
	for ( auto it = m_buttonGroups.begin(); it != m_buttonGroups.end(); ++it ) {
		delete it.value();
	}
	m_buttonGroups.clear();

	// Remove all widgets from layout
	QLayoutItem *item;
	while ( ( item = m_bodypartLayout->takeAt( 0 ) ) != nullptr ) {
		delete item->widget();
		delete item;
	}
}

void BodypartsPanel::populateBodyparts() {
	if ( !hasModel() ) return;

	int bodypartCount = m_viewport->getBodypartCount();

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

	// 90s Windows 95 style for radio buttons
	QString radioStyle =
	    "QRadioButton {"
	    "    color: #000000;"
	    "    spacing: 6px;"
	    "}"
	    "QRadioButton::indicator {"
	    "    width: 13px;"
	    "    height: 13px;"
	    "}"
	    "QRadioButton::indicator:unchecked {"
	    "    border: 2px inset;"
	    "    border-top-color: #808080;"
	    "    border-left-color: #808080;"
	    "    border-right-color: #ffffff;"
	    "    border-bottom-color: #ffffff;"
	    "    border-radius: 7px;"
	    "    background: #ffffff;"
	    "}"
	    "QRadioButton::indicator:checked {"
	    "    border: 2px inset;"
	    "    border-top-color: #808080;"
	    "    border-left-color: #808080;"
	    "    border-right-color: #ffffff;"
	    "    border-bottom-color: #ffffff;"
	    "    border-radius: 7px;"
	    "    background: #ffffff;"
	    "    image: url(:/icons/radio-checked.png);"
	    "}";

	for ( int bp = 0; bp < bodypartCount; ++bp ) {
		QString bodypartName = m_viewport->getBodypartName( bp );
		int submodelCount = m_viewport->getSubmodelCount( bp );
		int activeSubmodel = m_viewport->getActiveSubmodel( bp );

		// Create group box for this bodypart
		QGroupBox *groupBox = new QGroupBox( bodypartName, m_bodypartContainer );
		groupBox->setStyleSheet( groupBoxStyle );

		QVBoxLayout *groupLayout = new QVBoxLayout( groupBox );
		groupLayout->setContentsMargins( 8, 8, 8, 8 );
		groupLayout->setSpacing( 4 );

		// Create button group for exclusive selection
		QButtonGroup *buttonGroup = new QButtonGroup( this );
		m_buttonGroups[bp] = buttonGroup;

		for ( int sub = 0; sub < submodelCount; ++sub ) {
			QString submodelName = m_viewport->getSubmodelName( bp, sub );
			int vertexCount = m_viewport->getSubmodelVertexCount( bp, sub );

			QString label = QString( "%1  (%2 verts)" ).arg( submodelName ).arg( vertexCount );

			QRadioButton *radio = new QRadioButton( label, groupBox );
			radio->setStyleSheet( radioStyle );
			radio->setChecked( sub == activeSubmodel );
			radio->setToolTip( QString( "Select %1 submodel for this bodypart" ).arg( submodelName ) );

			buttonGroup->addButton( radio, sub );
			groupLayout->addWidget( radio );
		}

		// Connect button group to slot
		connect( buttonGroup, QOverload<int>::of( &QButtonGroup::idClicked ),
		         this, [this, bp]( int subIndex ) {
			         onSubmodelSelected( bp, subIndex );
		         } );

		m_bodypartLayout->addWidget( groupBox );
	}

	// Add stretch at the end
	m_bodypartLayout->addStretch();
}
