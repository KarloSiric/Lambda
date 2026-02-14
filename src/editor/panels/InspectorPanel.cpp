/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   InspectorPanel.cpp  -  Base class implementation
 *
 * ===========================================================================
 */

#include "InspectorPanel.h"
#include "../widgets/ModelViewport.h"

InspectorPanel::InspectorPanel( QWidget *parent )
    : QWidget( parent )
    , m_viewport( nullptr )
    , m_mainLayout( nullptr )
    , m_contentWidget( nullptr )
    , m_placeholderLabel( nullptr ) {
	m_mainLayout = new QVBoxLayout( this );
	m_mainLayout->setContentsMargins( 4, 4, 4, 4 );
	m_mainLayout->setSpacing( 4 );

	// Create placeholder label (centered when shown)
	m_placeholderLabel = createPlaceholder();
	m_mainLayout->addWidget( m_placeholderLabel, 1 );  // stretch to fill

	// Content widget will be added by subclasses - fills all available space
	m_contentWidget = new QWidget( this );
	m_contentWidget->hide();
	m_contentWidget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
	m_mainLayout->addWidget( m_contentWidget, 1 );  // stretch factor 1 to fill space
}

void InspectorPanel::setViewport( ModelViewport *viewport ) {
	// Disconnect from old viewport
	if ( m_viewport ) {
		disconnect( m_viewport, nullptr, this, nullptr );
	}

	m_viewport = viewport;

	// Connect to new viewport signals
	if ( m_viewport ) {
		connect( m_viewport, &ModelViewport::modelLoaded, this, &InspectorPanel::onModelLoaded );
		connect( m_viewport, &ModelViewport::modelClosed, this, &InspectorPanel::onModelClosed );

		// If model is already loaded, refresh
		if ( m_viewport->hasModelLoaded() ) {
			onModelLoaded();
		} else {
			onModelClosed();
		}
	} else {
		onModelClosed();
	}
}

bool InspectorPanel::hasModel() const {
	return m_viewport && m_viewport->hasModelLoaded();
}

void InspectorPanel::onModelLoaded() {
	showPlaceholder( false );
	refresh();
}

void InspectorPanel::onModelClosed() {
	showPlaceholder( true );
}

void InspectorPanel::refresh() {
	// Subclasses override this to populate their content
}

QGroupBox *InspectorPanel::createSection( const QString &title, bool collapsed ) {
	QGroupBox *section = new QGroupBox( title, this );
	section->setCheckable( true );
	section->setChecked( !collapsed );

	// Style is applied via ThemeManager
	return section;
}

QLabel *InspectorPanel::createPlaceholder( const QString &message ) {
	QLabel *label = new QLabel( message, this );
	label->setAlignment( Qt::AlignCenter );
	label->setStyleSheet(
	    "QLabel {"
	    "    color: #808080;"
	    "    font-style: italic;"
	    "    padding: 20px;"
	    "}" );
	return label;
}

void InspectorPanel::showPlaceholder( bool show ) {
	if ( m_placeholderLabel ) {
		m_placeholderLabel->setVisible( show );
	}
	if ( m_contentWidget ) {
		m_contentWidget->setVisible( !show );
	}
}
