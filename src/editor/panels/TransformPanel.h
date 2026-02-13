/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   TransformPanel.h  -  Model transform control panel
 *
 *   Provides transform controls including:
 *   - Model position offset display
 *   - Model rotation controls
 *   - Reset buttons
 *
 * ===========================================================================
 */

#ifndef TRANSFORM_PANEL_H
#define TRANSFORM_PANEL_H

#include "InspectorPanel.h"
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QDoubleSpinBox>

class TransformPanel : public InspectorPanel {
	Q_OBJECT

  public:
	explicit TransformPanel( QWidget *parent = nullptr );

	void refresh() override;

  private slots:
	void onRotationYChanged( int value );
	void onResetPositionClicked();
	void onResetRotationClicked();
	void onResetAllClicked();

  private:
	void setupUI();
	void updatePositionDisplay();

	// Position display
	QLabel *m_posXLabel;
	QLabel *m_posYLabel;
	QLabel *m_posZLabel;

	// Rotation control
	QSlider *m_rotationYSlider;
	QLabel *m_rotationYLabel;

	// Buttons
	QPushButton *m_resetPositionBtn;
	QPushButton *m_resetRotationBtn;
	QPushButton *m_resetAllBtn;
};

#endif // TRANSFORM_PANEL_H
