/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   TextureViewWidget.cpp  -  Texture viewer widget implementation
 *
 * ===========================================================================
 */

#include "TextureViewWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

TextureViewWidget::TextureViewWidget( const QImage &image, const QString &name,
                                      int width, int height, int flags,
                                      QWidget *parent )
    : QWidget( parent )
    , m_image( image )
    , m_name( name )
    , m_width( width )
    , m_height( height )
    , m_flags( flags )
    , m_scrollArea( nullptr )
    , m_imageLabel( nullptr )
    , m_infoLabel( nullptr ) {
	setupUI();
}

void TextureViewWidget::setupUI() {
	QVBoxLayout *layout = new QVBoxLayout( this );
	layout->setContentsMargins( 0, 0, 0, 0 );
	layout->setSpacing( 0 );

	// Scrollable image area with black background
	m_scrollArea = new QScrollArea( this );
	m_scrollArea->setWidgetResizable( false );
	m_scrollArea->setAlignment( Qt::AlignCenter );
	m_scrollArea->setStyleSheet(
	    "QScrollArea {"
	    "    background-color: #000000;"
	    "    border: none;"
	    "}"
	    "QScrollBar:vertical {"
	    "    background-color: #c0c0c0;"
	    "    width: 16px;"
	    "    border: 1px solid #808080;"
	    "}"
	    "QScrollBar::handle:vertical {"
	    "    background-color: #c0c0c0;"
	    "    border: 2px solid;"
	    "    border-top-color: #ffffff;"
	    "    border-left-color: #ffffff;"
	    "    border-right-color: #808080;"
	    "    border-bottom-color: #808080;"
	    "    min-height: 20px;"
	    "}"
	    "QScrollBar:horizontal {"
	    "    background-color: #c0c0c0;"
	    "    height: 16px;"
	    "    border: 1px solid #808080;"
	    "}"
	    "QScrollBar::handle:horizontal {"
	    "    background-color: #c0c0c0;"
	    "    border: 2px solid;"
	    "    border-top-color: #ffffff;"
	    "    border-left-color: #ffffff;"
	    "    border-right-color: #808080;"
	    "    border-bottom-color: #808080;"
	    "    min-width: 20px;"
	    "}" );

	// Image label
	m_imageLabel = new QLabel( m_scrollArea );
	m_imageLabel->setPixmap( QPixmap::fromImage( m_image ) );
	m_imageLabel->setAlignment( Qt::AlignCenter );
	m_imageLabel->setStyleSheet( "background-color: #000000; padding: 8px;" );
	m_scrollArea->setWidget( m_imageLabel );

	layout->addWidget( m_scrollArea, 1 );

	// Info bar at bottom with 90s Windows style
	m_infoLabel = new QLabel( this );
	m_infoLabel->setText( QString( "  %1  |  Size: %2 x %3  |  Flags: %4" )
	                          .arg( m_name )
	                          .arg( m_width )
	                          .arg( m_height )
	                          .arg( formatTextureFlags( m_flags ) ) );
	m_infoLabel->setStyleSheet(
	    "QLabel {"
	    "    background-color: #c0c0c0;"
	    "    color: #000000;"
	    "    border-top: 2px solid;"
	    "    border-top-color: #808080;"
	    "    padding: 4px 8px;"
	    "    font-family: 'MS Sans Serif', 'Segoe UI', sans-serif;"
	    "}" );
	m_infoLabel->setMinimumHeight( 24 );

	layout->addWidget( m_infoLabel );
}

QString TextureViewWidget::formatTextureFlags( int flags ) const {
	QStringList flagNames;

	// MDL texture flags
	if ( flags & 0x0001 ) flagNames << "FLATSHADE";
	if ( flags & 0x0002 ) flagNames << "CHROME";
	if ( flags & 0x0004 ) flagNames << "FULLBRIGHT";
	if ( flags & 0x0008 ) flagNames << "NOMIPS";
	if ( flags & 0x0010 ) flagNames << "ALPHA";
	if ( flags & 0x0020 ) flagNames << "ADDITIVE";
	if ( flags & 0x0040 ) flagNames << "MASKED";

	if ( flagNames.isEmpty() ) {
		return QString( "0x%1" ).arg( flags, 4, 16, QChar( '0' ) );
	}

	return flagNames.join( ", " );
}
