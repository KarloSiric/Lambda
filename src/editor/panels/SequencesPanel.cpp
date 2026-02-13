/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   SequencesPanel.cpp  -  Animation sequence controller implementation
 *
 * ===========================================================================
 */

#include "SequencesPanel.h"
#include "../widgets/ModelViewport.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QFrame>
#include <QDateTime>

SequencesPanel::SequencesPanel( QWidget *parent )
    : InspectorPanel( parent )
    , m_playButton( nullptr )
    , m_stopButton( nullptr )
    , m_loopCheckbox( nullptr )
    , m_frameSlider( nullptr )
    , m_frameLabel( nullptr )
    , m_speedSlider( nullptr )
    , m_speedLabel( nullptr )
    , m_sequenceTable( nullptr )
    , m_infoLabel( nullptr )
    , m_updatingSlider( false )
    , m_currentTotalFrames( 0 )
    , m_lastFrameUpdate( 0 ) {
	setupUI();
}

void SequencesPanel::setupUI() {
	QVBoxLayout *contentLayout = new QVBoxLayout( m_contentWidget );
	contentLayout->setContentsMargins( 4, 4, 4, 4 );
	contentLayout->setSpacing( 8 );

	// Transport controls row
	QHBoxLayout *transportLayout = new QHBoxLayout();
	transportLayout->setSpacing( 4 );

	m_playButton = new QPushButton( "Play", m_contentWidget );
	m_playButton->setFixedWidth( 60 );
	m_playButton->setToolTip( "Play/pause the current animation sequence (Space)" );
	connect( m_playButton, &QPushButton::clicked, this, &SequencesPanel::onPlayClicked );

	m_stopButton = new QPushButton( "Stop", m_contentWidget );
	m_stopButton->setFixedWidth( 60 );
	m_stopButton->setToolTip( "Stop animation and reset to first frame" );
	connect( m_stopButton, &QPushButton::clicked, this, &SequencesPanel::onStopClicked );

	m_loopCheckbox = new QCheckBox( "Loop", m_contentWidget );
	m_loopCheckbox->setChecked( true );
	m_loopCheckbox->setToolTip( "Loop animation when it reaches the end" );
	connect( m_loopCheckbox, &QCheckBox::toggled, this, &SequencesPanel::onLoopToggled );

	transportLayout->addWidget( m_playButton );
	transportLayout->addWidget( m_stopButton );
	transportLayout->addWidget( m_loopCheckbox );
	transportLayout->addStretch();
	contentLayout->addLayout( transportLayout );

	// Frame slider row
	QHBoxLayout *frameLayout = new QHBoxLayout();
	frameLayout->setSpacing( 8 );

	QLabel *frameLabelText = new QLabel( "Frame:", m_contentWidget );
	frameLabelText->setStyleSheet( "color: #000000;" );

	m_frameSlider = new QSlider( Qt::Horizontal, m_contentWidget );
	m_frameSlider->setMinimum( 0 );
	m_frameSlider->setMaximum( 100 );
	m_frameSlider->setValue( 0 );
	m_frameSlider->setToolTip( "Scrub through animation frames" );
	connect( m_frameSlider, &QSlider::valueChanged, this, &SequencesPanel::onFrameSliderChanged );

	m_frameLabel = new QLabel( "0 / 0", m_contentWidget );
	m_frameLabel->setMinimumWidth( 60 );
	m_frameLabel->setStyleSheet( "color: #000000;" );

	frameLayout->addWidget( frameLabelText );
	frameLayout->addWidget( m_frameSlider, 1 );
	frameLayout->addWidget( m_frameLabel );
	contentLayout->addLayout( frameLayout );

	// Speed slider row
	QHBoxLayout *speedLayout = new QHBoxLayout();
	speedLayout->setSpacing( 8 );

	QLabel *speedLabelText = new QLabel( "Speed:", m_contentWidget );
	speedLabelText->setStyleSheet( "color: #000000;" );

	m_speedSlider = new QSlider( Qt::Horizontal, m_contentWidget );
	m_speedSlider->setMinimum( 10 );  // 0.1x
	m_speedSlider->setMaximum( 300 ); // 3.0x
	m_speedSlider->setValue( 100 );   // 1.0x
	m_speedSlider->setToolTip( "Adjust playback speed (0.1x to 3.0x)" );
	connect( m_speedSlider, &QSlider::valueChanged, this, &SequencesPanel::onSpeedSliderChanged );

	m_speedLabel = new QLabel( "1.0x", m_contentWidget );
	m_speedLabel->setMinimumWidth( 40 );
	m_speedLabel->setStyleSheet( "color: #000000;" );

	speedLayout->addWidget( speedLabelText );
	speedLayout->addWidget( m_speedSlider, 1 );
	speedLayout->addWidget( m_speedLabel );
	contentLayout->addLayout( speedLayout );

	// Separator line
	QFrame *separator = new QFrame( m_contentWidget );
	separator->setFrameShape( QFrame::HLine );
	separator->setStyleSheet( "background-color: #808080;" );
	contentLayout->addWidget( separator );

	// Sequence list table
	m_sequenceTable = new QTableWidget( m_contentWidget );
	m_sequenceTable->setColumnCount( 4 );
	m_sequenceTable->setHorizontalHeaderLabels( { "#", "Name", "FPS", "Frames" } );
	m_sequenceTable->horizontalHeader()->setVisible( true );
	m_sequenceTable->verticalHeader()->setVisible( false );
	m_sequenceTable->setAlternatingRowColors( true );
	m_sequenceTable->setEditTriggers( QAbstractItemView::NoEditTriggers );
	m_sequenceTable->setSelectionBehavior( QAbstractItemView::SelectRows );
	m_sequenceTable->setSelectionMode( QAbstractItemView::SingleSelection );
	m_sequenceTable->setToolTip( "Click to select an animation sequence" );

	// Column sizing
	m_sequenceTable->horizontalHeader()->setSectionResizeMode( 0, QHeaderView::Fixed );
	m_sequenceTable->setColumnWidth( 0, 30 );
	m_sequenceTable->horizontalHeader()->setSectionResizeMode( 1, QHeaderView::Stretch );
	m_sequenceTable->horizontalHeader()->setSectionResizeMode( 2, QHeaderView::Fixed );
	m_sequenceTable->setColumnWidth( 2, 45 );
	m_sequenceTable->horizontalHeader()->setSectionResizeMode( 3, QHeaderView::Fixed );
	m_sequenceTable->setColumnWidth( 3, 55 );
	m_sequenceTable->verticalHeader()->setDefaultSectionSize( 20 );

	m_sequenceTable->setStyleSheet(
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

	connect( m_sequenceTable, &QTableWidget::cellClicked, this, &SequencesPanel::onSequenceRowClicked );
	contentLayout->addWidget( m_sequenceTable, 1 );

	// Info box
	m_infoLabel = new QLabel( m_contentWidget );
	m_infoLabel->setWordWrap( true );
	m_infoLabel->setMinimumHeight( 40 );
	m_infoLabel->setToolTip( "Details about the selected animation sequence" );
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

	// Style buttons - 90s Windows 95 raised/sunken 3D look
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
	m_playButton->setStyleSheet( buttonStyle );
	m_stopButton->setStyleSheet( buttonStyle );

	// Style checkbox - 90s style
	m_loopCheckbox->setStyleSheet( "QCheckBox { color: #000000; }" );
}

void SequencesPanel::setViewport( ModelViewport *viewport ) {
	// Disconnect old viewport signals
	disconnectViewportSignals();

	// Call base class
	InspectorPanel::setViewport( viewport );

	// Connect new viewport signals if visible
	if ( isVisible() ) {
		connectViewportSignals();
	}

	// Initial state
	if ( m_viewport ) {
		updateTransportButtons();
	}
}

void SequencesPanel::connectViewportSignals() {
	if ( !m_viewport ) return;

	connect( m_viewport, &ModelViewport::frameChanged,
	         this, &SequencesPanel::onFrameChanged, Qt::UniqueConnection );
	connect( m_viewport, &ModelViewport::sequenceChanged,
	         this, &SequencesPanel::onSequenceChanged, Qt::UniqueConnection );
	connect( m_viewport, &ModelViewport::animationPlayStateChanged,
	         this, &SequencesPanel::onAnimationPlayStateChanged, Qt::UniqueConnection );
}

void SequencesPanel::disconnectViewportSignals() {
	if ( !m_viewport ) return;

	disconnect( m_viewport, &ModelViewport::frameChanged,
	            this, &SequencesPanel::onFrameChanged );
	disconnect( m_viewport, &ModelViewport::sequenceChanged,
	            this, &SequencesPanel::onSequenceChanged );
	disconnect( m_viewport, &ModelViewport::animationPlayStateChanged,
	            this, &SequencesPanel::onAnimationPlayStateChanged );
}

void SequencesPanel::showEvent( QShowEvent *event ) {
	InspectorPanel::showEvent( event );

	// Connect signals when panel becomes visible
	connectViewportSignals();
	refresh();  // Update UI with current state
}

void SequencesPanel::hideEvent( QHideEvent *event ) {
	// Disconnect signals when panel is hidden to prevent unnecessary updates
	disconnectViewportSignals();

	InspectorPanel::hideEvent( event );
}

void SequencesPanel::refresh() {
	if ( !hasModel() ) {
		m_sequenceTable->setRowCount( 0 );
		m_infoLabel->clear();
		return;
	}

	populateSequenceList();

	// Select current sequence
	int currentSeq = m_viewport->getCurrentSequence();
	if ( currentSeq >= 0 && currentSeq < m_sequenceTable->rowCount() ) {
		m_sequenceTable->selectRow( currentSeq );
		updateInfoBox( currentSeq );
	}

	// Update frame slider range
	m_currentTotalFrames = m_viewport->getSequenceFrameCount( currentSeq );
	m_frameSlider->setMaximum( m_currentTotalFrames > 0 ? m_currentTotalFrames - 1 : 0 );

	// Update speed slider from viewport
	float speed = m_viewport->getPlaybackSpeed();
	m_speedSlider->setValue( static_cast<int>( speed * 100 ) );
	updateSpeedLabel();

	// Update loop checkbox
	m_loopCheckbox->setChecked( m_viewport->isAnimationLooping() );

	updateTransportButtons();
	updateFrameLabel();
}

void SequencesPanel::onFrameChanged( float frame ) {
	if ( m_updatingSlider ) return;

	// Throttle UI updates to max 30 FPS to prevent performance issues
	qint64 now = QDateTime::currentMSecsSinceEpoch();
	if ( now - m_lastFrameUpdate < 33 ) return;  // ~30 FPS
	m_lastFrameUpdate = now;

	m_updatingSlider = true;
	m_frameSlider->setValue( static_cast<int>( frame ) );
	updateFrameLabel();
	m_updatingSlider = false;
}

void SequencesPanel::onSequenceChanged( int index ) {
	if ( index >= 0 && index < m_sequenceTable->rowCount() ) {
		m_sequenceTable->selectRow( index );
		updateInfoBox( index );

		// Update frame slider range for new sequence
		m_currentTotalFrames = m_viewport->getSequenceFrameCount( index );
		m_frameSlider->setMaximum( m_currentTotalFrames > 0 ? m_currentTotalFrames - 1 : 0 );
		updateFrameLabel();
	}
}

void SequencesPanel::onAnimationPlayStateChanged( bool playing ) {
	updateTransportButtons();
}

void SequencesPanel::onPlayClicked() {
	if ( m_viewport ) {
		m_viewport->playAnimation( true );
	}
}

void SequencesPanel::onStopClicked() {
	if ( m_viewport ) {
		m_viewport->playAnimation( false );
	}
}

void SequencesPanel::onLoopToggled( bool checked ) {
	if ( m_viewport ) {
		m_viewport->setAnimationLooping( checked );
	}
}

void SequencesPanel::onFrameSliderChanged( int value ) {
	if ( m_updatingSlider ) return;
	if ( m_viewport ) {
		m_viewport->setAnimationFrame( static_cast<float>( value ) );
	}
	updateFrameLabel();
}

void SequencesPanel::onSpeedSliderChanged( int value ) {
	float speed = value / 100.0f;
	if ( m_viewport ) {
		m_viewport->setPlaybackSpeed( speed );
	}
	updateSpeedLabel();
}

void SequencesPanel::onSequenceRowClicked( int row, int column ) {
	Q_UNUSED( column );
	if ( m_viewport && row >= 0 ) {
		m_viewport->setSequence( row );
	}
}

void SequencesPanel::populateSequenceList() {
	m_sequenceTable->setRowCount( 0 );

	if ( !hasModel() ) return;

	int count = m_viewport->getSequenceCount();
	for ( int i = 0; i < count; ++i ) {
		int row = m_sequenceTable->rowCount();
		m_sequenceTable->insertRow( row );

		// Index
		QTableWidgetItem *indexItem = new QTableWidgetItem( QString::number( i ) );
		indexItem->setTextAlignment( Qt::AlignCenter );
		m_sequenceTable->setItem( row, 0, indexItem );

		// Name
		QString name = m_viewport->getSequenceName( i );
		QTableWidgetItem *nameItem = new QTableWidgetItem( name );
		m_sequenceTable->setItem( row, 1, nameItem );

		// FPS
		float fps = m_viewport->getSequenceFPS( i );
		QTableWidgetItem *fpsItem = new QTableWidgetItem( QString::number( static_cast<int>( fps ) ) );
		fpsItem->setTextAlignment( Qt::AlignCenter );
		m_sequenceTable->setItem( row, 2, fpsItem );

		// Frames
		int frames = m_viewport->getSequenceFrameCount( i );
		QTableWidgetItem *framesItem = new QTableWidgetItem( QString::number( frames ) );
		framesItem->setTextAlignment( Qt::AlignCenter );
		m_sequenceTable->setItem( row, 3, framesItem );
	}
}

void SequencesPanel::updateFrameLabel() {
	int current = m_frameSlider->value();
	m_frameLabel->setText( QString( "%1 / %2" ).arg( current ).arg( m_currentTotalFrames ) );
}

void SequencesPanel::updateSpeedLabel() {
	float speed = m_speedSlider->value() / 100.0f;
	m_speedLabel->setText( QString( "%1x" ).arg( speed, 0, 'f', 1 ) );
}

void SequencesPanel::updateTransportButtons() {
	if ( !m_viewport ) return;

	bool playing = m_viewport->isAnimationPlaying();
	m_playButton->setText( playing ? "Pause" : "Play" );
	m_stopButton->setEnabled( true );
}

void SequencesPanel::updateInfoBox( int sequenceIndex ) {
	if ( !hasModel() || sequenceIndex < 0 ) {
		m_infoLabel->clear();
		return;
	}

	QString name = m_viewport->getSequenceName( sequenceIndex );
	int frames = m_viewport->getSequenceFrameCount( sequenceIndex );
	float fps = m_viewport->getSequenceFPS( sequenceIndex );
	QString activity = m_viewport->getSequenceActivityName( sequenceIndex );

	QString info = QString( "<b>%1</b>: %2 frames at %3 FPS" )
	                   .arg( name )
	                   .arg( frames )
	                   .arg( static_cast<int>( fps ) );

	if ( !activity.isEmpty() ) {
		info += QString( "<br>Activity: %1" ).arg( activity );
	}

	m_infoLabel->setText( info );
}
