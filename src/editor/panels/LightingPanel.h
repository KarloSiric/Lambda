/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   LightingPanel.h  -  Model lighting control panel
 *
 *   Provides lighting controls including:
 *   - Enable/disable lighting
 *   - Ambient light intensity
 *   - Light direction controls
 *   - Light color selection
 *   - Chrome effect toggle
 *
 * ===========================================================================
 */

#ifndef LIGHTING_PANEL_H
#define LIGHTING_PANEL_H

#include "InspectorPanel.h"
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QCheckBox>
#include <QColor>

class LightingPanel : public InspectorPanel {
	Q_OBJECT

  public:
	explicit LightingPanel( QWidget *parent = nullptr );

	void refresh() override;

  signals:
	// Emitted when lighting toggle changes (for toolbar sync)
	void viewportStateChanged();

  private slots:
	void onLightingToggled( bool enabled );
	void onAmbientChanged( int value );
	void onLightDirXChanged( int value );
	void onLightDirYChanged( int value );
	void onLightDirZChanged( int value );
	void onLightColorClicked();
	void onChromeToggled( bool enabled );
	void onResetLightingClicked();

  private:
	void setupUI();
	void updateLightColorButton();

	// Enable lighting
	QCheckBox *m_lightingEnabled;

	// Ambient controls
	QSlider *m_ambientSlider;
	QLabel *m_ambientLabel;

	// Light direction controls
	QSlider *m_lightDirXSlider;
	QSlider *m_lightDirYSlider;
	QSlider *m_lightDirZSlider;
	QLabel *m_lightDirXLabel;
	QLabel *m_lightDirYLabel;
	QLabel *m_lightDirZLabel;

	// Light color
	QPushButton *m_lightColorBtn;
	QColor m_lightColor;

	// Chrome effect
	QCheckBox *m_chromeEnabled;

	// Reset button
	QPushButton *m_resetBtn;
};

#endif // LIGHTING_PANEL_H
