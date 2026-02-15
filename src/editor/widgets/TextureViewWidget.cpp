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
#include <QWheelEvent>

TextureViewWidget::TextureViewWidget( const QImage &image, const QString &name,
                                      int width, int height, int flags,
                                      QWidget *parent )
    : QWidget( parent )
    , m_image( image )
    , m_name( name )
    , m_width( width )
    , m_height( height )
    , m_flags( flags )
    , m_zoomLevel( 2.0f )
    , m_scrollArea( nullptr )
    , m_imageLabel( nullptr )
    , m_infoLabel( nullptr )
    , m_zoomLabel( nullptr )
    , m_zoomSlider( nullptr ) {
	setupUI();
}

void TextureViewWidget::setupUI() {
	QVBoxLayout *layout = new QVBoxLayout( this );
	layout->setContentsMargins( 0, 0, 0, 0 );
	layout->setSpacing( 0 );

	// 90s Windows button style
	QString buttonStyle =
	    "QPushButton {"
	    "    background-color: #c0c0c0;"
	    "    border: 2px outset;"
	    "    border-top-color: #ffffff;"
	    "    border-left-color: #ffffff;"
	    "    border-right-color: #808080;"
	    "    border-bottom-color: #808080;"
	    "    padding: 2px 8px;"
	    "    color: #000000;"
	    "    min-width: 40px;"
	    "}"
	    "QPushButton:pressed {"
	    "    border: 2px inset;"
	    "    border-top-color: #808080;"
	    "    border-left-color: #808080;"
	    "    border-right-color: #ffffff;"
	    "    border-bottom-color: #ffffff;"
	    "}";

	// Zoom toolbar at top
	QWidget *zoomBar = new QWidget( this );
	zoomBar->setStyleSheet( "background-color: #c0c0c0;" );
	QHBoxLayout *zoomLayout = new QHBoxLayout( zoomBar );
	zoomLayout->setContentsMargins( 4, 4, 4, 4 );
	zoomLayout->setSpacing( 4 );

	// Zoom out button
	QPushButton *zoomOutBtn = new QPushButton( "-", zoomBar );
	zoomOutBtn->setStyleSheet( buttonStyle );
	zoomOutBtn->setFixedWidth( 28 );
	zoomOutBtn->setToolTip( "Zoom Out" );
	connect( zoomOutBtn, &QPushButton::clicked, this, &TextureViewWidget::onZoomOut );
	zoomLayout->addWidget( zoomOutBtn );

	// Zoom slider (25% to 800%)
	m_zoomSlider = new QSlider( Qt::Horizontal, zoomBar );
	m_zoomSlider->setRange( 25, 800 );
	m_zoomSlider->setValue( 200 );  // Start at 200% for better visibility of small HL textures
	m_zoomSlider->setFixedWidth( 150 );
	m_zoomSlider->setStyleSheet(
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
	    "}" );
	connect( m_zoomSlider, &QSlider::valueChanged, this, &TextureViewWidget::onZoomSliderChanged );
	zoomLayout->addWidget( m_zoomSlider );

	// Zoom in button
	QPushButton *zoomInBtn = new QPushButton( "+", zoomBar );
	zoomInBtn->setStyleSheet( buttonStyle );
	zoomInBtn->setFixedWidth( 28 );
	zoomInBtn->setToolTip( "Zoom In" );
	connect( zoomInBtn, &QPushButton::clicked, this, &TextureViewWidget::onZoomIn );
	zoomLayout->addWidget( zoomInBtn );

	// Zoom label
	m_zoomLabel = new QLabel( "200%", zoomBar );
	m_zoomLabel->setFixedWidth( 50 );
	m_zoomLabel->setStyleSheet( "color: #000000; font-weight: bold;" );
	zoomLayout->addWidget( m_zoomLabel );

	zoomLayout->addSpacing( 8 );

	// Fit button
	QPushButton *fitBtn = new QPushButton( "Fit", zoomBar );
	fitBtn->setStyleSheet( buttonStyle );
	fitBtn->setToolTip( "Fit to window" );
	connect( fitBtn, &QPushButton::clicked, this, &TextureViewWidget::onZoomFit );
	zoomLayout->addWidget( fitBtn );

	// 100% button
	QPushButton *btn100 = new QPushButton( "100%", zoomBar );
	btn100->setStyleSheet( buttonStyle );
	btn100->setToolTip( "Original size (100%)" );
	connect( btn100, &QPushButton::clicked, this, &TextureViewWidget::onZoom100 );
	zoomLayout->addWidget( btn100 );

	zoomLayout->addStretch();

	layout->addWidget( zoomBar );

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

	// Initial display at 100% or fit to window if small
	updateImageDisplay();
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

void TextureViewWidget::updateImageDisplay() {
	if ( m_image.isNull() ) return;

	int scaledWidth = static_cast<int>( m_image.width() * m_zoomLevel );
	int scaledHeight = static_cast<int>( m_image.height() * m_zoomLevel );

	QImage scaled = m_image.scaled( scaledWidth, scaledHeight,
	                                Qt::KeepAspectRatio,
	                                Qt::FastTransformation );
	m_imageLabel->setPixmap( QPixmap::fromImage( scaled ) );
	m_imageLabel->adjustSize();
}

void TextureViewWidget::updateZoomLabel() {
	int percent = static_cast<int>( m_zoomLevel * 100.0f );
	m_zoomLabel->setText( QString( "%1%" ).arg( percent ) );
}

void TextureViewWidget::onZoomIn() {
	m_zoomLevel = qMin( 8.0f, m_zoomLevel * 1.25f );
	m_zoomSlider->blockSignals( true );
	m_zoomSlider->setValue( static_cast<int>( m_zoomLevel * 100 ) );
	m_zoomSlider->blockSignals( false );
	updateZoomLabel();
	updateImageDisplay();
}

void TextureViewWidget::onZoomOut() {
	m_zoomLevel = qMax( 0.25f, m_zoomLevel / 1.25f );
	m_zoomSlider->blockSignals( true );
	m_zoomSlider->setValue( static_cast<int>( m_zoomLevel * 100 ) );
	m_zoomSlider->blockSignals( false );
	updateZoomLabel();
	updateImageDisplay();
}

void TextureViewWidget::onZoomFit() {
	// Calculate zoom level to fit in available space
	int availableWidth = m_scrollArea->viewport()->width() - 20;
	int availableHeight = m_scrollArea->viewport()->height() - 20;

	float scaleX = static_cast<float>( availableWidth ) / m_image.width();
	float scaleY = static_cast<float>( availableHeight ) / m_image.height();
	m_zoomLevel = qMin( scaleX, scaleY );
	m_zoomLevel = qBound( 0.25f, m_zoomLevel, 8.0f );

	m_zoomSlider->blockSignals( true );
	m_zoomSlider->setValue( static_cast<int>( m_zoomLevel * 100 ) );
	m_zoomSlider->blockSignals( false );
	updateZoomLabel();
	updateImageDisplay();
}

void TextureViewWidget::onZoom100() {
	m_zoomLevel = 1.0f;
	m_zoomSlider->blockSignals( true );
	m_zoomSlider->setValue( 100 );
	m_zoomSlider->blockSignals( false );
	updateZoomLabel();
	updateImageDisplay();
}

void TextureViewWidget::onZoomSliderChanged( int value ) {
	m_zoomLevel = value / 100.0f;
	updateZoomLabel();
	updateImageDisplay();
}

void TextureViewWidget::wheelEvent( QWheelEvent *event ) {
	// Default: mouse wheel zooms the texture
	// Hold Shift to scroll instead
	if ( event->modifiers() & Qt::ShiftModifier ) {
		// Shift+Wheel for scrolling
		QWidget::wheelEvent( event );
	} else {
		// Mouse wheel zooms
		int delta = event->angleDelta().y();
		if ( delta > 0 ) {
			onZoomIn();
		} else if ( delta < 0 ) {
			onZoomOut();
		}
		event->accept();
	}
}
