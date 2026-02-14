/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   TexturesPanel.cpp  -  Texture thumbnail browser (BSP/Quake editor style)
 *
 * ===========================================================================
 */

#include "TexturesPanel.h"
#include "../widgets/ModelViewport.h"
#include "../widgets/FlowLayout.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QDialog>
#include <QMouseEvent>
#include <QMenu>
#include <QPainter>
#include <QFontMetrics>

// ============================================================================
// TextureThumbnail - Clean BSP/Quake style thumbnail
// ============================================================================

static const int THUMB_SIZE = 64;      // Thumbnail image size
static const int THUMB_PADDING = 4;    // Padding around image
static const int TEXT_HEIGHT = 16;     // Height for name text
static const int WIDGET_WIDTH = THUMB_SIZE + THUMB_PADDING * 2;
static const int WIDGET_HEIGHT = THUMB_SIZE + THUMB_PADDING * 2 + TEXT_HEIGHT;

TextureThumbnail::TextureThumbnail( int index, const QImage &image, const QString &name, QWidget *parent )
    : QWidget( parent )
    , m_index( index )
    , m_name( name )
    , m_selected( false )
    , m_hovered( false ) {
	// Scale image to thumbnail size
	QImage scaled = image.scaled( THUMB_SIZE, THUMB_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation );
	m_pixmap = QPixmap::fromImage( scaled );

	// Fixed size for consistent grid
	setFixedSize( WIDGET_WIDTH, WIDGET_HEIGHT );
	setCursor( Qt::PointingHandCursor );

	// Truncate long names
	QFontMetrics fm( font() );
	if ( fm.horizontalAdvance( m_name ) > WIDGET_WIDTH - 4 ) {
		m_name = fm.elidedText( m_name, Qt::ElideRight, WIDGET_WIDTH - 4 );
	}
}

void TextureThumbnail::setSelected( bool selected ) {
	if ( m_selected != selected ) {
		m_selected = selected;
		update();
	}
}

void TextureThumbnail::paintEvent( QPaintEvent * ) {
	QPainter p( this );
	p.setRenderHint( QPainter::Antialiasing, false );

	// Background
	if ( m_selected ) {
		p.fillRect( rect(), QColor( 0x00, 0x00, 0x80 ) );  // Dark blue selection
	} else if ( m_hovered ) {
		p.fillRect( rect(), QColor( 0x50, 0x50, 0x50 ) );  // Slightly lighter on hover
	} else {
		p.fillRect( rect(), QColor( 0x40, 0x40, 0x40 ) );  // Dark gray background
	}

	// Draw thumbnail image centered
	int imgX = ( width() - m_pixmap.width() ) / 2;
	int imgY = THUMB_PADDING;
	p.drawPixmap( imgX, imgY, m_pixmap );

	// Draw border around image
	QRect imgRect( imgX - 1, imgY - 1, m_pixmap.width() + 2, m_pixmap.height() + 2 );
	if ( m_selected ) {
		p.setPen( QColor( 0x80, 0x80, 0xFF ) );  // Light blue border when selected
	} else {
		p.setPen( QColor( 0x60, 0x60, 0x60 ) );  // Subtle border
	}
	p.drawRect( imgRect );

	// Draw name below image
	p.setPen( m_selected ? Qt::white : QColor( 0xC0, 0xC0, 0xC0 ) );
	QRect textRect( 2, THUMB_SIZE + THUMB_PADDING * 2, width() - 4, TEXT_HEIGHT );
	p.drawText( textRect, Qt::AlignHCenter | Qt::AlignTop, m_name );
}

void TextureThumbnail::mousePressEvent( QMouseEvent *event ) {
	if ( event->button() == Qt::LeftButton ) {
		emit clicked( m_index );
	} else if ( event->button() == Qt::RightButton ) {
		emit rightClicked( m_index, event->globalPosition().toPoint() );
	}
}

void TextureThumbnail::mouseDoubleClickEvent( QMouseEvent *event ) {
	if ( event->button() == Qt::LeftButton ) {
		emit doubleClicked( m_index );
	}
}

void TextureThumbnail::enterEvent( QEnterEvent * ) {
	m_hovered = true;
	update();
}

void TextureThumbnail::leaveEvent( QEvent * ) {
	m_hovered = false;
	update();
}

// ============================================================================
// TexturesPanel
// ============================================================================

TexturesPanel::TexturesPanel( QWidget *parent )
    : InspectorPanel( parent )
    , m_skinSelector( nullptr )
    , m_scrollArea( nullptr )
    , m_thumbnailContainer( nullptr )
    , m_flowLayout( nullptr )
    , m_selectionInfo( nullptr )
    , m_selectedTextureIndex( -1 ) {
	setupUI();
}

