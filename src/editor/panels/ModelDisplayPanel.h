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
#include <QSlider>
#include <QDoubleSpinBox>
#include <QPushButton>

class ModelDisplayPanel : public InspectorPanel {
	Q_OBJECT

  public:
	explicit ModelDisplayPanel( QWidget *parent = nullptr );

	void refresh() override;

  signals:
	// Emitted when any viewport display state changes (for toolbar sync)
	void viewportStateChanged();

  private slots:
	// Render toggles
	void onWireframeToggled( bool checked );
	void onShowGridToggled( bool checked );
	void onShowGroundToggled( bool checked );
	void onShowAxesToggled( bool checked );
	void onShowBonesToggled( bool checked );
	void onShowHitboxesToggled( bool checked );

	// Rotation controls
	void onRotationXChanged( int value );
	void onRotationYChanged( int value );
	void onRotationZChanged( int value );

	// Scale controls
	void onMeshScaleChanged( double value );
	void onBoneScaleChanged( double value );

	// Mirror controls
	void onMirrorXToggled( bool checked );
	void onMirrorYToggled( bool checked );
	void onMirrorZToggled( bool checked );

	// Camera controls
	void onFovChanged( int value );
	void onWeaponOriginToggled( bool checked );

	// Reset
	void onResetAll();

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

	// Rotation controls
	QSlider *m_rotationXSlider;
	QSlider *m_rotationYSlider;
	QSlider *m_rotationZSlider;

	// Scale controls
	QDoubleSpinBox *m_meshScaleSpin;
	QDoubleSpinBox *m_boneScaleSpin;

	// Mirror controls
	QPushButton *m_mirrorXBtn;
	QPushButton *m_mirrorYBtn;
	QPushButton *m_mirrorZBtn;

	// Camera controls
	QSlider *m_fovSlider;
	QPushButton *m_weaponOriginBtn;

	// Reset button
	QPushButton *m_resetAllBtn;
};

#endif // MODEL_DISPLAY_PANEL_H
