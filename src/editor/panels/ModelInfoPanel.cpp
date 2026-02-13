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
#include <QHBoxLayout>
#include <QDialog>
#include <QTextEdit>
#include <QClipboard>
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>

ModelInfoPanel::ModelInfoPanel( QWidget *parent )
    : InspectorPanel( parent )
    , m_propertyTable( nullptr )
    , m_dumpModelBtn( nullptr )
    , m_dumpTextureBtn( nullptr )
    , m_currentRow( 0 ) {
	setupUI();
}

void ModelInfoPanel::setupUI() {
	QVBoxLayout *contentLayout = new QVBoxLayout( m_contentWidget );
	contentLayout->setContentsMargins( 4, 4, 4, 4 );
	contentLayout->setSpacing( 4 );

	// 90s style for buttons
	QString buttonStyle =
	    "QPushButton {"
	    "    background-color: #c0c0c0;"
	    "    border: 2px outset;"
	    "    border-top-color: #ffffff;"
	    "    border-left-color: #ffffff;"
	    "    border-right-color: #808080;"
	    "    border-bottom-color: #808080;"
	    "    padding: 4px 8px;"
	    "    color: #000000;"
	    "    font-weight: bold;"
	    "}"
	    "QPushButton:pressed {"
	    "    border: 2px inset;"
	    "    border-top-color: #808080;"
	    "    border-left-color: #808080;"
	    "    border-right-color: #ffffff;"
	    "    border-bottom-color: #ffffff;"
	    "}"
	    "QPushButton:disabled {"
	    "    color: #808080;"
	    "}"
	    "QPushButton:hover {"
	    "    background-color: #d4d4d4;"
	    "}";

	// Button row
	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->setSpacing( 4 );

	m_dumpModelBtn = new QPushButton( "Dump Model Info", m_contentWidget );
	m_dumpModelBtn->setStyleSheet( buttonStyle );
	m_dumpModelBtn->setToolTip( "Generate detailed model information report" );
	connect( m_dumpModelBtn, &QPushButton::clicked, this, &ModelInfoPanel::onDumpModelInfo );
	buttonLayout->addWidget( m_dumpModelBtn );

	m_dumpTextureBtn = new QPushButton( "Dump Texture Info", m_contentWidget );
	m_dumpTextureBtn->setStyleSheet( buttonStyle );
	m_dumpTextureBtn->setToolTip( "Generate detailed texture information report" );
	connect( m_dumpTextureBtn, &QPushButton::clicked, this, &ModelInfoPanel::onDumpTextureInfo );
	buttonLayout->addWidget( m_dumpTextureBtn );

	contentLayout->addLayout( buttonLayout );

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

	contentLayout->addWidget( m_propertyTable );
}

