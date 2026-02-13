/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   BonesPanel.cpp  -  Bone hierarchy tree viewer implementation
 *
 * ===========================================================================
 */

#include "BonesPanel.h"
#include "../widgets/ModelViewport.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QMap>

BonesPanel::BonesPanel( QWidget *parent )
    : InspectorPanel( parent )
    , m_boneTree( nullptr )
    , m_infoLabel( nullptr ) {
	setupUI();
}

void BonesPanel::setupUI() {
	QVBoxLayout *contentLayout = new QVBoxLayout( m_contentWidget );
	contentLayout->setContentsMargins( 4, 4, 4, 4 );
	contentLayout->setSpacing( 8 );

	// Bone tree widget
	m_boneTree = new QTreeWidget( m_contentWidget );
	m_boneTree->setHeaderLabels( { "Bone", "Index" } );
	m_boneTree->setColumnCount( 2 );
	m_boneTree->header()->setSectionResizeMode( 0, QHeaderView::Stretch );
	m_boneTree->header()->setSectionResizeMode( 1, QHeaderView::Fixed );
	m_boneTree->setColumnWidth( 1, 50 );
	m_boneTree->setAlternatingRowColors( true );
	m_boneTree->setIndentation( 16 );
	m_boneTree->setToolTip( "Click a bone to view its details and parent relationship" );

	m_boneTree->setStyleSheet(
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
	    "QTreeWidget::branch {"
	    "    background-color: transparent;"
	    "}"
	    "QTreeWidget::branch:has-children:!has-siblings:closed,"
	    "QTreeWidget::branch:closed:has-children:has-siblings {"
	    "    image: url(:/icons/branch-closed.png);"
	    "}"
	    "QTreeWidget::branch:open:has-children:!has-siblings,"
	    "QTreeWidget::branch:open:has-children:has-siblings {"
	    "    image: url(:/icons/branch-open.png);"
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

	connect( m_boneTree, &QTreeWidget::itemClicked, this, &BonesPanel::onBoneSelected );
	contentLayout->addWidget( m_boneTree, 1 );

	// Info label for selected bone
	m_infoLabel = new QLabel( m_contentWidget );
	m_infoLabel->setMinimumHeight( 40 );
	m_infoLabel->setWordWrap( true );
	m_infoLabel->setToolTip( "Information about the selected bone" );
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

void BonesPanel::refresh() {
	m_boneTree->clear();
	m_infoLabel->clear();

	if ( !hasModel() ) {
		return;
	}

	buildBoneTree();

	// Expand all by default
	m_boneTree->expandAll();

	// Show summary
	m_infoLabel->setText( QString( "Total bones: %1" ).arg( m_viewport->getBoneCount() ) );
}

void BonesPanel::onBoneSelected( QTreeWidgetItem *item, int column ) {
	Q_UNUSED( column );

	if ( !item || !hasModel() ) return;

	// Get bone index from item data
	int boneIndex = item->data( 0, Qt::UserRole ).toInt();
	QString boneName = m_viewport->getBoneName( boneIndex );
	int parentIndex = m_viewport->getBoneParent( boneIndex );

	QString info = QString( "<b>%1</b> (Index: %2)" ).arg( boneName ).arg( boneIndex );

	if ( parentIndex >= 0 ) {
		QString parentName = m_viewport->getBoneName( parentIndex );
		info += QString( "<br>Parent: %1 (%2)" ).arg( parentName ).arg( parentIndex );
	} else {
		info += "<br>Parent: None (root bone)";
	}

	m_infoLabel->setText( info );
}

void BonesPanel::buildBoneTree() {
	if ( !hasModel() ) return;

	int boneCount = m_viewport->getBoneCount();

	// Map bone index to tree widget item
	QMap<int, QTreeWidgetItem *> boneItems;

	// First pass: create all items
	for ( int i = 0; i < boneCount; ++i ) {
		QString name = m_viewport->getBoneName( i );

		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText( 0, name );
		item->setText( 1, QString::number( i ) );
		item->setData( 0, Qt::UserRole, i ); // Store bone index
		item->setTextAlignment( 1, Qt::AlignCenter );

		boneItems[i] = item;
	}

	// Second pass: build hierarchy
	for ( int i = 0; i < boneCount; ++i ) {
		int parent = m_viewport->getBoneParent( i );

		if ( parent < 0 ) {
			// Root bone
			m_boneTree->addTopLevelItem( boneItems[i] );
		} else {
			// Child bone
			if ( boneItems.contains( parent ) ) {
				boneItems[parent]->addChild( boneItems[i] );
			} else {
				// Parent not found, add as root (shouldn't happen in valid MDL)
				m_boneTree->addTopLevelItem( boneItems[i] );
			}
		}
	}
}
