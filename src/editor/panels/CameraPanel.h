/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   CameraPanel.h  -  Camera control panel
 *
 *   Provides camera controls including:
 *   - FOV (Field of View) slider
 *   - Camera distance control
 *   - Camera position display
 *   - Reset and Center buttons
 *
 * ===========================================================================
 */

#ifndef CAMERA_PANEL_H
#define CAMERA_PANEL_H

#include "InspectorPanel.h"
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QDoubleSpinBox>

class CameraPanel : public InspectorPanel {
	Q_OBJECT

  public:
	explicit CameraPanel( QWidget *parent = nullptr );

	void refresh() override;

  private slots:
	void onFovChanged( int value );
	void onDistanceChanged( double value );
	void onResetCameraClicked();
	void onCenterModelClicked();

  private:
	void setupUI();
	void updatePositionDisplay();

	// FOV control
	QSlider *m_fovSlider;
	QLabel *m_fovLabel;

	// Distance control
	QDoubleSpinBox *m_distanceSpinBox;

	// Position display
	QLabel *m_positionLabel;

	// Buttons
	QPushButton *m_resetCameraBtn;
	QPushButton *m_centerModelBtn;
};

#endif // CAMERA_PANEL_H
