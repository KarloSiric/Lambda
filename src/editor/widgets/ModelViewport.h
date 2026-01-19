/*
 * ═══════════════════════════════════════════════════════════════════════════
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ═══════════════════════════════════════════════════════════════════════════
 *
 *   Copyright (c) 1996-2002, Valve LLC. All rights reserved.
 *
 *   This product contains software technology licensed from Id
 *   Software, Inc. ("Id Technology"). Id Technology (c) 1996 Id Software, Inc.
 *   All Rights Reserved.
 *
 *   Use, distribution, and modification of this source code and/or resulting
 *   object code is restricted to non-commercial enhancements to products from
 *   Valve LLC. All other use, distribution, or modification is prohibited
 *   without written permission from Valve LLC.
 *
 * ───────────────────────────────────────────────────────────────────────────
 *   Author: Karlo Siric
 *   Purpose: Qt OpenGL viewport widget for rendering Half-Life models
 * ═══════════════════════════════════════════════════════════════════════════
 */

#ifndef MODELVIEWEPORT_H
#define MODELVIEWEPORT_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QTimer>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QtCore/qobject.h>
#include <QtCore/qtmetamacros.h>
#include <QtCore/qtypes.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qtabwidget.h>
#include <QEnterEvent>

// Forward declarations
class StatusBarWidget;

// @Note: C backend includes - everything is coevered in these includes
extern "C" {

#include "mdl/mdl_loader.h"
#include "mdl/mdl_animations.h"
#include "r/r_draw.h"
#include "r/r_grid.h"
#include "r/r_camera.h"
#include "math/math_types.h"
#include "math/math_matrix.h"
#include "math/math_vector.h"
#include <cglm/cglm.h>
}
}

class ModelViewport : public QOpenGLWidget, protected QOpenGLFunctions {
	Q_OBJECT
  public:
	explicit ModelViewport( QWidget *parent = nullptr );
	~ModelViewport() override;

	// @Note: Model managment - using C backend
	bool loadModel( const QString &modelPath );
	void closeModel();
	bool hasModelLoaded();

	// @Note: Model animations managment - uses C backend API
	void setSequence( int sequenceIndex );
	void playAnimation( bool play );
	void setAnimationFrame( float frame );
	int getCurrentSequence() const;
	float getCurrentFrame() const;
	bool isAnimationPlaying() const;
    
    // @Note: For fps getter and setters
    float getCurrentFps() const;
    
    // OpenGL info storage (for delayed printing)
    static QString s_gpuRenderer;
    static QString s_gpuVendor;
    static QString s_glVersion;
    static QString s_glslVersion;
    static bool s_glInfoReady;
    

	// @Note: Rendering toggles - uses C backend r_draw and r_grid API
	void setWireframeMode( bool enabled );
	void setShowGrid( bool show );
	void setShowGround( bool show );
	void setShowAxes( bool show );
	void setShowBones( bool show );
	void setShowHitboxes( bool show );

	// @Note: Camera control - uses C backend r_camera API
	void resetCamera();
	void setCameraDistance( float distance );
	float getCameraDistance() const;
	void getCameraPosition( float &x, float &y, float &z ) const;

	// @Note: Model info getters for status bar
	int getVertexCount() const;
	int getTriangleCount() const;
	int getBoneCount() const;
	int getSequenceCount() const;
	int getTextureCount() const;
	int getBodypartCount() const;

	// @Note: Status bar integration
	void setStatusBar( StatusBarWidget *statusBar );

	// @Note: Skin family control - uses C backend r_draw API
	void setSkinFamily( int family );
	void nextSkinFamily();
	void prevSkinFamily();
	int getCurrentSkinFamily() const;
	int getNumSkinFamilies() const;

  signals:

	// @Note: What happens when the model is loaded and everything
	void modelLoaded( const QString &path );
	void modelClosed();

	void sequenceChanged( int sequenceIndex );
	void frameChanged( float frame );
	void animationPlayStateChanged( bool playing );
  protected:
	void initializeGL() override;
	void paintGL() override;
	void resizeGL( int width, int height ) override;

	// @Note: Need to override all of the Qt mouse events - C backend camera
	void mousePressEvent( QMouseEvent *event ) override;
	void mouseMoveEvent( QMouseEvent *event ) override;
	void mouseReleaseEvent( QMouseEvent *event ) override;
	void wheelEvent( QWheelEvent *event ) override;
	void enterEvent( QEnterEvent *event ) override;
	void leaveEvent( QEvent *event ) override;

  private slots:

	void onAnimationTick();
  private:
	// @Note: model is owned by Qt but C backend manages it
	mdl_model_t *m_model;

	// @Note: Per-viewport rendering instance (Qt only - CLI uses globals)
	r_qt_instance_t *m_renderInstance;

	// @Note: Animatons same thing, C backend manages them all
	mdl_animation_state_t m_animState;
	bool m_animationPlaying;
	QTimer *m_animationTimer;

	// @Note: Camera state
	r_camera_t m_camera;

	// @Note: Adding camera control variables ( spherical coordinates like in Lambda CLI version )
	float m_cameraPitch;
	float m_cameraYaw;
	float m_cameraDistance;
	math_vec3_t m_cameraTarget;
    
    // Adding fps counter variables
    int m_frameCount;
    qint64 m_lastFpsTime;
    float m_currentFps;

	// View/projection matrices for rendering
	math_mat4_t m_viewMatrix;
	math_mat4_t m_projMatrix;

	// Rendering flags
	bool m_showGrid;
	bool m_showGround;
	bool m_showAxes;
	bool m_wireframeMode;
	bool m_showBones;
	bool m_showHitboxes;

	// Skin family selection
	int m_currentSkinFamily;

	// Ground positioning (automatically set to model's bbmin.z)
	float m_groundHeight;

	// Mouse input tracking
	QPoint m_lastMousePos;
	Qt::MouseButton m_activeButton;

	// Status bar pointer for updating viewport info
	StatusBarWidget *m_statusBar;

	// Helper functions
	void updateAnimationState( float deltaTime );
	void setupProjectionMatrix( int width, int height );
	void renderScene();
	void frameModel(); // Auto-frame camera to fit loaded model
};

#endif
