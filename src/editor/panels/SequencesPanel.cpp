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
    , m_prevFrameButton( nullptr )
    , m_nextFrameButton( nullptr )
    , m_loopButton( nullptr )
    , m_frameSlider( nullptr )
    , m_frameLabel( nullptr )
    , m_speedSlider( nullptr )
    , m_speedLabel( nullptr )
    , m_sequenceTable( nullptr )
    , m_infoLabel( nullptr )
    , m_updatingSlider( false )
    , m_currentTotalFrames( 0 )
    , m_lastFrameUpdate( 0 )
    , m_frameRepeatTimer( nullptr )
    , m_frameStepDirection( 0 ) {
	setupUI();

	// Setup frame repeat timer for press-and-hold
	m_frameRepeatTimer = new QTimer( this );
	m_frameRepeatTimer->setInterval( 50 );  // Repeat every 50ms while held
	connect( m_frameRepeatTimer, &QTimer::timeout, this, &SequencesPanel::onFrameRepeatTimer );
}

void SequencesPanel::setupUI() {
	QVBoxLayout *contentLayout = new QVBoxLayout( m_contentWidget );
	contentLayout->setContentsMargins( 4, 4, 4, 4 );
	contentLayout->setSpacing( 8 );

	// Transport controls row with icon buttons
	QHBoxLayout *transportLayout = new QHBoxLayout();
	transportLayout->setSpacing( 2 );

	// Classic 90s Windows button style for transport controls
	QString transportButtonStyle =
	    "QPushButton {"
	    "    background-color: #c0c0c0;"
	    "    border: 2px outset;"
	    "    border-top-color: #ffffff;"
	    "    border-left-color: #ffffff;"
	    "    border-right-color: #808080;"
	    "    border-bottom-color: #808080;"
	    "    padding: 2px;"
	    "    min-width: 28px;"
	    "    min-height: 24px;"
	    "}"
	    "QPushButton:pressed {"
	    "    border: 2px inset;"
	    "    border-top-color: #808080;"
	    "    border-left-color: #808080;"
	    "    border-right-color: #ffffff;"
	    "    border-bottom-color: #ffffff;"
	    "}"
	    "QPushButton:checked {"
	    "    background-color: #a0a0a0;"
	    "    border: 2px inset;"
	    "    border-top-color: #808080;"
	    "    border-left-color: #808080;"
	    "    border-right-color: #ffffff;"
	    "    border-bottom-color: #ffffff;"
	    "}";

	// Previous frame button (supports press-and-hold)
	m_prevFrameButton = new QPushButton( m_contentWidget );
	m_prevFrameButton->setIcon( QIcon( ":/icons/previous-frame-icon.png" ) );
	m_prevFrameButton->setIconSize( QSize( 18, 18 ) );
	m_prevFrameButton->setFixedSize( 28, 26 );
	m_prevFrameButton->setStyleSheet( transportButtonStyle );
	m_prevFrameButton->setToolTip( "Previous frame (hold to repeat)" );
	m_prevFrameButton->setAutoRepeat( false );  // We handle repeat manually
	connect( m_prevFrameButton, &QPushButton::pressed, this, &SequencesPanel::onPrevFramePressed );
	connect( m_prevFrameButton, &QPushButton::released, this, &SequencesPanel::onFrameButtonReleased );

	// Play/Pause button
	m_playButton = new QPushButton( m_contentWidget );
	m_playButton->setIcon( QIcon( ":/icons/play-icon.png" ) );
	m_playButton->setIconSize( QSize( 18, 18 ) );
	m_playButton->setFixedSize( 28, 26 );
	m_playButton->setStyleSheet( transportButtonStyle );
	m_playButton->setToolTip( "Play/Pause animation (Space)" );
	connect( m_playButton, &QPushButton::clicked, this, &SequencesPanel::onPlayClicked );

	// Stop button
	m_stopButton = new QPushButton( m_contentWidget );
	m_stopButton->setIcon( QIcon( ":/icons/stop-icon.png" ) );
	m_stopButton->setIconSize( QSize( 18, 18 ) );
	m_stopButton->setFixedSize( 28, 26 );
	m_stopButton->setStyleSheet( transportButtonStyle );
	m_stopButton->setToolTip( "Stop and reset to first frame" );
	connect( m_stopButton, &QPushButton::clicked, this, &SequencesPanel::onStopClicked );

	// Next frame button (supports press-and-hold)
	m_nextFrameButton = new QPushButton( m_contentWidget );
	m_nextFrameButton->setIcon( QIcon( ":/icons/next-frame-icon.png" ) );
	m_nextFrameButton->setIconSize( QSize( 18, 18 ) );
	m_nextFrameButton->setFixedSize( 28, 26 );
	m_nextFrameButton->setStyleSheet( transportButtonStyle );
	m_nextFrameButton->setToolTip( "Next frame (hold to repeat)" );
	m_nextFrameButton->setAutoRepeat( false );  // We handle repeat manually
	connect( m_nextFrameButton, &QPushButton::pressed, this, &SequencesPanel::onNextFramePressed );
	connect( m_nextFrameButton, &QPushButton::released, this, &SequencesPanel::onFrameButtonReleased );

	// Loop toggle button (checkable)
	m_loopButton = new QPushButton( m_contentWidget );
	m_loopButton->setIcon( QIcon( ":/icons/loop-icon.png" ) );
	m_loopButton->setIconSize( QSize( 18, 18 ) );
	m_loopButton->setFixedSize( 28, 26 );
	m_loopButton->setCheckable( true );
	m_loopButton->setChecked( true );
	m_loopButton->setStyleSheet( transportButtonStyle );
	m_loopButton->setToolTip( "Loop animation (currently ON)" );
	connect( m_loopButton, &QPushButton::clicked, this, &SequencesPanel::onLoopToggled );

	transportLayout->addWidget( m_prevFrameButton );
	transportLayout->addWidget( m_playButton );
	transportLayout->addWidget( m_stopButton );
	transportLayout->addWidget( m_nextFrameButton );
	transportLayout->addSpacing( 8 );
	transportLayout->addWidget( m_loopButton );
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

	// Update loop button
	bool isLooping = m_viewport->isAnimationLooping();
	m_loopButton->setChecked( isLooping );
	m_loopButton->setToolTip( isLooping ? "Loop animation (currently ON)" : "Loop animation (currently OFF)" );

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
		// Toggle play/pause
		bool isPlaying = m_viewport->isAnimationPlaying();
		m_viewport->playAnimation( !isPlaying );
	}
}

