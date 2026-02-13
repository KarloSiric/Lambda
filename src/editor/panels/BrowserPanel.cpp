/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   BrowserPanel.cpp  -  File/folder browser panel implementation
 *
 * ===========================================================================
 */

#include "BrowserPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QHeaderView>
#include <QMessageBox>

BrowserPanel::BrowserPanel( QWidget *parent )
    : InspectorPanel( parent )
    , m_openFolderBtn( nullptr )
    , m_openPakBtn( nullptr )
    , m_searchBox( nullptr )
    , m_fileTree( nullptr ) {
	setupUI();

	// Browser always shows content (no model needed)
	showPlaceholder( false );
}

void BrowserPanel::setViewport( ModelViewport *viewport ) {
	// Store viewport but don't connect signals - browser is independent
	m_viewport = viewport;
}

void BrowserPanel::refresh() {
	// Refresh current folder if one is open
	if ( !m_currentFolder.isEmpty() ) {
		populateFolder( m_currentFolder );
	}
}

void BrowserPanel::setupUI() {
	QVBoxLayout *contentLayout = new QVBoxLayout( m_contentWidget );
	contentLayout->setContentsMargins( 4, 4, 4, 4 );
	contentLayout->setSpacing( 8 );

	// Toolbar with Open Folder, Open PAK buttons
	QHBoxLayout *toolbarLayout = new QHBoxLayout();
	toolbarLayout->setSpacing( 4 );

	// 90s Windows 95 style for buttons
	QString buttonStyle =
	    "QPushButton {"
	    "    background-color: #c0c0c0;"
	    "    border: 2px solid;"
	    "    border-top-color: #ffffff;"
	    "    border-left-color: #ffffff;"
	    "    border-right-color: #808080;"
	    "    border-bottom-color: #808080;"
	    "    color: #000000;"
	    "    padding: 4px 8px;"
	    "    font-size: 11px;"
	    "}"
	    "QPushButton:pressed {"
	    "    border-top-color: #808080;"
	    "    border-left-color: #808080;"
	    "    border-right-color: #ffffff;"
	    "    border-bottom-color: #ffffff;"
	    "}"
	    "QPushButton:hover {"
	    "    background-color: #d4d4d4;"
	    "}"
	    "QPushButton:disabled {"
	    "    color: #808080;"
	    "}";

	m_openFolderBtn = new QPushButton( "Open Folder", m_contentWidget );
	m_openFolderBtn->setStyleSheet( buttonStyle );
	m_openFolderBtn->setToolTip( "Browse for a folder containing MDL model files" );
	connect( m_openFolderBtn, &QPushButton::clicked, this, &BrowserPanel::onOpenFolder );

	m_openPakBtn = new QPushButton( "Open PAK", m_contentWidget );
	m_openPakBtn->setStyleSheet( buttonStyle );
	m_openPakBtn->setEnabled( false ); // TODO: Implement PAK support
	m_openPakBtn->setToolTip( "PAK archive support coming soon" );
	connect( m_openPakBtn, &QPushButton::clicked, this, &BrowserPanel::onOpenPak );

	toolbarLayout->addWidget( m_openFolderBtn );
	toolbarLayout->addWidget( m_openPakBtn );
	toolbarLayout->addStretch();
	contentLayout->addLayout( toolbarLayout );

	// Search box - 90s Windows 95 style
	m_searchBox = new QLineEdit( m_contentWidget );
	m_searchBox->setPlaceholderText( "Search files..." );
	m_searchBox->setClearButtonEnabled( true );
	m_searchBox->setToolTip( "Filter files by name (case-insensitive)" );
	m_searchBox->setStyleSheet(
	    "QLineEdit {"
	    "    background-color: #ffffff;"
	    "    border: 2px inset;"
	    "    border-top-color: #808080;"
	    "    border-left-color: #808080;"
	    "    border-right-color: #ffffff;"
	    "    border-bottom-color: #ffffff;"
	    "    color: #000000;"
	    "    padding: 4px 8px;"
	    "}"
	    "QLineEdit:focus {"
	    "    background-color: #ffffff;"
	    "}" );
	connect( m_searchBox, &QLineEdit::textChanged, this, &BrowserPanel::onSearchTextChanged );
	contentLayout->addWidget( m_searchBox );

	// File tree - 90s Windows 95 style
	m_fileTree = new QTreeWidget( m_contentWidget );
	m_fileTree->setHeaderLabels( { "Name", "Size" } );
	m_fileTree->setColumnCount( 2 );
	m_fileTree->header()->setSectionResizeMode( 0, QHeaderView::Stretch );
	m_fileTree->header()->setSectionResizeMode( 1, QHeaderView::Fixed );
	m_fileTree->setColumnWidth( 1, 70 );
	m_fileTree->setAlternatingRowColors( true );
	m_fileTree->setIndentation( 16 );
	m_fileTree->setRootIsDecorated( true );
	m_fileTree->setToolTip( "Double-click an MDL file to load it" );

	m_fileTree->setStyleSheet(
	    "QTreeWidget {"
	    "    background-color: #ffffff;"
	    "    alternate-background-color: #f0f0f0;"
	    "    color: #000000;"
	    "    border: 2px inset;"
	    "    border-top-color: #808080;"
	    "    border-left-color: #808080;"
	    "    border-right-color: #ffffff;"
	    "    border-bottom-color: #ffffff;"
	    "}"
	    "QTreeWidget::item {"
	    "    padding: 2px;"
	    "}"
	    "QTreeWidget::item:selected {"
	    "    background-color: #0a246a;"
	    "    color: #ffffff;"
	    "}"
	    "QHeaderView::section {"
	    "    background-color: #c0c0c0;"
	    "    color: #000000;"
	    "    padding: 2px 4px;"
	    "    border: 1px outset;"
	    "    border-top-color: #ffffff;"
	    "    border-left-color: #ffffff;"
	    "    border-right-color: #808080;"
	    "    border-bottom-color: #808080;"
	    "}" );

	connect( m_fileTree, &QTreeWidget::itemDoubleClicked, this, &BrowserPanel::onItemDoubleClicked );
	connect( m_fileTree, &QTreeWidget::itemExpanded, this, &BrowserPanel::onItemExpanded );
	contentLayout->addWidget( m_fileTree, 1 );
}

