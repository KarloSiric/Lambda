/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   TexturesPanel.cpp  -  Texture thumbnail browser implementation
 *
 * ===========================================================================
 */

#include "TexturesPanel.h"
#include "../widgets/ModelViewport.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QScrollArea>
#include <QDialog>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QMenu>

// ============================================================================
// TextureThumbnail
// ============================================================================

TextureThumbnail::TextureThumbnail( int index, const QImage &image, const QString &name, QWidget *parent )
    : QPushButton( parent )
    , m_index( index ) {
	// Square-ish thumbnail like Worldcraft: 68x82 (64x64 image + name)
	setFixedSize( 68, 82 );
	setCheckable( true );
	setAutoExclusive( true );

	// Create layout with image and name
	QVBoxLayout *layout = new QVBoxLayout( this );
	layout->setContentsMargins( 2, 2, 2, 2 );
	layout->setSpacing( 1 );

	// Thumbnail image - 64x64 square (standard power-of-2 texture size)
	QLabel *imageLabel = new QLabel( this );
	QPixmap pixmap = QPixmap::fromImage( image.scaled( 64, 64, Qt::IgnoreAspectRatio, Qt::SmoothTransformation ) );
	imageLabel->setPixmap( pixmap );
	imageLabel->setAlignment( Qt::AlignCenter );
	imageLabel->setFixedSize( 64, 64 );
	imageLabel->setStyleSheet( "border: none; background: #000000;" );

	// Name label (truncate if too long) - white text for dark background
	QLabel *nameLabel = new QLabel( name, this );
	nameLabel->setAlignment( Qt::AlignCenter );
	nameLabel->setStyleSheet( "color: #ffffff; font-size: 8px; border: none; background: transparent;" );
	nameLabel->setMaximumWidth( 64 );
	nameLabel->setFixedHeight( 12 );

	// Truncate name
	QFontMetrics fm( nameLabel->font() );
	QString truncated = fm.elidedText( name, Qt::ElideMiddle, 60 );
	nameLabel->setText( truncated );

	layout->addWidget( imageLabel );
	layout->addWidget( nameLabel );

	// Worldcraft-style: dark background, cyan highlight on selection
	setStyleSheet(
	    "QPushButton {"
	    "    background-color: #404040;"
	    "    border: 1px solid #606060;"
	    "    padding: 0px;"
	    "}"
	    "QPushButton:hover {"
	    "    border: 1px solid #00ffff;"
	    "}"
	    "QPushButton:checked {"
	    "    border: 2px solid #00ffff;"
	    "    background-color: #505050;"
	    "}" );
}

void TextureThumbnail::mouseDoubleClickEvent( QMouseEvent *event ) {
	QPushButton::mouseDoubleClickEvent( event );
	emit doubleClicked( m_index );
}

void TextureThumbnail::contextMenuEvent( QContextMenuEvent *event ) {
	QMenu menu( this );

	QAction *tabAction = menu.addAction( "Open in New Tab" );
	tabAction->setToolTip( "Open texture as a new tab in the viewport area" );

	QAction *windowAction = menu.addAction( "Open in Separate Window" );
	windowAction->setToolTip( "Open texture in a separate preview window" );

	QAction *selected = menu.exec( event->globalPos() );

	if ( selected == tabAction ) {
		emit openInNewTab( m_index );
	} else if ( selected == windowAction ) {
		emit openInSeparateWindow( m_index );
	}
}

// ============================================================================
// TexturesPanel
// ============================================================================

TexturesPanel::TexturesPanel( QWidget *parent )
    : InspectorPanel( parent )
    , m_skinSelector( nullptr )
    , m_thumbnailContainer( nullptr )
    , m_thumbnailLayout( nullptr )
    , m_selectionInfo( nullptr )
    , m_selectedTextureIndex( -1 ) {
	setupUI();
}