void ModelInfoPanel::refresh() {
	bool hasModelLoaded = hasModel();
	m_dumpModelBtn->setEnabled( hasModelLoaded );
	m_dumpTextureBtn->setEnabled( hasModelLoaded );

	if ( !hasModelLoaded ) {
		clearProperties();
		return;
	}

	clearProperties();

	// === Model Header ===
	QString filePath = m_viewport->property( "modelPath" ).toString();
	QFileInfo fileInfo( filePath );

	addProperty( "Name", fileInfo.fileName() );
	addProperty( "Internal", m_viewport->getHeaderName() );

	// Format header ID as magic string (IDST)
	int headerId = m_viewport->getHeaderId();
	char magic[5] = { 0 };
	magic[0] = headerId & 0xFF;
	magic[1] = ( headerId >> 8 ) & 0xFF;
	magic[2] = ( headerId >> 16 ) & 0xFF;
	magic[3] = ( headerId >> 24 ) & 0xFF;
	addProperty( "ID", QString( "%1 (0x%2)" ).arg( magic ).arg( headerId, 8, 16, QChar( '0' ) ) );

	addProperty( "Version", QString( "MDL v%1 (HL1)" ).arg( m_viewport->getMDLVersion() ) );
	addProperty( "File Size", formatFileSize( m_viewport->getFileLength() ) );
	addProperty( "Flags", QString( "0x%1" ).arg( m_viewport->getModelFlags(), 4, 16, QChar( '0' ) ) );

	// Separator - Geometry
	addProperty( "", "", true );

	// Geometry counts
	addProperty( "Vertices", QString::number( m_viewport->getVertexCount() ) );
	addProperty( "Triangles", QString::number( m_viewport->getTriangleCount() ) );
	addProperty( "Bodyparts", QString::number( m_viewport->getBodypartCount() ) );
	addProperty( "Bones", QString::number( m_viewport->getBoneCount() ) );
	addProperty( "Controllers", QString::number( m_viewport->getControllerCount() ) );

	// Separator - Structures
	addProperty( "", "", true );

	// Structure counts
	addProperty( "Sequences", QString::number( m_viewport->getSequenceCount() ) );
	addProperty( "Seq Groups", QString::number( m_viewport->getSeqGroupCount() ) );
	addProperty( "Transitions", QString::number( m_viewport->getTransitionCount() ) );
	addProperty( "Textures", QString::number( m_viewport->getTextureCount() ) );
	addProperty( "Skin Refs", QString::number( m_viewport->getSkinRefCount() ) );
	addProperty( "Skins", QString::number( m_viewport->getNumSkinFamilies() ) );
	addProperty( "Attachments", QString::number( m_viewport->getAttachmentCount() ) );
	addProperty( "Hitboxes", QString::number( m_viewport->getHitboxCount() ) );
	addProperty( "Sound Groups", QString::number( m_viewport->getSoundGroupCount() ) );

	// Separator - Bounds
	addProperty( "", "", true );

	// Bounding boxes
	float bbmin[3], bbmax[3];
	m_viewport->getBoundingBox( bbmin, bbmax );
	addProperty( "BBox Min", formatVector( bbmin[0], bbmin[1], bbmin[2] ) );
	addProperty( "BBox Max", formatVector( bbmax[0], bbmax[1], bbmax[2] ) );

	// Movement bounds (clipping hull)
	float min[3], max[3];
	m_viewport->getMovementBox( min, max );
	addProperty( "Hull Min", formatVector( min[0], min[1], min[2] ) );
	addProperty( "Hull Max", formatVector( max[0], max[1], max[2] ) );

	// Eye position
	float eyeX, eyeY, eyeZ;
	m_viewport->getEyePosition( eyeX, eyeY, eyeZ );
	addProperty( "Eye Pos", formatVector( eyeX, eyeY, eyeZ ) );

	// Separator - Memory Breakdown
	addProperty( "", "", true );

	// Memory breakdown (estimated sizes from MDL format structure sizes)
	int numBones = m_viewport->getBoneCount();
	int numControllers = m_viewport->getControllerCount();
	int numHitboxes = m_viewport->getHitboxCount();
	int numSeqs = m_viewport->getSequenceCount();
	int numTextures = m_viewport->getTextureCount();
	int numAttachments = m_viewport->getAttachmentCount();

	// Calculate estimated memory for structures (approximate sizes from MDL format)
	qint64 headerSize = 244;  // sizeof(studiohdr_t)
	qint64 boneSize = numBones * 112;  // sizeof(mstudiobone_t) ≈ 112
	qint64 ctrlSize = numControllers * 24;  // sizeof(mstudiobonecontroller_t) ≈ 24
	qint64 hitboxSize = numHitboxes * 68;  // sizeof(mstudiobbox_t) ≈ 68
	qint64 seqSize = numSeqs * 176;  // sizeof(mstudioseqdesc_t) ≈ 176
	qint64 texInfoSize = numTextures * 80;  // sizeof(mstudiotexture_t) ≈ 80
	qint64 attachSize = numAttachments * 88;  // sizeof(mstudioattachment_t) ≈ 88

	addProperty( "Header", formatFileSize( headerSize ) );
	addProperty( "Bones", QString( "%1 (%2)" ).arg( formatFileSize( boneSize ) ).arg( numBones ) );
	addProperty( "Controllers", QString( "%1 (%2)" ).arg( formatFileSize( ctrlSize ) ).arg( numControllers ) );
	addProperty( "Hitboxes", QString( "%1 (%2)" ).arg( formatFileSize( hitboxSize ) ).arg( numHitboxes ) );
	addProperty( "Sequences", QString( "%1 (%2)" ).arg( formatFileSize( seqSize ) ).arg( numSeqs ) );
	addProperty( "Tex Headers", QString( "%1 (%2)" ).arg( formatFileSize( texInfoSize ) ).arg( numTextures ) );
	addProperty( "Attachments", QString( "%1 (%2)" ).arg( formatFileSize( attachSize ) ).arg( numAttachments ) );
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

QString ModelInfoPanel::generateModelReport() const {
	if ( !m_viewport ) return QString();

	QString filePath = m_viewport->property( "modelPath" ).toString();
	QFileInfo fileInfo( filePath );

	QString report;
	report += "================================================================================\n";
	report += "                        HALF-LIFE MODEL REPORT\n";
	report += "================================================================================\n\n";

	// File Info
	report += "FILE INFORMATION\n";
	report += "----------------\n";
	report += QString( "File Name:       %1\n" ).arg( fileInfo.fileName() );
	report += QString( "Full Path:       %1\n" ).arg( filePath );
	report += QString( "Internal Name:   %1\n" ).arg( m_viewport->getHeaderName() );
	report += QString( "Version:         MDL v%1 (Half-Life 1)\n" ).arg( m_viewport->getMDLVersion() );
	report += QString( "File Size:       %1\n" ).arg( formatFileSize( m_viewport->getFileLength() ) );
	report += QString( "Flags:           0x%1\n" ).arg( m_viewport->getModelFlags(), 4, 16, QChar( '0' ) );
	report += "\n";

	// Geometry
	report += "GEOMETRY\n";
	report += "--------\n";
	report += QString( "Vertices:        %1\n" ).arg( m_viewport->getVertexCount() );
	report += QString( "Triangles:       %1\n" ).arg( m_viewport->getTriangleCount() );
	report += QString( "Bodyparts:       %1\n" ).arg( m_viewport->getBodypartCount() );
	report += "\n";

	// Bodypart details
	int bodypartCount = m_viewport->getBodypartCount();
	if ( bodypartCount > 0 ) {
		report += "BODYPARTS\n";
		report += "---------\n";
		for ( int i = 0; i < bodypartCount; i++ ) {
			QString bpName = m_viewport->getBodypartName( i );
			int submodelCount = m_viewport->getSubmodelCount( i );
			report += QString( "  [%1] %2 (%3 submodels)\n" ).arg( i ).arg( bpName ).arg( submodelCount );
			for ( int j = 0; j < submodelCount; j++ ) {
				QString subName = m_viewport->getSubmodelName( i, j );
				int vertCount = m_viewport->getSubmodelVertexCount( i, j );
				report += QString( "       - %1 (%2 verts)\n" ).arg( subName ).arg( vertCount );
			}
		}
		report += "\n";
	}

	// Skeleton
	report += "SKELETON\n";
	report += "--------\n";
	report += QString( "Bones:           %1\n" ).arg( m_viewport->getBoneCount() );
	report += QString( "Controllers:     %1\n" ).arg( m_viewport->getControllerCount() );
	report += QString( "Hitboxes:        %1\n" ).arg( m_viewport->getHitboxCount() );
	report += "\n";

	// Bone hierarchy
	int boneCount = m_viewport->getBoneCount();
	if ( boneCount > 0 ) {
		report += "BONE HIERARCHY\n";
		report += "--------------\n";
		for ( int i = 0; i < boneCount; i++ ) {
			QString boneName = m_viewport->getBoneName( i );
			int parent = m_viewport->getBoneParent( i );
			if ( parent == -1 ) {
				report += QString( "  [%1] %2 (ROOT)\n" ).arg( i, 2 ).arg( boneName );
			} else {
				report += QString( "  [%1] %2 (parent: %3)\n" ).arg( i, 2 ).arg( boneName ).arg( parent );
			}
		}
		report += "\n";
	}

	// Animations
	report += "ANIMATIONS\n";
	report += "----------\n";
	report += QString( "Sequences:       %1\n" ).arg( m_viewport->getSequenceCount() );
	report += QString( "Sequence Groups: %1\n" ).arg( m_viewport->getSeqGroupCount() );
	report += QString( "Transitions:     %1\n" ).arg( m_viewport->getTransitionCount() );
	report += "\n";

	// Sequence details
	int seqCount = m_viewport->getSequenceCount();
	if ( seqCount > 0 ) {
		report += "SEQUENCES\n";
		report += "---------\n";
		for ( int i = 0; i < seqCount; i++ ) {
			QString seqName = m_viewport->getSequenceName( i );
			int frames = m_viewport->getSequenceFrameCount( i );
			float fps = m_viewport->getSequenceFPS( i );
			QString activity = m_viewport->getSequenceActivityName( i );
			report += QString( "  [%1] %2\n" ).arg( i, 2 ).arg( seqName );
			report += QString( "       Frames: %1, FPS: %2, Activity: %3\n" ).arg( frames ).arg( fps, 0, 'f', 1 ).arg( activity );
		}
		report += "\n";
	}

	// Textures summary
	report += "TEXTURES\n";
	report += "--------\n";
	report += QString( "Texture Count:   %1\n" ).arg( m_viewport->getTextureCount() );
	report += QString( "Skin References: %1\n" ).arg( m_viewport->getSkinRefCount() );
	report += QString( "Skin Families:   %1\n" ).arg( m_viewport->getNumSkinFamilies() );
	report += "\n";

	// Attachments
	int attachCount = m_viewport->getAttachmentCount();
	if ( attachCount > 0 ) {
		report += "ATTACHMENTS\n";
		report += "-----------\n";
		for ( int i = 0; i < attachCount; i++ ) {
			QString attachName = m_viewport->getAttachmentName( i );
			int bone = m_viewport->getAttachmentBone( i );
			float x, y, z;
			m_viewport->getAttachmentPosition( i, x, y, z );
			report += QString( "  [%1] %2\n" ).arg( i ).arg( attachName );
			report += QString( "       Bone: %1, Position: %2\n" ).arg( bone ).arg( formatVector( x, y, z ) );
		}
		report += "\n";
	}

	// Bounds
	report += "BOUNDING VOLUMES\n";
	report += "----------------\n";
	float bbmin[3], bbmax[3];
	m_viewport->getBoundingBox( bbmin, bbmax );
	report += QString( "BBox Min:        %1\n" ).arg( formatVector( bbmin[0], bbmin[1], bbmin[2] ) );
	report += QString( "BBox Max:        %1\n" ).arg( formatVector( bbmax[0], bbmax[1], bbmax[2] ) );

	float hmin[3], hmax[3];
	m_viewport->getMovementBox( hmin, hmax );
	report += QString( "Hull Min:        %1\n" ).arg( formatVector( hmin[0], hmin[1], hmin[2] ) );
	report += QString( "Hull Max:        %1\n" ).arg( formatVector( hmax[0], hmax[1], hmax[2] ) );

	float eyeX, eyeY, eyeZ;
	m_viewport->getEyePosition( eyeX, eyeY, eyeZ );
	report += QString( "Eye Position:    %1\n" ).arg( formatVector( eyeX, eyeY, eyeZ ) );
	report += "\n";

	report += "================================================================================\n";
	report += "                    Generated by Lambda MDL Editor\n";
	report += "================================================================================\n";

	return report;
}

QString ModelInfoPanel::generateTextureReport() const {
	if ( !m_viewport ) return QString();

	QString filePath = m_viewport->property( "modelPath" ).toString();
	QFileInfo fileInfo( filePath );

	QString report;
	report += "================================================================================\n";
	report += "                      HALF-LIFE TEXTURE REPORT\n";
	report += "================================================================================\n\n";

	report += QString( "Model: %1\n\n" ).arg( fileInfo.fileName() );

	int texCount = m_viewport->getTextureCount();
	report += QString( "TEXTURES (%1 total)\n" ).arg( texCount );
	report += "-------------------\n\n";

	for ( int i = 0; i < texCount; i++ ) {
		QString texName = m_viewport->getTextureName( i );
		int w, h;
		m_viewport->getTextureDimensions( i, w, h );
		int flags = m_viewport->getTextureFlags( i );

		report += QString( "[%1] %2\n" ).arg( i, 2 ).arg( texName );
		report += QString( "    Dimensions:  %1 x %2\n" ).arg( w ).arg( h );
		report += QString( "    Pixels:      %1\n" ).arg( w * h );
		report += QString( "    Flags:       0x%1\n" ).arg( flags, 4, 16, QChar( '0' ) );

		// Decode common texture flags
		QStringList flagNames;
		if ( flags & 0x0001 ) flagNames << "FLATSHADE";
		if ( flags & 0x0002 ) flagNames << "CHROME";
		if ( flags & 0x0004 ) flagNames << "FULLBRIGHT";
		if ( flags & 0x0008 ) flagNames << "MIPMAPS";
		if ( flags & 0x0010 ) flagNames << "ALPHA";
		if ( flags & 0x0020 ) flagNames << "ADDITIVE";
		if ( flags & 0x0040 ) flagNames << "MASKED";
		if ( !flagNames.isEmpty() ) {
			report += QString( "    Flag Names:  %1\n" ).arg( flagNames.join( ", " ) );
		}
		report += "\n";
	}

	// Skin families
	int skinCount = m_viewport->getNumSkinFamilies();
	int skinRefs = m_viewport->getSkinRefCount();
	report += QString( "SKIN FAMILIES (%1 families, %2 references)\n" ).arg( skinCount ).arg( skinRefs );
	report += "--------------------------------------------\n\n";

	if ( skinCount > 1 ) {
		report += QString( "This model has %1 alternate skin families.\n" ).arg( skinCount - 1 );
		report += "Use the skin selector to cycle through them.\n\n";
	} else {
		report += "This model has only a single skin (no alternates).\n\n";
	}

	report += "================================================================================\n";
	report += "                    Generated by Lambda MDL Editor\n";
	report += "================================================================================\n";

	return report;
}

void ModelInfoPanel::showDumpDialog( const QString &title, const QString &content ) {
	// Create dialog
	QDialog *dialog = new QDialog( this );
	dialog->setWindowTitle( title );
	dialog->setMinimumSize( 600, 500 );
	dialog->setAttribute( Qt::WA_DeleteOnClose );

	// 90s Windows style
	dialog->setStyleSheet(
	    "QDialog {"
	    "    background-color: #c0c0c0;"
	    "}" );

	QVBoxLayout *layout = new QVBoxLayout( dialog );
	layout->setContentsMargins( 8, 8, 8, 8 );
	layout->setSpacing( 8 );

	// Text edit with monospace font
	QTextEdit *textEdit = new QTextEdit( dialog );
	textEdit->setReadOnly( true );
	textEdit->setPlainText( content );
	textEdit->setStyleSheet(
	    "QTextEdit {"
	    "    background-color: #ffffff;"
	    "    color: #000000;"
	    "    border: 2px inset;"
	    "    border-top-color: #808080;"
	    "    border-left-color: #808080;"
	    "    border-right-color: #ffffff;"
	    "    border-bottom-color: #ffffff;"
	    "    font-family: 'Courier New', Consolas, monospace;"
	    "    font-size: 10pt;"
	    "}" );
	layout->addWidget( textEdit );

	// Button row
	QString buttonStyle =
	    "QPushButton {"
	    "    background-color: #c0c0c0;"
	    "    border: 2px outset;"
	    "    border-top-color: #ffffff;"
	    "    border-left-color: #ffffff;"
	    "    border-right-color: #808080;"
	    "    border-bottom-color: #808080;"
	    "    padding: 6px 16px;"
	    "    color: #000000;"
	    "    font-weight: bold;"
	    "    min-width: 80px;"
	    "}"
	    "QPushButton:pressed {"
	    "    border: 2px inset;"
	    "    border-top-color: #808080;"
	    "    border-left-color: #808080;"
	    "    border-right-color: #ffffff;"
	    "    border-bottom-color: #ffffff;"
	    "}"
	    "QPushButton:hover {"
	    "    background-color: #d4d4d4;"
	    "}";

	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->addStretch();

	QPushButton *copyBtn = new QPushButton( "Copy to Clipboard", dialog );
	copyBtn->setStyleSheet( buttonStyle );
	connect( copyBtn, &QPushButton::clicked, [textEdit]() {
		QApplication::clipboard()->setText( textEdit->toPlainText() );
	} );
	buttonLayout->addWidget( copyBtn );

	QPushButton *saveBtn = new QPushButton( "Save to File...", dialog );
	saveBtn->setStyleSheet( buttonStyle );
	connect( saveBtn, &QPushButton::clicked, [this, textEdit]() {
		QString defaultName = m_viewport ? m_viewport->getHeaderName() + "_report.txt" : "report.txt";
		QString fileName = QFileDialog::getSaveFileName(
		    const_cast<ModelInfoPanel *>( this ),
		    "Save Report",
		    defaultName,
		    "Text Files (*.txt);;All Files (*)" );
		if ( !fileName.isEmpty() ) {
			QFile file( fileName );
			if ( file.open( QIODevice::WriteOnly | QIODevice::Text ) ) {
				QTextStream out( &file );
				out << textEdit->toPlainText();
				file.close();
				QMessageBox::information( const_cast<ModelInfoPanel *>( this ), "Saved", "Report saved successfully." );
			} else {
				QMessageBox::warning( const_cast<ModelInfoPanel *>( this ), "Error", "Could not save file." );
			}
		}
	} );
	buttonLayout->addWidget( saveBtn );

	QPushButton *closeBtn = new QPushButton( "Close", dialog );
	closeBtn->setStyleSheet( buttonStyle );
	connect( closeBtn, &QPushButton::clicked, dialog, &QDialog::accept );
	buttonLayout->addWidget( closeBtn );

	layout->addLayout( buttonLayout );

	dialog->show();
}

void ModelInfoPanel::onDumpModelInfo() {
	if ( !hasModel() ) return;
	QString report = generateModelReport();
	showDumpDialog( "Model Information Report", report );
}

void ModelInfoPanel::onDumpTextureInfo() {
	if ( !hasModel() ) return;
	QString report = generateTextureReport();
	showDumpDialog( "Texture Information Report", report );
}