void BrowserPanel::onOpenFolder() {
	QString folder = QFileDialog::getExistingDirectory(
	    this,
	    "Select Folder",
	    m_currentFolder.isEmpty() ? QDir::homePath() : m_currentFolder,
	    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );

	if ( !folder.isEmpty() ) {
		populateFolder( folder );
	}
}

void BrowserPanel::onOpenPak() {
	QString pakFile = QFileDialog::getOpenFileName(
	    this,
	    "Open PAK Archive",
	    m_currentFolder.isEmpty() ? QDir::homePath() : m_currentFolder,
	    "PAK Archives (*.pak);;All Files (*)" );

	if ( !pakFile.isEmpty() ) {
		// TODO: Implement PAK archive browsing
		QMessageBox::information( this, "Not Implemented",
		                          "PAK archive browsing will be implemented soon." );
	}
}

void BrowserPanel::onSearchTextChanged( const QString &text ) {
	m_currentFilter = text;
	filterTree( text );
}

void BrowserPanel::onItemDoubleClicked( QTreeWidgetItem *item, int column ) {
	Q_UNUSED( column );

	if ( !item ) return;

	QString path = item->data( 0, Qt::UserRole ).toString();
	QFileInfo info( path );

	if ( info.isFile() ) {
		QString suffix = info.suffix().toLower();
		if ( suffix == "mdl" ) {
			emit modelFileSelected( path );
		}
	}
}

void BrowserPanel::onItemExpanded( QTreeWidgetItem *item ) {
	if ( !item ) return;

	// Lazy loading: populate children when expanded
	if ( item->childCount() == 1 && item->child( 0 )->text( 0 ).isEmpty() ) {
		// Remove dummy item
		delete item->takeChild( 0 );

		// Populate with actual children
		QString path = item->data( 0, Qt::UserRole ).toString();
		populateTreeItem( item, path );
	}
}

