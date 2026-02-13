/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   ModelDisplayPanel.h  -  Render toggle controls panel
 *
 *   Display options for the 3D viewport including:
 *   - Wireframe mode toggle
 *   - Grid, ground, axes visibility
 *   - Bones and hitboxes overlay
 *
 * ===========================================================================
 */

#ifndef MODEL_DISPLAY_PANEL_H
#define MODEL_DISPLAY_PANEL_H

#include "InspectorPanel.h"
#include <QCheckBox>
#include <QGroupBox>

class ModelDisplayPanel : public InspectorPanel {
	Q_OBJECT

  public:
	explicit ModelDisplayPanel( QWidget *parent = nullptr );

	void refresh() override;

  private slots:
	void onWireframeToggled( bool checked );
	void onShowGridToggled( bool checked );
	void onShowGroundToggled( bool checked );
	void onShowAxesToggled( bool checked );
	void onShowBonesToggled( bool checked );
	void onShowHitboxesToggled( bool checked );

  private:
	void setupUI();
	void syncWithViewport();

	// Render toggle checkboxes
	QCheckBox *m_wireframeCheck;
	QCheckBox *m_showGridCheck;
	QCheckBox *m_showGroundCheck;
	QCheckBox *m_showAxesCheck;
	QCheckBox *m_showBonesCheck;
	QCheckBox *m_showHitboxesCheck;
};

#endif // MODEL_DISPLAY_PANEL_H
