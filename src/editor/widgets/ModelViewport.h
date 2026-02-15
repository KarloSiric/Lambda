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
#include <QColor>

// Forward declarations
class StatusBarWidget;

// @Note: C backend includes - everything is coevered in these includes
extern "C" {

#include "mdl_loader.h"
#include "mdl_animations.h"
#include "mdl_bonecontrollers.h"
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
	void setWireframeOverlay( bool enabled );
	void setShowGrid( bool show );
	void setShowGround( bool show );
	void setShowAxes( bool show );
	void setShowBones( bool show );
	void setShowHitboxes( bool show );
	void setShowAttachments( bool show );
	void setFlatShading( bool enabled );

	// Getters for rendering toggles
	bool isWireframeMode() const { return m_wireframeMode; }
	bool isWireframeOverlay() const { return m_wireframeOverlay; }
	bool isShowGrid() const { return m_showGrid; }
	bool isShowGround() const { return m_showGround; }
	bool isShowAxes() const { return m_showAxes; }
	bool isShowBones() const { return m_showBones; }
	bool isShowHitboxes() const { return m_showHitboxes; }
	bool isShowAttachments() const { return m_showAttachments; }
	bool isFlatShading() const { return m_flatShading; }

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
	void setModelRotationX( float radians );
	void setModelRotationY( float radians );
	void setModelRotationZ( float radians );
	float getModelRotationX() const;
	float getModelRotationY() const;
	float getModelRotationZ() const;

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
	int getControllerBone( int index ) const;           // Which bone this controller affects
	int getControllerType( int index ) const;           // Type of motion (X, Y, Z rot/pos, mouth)
	float getControllerStart( int index ) const;        // Min value
	float getControllerEnd( int index ) const;          // Max value
	int getControllerRest( int index ) const;           // Rest position
	int getControllerIndex( int index ) const;          // Controller slot (0-5)
	void setControllerValue( int controller, float value );
	float getControllerValue( int controller ) const;
	int getSeqGroupCount() const;
	int getAttachmentCount() const;
	QString getAttachmentName( int index ) const;
	int getAttachmentBone( int index ) const;
	int getAttachmentType( int index ) const;
	void getAttachmentPosition( int index, float &x, float &y, float &z ) const;
	int getHitboxCount() const;
	void getEyePosition( float &x, float &y, float &z ) const;
	void getBoundingBox( float bbmin[3], float bbmax[3] ) const;

	// @Note: Extended header info for Model Info panel
	int getHeaderId() const;                    // MDL magic (IDST)
	QString getHeaderName() const;              // Internal model name from header
	int getTransitionCount() const;             // Sequence transitions
	int getSoundGroupCount() const;             // Sound groups
	int getSkinRefCount() const;                // Skin references
	int getFileLength() const;                  // Total file length from header
	void getMovementBox( float min[3], float max[3] ) const; // Movement bounds (min/max)

	// @Note: Animation control for Sequences panel
	void setPlaybackSpeed( float multiplier );
	float getPlaybackSpeed() const;
	void setAnimationLooping( bool loop );
	bool isAnimationLooping() const;
	int getSequenceFrameCount( int seqIndex ) const;
	float getSequenceFPS( int seqIndex ) const;
	QString getSequenceActivityName( int seqIndex ) const;
	int getSequenceEventCount( int seqIndex ) const;
	int getSequenceEventFrame( int seqIndex, int eventIndex ) const;
	int getSequenceEventCode( int seqIndex, int eventIndex ) const;
	QString getSequenceEventOptions( int seqIndex, int eventIndex ) const;

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

	// @Note: Lighting controls for Lighting panel
	void setLightingEnabled( bool enabled );
	bool isLightingEnabled() const;
	void setAmbientIntensity( float intensity );
	float getAmbientIntensity() const;
	void setLightDirection( float x, float y, float z );
	void getLightDirection( float &x, float &y, float &z ) const;
	void setLightColor( const QColor &color );
	QColor getLightColor() const;
	void setChromeEnabled( bool enabled );
	bool isChromeEnabled() const;
	void resetLighting();

	// @Note: Scale controls for ModelDisplay panel
	void setMeshScale( float scale );
	float getMeshScale() const;
	void setBoneScale( float scale );
	float getBoneScale() const;

	// @Note: Mirror controls for ModelDisplay panel
	void setMirrorX( bool enabled );
	void setMirrorY( bool enabled );
	void setMirrorZ( bool enabled );
	bool isMirrorX() const;
	bool isMirrorY() const;
	bool isMirrorZ() const;

	// @Note: FOV control for camera
	void setFov( float degrees );
	float getFov() const;

	// @Note: Weapon origin view mode (first-person weapon position)
	void setWeaponOriginMode( bool enabled );
	bool isWeaponOriginMode() const;

  signals:

	// @Note: What happens when the model is loaded and everything
	void modelLoaded( const QString &path );
	void modelClosed();

	void sequenceChanged( int sequenceIndex );
	void frameChanged( float frame );
	void animationPlayStateChanged( bool playing );
	void cameraDistanceChanged( float distance );
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
	mdl_bonecontrollers_state_t m_controllerState;  // Bone controller values (mouth, eyes, etc.)
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
    
    // FPS counter and frame limiting (HLMV-style)
    int m_frameCount;
    qint64 m_lastFpsTime;
    float m_currentFps;
    float m_maxFps;            // Target max FPS (default 60)
    qint64 m_lastRenderTime;   // Last time we actually rendered

    // Animation timing for frame-rate independent playback
    qint64 m_lastAnimTime;
    qint64 m_lastFrameSignalTime;  // Throttle frameChanged signal to reduce UI overhead

	// View/projection matrices for rendering
	math_mat4_t m_viewMatrix;
	math_mat4_t m_projMatrix;

	// Rendering flags
	bool m_showGrid;
	bool m_showGround;
	bool m_showAxes;
	bool m_wireframeMode;
	bool m_wireframeOverlay;
	bool m_showBones;
	bool m_showHitboxes;
	bool m_showAttachments;
	bool m_flatShading;

	// Skin family selection
	int m_currentSkinFamily;

	// Ground positioning (automatically set to model's bbmin.z)
	float m_groundHeight;

	// Lighting settings
	bool m_lightingEnabled;
	float m_ambientIntensity;
	float m_lightDirX;
	float m_lightDirY;
	float m_lightDirZ;
	QColor m_lightColor;
	bool m_chromeEnabled;

	// Scale settings
	float m_meshScale;
	float m_boneScale;

	// Mirror settings
	bool m_mirrorX;
	bool m_mirrorY;
	bool m_mirrorZ;

	// FOV setting
	float m_fov;
	float m_savedFov;  // Saved FOV when entering weapon origin mode
	float m_savedCameraPitch;
	float m_savedCameraYaw;
	float m_savedCameraDistance;
	float m_savedCameraTarget[3];
	float m_savedModelOffset[3];

	// Weapon origin mode
	bool m_weaponOriginMode;

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

	// Overlay rendering resources (for bones, hitboxes, attachments)
	void initOverlayRendering();
	void cleanupOverlayRendering();
	GLuint m_overlayShader;
	GLuint m_boneLineVAO, m_boneLineVBO;
	GLuint m_bonePointVAO, m_bonePointVBO;
	GLuint m_hitboxVAO, m_hitboxVBO;
	GLuint m_attachmentPointVAO, m_attachmentPointVBO;
	GLuint m_attachmentLineVAO, m_attachmentLineVBO;
	GLint m_overlayModelLoc, m_overlayViewLoc, m_overlayProjLoc, m_overlayPointSizeLoc;
	bool m_overlayInitialized;
};
#endif
