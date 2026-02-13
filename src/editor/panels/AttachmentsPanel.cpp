/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   AttachmentsPanel.cpp  -  Attachment points viewer implementation
 *
 * ===========================================================================
 */

#include "AttachmentsPanel.h"
#include "../widgets/ModelViewport.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QFrame>

AttachmentsPanel::AttachmentsPanel( QWidget *parent )
    : InspectorPanel( parent )
    , m_attachmentTable( nullptr )
    , m_infoLabel( nullptr ) {
	setupUI();
}

void AttachmentsPanel::setupUI() {
	QVBoxLayout *contentLayout = new QVBoxLayout( m_contentWidget );
	contentLayout->setContentsMargins( 4, 4, 4, 4 );
	contentLayout->setSpacing( 8 );

	// Attachment table
	m_attachmentTable = new QTableWidget( m_contentWidget );
	m_attachmentTable->setColumnCount( 4 );
	m_attachmentTable->setHorizontalHeaderLabels( { "#", "Name", "Bone", "Type" } );
	m_attachmentTable->horizontalHeader()->setVisible( true );
	m_attachmentTable->verticalHeader()->setVisible( false );
	m_attachmentTable->setAlternatingRowColors( true );
	m_attachmentTable->setEditTriggers( QAbstractItemView::NoEditTriggers );
	m_attachmentTable->setSelectionBehavior( QAbstractItemView::SelectRows );
	m_attachmentTable->setSelectionMode( QAbstractItemView::SingleSelection );
	m_attachmentTable->setToolTip( "Click an attachment to view its details" );

	// Column sizing
	m_attachmentTable->horizontalHeader()->setSectionResizeMode( 0, QHeaderView::Fixed );
	m_attachmentTable->setColumnWidth( 0, 30 );
	m_attachmentTable->horizontalHeader()->setSectionResizeMode( 1, QHeaderView::Stretch );
	m_attachmentTable->horizontalHeader()->setSectionResizeMode( 2, QHeaderView::Stretch );
	m_attachmentTable->horizontalHeader()->setSectionResizeMode( 3, QHeaderView::Fixed );
	m_attachmentTable->setColumnWidth( 3, 45 );
	m_attachmentTable->verticalHeader()->setDefaultSectionSize( 20 );

	// 90s Windows 95 style for table
	m_attachmentTable->setStyleSheet(
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

	connect( m_attachmentTable, &QTableWidget::cellClicked, this, &AttachmentsPanel::onAttachmentSelected );
	contentLayout->addWidget( m_attachmentTable, 1 );

	// Separator
	QFrame *separator = new QFrame( m_contentWidget );
	separator->setFrameShape( QFrame::HLine );
	separator->setStyleSheet( "background-color: #808080;" );
	contentLayout->addWidget( separator );

	// Info box for selected attachment
	m_infoLabel = new QLabel( m_contentWidget );
	m_infoLabel->setMinimumHeight( 60 );
	m_infoLabel->setWordWrap( true );
	m_infoLabel->setToolTip( "Details about the selected attachment point" );
	m_infoLabel->setStyleSheet(
	    "QLabel {"
	    "    background-color: #ffffff;"
	    "    color: #000000;"
	    "    padding: 6px;"
	    "    border: 2px inset;"
	    "    border-top-color: #808080;"
	    "    border-left-color: #808080;"
	    "    border-right-color: #ffffff;"
	    "    border-bottom-color: #ffffff;"
	    "}" );
	contentLayout->addWidget( m_infoLabel );
}

void AttachmentsPanel::refresh() {
	m_attachmentTable->setRowCount( 0 );
	m_infoLabel->clear();

	if ( !hasModel() ) {
		m_infoLabel->setText( "No model loaded" );
		return;
	}

	populateAttachments();

	// Show summary
	int count = m_viewport->getAttachmentCount();
	if ( count == 0 ) {
		m_infoLabel->setText( "This model has no attachment points" );
	} else {
		m_infoLabel->setText( QString( "Total attachments: %1\nClick a row to view details" ).arg( count ) );
	}
}

void AttachmentsPanel::onAttachmentSelected( int row, int column ) {
	Q_UNUSED( column );
	updateInfoBox( row );
}

void AttachmentsPanel::populateAttachments() {
	if ( !hasModel() ) return;

	int count = m_viewport->getAttachmentCount();
	for ( int i = 0; i < count; ++i ) {
		int row = m_attachmentTable->rowCount();
		m_attachmentTable->insertRow( row );

		// Index
		QTableWidgetItem *indexItem = new QTableWidgetItem( QString::number( i ) );
		indexItem->setTextAlignment( Qt::AlignCenter );
		m_attachmentTable->setItem( row, 0, indexItem );

		// Name
		QString name = m_viewport->getAttachmentName( i );
		if ( name.isEmpty() ) name = QString( "attachment_%1" ).arg( i );
		QTableWidgetItem *nameItem = new QTableWidgetItem( name );
		m_attachmentTable->setItem( row, 1, nameItem );

		// Bone name
		int boneIndex = m_viewport->getAttachmentBone( i );
		QString boneName;
		if ( boneIndex >= 0 && boneIndex < m_viewport->getBoneCount() ) {
			boneName = m_viewport->getBoneName( boneIndex );
		} else {
			boneName = QString( "bone %1" ).arg( boneIndex );
		}
		QTableWidgetItem *boneItem = new QTableWidgetItem( boneName );
		m_attachmentTable->setItem( row, 2, boneItem );

		// Type
		int type = m_viewport->getAttachmentType( i );
		QTableWidgetItem *typeItem = new QTableWidgetItem( QString::number( type ) );
		typeItem->setTextAlignment( Qt::AlignCenter );
		m_attachmentTable->setItem( row, 3, typeItem );
	}
}

void AttachmentsPanel::updateInfoBox( int attachmentIndex ) {
	if ( !hasModel() || attachmentIndex < 0 || attachmentIndex >= m_viewport->getAttachmentCount() ) {
		m_infoLabel->clear();
		return;
	}

	QString name = m_viewport->getAttachmentName( attachmentIndex );
	if ( name.isEmpty() ) name = QString( "attachment_%1" ).arg( attachmentIndex );

	int boneIndex = m_viewport->getAttachmentBone( attachmentIndex );
	QString boneName;
	if ( boneIndex >= 0 && boneIndex < m_viewport->getBoneCount() ) {
		boneName = m_viewport->getBoneName( boneIndex );
	} else {
		boneName = QString( "bone %1" ).arg( boneIndex );
	}

	float x, y, z;
	m_viewport->getAttachmentPosition( attachmentIndex, x, y, z );

	int type = m_viewport->getAttachmentType( attachmentIndex );

	QString info = QString( "<b>%1</b> (Index: %2)<br>"
	                        "Bone: %3 (%4)<br>"
	                        "Position: (%.2f, %.2f, %.2f)<br>"
	                        "Type: %5" )
	                   .arg( name )
	                   .arg( attachmentIndex )
	                   .arg( boneName )
	                   .arg( boneIndex )
	                   .arg( x )
	                   .arg( y )
	                   .arg( z )
	                   .arg( type );

	m_infoLabel->setText( info );
}
