/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   BoneControllersPanel.h  -  Bone controller sliders (mouth, eyes, etc.)
 *
 *   Displays sliders for each bone controller defined in the model. These
 *   controllers typically control things like mouth movement, eye tracking,
 *   and other procedural bone adjustments.
 *
 * ===========================================================================
 */

#ifndef BONE_CONTROLLERS_PANEL_H
#define BONE_CONTROLLERS_PANEL_H

#include "InspectorPanel.h"
#include <QSlider>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QVector>

class BoneControllersPanel : public InspectorPanel {
	Q_OBJECT

  public:
	explicit BoneControllersPanel( QWidget *parent = nullptr );

	void refresh() override;

  private slots:
	void onControllerValueChanged( int value );
	void onSpinBoxValueChanged( double value );
	void onResetAll();

  private:
	void setupUI();
	void clearControllers();
	void createControllerWidgets();
	QString getControllerTypeName( int type ) const;
	QString getControllerIndexName( int index ) const;

	struct ControllerWidget {
		int controllerIndex;  // The actual controller slot (0-5)
		QSlider *slider;
		QDoubleSpinBox *spinBox;
		QLabel *nameLabel;
		float minValue;
		float maxValue;
	};

	QVector<ControllerWidget> m_controllers;
	QWidget *m_controllersContainer;
	QVBoxLayout *m_controllersLayout;
	QLabel *m_noControllersLabel;
	QPushButton *m_resetAllBtn;
};

#endif // BONE_CONTROLLERS_PANEL_H