void TexturesPanel::setupUI() {
	QVBoxLayout *contentLayout = new QVBoxLayout( m_contentWidget );
	contentLayout->setContentsMargins( 4, 4, 4, 4 );
	contentLayout->setSpacing( 4 );

	// Skin selector row
	QHBoxLayout *skinLayout = new QHBoxLayout();
	skinLayout->setSpacing( 4 );

	QLabel *skinLabel = new QLabel( "Skin:", m_contentWidget );
	skinLabel->setStyleSheet( "color: #000000; font-weight: bold;" );

	m_skinSelector = new QComboBox( m_contentWidget );
	m_skinSelector->setMinimumWidth( 80 );
	connect( m_skinSelector, QOverload<int>::of( &QComboBox::currentIndexChanged ),
	         this, &TexturesPanel::onSkinFamilyChanged );

	skinLayout->addWidget( skinLabel );
	skinLayout->addWidget( m_skinSelector, 1 );
	contentLayout->addLayout( skinLayout );

	// Scrollable thumbnail area - dark background like BSP/Quake editors
	m_scrollArea = new QScrollArea( m_contentWidget );
	m_scrollArea->setWidgetResizable( true );
	m_scrollArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	m_scrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
	m_scrollArea->setStyleSheet(
	    "QScrollArea { background-color: #303030; border: 1px solid #505050; }"
	    "QScrollBar:vertical { background: #404040; width: 12px; }"
	    "QScrollBar::handle:vertical { background: #606060; min-height: 20px; }"
	    "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }" );

	m_thumbnailContainer = new QWidget( m_scrollArea );
	m_thumbnailContainer->setStyleSheet( "background-color: #303030;" );

	// Flow layout for wrapping thumbnails
	m_flowLayout = new FlowLayout( m_thumbnailContainer, 6, 4, 4 );

	m_scrollArea->setWidget( m_thumbnailContainer );
	contentLayout->addWidget( m_scrollArea, 1 );

	// Selection info at bottom
	m_selectionInfo = new QLabel( m_contentWidget );
	m_selectionInfo->setStyleSheet(
	    "QLabel { color: #000000; background-color: #e0e0e0; "
	    "border: 1px solid #808080; padding: 4px; font-size: 11px; }" );
	m_selectionInfo->setWordWrap( true );
	m_selectionInfo->setMinimumHeight( 36 );
	m_selectionInfo->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
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
	if ( !m_thumbnails.isEmpty() ) {
		selectThumbnail( 0 );
	} else {
		m_selectionInfo->setText( "No textures" );
	}
}

void TexturesPanel::onSkinFamilyChanged( int index ) {
	if ( m_viewport && index >= 0 ) {
		m_viewport->setSkinFamily( index );
	}
}

void TexturesPanel::onThumbnailClicked( int index ) {
	selectThumbnail( index );
}

void TexturesPanel::onThumbnailDoubleClicked( int index ) {
	showTexturePreview( index );
}

void TexturesPanel::onThumbnailRightClicked( int index, const QPoint &globalPos ) {
	selectThumbnail( index );

	QMenu menu( this );
	menu.setStyleSheet(
	    "QMenu { background-color: #e0e0e0; border: 1px solid #808080; }"
	    "QMenu::item { padding: 4px 16px; color: #000000; }"
	    "QMenu::item:selected { background-color: #000080; color: #ffffff; }" );

	QAction *previewAction = menu.addAction( "Preview Full Size" );
	QAction *tabAction = menu.addAction( "Open in Tab" );

	QAction *selected = menu.exec( globalPos );
	if ( selected == previewAction ) {
		showTexturePreview( index );
	} else if ( selected == tabAction ) {
		if ( hasModel() && index >= 0 && index < m_viewport->getTextureCount() ) {
			QImage image = ( index < m_cachedFullImages.size() )
			                   ? m_cachedFullImages[index]
			                   : m_viewport->getTextureImage( index );
			QString name = m_viewport->getTextureName( index );
			int width, height;
			m_viewport->getTextureDimensions( index, width, height );
			int flags = m_viewport->getTextureFlags( index );
			emit requestTextureTab( image, name, width, height, flags );
		}
	}
}

void TexturesPanel::selectThumbnail( int index ) {
	// Deselect previous
	if ( m_selectedTextureIndex >= 0 && m_selectedTextureIndex < m_thumbnails.size() ) {
		m_thumbnails[m_selectedTextureIndex]->setSelected( false );
	}

	m_selectedTextureIndex = index;

	// Select new
	if ( index >= 0 && index < m_thumbnails.size() ) {
		m_thumbnails[index]->setSelected( true );
		updateSelectionInfo( index );
	}
}