void BrowserPanel::populateFolder( const QString &folderPath ) {
	m_currentFolder = folderPath;
	m_fileTree->clear();

	QDir dir( folderPath );
	if ( !dir.exists() ) return;

	// Create root item for the folder
	QTreeWidgetItem *rootItem = new QTreeWidgetItem();
	rootItem->setText( 0, dir.dirName() );
	rootItem->setData( 0, Qt::UserRole, folderPath );
	rootItem->setIcon( 0, getFileIcon( "" ) ); // Folder icon

	// Populate first level
	populateTreeItem( rootItem, folderPath );

	m_fileTree->addTopLevelItem( rootItem );
	rootItem->setExpanded( true );

	// Apply any active filter
	if ( !m_currentFilter.isEmpty() ) {
		filterTree( m_currentFilter );
	}
}

void BrowserPanel::populateTreeItem( QTreeWidgetItem *parentItem, const QString &dirPath ) {
	QDir dir( dirPath );

	// Filter for relevant files
	QStringList filters;
	filters << "*.mdl" << "*.bmp" << "*.tga" << "*.png" << "*.pak" << "*.wad";

	// Get directories
	QFileInfoList dirs = dir.entryInfoList( QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name );
	for ( const QFileInfo &info : dirs ) {
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText( 0, info.fileName() );
		item->setData( 0, Qt::UserRole, info.absoluteFilePath() );
		item->setIcon( 0, getFileIcon( "" ) ); // Folder icon

		// Add dummy child for lazy loading
		QTreeWidgetItem *dummy = new QTreeWidgetItem();
		item->addChild( dummy );

		parentItem->addChild( item );
	}

	// Get files
	QFileInfoList files = dir.entryInfoList( filters, QDir::Files, QDir::Name );
	for ( const QFileInfo &info : files ) {
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText( 0, info.fileName() );
		item->setData( 0, Qt::UserRole, info.absoluteFilePath() );
		item->setIcon( 0, getFileIcon( info.fileName() ) );

		// Format file size
		qint64 size = info.size();
		QString sizeStr;
		if ( size < 1024 ) {
			sizeStr = QString( "%1 B" ).arg( size );
		} else if ( size < 1024 * 1024 ) {
			sizeStr = QString( "%1 KB" ).arg( size / 1024.0, 0, 'f', 1 );
		} else {
			sizeStr = QString( "%1 MB" ).arg( size / ( 1024.0 * 1024.0 ), 0, 'f', 2 );
		}
		item->setText( 1, sizeStr );
		item->setTextAlignment( 1, Qt::AlignRight | Qt::AlignVCenter );

		parentItem->addChild( item );
	}
}

void BrowserPanel::filterTree( const QString &filter ) {
	QTreeWidgetItemIterator it( m_fileTree );
	while ( *it ) {
		QTreeWidgetItem *item = *it;
		bool matches = filter.isEmpty() || itemMatchesFilter( item, filter );
		item->setHidden( !matches );
		++it;
	}

	// Make sure parent items are visible if they have visible children
	if ( !filter.isEmpty() ) {
		QTreeWidgetItemIterator it2( m_fileTree );
		while ( *it2 ) {
			QTreeWidgetItem *item = *it2;
			if ( !item->isHidden() ) {
				// Show all parents
				QTreeWidgetItem *parent = item->parent();
				while ( parent ) {
					parent->setHidden( false );
					parent->setExpanded( true );
					parent = parent->parent();
				}
			}
			++it2;
		}
	}
}

bool BrowserPanel::itemMatchesFilter( QTreeWidgetItem *item, const QString &filter ) {
	QString name = item->text( 0 );
	return name.contains( filter, Qt::CaseInsensitive );
}

QIcon BrowserPanel::getFileIcon( const QString &fileName ) const {
	// Simple icon based on file type
	// In a full implementation, these would be actual icon resources
	QString suffix = QFileInfo( fileName ).suffix().toLower();

	// For now, return empty icon - could add actual icons later
	// QStyle::SP_DirIcon for folders, SP_FileIcon for files
	if ( fileName.isEmpty() ) {
		return style()->standardIcon( QStyle::SP_DirIcon );
	} else if ( suffix == "mdl" ) {
		return style()->standardIcon( QStyle::SP_FileIcon );
	} else if ( suffix == "bmp" || suffix == "tga" || suffix == "png" ) {
		return style()->standardIcon( QStyle::SP_FileIcon );
	} else if ( suffix == "pak" || suffix == "wad" ) {
		return style()->standardIcon( QStyle::SP_DriveHDIcon );
	}

	return style()->standardIcon( QStyle::SP_FileIcon );
}