void TexturesPanel::setupUI() {
	QVBoxLayout *contentLayout = new QVBoxLayout( m_contentWidget );
	contentLayout->setContentsMargins( 4, 4, 4, 4 );
	contentLayout->setSpacing( 8 );

	// Skin family selector row
	QHBoxLayout *skinLayout = new QHBoxLayout();
	skinLayout->setSpacing( 8 );

	QLabel *skinLabel = new QLabel( "Skin:", m_contentWidget );
	skinLabel->setStyleSheet( "color: #000000;" );
	skinLabel->setToolTip( "Select skin family to apply different texture mappings" );

	m_skinSelector = new QComboBox( m_contentWidget );
	m_skinSelector->setMinimumWidth( 120 );
	m_skinSelector->setToolTip( "Skin families define different texture sets for the model" );
	connect( m_skinSelector, QOverload<int>::of( &QComboBox::currentIndexChanged ),
	         this, &TexturesPanel::onSkinFamilyChanged );

	skinLayout->addWidget( skinLabel );
	skinLayout->addWidget( m_skinSelector, 1 );
	contentLayout->addLayout( skinLayout );

	// Separator
	QFrame *separator1 = new QFrame( m_contentWidget );
	separator1->setFrameShape( QFrame::HLine );
	separator1->setStyleSheet( "background-color: #808080;" );
	contentLayout->addWidget( separator1 );

	// Scrollable thumbnail grid - Worldcraft-style dark background
	QScrollArea *scrollArea = new QScrollArea( m_contentWidget );
	scrollArea->setWidgetResizable( true );
	scrollArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	scrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
	scrollArea->setStyleSheet(
	    "QScrollArea { background-color: #303030; border: 2px inset; "
	    "border-top-color: #808080; border-left-color: #808080; "
	    "border-right-color: #ffffff; border-bottom-color: #ffffff; }" );

	m_thumbnailContainer = new QWidget( scrollArea );
	m_thumbnailContainer->setStyleSheet( "background-color: #303030;" );
	m_thumbnailLayout = new QGridLayout( m_thumbnailContainer );
	m_thumbnailLayout->setContentsMargins( 4, 4, 4, 4 );
	m_thumbnailLayout->setSpacing( 4 );
	m_thumbnailLayout->setAlignment( Qt::AlignTop | Qt::AlignLeft );

	scrollArea->setWidget( m_thumbnailContainer );
	contentLayout->addWidget( scrollArea, 1 );

	// Separator
	QFrame *separator2 = new QFrame( m_contentWidget );
	separator2->setFrameShape( QFrame::HLine );
	separator2->setStyleSheet( "background-color: #808080;" );
	contentLayout->addWidget( separator2 );

	// Selection info box
	m_selectionInfo = new QLabel( m_contentWidget );
	m_selectionInfo->setMinimumHeight( 50 );
	m_selectionInfo->setWordWrap( true );
	m_selectionInfo->setToolTip( "Double-click a texture to open full-size preview" );
	contentLayout->addWidget( m_selectionInfo );
}

void TexturesPanel::refresh() {
	clearThumbnails();
	m_skinSelector->clear();

	if ( !hasModel() ) {
		m_selectionInfo->setText( "No model loaded" );
		return;
	}

	// Populate skin families
	int numSkins = m_viewport->getNumSkinFamilies();
	for ( int i = 0; i < numSkins; ++i ) {
		m_skinSelector->addItem( QString( "Skin %1" ).arg( i ) );
	}
	m_skinSelector->setCurrentIndex( m_viewport->getCurrentSkinFamily() );

	// Populate thumbnails
	populateThumbnails();

	// Select first texture
	if ( m_viewport->getTextureCount() > 0 ) {
		updateSelectionInfo( 0 );
	} else {
		m_selectionInfo->setText( "No textures in model" );
	}
}

void TexturesPanel::onSkinFamilyChanged( int index ) {
	if ( m_viewport && index >= 0 ) {
		m_viewport->setSkinFamily( index );
	}
}

void TexturesPanel::onThumbnailClicked() {
	TextureThumbnail *thumbnail = qobject_cast<TextureThumbnail *>( sender() );
	if ( thumbnail ) {
		m_selectedTextureIndex = thumbnail->textureIndex();
		updateSelectionInfo( m_selectedTextureIndex );
	}
}

void TexturesPanel::populateThumbnails() {
	if ( !hasModel() ) return;

	int count = m_viewport->getTextureCount();
	int columns = 2; // 2 thumbnails per row (larger thumbnails)

	// Pre-cache all texture images for performance
	m_cachedFullImages.clear();
	m_cachedFullImages.reserve( count );

	for ( int i = 0; i < count; ++i ) {
		QImage image = m_viewport->getTextureImage( i );
		QString name = m_viewport->getTextureName( i );

		// Cache the full image for preview dialog
		m_cachedFullImages.append( image );

		TextureThumbnail *thumbnail = new TextureThumbnail( i, image, name, m_thumbnailContainer );
		connect( thumbnail, &QPushButton::clicked, this, &TexturesPanel::onThumbnailClicked );
		connect( thumbnail, &TextureThumbnail::doubleClicked, this, &TexturesPanel::onThumbnailDoubleClicked );
		connect( thumbnail, &TextureThumbnail::openInNewTab, this, &TexturesPanel::onOpenInNewTab );
		connect( thumbnail, &TextureThumbnail::openInSeparateWindow, this, &TexturesPanel::onOpenInSeparateWindow );

		int row = i / columns;
		int col = i % columns;
		m_thumbnailLayout->addWidget( thumbnail, row, col );

		// Select first thumbnail
		if ( i == 0 ) {
			thumbnail->setChecked( true );
		}
	}
}

void TexturesPanel::onThumbnailDoubleClicked( int index ) {
	showTexturePreview( index );
}