void TexturesPanel::populateThumbnails() {
	if ( !hasModel() ) return;

	int count = m_viewport->getTextureCount();

	m_cachedFullImages.clear();
	m_cachedFullImages.reserve( count );
	m_thumbnails.clear();
	m_thumbnails.reserve( count );

	for ( int i = 0; i < count; ++i ) {
		QImage image = m_viewport->getTextureImage( i );
		QString name = m_viewport->getTextureName( i );

		m_cachedFullImages.append( image );

		TextureThumbnail *thumb = new TextureThumbnail( i, image, name, m_thumbnailContainer );
		connect( thumb, &TextureThumbnail::clicked, this, &TexturesPanel::onThumbnailClicked );
		connect( thumb, &TextureThumbnail::doubleClicked, this, &TexturesPanel::onThumbnailDoubleClicked );
		connect( thumb, &TextureThumbnail::rightClicked, this, &TexturesPanel::onThumbnailRightClicked );

		m_flowLayout->addWidget( thumb );
		m_thumbnails.append( thumb );
	}
}

void TexturesPanel::showTexturePreview( int textureIndex ) {
	if ( !hasModel() || textureIndex < 0 || textureIndex >= m_viewport->getTextureCount() ) {
		return;
	}

	QImage fullImage = ( textureIndex < m_cachedFullImages.size() )
	                       ? m_cachedFullImages[textureIndex]
	                       : m_viewport->getTextureImage( textureIndex );
	if ( fullImage.isNull() ) return;

	QString name = m_viewport->getTextureName( textureIndex );
	int width, height;
	m_viewport->getTextureDimensions( textureIndex, width, height );
	int flags = m_viewport->getTextureFlags( textureIndex );

	QDialog *preview = new QDialog( this );
	preview->setWindowTitle( QString( "Texture: %1 (%2x%3)" ).arg( name ).arg( width ).arg( height ) );
	preview->setAttribute( Qt::WA_DeleteOnClose );
	preview->setStyleSheet( "QDialog { background-color: #202020; }" );

	QVBoxLayout *layout = new QVBoxLayout( preview );
	layout->setContentsMargins( 8, 8, 8, 8 );

	QScrollArea *scroll = new QScrollArea( preview );
	scroll->setWidgetResizable( false );
	scroll->setAlignment( Qt::AlignCenter );
	scroll->setStyleSheet( "QScrollArea { background-color: #000000; border: none; }" );

	QLabel *imageLabel = new QLabel( scroll );
	imageLabel->setPixmap( QPixmap::fromImage( fullImage ) );
	imageLabel->setAlignment( Qt::AlignCenter );
	scroll->setWidget( imageLabel );

	layout->addWidget( scroll, 1 );

	QLabel *infoLabel = new QLabel( preview );
	infoLabel->setText( QString( "Size: %1 x %2  |  Flags: %3" )
	                        .arg( width )
	                        .arg( height )
	                        .arg( formatTextureFlags( flags ) ) );
	infoLabel->setStyleSheet( "color: #c0c0c0; padding: 4px;" );
	infoLabel->setAlignment( Qt::AlignCenter );
	layout->addWidget( infoLabel );

	int dialogWidth = qMin( fullImage.width() + 40, 600 );
	int dialogHeight = qMin( fullImage.height() + 60, 500 );
	preview->resize( qMax( dialogWidth, 200 ), qMax( dialogHeight, 150 ) );
	preview->show();
}

void TexturesPanel::updateSelectionInfo( int textureIndex ) {
	if ( !hasModel() || textureIndex < 0 || textureIndex >= m_viewport->getTextureCount() ) {
		m_selectionInfo->setText( "" );
		return;
	}

	QString name = m_viewport->getTextureName( textureIndex );
	int width, height;
	m_viewport->getTextureDimensions( textureIndex, width, height );
	int flags = m_viewport->getTextureFlags( textureIndex );

	m_selectionInfo->setText( QString( "<b>%1</b> | %2x%3 | %4" )
	                              .arg( name )
	                              .arg( width )
	                              .arg( height )
	                              .arg( formatTextureFlags( flags ) ) );
}

void TexturesPanel::clearThumbnails() {
	for ( TextureThumbnail *thumb : m_thumbnails ) {
		m_flowLayout->removeWidget( thumb );
		delete thumb;
	}
	m_thumbnails.clear();
	m_cachedFullImages.clear();
	m_selectedTextureIndex = -1;
}

QString TexturesPanel::formatTextureFlags( int flags ) const {
	QStringList flagNames;
	if ( flags & 0x0001 ) flagNames << "Flat";
	if ( flags & 0x0002 ) flagNames << "Chrome";
	if ( flags & 0x0004 ) flagNames << "Fullbright";
	if ( flags & 0x0008 ) flagNames << "NoMips";
	if ( flags & 0x0010 ) flagNames << "Alpha";
	if ( flags & 0x0020 ) flagNames << "Additive";
	if ( flags & 0x0040 ) flagNames << "Masked";

	return flagNames.isEmpty() ? "None" : flagNames.join( ", " );
}