void SequencesPanel::onStopClicked() {
	if ( m_viewport ) {
		m_viewport->playAnimation( false );
		m_viewport->setAnimationFrame( 0 );  // Reset to first frame
	}
}

void SequencesPanel::onPrevFrameClicked() {
	if ( !m_viewport ) return;

	// Go back one frame (don't stop animation - let user control that)
	float currentFrame = m_viewport->getCurrentFrame();
	float newFrame = currentFrame - 1.0f;
	if ( newFrame < 0 ) {
		newFrame = static_cast<float>( m_currentTotalFrames - 1 );
	}
	m_viewport->setAnimationFrame( newFrame );
}

void SequencesPanel::onNextFrameClicked() {
	if ( !m_viewport ) return;

	// Go forward one frame (don't stop animation - let user control that)
	float currentFrame = m_viewport->getCurrentFrame();
	float newFrame = currentFrame + 1.0f;
	if ( newFrame >= m_currentTotalFrames ) {
		newFrame = 0;
	}
	m_viewport->setAnimationFrame( newFrame );
}

void SequencesPanel::onPrevFramePressed() {
	// First frame step + start repeat timer
	onPrevFrameClicked();
	m_frameStepDirection = -1;
	m_frameRepeatTimer->start();
}

void SequencesPanel::onNextFramePressed() {
	// First frame step + start repeat timer
	onNextFrameClicked();
	m_frameStepDirection = 1;
	m_frameRepeatTimer->start();
}

void SequencesPanel::onFrameButtonReleased() {
	m_frameRepeatTimer->stop();
	m_frameStepDirection = 0;
}

void SequencesPanel::onFrameRepeatTimer() {
	if ( m_frameStepDirection < 0 ) {
		onPrevFrameClicked();
	} else if ( m_frameStepDirection > 0 ) {
		onNextFrameClicked();
	}
}

void SequencesPanel::onLoopToggled() {
	if ( m_viewport && m_loopButton ) {
		bool isLooping = m_loopButton->isChecked();
		m_viewport->setAnimationLooping( isLooping );
		m_loopButton->setToolTip( isLooping ? "Loop animation (currently ON)" : "Loop animation (currently OFF)" );
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
	// Update play button icon based on state
	if ( playing ) {
		m_playButton->setIcon( QIcon( ":/icons/pause-icon.png" ) );
		m_playButton->setToolTip( "Pause animation (Space)" );
	} else {
		m_playButton->setIcon( QIcon( ":/icons/play-icon.png" ) );
		m_playButton->setToolTip( "Play animation (Space)" );
	}
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
	int eventCount = m_viewport->getSequenceEventCount( sequenceIndex );

	QString info = QString( "<b>%1</b>: %2 frames at %3 FPS" )
	                   .arg( name )
	                   .arg( frames )
	                   .arg( static_cast<int>( fps ) );

	if ( !activity.isEmpty() ) {
		info += QString( "<br>Activity: %1" ).arg( activity );
	}

	// Show events if any
	if ( eventCount > 0 ) {
		info += QString( "<br><br><b>Events (%1):</b>" ).arg( eventCount );
		for ( int i = 0; i < eventCount && i < 8; i++ ) {  // Limit to 8 for display
			int frame = m_viewport->getSequenceEventFrame( sequenceIndex, i );
			int code = m_viewport->getSequenceEventCode( sequenceIndex, i );
			QString options = m_viewport->getSequenceEventOptions( sequenceIndex, i );

			// Format event code description
			QString codeDesc;
			if ( code >= 5000 && code <= 5099 ) {
				codeDesc = "Sound";  // 5000-5099 are typically sound events
			} else if ( code >= 1000 && code <= 1999 ) {
				codeDesc = "Script";
			} else {
				codeDesc = QString::number( code );
			}

			info += QString( "<br>  Frame %1: %2" ).arg( frame ).arg( codeDesc );
			if ( !options.isEmpty() ) {
				info += QString( " (%1)" ).arg( options );
			}
		}
		if ( eventCount > 8 ) {
			info += QString( "<br>  ... and %1 more" ).arg( eventCount - 8 );
		}
	}

	m_infoLabel->setText( info );
}