void TexturesPanel::onOpenInNewTab( int index ) {
	if ( !hasModel() || index < 0 || index >= m_viewport->getTextureCount() ) {
		return;
	}

	// Get texture data
	QImage image;
	if ( index < m_cachedFullImages.size() ) {
		image = m_cachedFullImages[index];
	} else {
		image = m_viewport->getTextureImage( index );
	}

	QString name = m_viewport->getTextureName( index );
	int width, height;
	m_viewport->getTextureDimensions( index, width, height );
	int flags = m_viewport->getTextureFlags( index );

	// Emit signal for MainWindow to create a new tab
	emit requestTextureTab( image, name, width, height, flags );
}

void TexturesPanel::onOpenInSeparateWindow( int index ) {
	// Same as double-click behavior
	showTexturePreview( index );
}

void TexturesPanel::showTexturePreview( int textureIndex ) {
	if ( !hasModel() || textureIndex < 0 || textureIndex >= m_viewport->getTextureCount() ) {
		return;
	}

	// Use cached image for better performance
	QImage fullImage;
	if ( textureIndex < m_cachedFullImages.size() ) {
		fullImage = m_cachedFullImages[textureIndex];
	} else {
		fullImage = m_viewport->getTextureImage( textureIndex );
	}
	if ( fullImage.isNull() ) return;

	QString name = m_viewport->getTextureName( textureIndex );
	int width, height;
	m_viewport->getTextureDimensions( textureIndex, width, height );

	// Create preview dialog with 90s Windows style
	QDialog *preview = new QDialog( this );
	preview->setWindowTitle( QString( "Texture: %1" ).arg( name ) );
	preview->setAttribute( Qt::WA_DeleteOnClose );
	preview->setStyleSheet(
	    "QDialog {"
	    "    background-color: #c0c0c0;"
	    "}"
	    "QLabel {"
	    "    color: #000000;"
	    "    background-color: transparent;"
	    "}"
	    "QScrollArea {"
	    "    background-color: #000000;"
	    "    border: 2px solid;"
	    "    border-top-color: #808080;"
	    "    border-left-color: #808080;"
	    "    border-right-color: #ffffff;"
	    "    border-bottom-color: #ffffff;"
	    "}" );

	QVBoxLayout *layout = new QVBoxLayout( preview );
	layout->setContentsMargins( 8, 8, 8, 8 );
	layout->setSpacing( 8 );

	// Scrollable image area
	QScrollArea *scroll = new QScrollArea( preview );
	scroll->setWidgetResizable( false );
	scroll->setAlignment( Qt::AlignCenter );

	QLabel *imageLabel = new QLabel( scroll );
	imageLabel->setPixmap( QPixmap::fromImage( fullImage ) );
	imageLabel->setAlignment( Qt::AlignCenter );
	imageLabel->setStyleSheet( "background-color: #000000; padding: 4px;" );
	scroll->setWidget( imageLabel );

	layout->addWidget( scroll, 1 );

	// Info label at bottom
	QLabel *infoLabel = new QLabel( preview );
	infoLabel->setText( QString( "Size: %1 x %2  |  Flags: %3" )
	                        .arg( width )
	                        .arg( height )
	                        .arg( formatTextureFlags( m_viewport->getTextureFlags( textureIndex ) ) ) );
	infoLabel->setAlignment( Qt::AlignCenter );
	layout->addWidget( infoLabel );

	// Size dialog based on texture size (with padding and max limit)
	int dialogWidth = qMin( fullImage.width() + 40, 800 );
	int dialogHeight = qMin( fullImage.height() + 80, 600 );
	preview->resize( qMax( dialogWidth, 200 ), qMax( dialogHeight, 150 ) );

	preview->show();
}

void TexturesPanel::updateSelectionInfo( int textureIndex ) {
	if ( !hasModel() || textureIndex < 0 || textureIndex >= m_viewport->getTextureCount() ) {
		m_selectionInfo->clear();
		return;
	}

	QString name = m_viewport->getTextureName( textureIndex );
	int width, height;
	m_viewport->getTextureDimensions( textureIndex, width, height );
	int flags = m_viewport->getTextureFlags( textureIndex );

	QString info = QString( "<b>%1</b><br>"
	                        "Size: %2 x %3<br>"
	                        "Index: %4<br>"
	                        "Flags: %5" )
	                   .arg( name )
	                   .arg( width )
	                   .arg( height )
	                   .arg( textureIndex )
	                   .arg( formatTextureFlags( flags ) );

	m_selectionInfo->setText( info );
}

void TexturesPanel::clearThumbnails() {
	// Remove all thumbnail widgets
	QLayoutItem *item;
	while ( ( item = m_thumbnailLayout->takeAt( 0 ) ) != nullptr ) {
		delete item->widget();
		delete item;
	}
	m_selectedTextureIndex = -1;

	// Clear cached images
	m_cachedFullImages.clear();
}

QString TexturesPanel::formatTextureFlags( int flags ) const {
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
