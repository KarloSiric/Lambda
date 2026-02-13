/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   ModelInfoPanel.cpp  -  Model metadata property grid implementation
 *
 * ===========================================================================
 */

#include "ModelInfoPanel.h"
#include "../widgets/ModelViewport.h"
#include <QHeaderView>
#include <QFileInfo>

ModelInfoPanel::ModelInfoPanel( QWidget *parent )
    : InspectorPanel( parent )
    , m_propertyTable( nullptr )
    , m_currentRow( 0 ) {
	setupUI();
}

void ModelInfoPanel::setupUI() {
	// Create table widget for property grid
	m_propertyTable = new QTableWidget( this );
	m_propertyTable->setColumnCount( 2 );
	m_propertyTable->setHorizontalHeaderLabels( { "Property", "Value" } );
	m_propertyTable->horizontalHeader()->setVisible( false );
	m_propertyTable->verticalHeader()->setVisible( false );
	m_propertyTable->setAlternatingRowColors( true );
	m_propertyTable->setEditTriggers( QAbstractItemView::NoEditTriggers );
	m_propertyTable->setSelectionBehavior( QAbstractItemView::SelectRows );
	m_propertyTable->setSelectionMode( QAbstractItemView::SingleSelection );
	m_propertyTable->setShowGrid( true );
	m_propertyTable->setGridStyle( Qt::SolidLine );

	// Column sizing
	m_propertyTable->horizontalHeader()->setSectionResizeMode( 0, QHeaderView::ResizeToContents );
	m_propertyTable->horizontalHeader()->setSectionResizeMode( 1, QHeaderView::Stretch );
	m_propertyTable->verticalHeader()->setSectionResizeMode( QHeaderView::Fixed );
	m_propertyTable->verticalHeader()->setDefaultSectionSize( 22 );

	// Style the table - 90s Windows 95 style
	m_propertyTable->setToolTip( "Model file information and statistics" );
	m_propertyTable->setStyleSheet(
	    "QTableWidget {"
	    "    background-color: #ffffff;"
	    "    alternate-background-color: #f0f0f0;"
	    "    gridline-color: #c0c0c0;"
	    "    color: #000000;"
	    "    selection-background-color: #0a246a;"
	    "    selection-color: #ffffff;"
	    "    border: 2px inset;"
	    "    border-top-color: #808080;"
	    "    border-left-color: #808080;"
	    "    border-right-color: #ffffff;"
	    "    border-bottom-color: #ffffff;"
	    "}"
	    "QTableWidget::item {"
	    "    padding: 2px 6px;"
	    "}" );

	// Replace the content widget layout
	QVBoxLayout *contentLayout = new QVBoxLayout( m_contentWidget );
	contentLayout->setContentsMargins( 0, 0, 0, 0 );
	contentLayout->addWidget( m_propertyTable );
}

void ModelInfoPanel::refresh() {
	if ( !hasModel() ) {
		clearProperties();
		return;
	}

	clearProperties();

	// File information section
	QString filePath = m_viewport->property( "modelPath" ).toString();
	QFileInfo fileInfo( filePath );

	addProperty( "Name", fileInfo.fileName() );
	addProperty( "Path", fileInfo.absolutePath() );
	addProperty( "Size", formatFileSize( fileInfo.size() ) );
	addProperty( "Version", QString( "MDL v%1 (HL1)" ).arg( m_viewport->getMDLVersion() ) );
	addProperty( "Flags", QString( "0x%1" ).arg( m_viewport->getModelFlags(), 4, 16, QChar( '0' ) ) );

	// Separator
	addProperty( "", "", true );

	// Geometry counts
	addProperty( "Vertices", QString::number( m_viewport->getVertexCount() ) );
	addProperty( "Triangles", QString::number( m_viewport->getTriangleCount() ) );
	addProperty( "Bodyparts", QString::number( m_viewport->getBodypartCount() ) );
	addProperty( "Bones", QString::number( m_viewport->getBoneCount() ) );
	addProperty( "Controllers", QString::number( m_viewport->getControllerCount() ) );

	// Separator
	addProperty( "", "", true );

	// Structure counts
	addProperty( "Sequences", QString::number( m_viewport->getSequenceCount() ) );
	addProperty( "Seq Groups", QString::number( m_viewport->getSeqGroupCount() ) );
	addProperty( "Textures", QString::number( m_viewport->getTextureCount() ) );
	addProperty( "Skins", QString::number( m_viewport->getNumSkinFamilies() ) );
	addProperty( "Attachments", QString::number( m_viewport->getAttachmentCount() ) );
	addProperty( "Hitboxes", QString::number( m_viewport->getHitboxCount() ) );

	// Separator
	addProperty( "", "", true );

	// Bounding box
	float bbmin[3], bbmax[3];
	m_viewport->getBoundingBox( bbmin, bbmax );
	addProperty( "BBox Min", formatVector( bbmin[0], bbmin[1], bbmin[2] ) );
	addProperty( "BBox Max", formatVector( bbmax[0], bbmax[1], bbmax[2] ) );

	// Eye position
	float eyeX, eyeY, eyeZ;
	m_viewport->getEyePosition( eyeX, eyeY, eyeZ );
	addProperty( "Eye Pos", formatVector( eyeX, eyeY, eyeZ ) );
}

void ModelInfoPanel::addProperty( const QString &name, const QString &value, bool isSeparator ) {
	int row = m_propertyTable->rowCount();
	m_propertyTable->insertRow( row );

	QTableWidgetItem *nameItem = new QTableWidgetItem( name );
	QTableWidgetItem *valueItem = new QTableWidgetItem( value );

	if ( isSeparator ) {
		// Make separator row visually distinct - 90s Windows 95 style
		nameItem->setBackground( QColor( 0xc0, 0xc0, 0xc0 ) );
		valueItem->setBackground( QColor( 0xc0, 0xc0, 0xc0 ) );
		nameItem->setFlags( Qt::NoItemFlags );
		valueItem->setFlags( Qt::NoItemFlags );
	} else {
		// Name column is bold - 90s Windows 95 style
		QFont boldFont = nameItem->font();
		boldFont.setBold( true );
		nameItem->setFont( boldFont );
		nameItem->setForeground( QColor( 0x00, 0x00, 0x00 ) );
	}

	m_propertyTable->setItem( row, 0, nameItem );
	m_propertyTable->setItem( row, 1, valueItem );
}

void ModelInfoPanel::clearProperties() {
	m_propertyTable->setRowCount( 0 );
	m_currentRow = 0;
}

QString ModelInfoPanel::formatFileSize( qint64 bytes ) const {
	if ( bytes < 1024 ) {
		return QString( "%1 B" ).arg( bytes );
	} else if ( bytes < 1024 * 1024 ) {
		return QString( "%1 KB" ).arg( bytes / 1024.0, 0, 'f', 1 );
	} else {
		return QString( "%1 MB" ).arg( bytes / ( 1024.0 * 1024.0 ), 0, 'f', 2 );
	}
}

QString ModelInfoPanel::formatVector( float x, float y, float z ) const {
	return QString( "(%1, %2, %3)" )
	    .arg( x, 0, 'f', 1 )
	    .arg( y, 0, 'f', 1 )
	    .arg( z, 0, 'f', 1 );
}
