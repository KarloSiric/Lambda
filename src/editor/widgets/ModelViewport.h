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
#include <QKeyEvent>
#include <QFocusEvent>
#include <QSet>
#include <QElapsedTimer>
#include <QImage>
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

#include "mdl_loader.h"
#include "mdl_animations.h"
#include "r_draw.h"
#include "r_grid.h"
#include "r_gizmo.h"
#include "r_compass.h"
#include "r_camera.h"
#include "math_types.h"
#include "math_matrix.h"
#include "math_vector.h"
#include <cglm/cglm.h>
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
	void setShowAttachments( bool show );

	// Getters for rendering toggles
	bool isWireframeMode() const { return m_wireframeMode; }
	bool isShowGrid() const { return m_showGrid; }
	bool isShowGround() const { return m_showGround; }
	bool isShowAxes() const { return m_showAxes; }
	bool isShowBones() const { return m_showBones; }
	bool isShowHitboxes() const { return m_showHitboxes; }
	bool isShowAttachments() const { return m_showAttachments; }

	// @Note: Camera control - uses C backend r_camera API
	void resetCamera();
	void setCameraDistance( float distance );
	float getCameraDistance() const;
	void getCameraPosition( float &x, float &y, float &z ) const;
	void frameModel(); // Auto-frame camera to fit loaded model

	// @Note: Model world-space translation (separate from camera pan)
	void resetModelPosition();
	void getModelOffset( float &x, float &y, float &z ) const;

	// @Note: Model rotation (Ctrl+right-drag, or gizmo rings)
	void resetModelRotation();
	void setModelRotationY( float radians );
	float getModelRotationY() const;

	// @Note: Rotation gizmo toggle (R key or external call)
	void setShowGizmo( bool show );
	bool isGizmoVisible() const;

	// @Note: Model info getters for status bar
	int getVertexCount() const;
	int getTriangleCount() const;
	int getBoneCount() const;
	int getSequenceCount() const;
	int getTextureCount() const;
	int getBodypartCount() const;
	QString getSequenceName( int index ) const;

	// @Note: Extended model info getters for Inspector panels
	int getMDLVersion() const;
	int getModelFlags() const;
	int getControllerCount() const;
	int getSeqGroupCount() const;
	int getAttachmentCount() const;
	QString getAttachmentName( int index ) const;
	int getAttachmentBone( int index ) const;
	int getAttachmentType( int index ) const;
	void getAttachmentPosition( int index, float &x, float &y, float &z ) const;
	int getHitboxCount() const;
	void getEyePosition( float &x, float &y, float &z ) const;
	void getBoundingBox( float bbmin[3], float bbmax[3] ) const;

	// @Note: Animation control for Sequences panel
	void setPlaybackSpeed( float multiplier );
	float getPlaybackSpeed() const;
	void setAnimationLooping( bool loop );
	bool isAnimationLooping() const;
	int getSequenceFrameCount( int seqIndex ) const;
	float getSequenceFPS( int seqIndex ) const;
	QString getSequenceActivityName( int seqIndex ) const;

	// @Note: Texture info for Textures panel
	QImage getTextureImage( int index ) const;
	QString getTextureName( int index ) const;
	void getTextureDimensions( int index, int &w, int &h ) const;
	int getTextureFlags( int index ) const;

	// @Note: Bodypart info for Bodyparts panel
	QString getBodypartName( int bpIndex ) const;
	int getSubmodelCount( int bpIndex ) const;
	QString getSubmodelName( int bpIndex, int subIndex ) const;
	int getSubmodelVertexCount( int bpIndex, int subIndex ) const;
	void setBodypart( int bpIndex, int subIndex );
	int getActiveSubmodel( int bpIndex ) const;

	// @Note: Bone info for Bones panel
	QString getBoneName( int index ) const;
	int getBoneParent( int index ) const;

	// @Note: Status bar integration
	void setStatusBar( StatusBarWidget *statusBar );

	// @Note: Raycasting - convert screen coordinates to 3D world position
	bool raycastToGround( int screenX, int screenY, float &worldX, float &worldY, float &worldZ );

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

	// @Note: Keyboard events for WASD camera movement
	void keyPressEvent( QKeyEvent *event ) override;
	void keyReleaseEvent( QKeyEvent *event ) override;
	void focusOutEvent( QFocusEvent *event ) override;

  private slots:

	void onAnimationTick();
	void onCameraMovementUpdate();
  private:
	// @Note: model is owned by Qt but C backend manages it
	mdl_model_t *m_model;

	// @Note: Per-viewport rendering instance (Qt only - CLI uses globals)
	r_qt_instance_t *m_renderInstance;

	// @Note: Animatons same thing, C backend manages them all
	mdl_animation_state_t m_animState;
	bool m_animationPlaying;
	bool m_animationLooping;
	float m_playbackSpeed;
	QTimer *m_animationTimer;

	// @Note: Camera state
	r_camera_t m_camera;

	// @Note: Adding camera control variables ( spherical coordinates like in Lambda CLI version )
	float m_cameraPitch;
	float m_cameraYaw;
	float m_cameraDistance;
	math_vec3_t m_cameraTarget;

	// @Note: Model world-space offset - right-click drag translates this
	math_vec3_t m_modelOffset;

	// @Note: Model rotation (all three axes)
	// Y = spin left/right (most common), X = tilt forward/back, Z = tilt sideways
	float m_modelRotationX;
	float m_modelRotationY;
	float m_modelRotationZ;

	// @Note: Rotation gizmo state
	bool  m_showGizmo;
	int   m_gizmoHoveredAxis;   // -1=none  0=X  1=Y  2=Z
	int   m_gizmoActiveAxis;    // -1=none  0=X  1=Y  2=Z
	float m_gizmoCenter[3];     // world-space pivot, updated each frame
	float m_gizmoRadius;        // ring radius, updated each frame
	float m_gizmoPrevHit[3];    // previous ray-plane hit for delta-angle drag
    
    // Adding fps counter variables
    int m_frameCount;
    qint64 m_lastFpsTime;
    float m_currentFps;

    // Animation timing for frame-rate independent playback
    qint64 m_lastAnimTime;

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
	bool m_showAttachments;

	// Skin family selection
	int m_currentSkinFamily;

	// Ground positioning (automatically set to model's bbmin.z)
	float m_groundHeight;

	// Mouse input tracking
	QPoint m_lastMousePos;
	Qt::MouseButton m_activeButton;

	// Status bar pointer for updating viewport info
	StatusBarWidget *m_statusBar;

	// WASD camera movement
	QTimer *m_movementTimer;
	QSet<int> m_pressedKeys;  // Track which keys are currently pressed
	qint64 m_lastMovementTime;  // For delta time calculation

	// Helper functions
	void updateAnimationState( float deltaTime );
	void setupProjectionMatrix( int width, int height );
	void renderScene();
	void updateCameraMovement( float deltaTime );

	// Overlay rendering helpers
	void renderBoneSkeleton();
	void renderHitboxes();
	void renderAttachmentPoints();
	void drawWireframeBox( const float boneMatrix[3][4], const float bbmin[3], const float bbmax[3] );

	// Gizmo helpers
	void  buildRayFromMouse( int mx, int my, float origin[3], float dir[3] ) const;
	int   gizmoHitTest( int mx, int my ) const;
	bool  rayPlaneIntersect( const float origin[3], const float dir[3],
	                         const float planeNorm[3], const float planePoint[3],
	                         float hit[3] ) const;
};
#endif
