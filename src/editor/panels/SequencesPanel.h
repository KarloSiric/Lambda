/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   SequencesPanel.h  -  Animation sequence controller
 *
 *   Full animation control panel featuring:
 *   - Transport controls (Play, Stop, Loop toggle)
 *   - Frame slider with current/total display
 *   - Playback speed slider
 *   - Sequence list table with FPS and frame count
 *   - Info box showing selected sequence details
 *
 * ===========================================================================
 */

#ifndef SEQUENCES_PANEL_H
#define SEQUENCES_PANEL_H

#include "InspectorPanel.h"
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QTableWidget>
#include <QCheckBox>
#include <QShowEvent>
#include <QHideEvent>
#include <QTimer>

class SequencesPanel : public InspectorPanel {
	Q_OBJECT

  public:
	explicit SequencesPanel( QWidget *parent = nullptr );

	void setViewport( ModelViewport *viewport ) override;
	void refresh() override;

  public slots:
	void onFrameChanged( float frame );
	void onSequenceChanged( int index );
	void onAnimationPlayStateChanged( bool playing );

  protected:
	void showEvent( QShowEvent *event ) override;
	void hideEvent( QHideEvent *event ) override;

  private slots:
	void onPlayClicked();
	void onStopClicked();
	void onPrevFrameClicked();
	void onNextFrameClicked();
	void onPrevFramePressed();
	void onNextFramePressed();
	void onFrameButtonReleased();
	void onFrameRepeatTimer();
	void onLoopToggled();
	void onFrameSliderChanged( int value );
	void onSpeedSliderChanged( int value );
	void onSequenceRowClicked( int row, int column );

  private:
	void connectViewportSignals();
	void disconnectViewportSignals();
	void setupUI();
	void populateSequenceList();
	void updateFrameLabel();
	void updateSpeedLabel();
	void updateTransportButtons();
	void updateInfoBox( int sequenceIndex );

	// Transport controls
	QPushButton *m_playButton;
	QPushButton *m_stopButton;
	QPushButton *m_prevFrameButton;
	QPushButton *m_nextFrameButton;
	QPushButton *m_loopButton;

	// Frame control
	QSlider *m_frameSlider;
	QLabel *m_frameLabel;

	// Speed control
	QSlider *m_speedSlider;
	QLabel *m_speedLabel;

	// Sequence list
	QTableWidget *m_sequenceTable;

	// Info box
	QLabel *m_infoLabel;

	// State tracking
	bool m_updatingSlider;
	int m_currentTotalFrames;
	qint64 m_lastFrameUpdate;  // Throttle frame updates to 30 FPS

	// Press-and-hold frame stepping
	QTimer *m_frameRepeatTimer;
	int m_frameStepDirection;  // -1 for prev, +1 for next
};

#endif // SEQUENCES_PANEL_H
