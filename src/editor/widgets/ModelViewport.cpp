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

#include "ModelViewport.h"
#include "StatusBarWidget.h"
#include "LoggerBridge.h"
#include "math_vector.h"
#include "mdl_loader.h"
#include "mdl_bodypart.h"
#include "mdl_bonecontrollers.h"
#include "r_draw.h"
#include "util_messages.h"
#include <QDebug>
#include <QMatrix4x4>
#include <QVector3D>
#include <QVector4D>
#include <QtCore/qdatetime.h>
#include <QtCore/qdebug.h>
#include <QtCore/qlogging.h>
#include <QtCore/qnamespace.h>
#include <QtCore/qobject.h>
#include <QtCore/qpoint.h>
#include <QtCore/qstringview.h>
#include <QtCore/qtypes.h>
#include <cmath>
#include <cstring>


// Static OpenGL info storage
QString ModelViewport::s_gpuRenderer;
QString ModelViewport::s_gpuVendor;
QString ModelViewport::s_glVersion;
QString ModelViewport::s_glslVersion;
bool ModelViewport::s_glInfoReady = false;


ModelViewport::ModelViewport( QWidget *parent )
	: QOpenGLWidget( parent ),
	  m_model( nullptr ),
	  m_renderInstance( nullptr ),
	  m_animationPlaying( false ),
	  m_animationLooping( true ),
	  m_playbackSpeed( 1.0f ),
	  m_animationTimer( nullptr ),
	  m_cameraPitch( 0.33f ),
	  m_cameraYaw( 0.0f ),
	  m_cameraDistance( 35.0f ),
	  m_cameraTarget{ 0.0f, 0.0f, 0.0f },
	  m_modelOffset{ 0.0f, 0.0f, 0.0f },
	  m_modelRotationX( 0.0f ),
	  m_modelRotationY( 0.0f ),
	  m_modelRotationZ( 0.0f ),
	  m_showGizmo( false ),
	  m_gizmoHoveredAxis( -1 ),
	  m_gizmoActiveAxis( -1 ),
	  m_gizmoCenter{ 0.0f, 0.0f, 0.0f },
	  m_gizmoRadius( 8.0f ),
	  m_gizmoPrevHit{ 0.0f, 0.0f, 0.0f },
	  m_frameCount( 0 ),
	  m_lastFpsTime( 0 ),
	  m_currentFps( 0.0f ),
	  m_maxFps( 60.0f ),
	  m_lastRenderTime( 0 ),
	  m_lastAnimTime( 0 ),
	  m_lastFrameSignalTime( 0 ),
	  m_showGrid( true ),
	  m_showGround( false ),
	  m_showAxes( true ),
	  m_wireframeMode( false ),
	  m_wireframeOverlay( false ),
	  m_showBones( false ),
	  m_showHitboxes( false ),
	  m_showAttachments( false ),
	  m_flatShading( false ),
	  m_currentSkinFamily( 0 ),
	  m_groundHeight( 0.0f ),
	  m_lightingEnabled( true ),
	  m_ambientIntensity( 0.2f ),
	  m_lightDirX( 0.0f ),
	  m_lightDirY( 0.0f ),
	  m_lightDirZ( -1.0f ),
	  m_lightColor( 255, 255, 255 ),
	  m_chromeEnabled( false ),
	  m_meshScale( 1.0f ),
	  m_boneScale( 1.0f ),
	  m_mirrorX( false ),
	  m_mirrorY( false ),
	  m_mirrorZ( false ),
	  m_fov( 65.0f ),
	  m_savedFov( 65.0f ),
	  m_savedCameraPitch( 0.0f ),
	  m_savedCameraYaw( 0.0f ),
	  m_savedCameraDistance( 35.0f ),
	  m_savedCameraTarget{ 0.0f, 0.0f, 0.0f },
	  m_savedModelOffset{ 0.0f, 0.0f, 0.0f },
	  m_weaponOriginMode( false ),
	  m_activeButton( Qt::NoButton ),
	  m_statusBar( nullptr ),
	  m_overlayShader( 0 ),
	  m_boneLineVAO( 0 ),
	  m_boneLineVBO( 0 ),
	  m_bonePointVAO( 0 ),
	  m_bonePointVBO( 0 ),
	  m_hitboxVAO( 0 ),
	  m_hitboxVBO( 0 ),
	  m_overlayModelLoc( -1 ),
	  m_overlayViewLoc( -1 ),
	  m_overlayProjLoc( -1 ),
	  m_overlayPointSizeLoc( -1 ),
	  m_overlayInitialized( false ) {
	mdl_animation_init( &m_animState );
	mdl_bonecontroller_init( &m_controllerState );  // Initialize bone controllers to default/rest

	// Create the new animationTimer so 60 fps ~16.66 ms ~16 ms
	m_animationTimer = new QTimer( this );
	m_animationTimer->setInterval( 16 );

	connect( m_animationTimer, &QTimer::timeout, this, &ModelViewport::onAnimationTick );

	m_animationTimer->start();

	//  Look at the origin
	math_vec3_t target = { 0.0f, 0.0f, 0.0f };

	// 100 units away the distance
	Camera_Init( &m_camera, target, 50.0f );

	// Set camera angles for isometric view
	m_camera.angles_deg[0] = 17.2f; // Pitch: look down 20 degrees
	m_camera.angles_deg[1] = 28.6f; // Yaw: rotate 45 degrees
	m_camera.angles_deg[2] = 0.0f; // Roll: no rotation

	// Enable mouse tracking for hover detection (status bar updates)
	setMouseTracking( true );

	// WASD camera movement setup
	m_movementTimer = new QTimer( this );
	m_movementTimer->setInterval( 16 );  // ~60 FPS for smooth movement
	connect( m_movementTimer, &QTimer::timeout, this, &ModelViewport::onCameraMovementUpdate );
	m_lastMovementTime = QDateTime::currentMSecsSinceEpoch();
	setFocusPolicy( Qt::StrongFocus );  // Allow keyboard focus
}

ModelViewport::~ModelViewport() {
    
    // TODO(Karlo): Some issues when application is closing
    if ( m_animationTimer && m_animationTimer->isActive() )
    {
        m_animationTimer->stop();
    }
    
    if ( m_animationTimer && m_movementTimer->isActive() )
    { 
        m_animationTimer->stop(); 
    }
    
	makeCurrent();

	if ( m_model )
    {
		free_model( m_model );
		m_model = nullptr;
	}

	if ( m_renderInstance )
    {
		r_qt_destroy_instance( m_renderInstance );
		m_renderInstance = nullptr;
	}

	r_gizmo_cleanup();
	r_compass_cleanup();
	cleanupOverlayRendering();

	doneCurrent();
}

void ModelViewport::initializeGL( void ) {
	initializeOpenGLFunctions();

	if ( !s_glInfoReady ) {
		const char *vendor = (const char *)glGetString( GL_VENDOR );
		const char *renderer = (const char *)glGetString( GL_RENDERER );
		const char *version = (const char *)glGetString( GL_VERSION );
		const char *glslVersion = (const char *)glGetString( GL_SHADING_LANGUAGE_VERSION );

		s_gpuVendor = QString::fromUtf8( vendor ? vendor : "Unknown" );
		s_gpuRenderer = QString::fromUtf8( renderer ? renderer : "Unknown" );
		s_glVersion = QString::fromUtf8( version ? version : "Unknown" );
		s_glslVersion = QString::fromUtf8( glslVersion ? glslVersion : "Unknown" );
		s_glInfoReady = true;
	}
	// Light cyan/blue background
	glClearColor( 0.45f, 0.55f, 0.60f, 1.0f );

	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LESS );

	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glFrontFace( GL_CW );

	// Create per-viewport rendering instance
	m_renderInstance = r_qt_create_instance();
	if ( !m_renderInstance ) {
		qCritical() << "ERROR: Failed to create Qt rendering instance!";
	}

	r_grid_init( 100.0f, 5.0f );
	r_ground_init( 100.0f );
	r_axes_init( 1000.0f );
	r_gizmo_init();
	r_compass_init();
	initOverlayRendering();
}

void ModelViewport::resizeGL( int width, int height ) {
	glViewport( 0, 0, width, height );

	float aspect = (float)width / (float)height;
	float nearPlane = 0.01f;
	float farPlane = 1000.0f;

	// Use m_fov member variable (adjustable via setFov())
	glm_perspective( glm_rad( m_fov ), aspect, nearPlane, farPlane, m_projMatrix );
}

void ModelViewport::paintGL() {
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	math_vec3_t camPos;
	math_vec3_t target;
	math_vec3_t up = { 0.0f, 1.0f, 0.0f }; // World up

	if ( m_weaponOriginMode ) {
		// ============================================================================
		// WEAPON ORIGIN VIEW: First-person camera (like holding the weapon in-game)
		// ============================================================================
		// In Half-Life, the v_model origin is at the player's eye position.
		// The weapon mesh extends forward and down from the origin point.
		// We position the camera slightly behind the origin looking forward,
		// so we can see where the weapon would appear in first-person view.
		//
		// This matches HLMV's weapon origin view mode used for positioning v_models.

		float eyeHeight = 6.4f;  // 64 HL units * 0.1 scale

		// Camera positioned slightly behind the weapon origin looking forward
		// This simulates the player's eye looking at where the weapon would be
		camPos[0] = 0.0f;
		camPos[1] = eyeHeight;
		camPos[2] = 15.0f;  // Behind the weapon, looking forward

		// Look at the weapon position (slightly in front and lower)
		target[0] = 0.0f;
		target[1] = eyeHeight - 2.0f;  // Slightly lower, where weapon hands would be
		target[2] = -10.0f;  // Forward
	} else {
		// ============================================================================
		// CAMERA CALCULATION: Orbit camera around target point (ARC BALL style)
		// ============================================================================
		float camDist = m_cameraDistance;
		float pitch = m_cameraPitch;
		float yaw = m_cameraYaw;

		// Calculate camera offset from target using spherical coordinates
		math_vec3_t offset;
		offset[0] = camDist * cosf( pitch ) * sinf( yaw ); // X offset
		offset[1] = camDist * sinf( pitch ); // Y offset (vertical)
		offset[2] = camDist * cosf( pitch ) * cosf( yaw ); // Z offset

		// CRITICAL: Camera position = target + offset (orbit around target!)
		camPos[0] = m_cameraTarget[0] + offset[0];
		camPos[1] = m_cameraTarget[1] + offset[1];
		camPos[2] = m_cameraTarget[2] + offset[2];

		// Optional: Clamp camera Y above ground
		if ( camPos[1] < m_groundHeight + 0.5f ) {
			camPos[1] = m_groundHeight + 0.5f;
		}

		Math_Vec3Copy( m_cameraTarget, target );
	}

	// Create view matrix - camera looks at target from offset position
	Math_Mat4_LookAt( camPos, target, up, m_viewMatrix );

	// Draw grid, axes, and ground at Y=0 (INDEPENDENTLY - each has its own toggle)
	if ( m_showGrid ) {
		r_grid_draw( m_viewMatrix, m_projMatrix, 0.0f );
	}
	if ( m_showAxes ) {
		r_axes_draw( m_viewMatrix, m_projMatrix, 0.0f );
	}
	if ( m_showGround ) {
		r_ground_draw( m_viewMatrix, m_projMatrix, 0.0f );
	}

	// Build model matrix - Add proper transformations
	mat4 modelMatrix;
	Math_Mat4_Identity( modelMatrix );

	// Scale down (HL units are huge - 0.1x scale) * user mesh scale
	float modelScale = 0.1f * m_meshScale;
	mat4 S = GLM_MAT4_IDENTITY_INIT;
	glm_scale( S, (vec3){ modelScale, modelScale, modelScale } );
	glm_mat4_mul( S, modelMatrix, modelMatrix );

	// Apply mirror transforms if enabled
	if ( m_mirrorX || m_mirrorY || m_mirrorZ ) {
		mat4 M = GLM_MAT4_IDENTITY_INIT;
		glm_scale( M, (vec3){ m_mirrorX ? -1.0f : 1.0f, m_mirrorY ? -1.0f : 1.0f, m_mirrorZ ? -1.0f : 1.0f } );
		glm_mat4_mul( M, modelMatrix, modelMatrix );
	}

	// Calculate ground offset (used for gizmo positioning even in weapon origin mode)
	float groundOffset = 0.0f;
	if ( m_model && m_model->header && m_model->data && m_model->header->numseq > 0 ) {
		mstudioseqdesc_t *sequences = (mstudioseqdesc_t *)( m_model->data + m_model->header->seqindex );
		groundOffset = -sequences[0].bbmin[2] * modelScale;
	}

	if ( m_weaponOriginMode ) {
		// ============================================================================
		// WEAPON ORIGIN MODE: Position weapon as it appears in first-person view
		// ============================================================================
		// In Half-Life, v_ models (view models) are designed to be viewed from first person
		// Camera is at eye level looking forward (-Z), weapon is positioned relative to that

		// Rotate weapon to face correct direction in first-person view
		// HL view models typically face +Y (forward), we need them to face -Z (into screen)
		mat4 RyWeapon = GLM_MAT4_IDENTITY_INIT;
		glm_rotate( RyWeapon, GLM_PI, (vec3){ 0, 1, 0 } );  // 180° to face -Z
		glm_mat4_mul( RyWeapon, modelMatrix, modelMatrix );

		// Position weapon at eye level (64 HL units = 6.4 scaled)
		// Weapons are typically offset down and forward from eye position
		mat4 TWeapon = GLM_MAT4_IDENTITY_INIT;
		glm_translate( TWeapon, (vec3){ 0.0f, 6.4f, 0.0f } );
		glm_mat4_mul( TWeapon, modelMatrix, modelMatrix );

	} else {
		// ============================================================================
		// NORMAL MODE: Model faces camera with ground alignment
		// ============================================================================

		// Rotate model to face camera (blue Z axis)
		// Half-Life models face +Y after axis remap, need -90° Y rotation to face +Z
		mat4 RyFace = GLM_MAT4_IDENTITY_INIT;
		glm_rotate( RyFace, -GLM_PI * 0.5f, (vec3){ 0, 1, 0 } );
		glm_mat4_mul( RyFace, modelMatrix, modelMatrix );

		// Ground alignment - translate model up so feet touch Y=0
		mat4 T = GLM_MAT4_IDENTITY_INIT;
		glm_translate( T, (vec3){ 0.0f, groundOffset, 0.0f } );
		glm_mat4_mul( T, modelMatrix, modelMatrix );
	}

	// Model rotation (all three axes, applied Ry → Rx → Rz)
	// Allows user to manually adjust model orientation
	if ( m_modelRotationX != 0.0f || m_modelRotationY != 0.0f || m_modelRotationZ != 0.0f ) {
		mat4 R = GLM_MAT4_IDENTITY_INIT;
		glm_rotate( R, m_modelRotationY, (vec3){ 0.0f, 1.0f, 0.0f } );
		glm_rotate( R, m_modelRotationX, (vec3){ 1.0f, 0.0f, 0.0f } );
		glm_rotate( R, m_modelRotationZ, (vec3){ 0.0f, 0.0f, 1.0f } );
		glm_mat4_mul( R, modelMatrix, modelMatrix );
	}

	// Apply world-space model offset (right-click drag translation)
	// Applied last so it works in world space regardless of model rotation/scale
	mat4 TOffset = GLM_MAT4_IDENTITY_INIT;
	glm_translate( TOffset, (vec3){ m_modelOffset[0], m_modelOffset[1], m_modelOffset[2] } );
	glm_mat4_mul( TOffset, modelMatrix, modelMatrix );

	// Render the model if loaded
	if ( m_renderInstance && m_model && m_model->header && m_model->data ) {
		// Apply wireframe mode if enabled (exclusive wireframe, no textures)
		if ( m_wireframeMode ) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}

		r_qt_render_with_matrices( m_renderInstance, m_viewMatrix, m_projMatrix, modelMatrix );

		// Reset polygon mode to fill
		if ( m_wireframeMode ) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}

		// Wireframe overlay mode: render wireframe on top of textured model
		if ( m_wireframeOverlay && !m_wireframeMode ) {
			// Enable polygon offset to avoid z-fighting
			glEnable( GL_POLYGON_OFFSET_LINE );
			glPolygonOffset( -1.0f, -1.0f );

			// Draw wireframe on top using the same shader
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			glLineWidth( 1.0f );

			// Enable wireframe overlay shader mode (renders solid color instead of texture)
			r_qt_set_wireframe_overlay( m_renderInstance, true, 0.0f, 1.0f, 0.0f );  // Green wireframe

			// Render wireframe with shader
			r_qt_render_with_matrices( m_renderInstance, m_viewMatrix, m_projMatrix, modelMatrix );

			// Disable wireframe overlay shader mode
			r_qt_set_wireframe_overlay( m_renderInstance, false, 0.0f, 0.0f, 0.0f );

			// Restore state
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
			glDisable( GL_POLYGON_OFFSET_LINE );
		}

		// Render overlay elements (bones, hitboxes, attachments)
		if ( m_showBones ) {
			renderBoneSkeleton();
		}
		if ( m_showHitboxes ) {
			renderHitboxes();
		}
		if ( m_showAttachments ) {
			renderAttachmentPoints();
		}
	}

	// ── Rotation gizmo ───────────────────────────────────────────────────
	// Compute and cache the gizmo centre/radius each frame so the hit-test
	// helpers (called from mouse events) always use up-to-date values.
	if ( m_showGizmo ) {
		// Default gizmo radius
		float gizmoRad = 8.0f;
		float gizmoCenterY = 0.0f;

		if ( m_model && m_model->header && m_model->data && m_model->header->numseq > 0 ) {
			const mstudioseqdesc_t *seqs =
				(const mstudioseqdesc_t *)( m_model->data + m_model->header->seqindex );
			const float scale = 0.1f;

			// Visual height in world units (HL Z=up → OpenGL Y)
			float modelHeight = ( seqs[0].bbmax[2] - seqs[0].bbmin[2] ) * scale;
			gizmoCenterY      = groundOffset + modelHeight * 0.5f;

			// Bounding box extent as ring radius
			float dx = ( seqs[0].bbmax[0] - seqs[0].bbmin[0] ) * scale;
			float dy = ( seqs[0].bbmax[1] - seqs[0].bbmin[1] ) * scale;
			float dz = modelHeight;
			float extent = sqrtf( dx * dx + dy * dy + dz * dz ) * 0.5f;
			gizmoRad = ( extent > 2.0f ) ? extent * 1.25f : 8.0f;
		}

		// Cache for mouse interaction (read in hit-test & drag helpers)
		m_gizmoCenter[0] = m_modelOffset[0];
		m_gizmoCenter[1] = gizmoCenterY + m_modelOffset[1];
		m_gizmoCenter[2] = m_modelOffset[2];
		m_gizmoRadius    = gizmoRad;

		vec3 center = { m_gizmoCenter[0], m_gizmoCenter[1], m_gizmoCenter[2] };
		r_gizmo_draw( m_viewMatrix, m_projMatrix, center, m_gizmoRadius,
		              m_gizmoHoveredAxis, m_gizmoActiveAxis );
	}

	// @Note( Karlo ): Adding fps calculation
	m_frameCount++;
	qint64 currentTime = QDateTime::currentMSecsSinceEpoch();

	if ( m_lastFpsTime == 0 ) {
		m_lastFpsTime = currentTime;
	}

	qint64 elapsed = currentTime - m_lastFpsTime;

	if ( elapsed >= 1000 ) {
		m_currentFps = static_cast<float>( m_frameCount ) * 1000.0f / static_cast<float>( elapsed );
		m_frameCount = 0;
		m_lastFpsTime = currentTime;
	}

	// ── Orientation compass widget (always visible, bottom-right corner) ──
	r_compass_draw( m_viewMatrix, width(), height() );
}

void ModelViewport::onAnimationTick() {
	qint64 currentTime = QDateTime::currentMSecsSinceEpoch();

	// HLMV-style frame limiting: skip if not enough time has passed
	float minFrameTime = 1000.0f / m_maxFps;  // ms per frame at max FPS
	qint64 elapsedSinceRender = currentTime - m_lastRenderTime;

	// Only advance animation if playing and model loaded
	if ( m_animationPlaying && m_model && m_model->header ) {
		// Calculate delta time
		float deltaTime = ( m_lastAnimTime > 0 )
		                    ? ( currentTime - m_lastAnimTime ) / 1000.0f
		                    : 0.016f;
		m_lastAnimTime = currentTime;

		// Clamp delta to prevent huge jumps
		if ( deltaTime > 0.05f ) {
			deltaTime = 0.05f;
		}

		// Apply playback speed multiplier
		deltaTime *= m_playbackSpeed;

		// Get the current sequence to check FPS
		if ( m_animState.current_sequence >= 0 &&
			 m_animState.current_sequence < m_model->header->numseq ) {

			// Get sequence info for looping check
			mstudioseqdesc_t* seq = (mstudioseqdesc_t*)((uint8_t*)m_model->header +
			                         m_model->header->seqindex) + m_animState.current_sequence;
			int frameCount = seq->numframes;

			// Update animation frame
			mdl_animation_update( &m_animState, deltaTime, m_model->header, m_model->data, m_model->seqgroups );

			// Handle looping - if animation reached end and looping is disabled, stop
			if ( !m_animationLooping && m_animState.current_frame >= frameCount - 1 ) {
				m_animState.current_frame = (float)( frameCount - 1 );
				m_animationPlaying = false;
				emit animationPlayStateChanged( false );
			}

			// Sync animation state to render instance for bone calculation
			if ( m_renderInstance ) {
				r_qt_set_animation_state( m_renderInstance, &m_animState );
				r_qt_set_animation_enabled( m_renderInstance, true );
			}

			// Emit frame changed signal for UI updates (throttled to 15 FPS)
			if ( currentTime - m_lastFrameSignalTime >= 66 ) {
				m_lastFrameSignalTime = currentTime;
				emit frameChanged( m_animState.current_frame );
			}
		}
	}

	// HLMV-style frame skipping: only redraw if enough time has passed AND there's a reason to redraw
	bool needsUpdate = m_animationPlaying || !m_pressedKeys.isEmpty() || ( m_activeButton != Qt::NoButton );
	if ( needsUpdate && elapsedSinceRender >= minFrameTime ) {
		m_lastRenderTime = currentTime;
		update();
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// Animation Control Methods
// ═══════════════════════════════════════════════════════════════════════════

void ModelViewport::setSequence( int sequenceIndex ) {
	if ( !m_model || !m_model->header ) return;

	// Validate sequence index
	if ( sequenceIndex < 0 || sequenceIndex >= m_model->header->numseq ) {
		log_warning( "Animation", "Invalid sequence index: %d (max: %d)",
			sequenceIndex, m_model->header->numseq - 1 );
		return;
	}

	// Set the sequence using C backend
	mdl_animation_set_sequence( &m_animState, sequenceIndex, m_model->header,
		m_model->data, m_model->seqgroups );

	// Sync animation state to render instance
	if ( m_renderInstance ) {
		r_qt_set_animation_state( m_renderInstance, &m_animState );
		r_qt_set_animation_enabled( m_renderInstance, true );
	}

	log_info( "Animation", "Sequence changed to %d", sequenceIndex );

	// Emit signal for UI updates
	emit sequenceChanged( sequenceIndex );
	emit frameChanged( m_animState.current_frame );

	// Request redraw
	update();
}

void ModelViewport::playAnimation( bool play ) {
	m_animationPlaying = play;

	if ( play ) {
		log_debug( "Animation", "Animation started" );
	} else {
		log_debug( "Animation", "Animation paused" );
	}

	emit animationPlayStateChanged( play );
}

void ModelViewport::setAnimationFrame( float frame ) {
	if ( !m_model || !m_model->header ) return;

	// Clamp frame to valid range
	if ( m_animState.current_sequence >= 0 &&
		 m_animState.current_sequence < m_model->header->numseq ) {

		mstudioseqdesc_t *sequences = (mstudioseqdesc_t *)( m_model->data + m_model->header->seqindex );
		mstudioseqdesc_t *seq = &sequences[m_animState.current_sequence];

		int numFrames = seq->numframes;
		if ( frame < 0 ) frame = 0;
		if ( frame >= numFrames ) frame = numFrames - 1;

		m_animState.current_frame = frame;

		// Sync animation state to render instance
		if ( m_renderInstance ) {
			r_qt_set_animation_state( m_renderInstance, &m_animState );
			r_qt_set_animation_enabled( m_renderInstance, true );
		}

		emit frameChanged( frame );
		update();
	}
}

int ModelViewport::getCurrentSequence() const {
	return m_animState.current_sequence;
}

float ModelViewport::getCurrentFrame() const {
	return m_animState.current_frame;
}

bool ModelViewport::isAnimationPlaying() const {
	return m_animationPlaying;
}

float ModelViewport::getCurrentFps() const {
	return m_currentFps;
}

void ModelViewport::mousePressEvent( QMouseEvent *event ) {
	// Gizmo takes priority: left-click on a ring starts rotation drag.
	if ( m_showGizmo && event->button() == Qt::LeftButton ) {
		int axis = gizmoHitTest( event->pos().x(), event->pos().y() );
		if ( axis >= 0 ) {
			m_gizmoActiveAxis = axis;

			// Record the initial ray-plane hit so we can compute angle deltas.
			static const float normals[3][3] = {
				{ 1.0f, 0.0f, 0.0f },  // X ring – YZ plane
				{ 0.0f, 1.0f, 0.0f },  // Y ring – XZ plane
				{ 0.0f, 0.0f, 1.0f }   // Z ring – XY plane
			};
			float origin[3], dir[3];
			buildRayFromMouse( event->pos().x(), event->pos().y(), origin, dir );
			rayPlaneIntersect( origin, dir, normals[axis], m_gizmoCenter, m_gizmoPrevHit );

			m_lastMousePos = event->pos();
			event->accept();
			return;  // Gizmo consumed this press
		}
	}

	// Normal camera / model-transform controls
	m_activeButton = event->button();
	m_lastMousePos = event->pos();

	event->accept();
}

void ModelViewport::mouseMoveEvent( QMouseEvent *event ) {
	QPoint currentPos = event->pos();

	// ── Gizmo: active drag ────────────────────────────────────────────────
	if ( m_gizmoActiveAxis >= 0 ) {
		static const float normals[3][3] = {
			{ 1.0f, 0.0f, 0.0f },  // X ring – YZ plane
			{ 0.0f, 1.0f, 0.0f },  // Y ring – XZ plane
			{ 0.0f, 0.0f, 1.0f }   // Z ring – XY plane
		};

		float origin[3], dir[3];
		buildRayFromMouse( currentPos.x(), currentPos.y(), origin, dir );

		float hit[3];
		if ( rayPlaneIntersect( origin, dir, normals[m_gizmoActiveAxis],
		                        m_gizmoCenter, hit ) ) {
			// Vectors from gizmo centre to previous and current hit
			float pv[3] = { m_gizmoPrevHit[0] - m_gizmoCenter[0],
			                m_gizmoPrevHit[1] - m_gizmoCenter[1],
			                m_gizmoPrevHit[2] - m_gizmoCenter[2] };
			float cv[3] = { hit[0] - m_gizmoCenter[0],
			                hit[1] - m_gizmoCenter[1],
			                hit[2] - m_gizmoCenter[2] };

			float prevAngle = 0.0f, currAngle = 0.0f;
			switch ( m_gizmoActiveAxis ) {
			case 0:  // X ring (YZ plane): atan2(z, y)
				prevAngle = atan2f( pv[2], pv[1] );
				currAngle = atan2f( cv[2], cv[1] );
				break;
			case 1:  // Y ring (XZ plane): atan2(x, z)
				prevAngle = atan2f( pv[0], pv[2] );
				currAngle = atan2f( cv[0], cv[2] );
				break;
			case 2:  // Z ring (XY plane): atan2(y, x)
				prevAngle = atan2f( pv[1], pv[0] );
				currAngle = atan2f( cv[1], cv[0] );
				break;
			}

			float delta = currAngle - prevAngle;
			// Wrap to [-π, π] to avoid sign flip at the atan2 discontinuity
			while ( delta >  (float)M_PI ) delta -= 2.0f * (float)M_PI;
			while ( delta < -(float)M_PI ) delta += 2.0f * (float)M_PI;

			switch ( m_gizmoActiveAxis ) {
			case 0: m_modelRotationX += delta; break;
			case 1: m_modelRotationY += delta; break;
			case 2: m_modelRotationZ += delta; break;
			}

			m_gizmoPrevHit[0] = hit[0];
			m_gizmoPrevHit[1] = hit[1];
			m_gizmoPrevHit[2] = hit[2];

			update();
		}

		m_lastMousePos = currentPos;
		event->accept();
		return;
	}

	// ── Gizmo: hover detection (no button held) ───────────────────────────
	if ( m_showGizmo && m_activeButton == Qt::NoButton ) {
		int hovered = gizmoHitTest( currentPos.x(), currentPos.y() );
		if ( hovered != m_gizmoHoveredAxis ) {
			m_gizmoHoveredAxis = hovered;
			update();
		}
	}

	// Get delta for camera / model controls
	QPoint delta = currentPos - m_lastMousePos;

	// HLAM-style controls: Left+Shift OR Middle button = PAN, Left alone = ORBIT
	bool isPanning = ( m_activeButton == Qt::LeftButton && ( event->modifiers() & Qt::ShiftModifier ) ) || ( m_activeButton == Qt::MiddleButton );

	if ( m_activeButton == Qt::LeftButton && !isPanning ) {
		// ORBIT MODE: Rotate camera around target (Left button without Shift)
		float sensitivity = 0.003f;

		m_cameraYaw += delta.x() * sensitivity;
		m_cameraPitch -= delta.y() * sensitivity;

		// Clamp pitch to prevent gimbal lock (~90 degrees = pi/2 radians)
		float maxPitch = 1.57f;
		if ( m_cameraPitch > maxPitch ) m_cameraPitch = maxPitch;
		if ( m_cameraPitch < -maxPitch ) m_cameraPitch = -maxPitch;
	} else if ( isPanning ) {
		// PAN MODE: Move camera target (Left+Shift OR Middle button)
		// Speed scales with distance so panning feels consistent at all zoom levels
		float panSpeed = m_cameraDistance * 0.0003f;

		// Calculate camera-relative right vector (perpendicular to view direction)
		math_vec3_t right;
		right[0] = cosf( m_cameraYaw );
		right[1] = 0.0f;
		right[2] = -sinf( m_cameraYaw );

		// World up vector (Y-axis in OpenGL)
		math_vec3_t up = { 0.0f, 1.0f, 0.0f };

		// Horizontal drag (X) = move along right vector
		m_cameraTarget[0] -= right[0] * delta.x() * panSpeed;
		m_cameraTarget[2] -= right[2] * delta.x() * panSpeed;

		// Vertical drag (Y) = move along up vector
		m_cameraTarget[1] += up[1] * delta.y() * panSpeed;

	} else if ( m_activeButton == Qt::RightButton ) {
		// MODEL TRANSFORM MODE: Right-click drag manipulates the model.
		//   No modifier  : translate in XZ plane (camera-relative horizontal only).
		//   Shift held   : translate on world Y axis only (up/down).
		//   Ctrl held    : rotate model around world Y axis (left/right drag).
		// Speed scales with camera distance for uniform visual feel at any zoom level.
		float moveSpeed = m_cameraDistance * 0.001f;

		// Camera right vector (screen-horizontal, XZ plane only)
		math_vec3_t camRight;
		camRight[0] = cosf( m_cameraYaw );
		camRight[1] = 0.0f;
		camRight[2] = -sinf( m_cameraYaw );

		if ( event->modifiers() & Qt::ControlModifier ) {
			// Ctrl + right drag: rotate model around world Y axis.
			// Drag right = clockwise rotation when viewed from above.
			const float rotSpeed = 0.008f;  // radians per pixel
			m_modelRotationY += delta.x() * rotSpeed;
		} else if ( event->modifiers() & Qt::ShiftModifier ) {
			// Shift + right drag: Y axis translation only.
			// Qt Y increases downward, negate so drag-up → model goes up.
			m_modelOffset[1] -= delta.y() * moveSpeed;
		} else {
			// No modifier: free XZ plane translation.
			// Horizontal drag → camera-right (XZ only).
			// Vertical drag   → camera-forward projected onto XZ (no Y).
			// Qt Y increases downward, so negate delta.y() so drag-up = forward.
			math_vec3_t camForward;
			camForward[0] = -sinf( m_cameraYaw );
			camForward[1] = 0.0f;
			camForward[2] = -cosf( m_cameraYaw );

			m_modelOffset[0] += ( camRight[0] * delta.x() - camForward[0] * delta.y() ) * moveSpeed;
			m_modelOffset[2] += ( camRight[2] * delta.x() - camForward[2] * delta.y() ) * moveSpeed;
		}
	}

	m_lastMousePos = currentPos;

	// Trigger repaint when camera changes
	update();
	event->accept();
}

void ModelViewport::mouseReleaseEvent( QMouseEvent *event ) {
	// If a gizmo drag was active, end it
	if ( m_gizmoActiveAxis >= 0 ) {
		m_gizmoActiveAxis = -1;
		event->accept();
		return;
	}

	m_activeButton = Qt::NoButton;
	event->accept();
}

void ModelViewport::wheelEvent( QWheelEvent *event ) {
	// @Note: Adding the wheel behavior for the camera movement
	int delta = event->angleDelta().y();

	if ( delta > 0 ) {
		m_cameraDistance *= 0.9f; // we are reducing the distance by this 90 %
	} else if ( delta < 0 ) {
		m_cameraDistance *= 1.1f; // we are increasing the distance by 110 %
	}

	// @Note: We need to clamp the distance so we dont get too close to the model
	if ( m_cameraDistance < 1.0f ) {
		m_cameraDistance = 1.0f; // this is the minimum length
	}

	if ( m_cameraDistance > 500.0f ) {
		m_cameraDistance = 500.0f; // this is the maximum length
	}

	// Notify panels about the distance change
	emit cameraDistanceChanged( m_cameraDistance );

	// Trigger repaint when zoom changes
	update();

	event->accept();
}

void ModelViewport::setStatusBar( StatusBarWidget *statusBar ) {
	m_statusBar = statusBar;
}

void ModelViewport::enterEvent( QEnterEvent *event ) {
	// Grab keyboard focus so WASD works as soon as the mouse enters the viewport.
	// Without this, the user would have to click first before keys register.
	setFocus();
	event->accept();
}

void ModelViewport::leaveEvent( QEvent *event ) {
	// Mouse left viewport - clear viewport-specific info
	if ( m_statusBar ) {
		m_statusBar->clearViewportInfo();
	}
	event->accept();
}

void ModelViewport::keyPressEvent( QKeyEvent *event ) {
	// Don't process if auto-repeat (key held down)
	if ( event->isAutoRepeat() ) {
		event->accept();
		return;
	}

	int key = event->key();

	// Space bar: immediate toggle for animation play/pause (not a held key)
	if ( key == Qt::Key_Space ) {
		playAnimation( !m_animationPlaying );
		event->accept();
		return;
	}

	// R key: toggle rotation gizmo visibility
	if ( key == Qt::Key_R ) {
		setShowGizmo( !m_showGizmo );
		event->accept();
		return;
	}

	m_pressedKeys.insert( key );

	// Start movement timer if not already running
	if ( !m_movementTimer->isActive() ) {
		m_lastMovementTime = QDateTime::currentMSecsSinceEpoch();
		m_movementTimer->start();
	}

	event->accept();
}

void ModelViewport::keyReleaseEvent( QKeyEvent *event ) {
	// Don't process if auto-repeat
	if ( event->isAutoRepeat() ) {
		event->accept();
		return;
	}

	int key = event->key();
	m_pressedKeys.remove( key );

	// Stop movement timer if no keys pressed
	if ( m_pressedKeys.isEmpty() && m_movementTimer->isActive() ) {
		m_movementTimer->stop();
	}

	event->accept();
}

void ModelViewport::focusOutEvent( QFocusEvent *event ) {
	// Clear all pressed keys when losing focus
	m_pressedKeys.clear();
	if ( m_movementTimer->isActive() ) {
		m_movementTimer->stop();
	}
	QOpenGLWidget::focusOutEvent( event );
}

void ModelViewport::onCameraMovementUpdate() {
	if ( m_pressedKeys.isEmpty() ) {
		return;
	}

	// Calculate delta time
	qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
	float deltaTime = ( currentTime - m_lastMovementTime ) / 1000.0f;
	m_lastMovementTime = currentTime;

	// Clamp delta time to prevent huge jumps
	if ( deltaTime > 0.1f ) deltaTime = 0.1f;

	updateCameraMovement( deltaTime );
	update();  // Trigger repaint
}

void ModelViewport::updateCameraMovement( float deltaTime ) {
	// Speed scales with camera distance so movement feels consistent at all zoom levels:
	// close in = slow fine movement, far out = fast broad movement
	float baseSpeed = m_cameraDistance * 0.8f;
	if ( baseSpeed < 2.0f ) baseSpeed = 2.0f;
	if ( baseSpeed > 200.0f ) baseSpeed = 200.0f;
	float currentSpeed = baseSpeed;

	// Modifiers
	if ( m_pressedKeys.contains( Qt::Key_Shift ) ) {
		currentSpeed *= 3.0f;  // 3x faster with Shift
	}
	if ( m_pressedKeys.contains( Qt::Key_Control ) ) {
		currentSpeed *= 0.3f;  // 0.3x slower with Ctrl
	}

	// Calculate movement distance this frame
	float moveDistance = currentSpeed * deltaTime;

	// Calculate camera forward and right vectors based on yaw.
	// Camera is at target + (cos(p)*sin(yaw), sin(p), cos(p)*cos(yaw))*dist,
	// so the true view direction (camera → target) is the negation of that offset.
	// forward = (-sin(yaw), 0, -cos(yaw)) makes W pan INTO the scene.
	math_vec3_t forward;
	forward[0] = -sinf( m_cameraYaw );  // X
	forward[1] = 0.0f;                   // Y (no vertical in forward)
	forward[2] = -cosf( m_cameraYaw );  // Z

	// Right = cross(up, forward) → perpendicular to forward in XZ plane
	math_vec3_t right;
	right[0] = cosf( m_cameraYaw );     // X
	right[1] = 0.0f;                     // Y
	right[2] = -sinf( m_cameraYaw );    // Z

	// Up vector (world Y-axis)
	math_vec3_t up = { 0.0f, 1.0f, 0.0f };

	// Movement accumulator
	math_vec3_t movement = { 0.0f, 0.0f, 0.0f };

	// WASD movement
	if ( m_pressedKeys.contains( Qt::Key_W ) ) {
		// Move forward
		movement[0] += forward[0] * moveDistance;
		movement[1] += forward[1] * moveDistance;
		movement[2] += forward[2] * moveDistance;
	}
	if ( m_pressedKeys.contains( Qt::Key_S ) ) {
		// Move backward
		movement[0] -= forward[0] * moveDistance;
		movement[1] -= forward[1] * moveDistance;
		movement[2] -= forward[2] * moveDistance;
	}
	if ( m_pressedKeys.contains( Qt::Key_A ) ) {
		// Strafe left
		movement[0] -= right[0] * moveDistance;
		movement[1] -= right[1] * moveDistance;
		movement[2] -= right[2] * moveDistance;
	}
	if ( m_pressedKeys.contains( Qt::Key_D ) ) {
		// Strafe right
		movement[0] += right[0] * moveDistance;
		movement[1] += right[1] * moveDistance;
		movement[2] += right[2] * moveDistance;
	}

	// Vertical movement (E=up, C=down)
	if ( m_pressedKeys.contains( Qt::Key_E ) ) {
		// Move up
		movement[0] += up[0] * moveDistance;
		movement[1] += up[1] * moveDistance;
		movement[2] += up[2] * moveDistance;
	}
	if ( m_pressedKeys.contains( Qt::Key_C ) ) {
		// Move down
		movement[0] -= up[0] * moveDistance;
		movement[1] -= up[1] * moveDistance;
		movement[2] -= up[2] * moveDistance;
	}

	// Apply movement to camera target
	m_cameraTarget[0] += movement[0];
	m_cameraTarget[1] += movement[1];
	m_cameraTarget[2] += movement[2];
}

bool ModelViewport::loadModel( const QString &modelPath ) {
	if ( m_model ) {
		free_model( m_model );
		m_model = nullptr;
	}

	QByteArray pathBytes = modelPath.toUtf8();
	const char *cPath = pathBytes.constData();

	mdl_result_t result = create_mdl_model( cPath, &m_model );

	if ( result != MDL_SUCCESS || m_model == nullptr ) {
		m_model = nullptr;
		return ( false );
	}

	// Set model data for this viewport's renderer instance
	if ( m_renderInstance ) {
		r_qt_set_model_data( m_renderInstance, m_model->header, m_model->data,
							 m_model->texture_header, m_model->texture_data,
							 m_model->seqgroups, m_model->num_seqgroups );
	} else {
		qCritical() << "ERROR: No render instance available!";
	}

	// Initialize bodypart manager for this model (called once per model load)
	bodypart_set_model( m_model->header, m_model->data );

	// Initialize animation state with sequence 0 (usually idle/reference)
	if ( m_model->header->numseq > 0 ) {
		mdl_animation_set_sequence( &m_animState, 0, m_model->header,
			m_model->data, m_model->seqgroups );

		// Sync to render instance
		if ( m_renderInstance ) {
			r_qt_set_animation_state( m_renderInstance, &m_animState );
			r_qt_set_animation_enabled( m_renderInstance, true );
		}
	}

	// Reset camera and model transform when loading new model
	resetCamera();
	m_modelOffset[0] = 0.0f;
	m_modelOffset[1] = 0.0f;
	m_modelOffset[2] = 0.0f;
	m_modelRotationX = 0.0f;
	m_modelRotationY = 0.0f;
	m_modelRotationZ = 0.0f;

	// CRITICAL: Trigger repaint to show the newly loaded model
	update();

	emit modelLoaded( modelPath );
	return ( true );
}

void ModelViewport::frameModel() {
	if ( !m_model || !m_model->header ) {
		return;
	}

	const float modelScale = 0.1f; // Same scale we use for rendering

	// Get bounding box from model header
	math_vec3_t bbmin, bbmax;
	Math_Vec3Copy( m_model->header->bbmin, bbmin );
	Math_Vec3Copy( m_model->header->bbmax, bbmax );

	// Calculate model dimensions in HL coords
	float sizeX = bbmax[0] - bbmin[0];
	float sizeY = bbmax[1] - bbmin[1];
	float sizeZ = bbmax[2] - bbmin[2]; // Height in HL coords

	// ============================================================================
	// GROUND POSITIONING: Place at model's actual lowest point
	// ============================================================================
	m_groundHeight = bbmin[2] * modelScale;

	// ============================================================================
	// CENTER MODEL: Look at the geometric center of the bounding box
	// ============================================================================
	// Calculate center in HL coordinates
	float centerX = ( bbmin[0] + bbmax[0] ) / 2.0f;
	float centerY = ( bbmin[1] + bbmax[1] ) / 2.0f;
	float centerZ = ( bbmin[2] + bbmax[2] ) / 2.0f; // Vertical center

	// Transform HL coords to OpenGL coords:
	// HL: X=right, Y=forward, Z=up → OpenGL: X=right, Y=up, Z=-forward
	m_cameraTarget[0] = centerX * modelScale;            // HL X → GL X
	m_cameraTarget[1] = centerZ * modelScale;            // HL Z → GL Y (up)
	m_cameraTarget[2] = -centerY * modelScale;           // HL Y → GL -Z

	// Reset model offset to put model at origin
	m_modelOffset[0] = 0.0f;
	m_modelOffset[1] = 0.0f;
	m_modelOffset[2] = 0.0f;

	// Calculate camera distance to fit the whole model
	float maxDim = sizeX;
	if ( sizeY > maxDim ) maxDim = sizeY;
	if ( sizeZ > maxDim ) maxDim = sizeZ;

	// Scale and add some padding so model fits nicely in view
	m_cameraDistance = maxDim * modelScale * 1.5f;

	// Clamp to prevent extreme values
	if ( m_cameraDistance < 2.0f ) m_cameraDistance = 2.0f;
	if ( m_cameraDistance > 100.0f ) m_cameraDistance = 100.0f;

	// Look straight at the center (no pitch), slightly angled view
	m_cameraPitch = 0.0f;   // Straight ahead, looking at center
	m_cameraYaw = 0.0f;     // Front view (user can rotate from here)

	// Trigger repaint to show new framing
	update();
}

bool ModelViewport::hasModelLoaded() {
	return ( m_model != nullptr );
}

void ModelViewport::getCameraPosition( float &x, float &y, float &z ) const {
	// Calculate camera position from spherical coordinates
	float pitch = m_cameraPitch;
	float yaw = m_cameraYaw;
	float dist = m_cameraDistance;

	x = m_cameraTarget[0] + dist * cosf( pitch ) * sinf( yaw );
	y = m_cameraTarget[1] + dist * sinf( pitch );
	z = m_cameraTarget[2] + dist * cosf( pitch ) * cosf( yaw );
}

float ModelViewport::getCameraDistance() const {
	return m_cameraDistance;
}

void ModelViewport::setCameraDistance( float distance ) {
	m_cameraDistance = qBound( 1.0f, distance, 500.0f );
	update();
}

bool ModelViewport::raycastToGround( int screenX, int screenY, float &worldX, float &worldY, float &worldZ ) {
	int w = width();
	int h = height();

	if ( w <= 0 || h <= 0 ) return false;

	// Build QMatrix4x4 from cglm matrices (column-major float[4][4])
	// cglm mat4: m[col][row], QMatrix4x4 ctor takes row-major
	QMatrix4x4 proj(
		m_projMatrix[0][0], m_projMatrix[1][0], m_projMatrix[2][0], m_projMatrix[3][0],
		m_projMatrix[0][1], m_projMatrix[1][1], m_projMatrix[2][1], m_projMatrix[3][1],
		m_projMatrix[0][2], m_projMatrix[1][2], m_projMatrix[2][2], m_projMatrix[3][2],
		m_projMatrix[0][3], m_projMatrix[1][3], m_projMatrix[2][3], m_projMatrix[3][3]
	);

	QMatrix4x4 view(
		m_viewMatrix[0][0], m_viewMatrix[1][0], m_viewMatrix[2][0], m_viewMatrix[3][0],
		m_viewMatrix[0][1], m_viewMatrix[1][1], m_viewMatrix[2][1], m_viewMatrix[3][1],
		m_viewMatrix[0][2], m_viewMatrix[1][2], m_viewMatrix[2][2], m_viewMatrix[3][2],
		m_viewMatrix[0][3], m_viewMatrix[1][3], m_viewMatrix[2][3], m_viewMatrix[3][3]
	);

	bool invertible = false;
	QMatrix4x4 vpInv = ( proj * view ).inverted( &invertible );

	if ( !invertible ) return false;

	// Convert screen to NDC
	float ndcX = ( 2.0f * screenX / w ) - 1.0f;
	float ndcY = 1.0f - ( 2.0f * screenY / h );

	// Unproject near and far clip planes to world space
	QVector4D nearWorld = vpInv * QVector4D( ndcX, ndcY, -1.0f, 1.0f );
	QVector4D farWorld  = vpInv * QVector4D( ndcX, ndcY,  1.0f, 1.0f );

	if ( nearWorld.w() == 0.0f || farWorld.w() == 0.0f ) return false;

	nearWorld /= nearWorld.w();
	farWorld  /= farWorld.w();

	QVector3D rayOrigin( nearWorld.x(), nearWorld.y(), nearWorld.z() );
	QVector3D rayDir = QVector3D( farWorld.x() - nearWorld.x(),
								  farWorld.y() - nearWorld.y(),
								  farWorld.z() - nearWorld.z() ).normalized();

	// Intersect with a plane facing the camera that passes through the camera target.
	// This gives all 3 axes (X, Y, Z) updating as the mouse moves - same as Blender.
	float camX, camY, camZ;
	getCameraPosition( camX, camY, camZ );

	QVector3D camPos( camX, camY, camZ );
	QVector3D target( m_cameraTarget[0], m_cameraTarget[1], m_cameraTarget[2] );

	// Plane normal = direction from target toward camera
	QVector3D planeNormal = ( camPos - target ).normalized();

	// Plane equation: dot(planeNormal, point) = dot(planeNormal, target)
	float planeD    = QVector3D::dotProduct( planeNormal, target );
	float rayDotN   = QVector3D::dotProduct( rayDir, planeNormal );

	if ( fabsf( rayDotN ) < 1e-6f ) return false;  // Ray parallel to plane

	float t = ( planeD - QVector3D::dotProduct( rayOrigin, planeNormal ) ) / rayDotN;
	if ( t < 0.0f ) return false;  // Intersection behind camera

	worldX = rayOrigin.x() + t * rayDir.x();
	worldY = rayOrigin.y() + t * rayDir.y();
	worldZ = rayOrigin.z() + t * rayDir.z();

	return true;
}

int ModelViewport::getVertexCount() const {
	if ( !m_model || !m_model->header ) return 0;

	int total = 0;
	mstudiobodyparts_t *bodyparts = (mstudiobodyparts_t *)( m_model->data + m_model->header->bodypartindex );
	for ( int bp = 0; bp < m_model->header->numbodyparts; bp++ ) {
		mstudiomodel_t *models = (mstudiomodel_t *)( m_model->data + bodyparts[bp].modelindex );
		for ( int m = 0; m < bodyparts[bp].nummodels; m++ ) {
			total += models[m].numverts;
		}
	}
	return total;
}

int ModelViewport::getTriangleCount() const {
	if ( !m_model || !m_model->header ) return 0;

	int total = 0;
	mstudiobodyparts_t *bodyparts = (mstudiobodyparts_t *)( m_model->data + m_model->header->bodypartindex );
	for ( int bp = 0; bp < m_model->header->numbodyparts; bp++ ) {
		mstudiomodel_t *models = (mstudiomodel_t *)( m_model->data + bodyparts[bp].modelindex );
		for ( int m = 0; m < bodyparts[bp].nummodels; m++ ) {
			mstudiomesh_t *meshes = (mstudiomesh_t *)( m_model->data + models[m].meshindex );
			for ( int mesh = 0; mesh < models[m].nummesh; mesh++ ) {
				total += meshes[mesh].numtris;
			}
		}
	}
	return total;
}

int ModelViewport::getBoneCount() const {
	if ( !m_model || !m_model->header ) return 0;
	return m_model->header->numbones;
}

int ModelViewport::getSequenceCount() const {
	if ( !m_model || !m_model->header ) return 0;
	return m_model->header->numseq;
}

int ModelViewport::getTextureCount() const {
	if ( !m_model || !m_model->header ) return 0;
	int count = m_model->header->numtextures;
	if ( count == 0 && m_model->texture_header ) {
		count = m_model->texture_header->numtextures;
	}
	return count;
}

int ModelViewport::getBodypartCount() const {
	if ( !m_model || !m_model->header ) return 0;
	return m_model->header->numbodyparts;
}

QString ModelViewport::getSequenceName( int index ) const {
	if ( !m_model || !m_model->header || !m_model->data ) {
		return QString();
	}

	if ( index < 0 || index >= m_model->header->numseq ) {
		return QString();
	}

	mstudioseqdesc_t *sequences = (mstudioseqdesc_t *)( m_model->data + m_model->header->seqindex );
	return QString::fromLatin1( sequences[index].label );
}

// ═══════════════════════════════════════════════════════════════════════════
// Extended Model Info Getters for Inspector Panels
// ═══════════════════════════════════════════════════════════════════════════

int ModelViewport::getMDLVersion() const {
	if ( !m_model || !m_model->header ) return 0;
	return m_model->header->version;
}

int ModelViewport::getModelFlags() const {
	if ( !m_model || !m_model->header ) return 0;
	return m_model->header->flags;
}

int ModelViewport::getControllerCount() const {
	if ( !m_model || !m_model->header ) return 0;
	return m_model->header->numbonecontrollers;
}

int ModelViewport::getControllerBone( int index ) const {
	if ( !m_model || !m_model->header ) return -1;
	if ( index < 0 || index >= m_model->header->numbonecontrollers ) return -1;
	mstudiobonecontroller_t *ctrl = (mstudiobonecontroller_t *)( m_model->data + m_model->header->bonecontrollerindex );
	return ctrl[index].bone;
}

int ModelViewport::getControllerType( int index ) const {
	if ( !m_model || !m_model->header ) return 0;
	if ( index < 0 || index >= m_model->header->numbonecontrollers ) return 0;
	mstudiobonecontroller_t *ctrl = (mstudiobonecontroller_t *)( m_model->data + m_model->header->bonecontrollerindex );
	return ctrl[index].type;
}

float ModelViewport::getControllerStart( int index ) const {
	if ( !m_model || !m_model->header ) return 0.0f;
	if ( index < 0 || index >= m_model->header->numbonecontrollers ) return 0.0f;
	mstudiobonecontroller_t *ctrl = (mstudiobonecontroller_t *)( m_model->data + m_model->header->bonecontrollerindex );
	return ctrl[index].start;
}

float ModelViewport::getControllerEnd( int index ) const {
	if ( !m_model || !m_model->header ) return 0.0f;
	if ( index < 0 || index >= m_model->header->numbonecontrollers ) return 0.0f;
	mstudiobonecontroller_t *ctrl = (mstudiobonecontroller_t *)( m_model->data + m_model->header->bonecontrollerindex );
	return ctrl[index].end;
}

int ModelViewport::getControllerRest( int index ) const {
	if ( !m_model || !m_model->header ) return 0;
	if ( index < 0 || index >= m_model->header->numbonecontrollers ) return 0;
	mstudiobonecontroller_t *ctrl = (mstudiobonecontroller_t *)( m_model->data + m_model->header->bonecontrollerindex );
	return ctrl[index].rest;
}

int ModelViewport::getControllerIndex( int index ) const {
	if ( !m_model || !m_model->header ) return -1;
	if ( index < 0 || index >= m_model->header->numbonecontrollers ) return -1;
	mstudiobonecontroller_t *ctrl = (mstudiobonecontroller_t *)( m_model->data + m_model->header->bonecontrollerindex );
	return ctrl[index].index;
}

void ModelViewport::setControllerValue( int controller, float value ) {
	if ( !m_model || !m_model->header ) return;
	if ( controller < 0 || controller > 5 ) return;  // Controller indices are 0-5 (or mouth at 4)
	mdl_bonecontroller_set_value( &m_controllerState, controller, value );
	// Pass controller value to render instance
	if ( m_renderInstance ) {
		r_qt_set_controller_value( m_renderInstance, controller, value );
	}
	update();  // Trigger repaint to show changes
}

float ModelViewport::getControllerValue( int controller ) const {
	if ( controller < 0 || controller > 5 ) return 0.0f;
	return mdl_bonecontroller_get_value( &m_controllerState, controller );
}

int ModelViewport::getSeqGroupCount() const {
	if ( !m_model || !m_model->header ) return 0;
	return m_model->header->numseqgroups;
}

int ModelViewport::getAttachmentCount() const {
	if ( !m_model || !m_model->header ) return 0;
	return m_model->header->numattachments;
}

QString ModelViewport::getAttachmentName( int index ) const {
	if ( !m_model || !m_model->header ) return QString();
	if ( index < 0 || index >= m_model->header->numattachments ) return QString();

	mstudioattachment_t *attachment = (mstudioattachment_t *)( (uint8_t *)m_model->header +
	                                                           m_model->header->attachmentindex ) +
	                                  index;
	return QString::fromLatin1( attachment->name, strnlen( attachment->name, 32 ) );
}

int ModelViewport::getAttachmentBone( int index ) const {
	if ( !m_model || !m_model->header ) return -1;
	if ( index < 0 || index >= m_model->header->numattachments ) return -1;

	mstudioattachment_t *attachment = (mstudioattachment_t *)( (uint8_t *)m_model->header +
	                                                           m_model->header->attachmentindex ) +
	                                  index;
	return attachment->bone;
}

int ModelViewport::getAttachmentType( int index ) const {
	if ( !m_model || !m_model->header ) return 0;
	if ( index < 0 || index >= m_model->header->numattachments ) return 0;

	mstudioattachment_t *attachment = (mstudioattachment_t *)( (uint8_t *)m_model->header +
	                                                           m_model->header->attachmentindex ) +
	                                  index;
	return attachment->type;
}

void ModelViewport::getAttachmentPosition( int index, float &x, float &y, float &z ) const {
	x = y = z = 0.0f;
	if ( !m_model || !m_model->header ) return;
	if ( index < 0 || index >= m_model->header->numattachments ) return;

	mstudioattachment_t *attachment = (mstudioattachment_t *)( (uint8_t *)m_model->header +
	                                                           m_model->header->attachmentindex ) +
	                                  index;
	x = attachment->org[0];
	y = attachment->org[1];
	z = attachment->org[2];
}

int ModelViewport::getHitboxCount() const {
	if ( !m_model || !m_model->header ) return 0;
	return m_model->header->numhitboxes;
}

void ModelViewport::getEyePosition( float &x, float &y, float &z ) const {
	if ( !m_model || !m_model->header ) {
		x = y = z = 0.0f;
		return;
	}
	x = m_model->header->eyeposition[0];
	y = m_model->header->eyeposition[1];
	z = m_model->header->eyeposition[2];
}

void ModelViewport::getBoundingBox( float bbmin[3], float bbmax[3] ) const {
	if ( !m_model || !m_model->header ) {
		bbmin[0] = bbmin[1] = bbmin[2] = 0.0f;
		bbmax[0] = bbmax[1] = bbmax[2] = 0.0f;
		return;
	}
	bbmin[0] = m_model->header->bbmin[0];
	bbmin[1] = m_model->header->bbmin[1];
	bbmin[2] = m_model->header->bbmin[2];
	bbmax[0] = m_model->header->bbmax[0];
	bbmax[1] = m_model->header->bbmax[1];
	bbmax[2] = m_model->header->bbmax[2];
}

// ═══════════════════════════════════════════════════════════════════════════
// Extended Header Info for Model Info Panel
// ═══════════════════════════════════════════════════════════════════════════

int ModelViewport::getHeaderId() const {
	if ( !m_model || !m_model->header ) return 0;
	return m_model->header->id;
}

QString ModelViewport::getHeaderName() const {
	if ( !m_model || !m_model->header ) return QString();
	return QString::fromLatin1( m_model->header->name, 64 ).trimmed();
}

int ModelViewport::getTransitionCount() const {
	if ( !m_model || !m_model->header ) return 0;
	return m_model->header->numtransitions;
}

int ModelViewport::getSoundGroupCount() const {
	if ( !m_model || !m_model->header ) return 0;
	return m_model->header->soundgroups;
}

int ModelViewport::getSkinRefCount() const {
	if ( !m_model || !m_model->header ) return 0;
	return m_model->header->numskinref;
}

int ModelViewport::getFileLength() const {
	if ( !m_model || !m_model->header ) return 0;
	return m_model->header->length;
}

void ModelViewport::getMovementBox( float min[3], float max[3] ) const {
	if ( !m_model || !m_model->header ) {
		min[0] = min[1] = min[2] = 0.0f;
		max[0] = max[1] = max[2] = 0.0f;
		return;
	}
	min[0] = m_model->header->min[0];
	min[1] = m_model->header->min[1];
	min[2] = m_model->header->min[2];
	max[0] = m_model->header->max[0];
	max[1] = m_model->header->max[1];
	max[2] = m_model->header->max[2];
}

// ═══════════════════════════════════════════════════════════════════════════
// Animation Control for Sequences Panel
// ═══════════════════════════════════════════════════════════════════════════

void ModelViewport::setPlaybackSpeed( float multiplier ) {
	m_playbackSpeed = qBound( 0.1f, multiplier, 3.0f );
}

float ModelViewport::getPlaybackSpeed() const {
	return m_playbackSpeed;
}

void ModelViewport::setAnimationLooping( bool loop ) {
	m_animationLooping = loop;
	m_animState.is_looping = loop;  // Also update the animation state
}

bool ModelViewport::isAnimationLooping() const {
	return m_animationLooping;
}

int ModelViewport::getSequenceFrameCount( int seqIndex ) const {
	if ( !m_model || !m_model->header || !m_model->data ) return 0;
	if ( seqIndex < 0 || seqIndex >= m_model->header->numseq ) return 0;

	mstudioseqdesc_t *sequences = (mstudioseqdesc_t *)( m_model->data + m_model->header->seqindex );
	return sequences[seqIndex].numframes;
}

float ModelViewport::getSequenceFPS( int seqIndex ) const {
	if ( !m_model || !m_model->header || !m_model->data ) return 0.0f;
	if ( seqIndex < 0 || seqIndex >= m_model->header->numseq ) return 0.0f;

	mstudioseqdesc_t *sequences = (mstudioseqdesc_t *)( m_model->data + m_model->header->seqindex );
	return sequences[seqIndex].fps;
}

QString ModelViewport::getSequenceActivityName( int seqIndex ) const {
	if ( !m_model || !m_model->header || !m_model->data ) return QString();
	if ( seqIndex < 0 || seqIndex >= m_model->header->numseq ) return QString();

	mstudioseqdesc_t *sequences = (mstudioseqdesc_t *)( m_model->data + m_model->header->seqindex );
	int activity = sequences[seqIndex].activity;

	// Return activity number for now - could map to names later
	if ( activity > 0 ) {
		return QString( "ACT_%1" ).arg( activity );
	}
	return QString();
}

int ModelViewport::getSequenceEventCount( int seqIndex ) const {
	if ( !m_model || !m_model->header || !m_model->data ) return 0;
	if ( seqIndex < 0 || seqIndex >= m_model->header->numseq ) return 0;

	mstudioseqdesc_t *sequences = (mstudioseqdesc_t *)( m_model->data + m_model->header->seqindex );
	return sequences[seqIndex].numevents;
}

int ModelViewport::getSequenceEventFrame( int seqIndex, int eventIndex ) const {
	if ( !m_model || !m_model->header || !m_model->data ) return -1;
	if ( seqIndex < 0 || seqIndex >= m_model->header->numseq ) return -1;

	mstudioseqdesc_t *sequences = (mstudioseqdesc_t *)( m_model->data + m_model->header->seqindex );
	if ( eventIndex < 0 || eventIndex >= sequences[seqIndex].numevents ) return -1;

	mstudioevent_t *events = (mstudioevent_t *)( m_model->data + sequences[seqIndex].eventindex );
	return events[eventIndex].frame;
}

int ModelViewport::getSequenceEventCode( int seqIndex, int eventIndex ) const {
	if ( !m_model || !m_model->header || !m_model->data ) return 0;
	if ( seqIndex < 0 || seqIndex >= m_model->header->numseq ) return 0;

	mstudioseqdesc_t *sequences = (mstudioseqdesc_t *)( m_model->data + m_model->header->seqindex );
	if ( eventIndex < 0 || eventIndex >= sequences[seqIndex].numevents ) return 0;

	mstudioevent_t *events = (mstudioevent_t *)( m_model->data + sequences[seqIndex].eventindex );
	return events[eventIndex].event;
}

QString ModelViewport::getSequenceEventOptions( int seqIndex, int eventIndex ) const {
	if ( !m_model || !m_model->header || !m_model->data ) return QString();
	if ( seqIndex < 0 || seqIndex >= m_model->header->numseq ) return QString();

	mstudioseqdesc_t *sequences = (mstudioseqdesc_t *)( m_model->data + m_model->header->seqindex );
	if ( eventIndex < 0 || eventIndex >= sequences[seqIndex].numevents ) return QString();

	mstudioevent_t *events = (mstudioevent_t *)( m_model->data + sequences[seqIndex].eventindex );
	return QString::fromLatin1( events[eventIndex].options );
}

// ═══════════════════════════════════════════════════════════════════════════
// Texture Info for Textures Panel
// ═══════════════════════════════════════════════════════════════════════════

QImage ModelViewport::getTextureImage( int index ) const {
	if ( !m_model ) return QImage();

	// Determine which header to use (main or texture model)
	studiohdr_t *texHdr = m_model->texture_header ? m_model->texture_header : m_model->header;
	unsigned char *texData = m_model->texture_data ? m_model->texture_data : m_model->data;

	if ( !texHdr || !texData ) return QImage();
	if ( index < 0 || index >= texHdr->numtextures ) return QImage();

	mstudiotexture_t *textures = (mstudiotexture_t *)( texData + texHdr->textureindex );
	mstudiotexture_t *tex = &textures[index];

	// Get pixel data and palette
	const uint8_t *pixels = texData + tex->index;
	const uint8_t *palette = pixels + ( tex->width * tex->height );

	// Create QImage
	QImage img( tex->width, tex->height, QImage::Format_RGB888 );
	for ( int y = 0; y < tex->height; ++y ) {
		for ( int x = 0; x < tex->width; ++x ) {
			int idx = pixels[y * tex->width + x];
			img.setPixel( x, y, qRgb( palette[idx * 3], palette[idx * 3 + 1], palette[idx * 3 + 2] ) );
		}
	}

	return img;
}

QString ModelViewport::getTextureName( int index ) const {
	if ( !m_model ) return QString();

	studiohdr_t *texHdr = m_model->texture_header ? m_model->texture_header : m_model->header;
	unsigned char *texData = m_model->texture_data ? m_model->texture_data : m_model->data;

	if ( !texHdr || !texData ) return QString();
	if ( index < 0 || index >= texHdr->numtextures ) return QString();

	mstudiotexture_t *textures = (mstudiotexture_t *)( texData + texHdr->textureindex );
	return QString::fromLatin1( textures[index].name );
}

void ModelViewport::getTextureDimensions( int index, int &w, int &h ) const {
	w = h = 0;
	if ( !m_model ) return;

	studiohdr_t *texHdr = m_model->texture_header ? m_model->texture_header : m_model->header;
	unsigned char *texData = m_model->texture_data ? m_model->texture_data : m_model->data;

	if ( !texHdr || !texData ) return;
	if ( index < 0 || index >= texHdr->numtextures ) return;

	mstudiotexture_t *textures = (mstudiotexture_t *)( texData + texHdr->textureindex );
	w = textures[index].width;
	h = textures[index].height;
}

int ModelViewport::getTextureFlags( int index ) const {
	if ( !m_model ) return 0;

	studiohdr_t *texHdr = m_model->texture_header ? m_model->texture_header : m_model->header;
	unsigned char *texData = m_model->texture_data ? m_model->texture_data : m_model->data;

	if ( !texHdr || !texData ) return 0;
	if ( index < 0 || index >= texHdr->numtextures ) return 0;

	mstudiotexture_t *textures = (mstudiotexture_t *)( texData + texHdr->textureindex );
	return textures[index].flags;
}

// ═══════════════════════════════════════════════════════════════════════════
// Bodypart Info for Bodyparts Panel
// ═══════════════════════════════════════════════════════════════════════════

QString ModelViewport::getBodypartName( int bpIndex ) const {
	if ( !m_model || !m_model->header || !m_model->data ) return QString();
	if ( bpIndex < 0 || bpIndex >= m_model->header->numbodyparts ) return QString();

	mstudiobodyparts_t *bodyparts = (mstudiobodyparts_t *)( m_model->data + m_model->header->bodypartindex );
	return QString::fromLatin1( bodyparts[bpIndex].name );
}

int ModelViewport::getSubmodelCount( int bpIndex ) const {
	if ( !m_model || !m_model->header || !m_model->data ) return 0;
	if ( bpIndex < 0 || bpIndex >= m_model->header->numbodyparts ) return 0;

	mstudiobodyparts_t *bodyparts = (mstudiobodyparts_t *)( m_model->data + m_model->header->bodypartindex );
	return bodyparts[bpIndex].nummodels;
}

QString ModelViewport::getSubmodelName( int bpIndex, int subIndex ) const {
	if ( !m_model || !m_model->header || !m_model->data ) return QString();
	if ( bpIndex < 0 || bpIndex >= m_model->header->numbodyparts ) return QString();

	mstudiobodyparts_t *bodyparts = (mstudiobodyparts_t *)( m_model->data + m_model->header->bodypartindex );
	if ( subIndex < 0 || subIndex >= bodyparts[bpIndex].nummodels ) return QString();

	mstudiomodel_t *models = (mstudiomodel_t *)( m_model->data + bodyparts[bpIndex].modelindex );
	return QString::fromLatin1( models[subIndex].name );
}

int ModelViewport::getSubmodelVertexCount( int bpIndex, int subIndex ) const {
	if ( !m_model || !m_model->header || !m_model->data ) return 0;
	if ( bpIndex < 0 || bpIndex >= m_model->header->numbodyparts ) return 0;

	mstudiobodyparts_t *bodyparts = (mstudiobodyparts_t *)( m_model->data + m_model->header->bodypartindex );
	if ( subIndex < 0 || subIndex >= bodyparts[bpIndex].nummodels ) return 0;

	mstudiomodel_t *models = (mstudiomodel_t *)( m_model->data + bodyparts[bpIndex].modelindex );
	return models[subIndex].numverts;
}

void ModelViewport::setBodypart( int bpIndex, int subIndex ) {
	if ( !m_model || !m_model->header ) return;
	if ( bpIndex < 0 || bpIndex >= m_model->header->numbodyparts ) return;

	// Set the submodel for this bodypart using the C backend
	// Note: bodypart_set_model is called once in loadModel(), not here
	bodypart_set_submodel( bpIndex, subIndex );

	// Rebuild render data for the Qt instance with new bodypart selection
	if ( m_renderInstance ) {
		r_qt_rebuild_mesh_data( m_renderInstance );
	}

	update();
}

int ModelViewport::getActiveSubmodel( int bpIndex ) const {
	if ( !m_model || !m_model->header ) return 0;
	if ( bpIndex < 0 || bpIndex >= m_model->header->numbodyparts ) return 0;

	// Get from bodypart manager
	return bodypart_get_model_index( bpIndex );
}

// ═══════════════════════════════════════════════════════════════════════════
// Bone Info for Bones Panel
// ═══════════════════════════════════════════════════════════════════════════

QString ModelViewport::getBoneName( int index ) const {
	if ( !m_model || !m_model->header || !m_model->data ) return QString();
	if ( index < 0 || index >= m_model->header->numbones ) return QString();

	mstudiobone_t *bones = (mstudiobone_t *)( m_model->data + m_model->header->boneindex );
	return QString::fromLatin1( bones[index].name );
}

int ModelViewport::getBoneParent( int index ) const {
	if ( !m_model || !m_model->header || !m_model->data ) return -1;
	if ( index < 0 || index >= m_model->header->numbones ) return -1;

	mstudiobone_t *bones = (mstudiobone_t *)( m_model->data + m_model->header->boneindex );
	return bones[index].parent;
}

// ═══════════════════════════════════════════════════════════════════════════
// Skin Family Control
// ═══════════════════════════════════════════════════════════════════════════

void ModelViewport::setSkinFamily( int family ) {
	if ( !m_model || !m_model->header ) return;

	int numSkins = m_model->header->numskinfamilies;
	if ( numSkins <= 0 ) numSkins = 1;

	m_currentSkinFamily = qBound( 0, family, numSkins - 1 );
	update();
}

void ModelViewport::nextSkinFamily() {
	if ( !m_model || !m_model->header ) return;

	int numSkins = m_model->header->numskinfamilies;
	if ( numSkins <= 0 ) numSkins = 1;

	m_currentSkinFamily = ( m_currentSkinFamily + 1 ) % numSkins;
	update();
}

void ModelViewport::prevSkinFamily() {
	if ( !m_model || !m_model->header ) return;

	int numSkins = m_model->header->numskinfamilies;
	if ( numSkins <= 0 ) numSkins = 1;

	m_currentSkinFamily = ( m_currentSkinFamily - 1 + numSkins ) % numSkins;
	update();
}

int ModelViewport::getCurrentSkinFamily() const {
	return m_currentSkinFamily;
}

int ModelViewport::getNumSkinFamilies() const {
	if ( !m_model || !m_model->header ) return 1;

	int numSkins = m_model->header->numskinfamilies;
	return numSkins > 0 ? numSkins : 1;
}

// ═══════════════════════════════════════════════════════════════════════════
// Lighting Controls
// ═══════════════════════════════════════════════════════════════════════════

void ModelViewport::setLightingEnabled( bool enabled ) {
	m_lightingEnabled = enabled;
	if ( m_renderInstance ) {
		r_qt_set_lighting_enabled( m_renderInstance, enabled );
	}
	update();
}

bool ModelViewport::isLightingEnabled() const {
	return m_lightingEnabled;
}

void ModelViewport::setAmbientIntensity( float intensity ) {
	m_ambientIntensity = qBound( 0.0f, intensity, 1.0f );
	if ( m_renderInstance ) {
		r_qt_set_ambient_intensity( m_renderInstance, m_ambientIntensity );
	}
	update();
}

float ModelViewport::getAmbientIntensity() const {
	return m_ambientIntensity;
}

void ModelViewport::setLightDirection( float x, float y, float z ) {
	m_lightDirX = qBound( -1.0f, x, 1.0f );
	m_lightDirY = qBound( -1.0f, y, 1.0f );
	m_lightDirZ = qBound( -1.0f, z, 1.0f );

	// Convert direction to light position (scaled)
	// Position the light at a distance along the direction vector
	float lightDist = 10.0f;
	if ( m_renderInstance ) {
		r_qt_set_light_position( m_renderInstance,
		                         m_lightDirX * lightDist,
		                         m_lightDirY * lightDist + 5.0f,  // Offset up
		                         m_lightDirZ * lightDist );
	}
	update();
}

void ModelViewport::getLightDirection( float &x, float &y, float &z ) const {
	x = m_lightDirX;
	y = m_lightDirY;
	z = m_lightDirZ;
}

void ModelViewport::setLightColor( const QColor &color ) {
	m_lightColor = color;
	if ( m_renderInstance ) {
		r_qt_set_light_color( m_renderInstance,
		                      color.redF(),
		                      color.greenF(),
		                      color.blueF() );
	}
	update();
}

QColor ModelViewport::getLightColor() const {
	return m_lightColor;
}

void ModelViewport::setChromeEnabled( bool enabled ) {
	m_chromeEnabled = enabled;
	update();
}

bool ModelViewport::isChromeEnabled() const {
	return m_chromeEnabled;
}

void ModelViewport::resetLighting() {
	m_lightingEnabled = true;
	m_ambientIntensity = 1.0f;  // Full ambient for proper visibility (like HLMV)
	m_lightDirX = 0.0f;
	m_lightDirY = 0.0f;
	m_lightDirZ = -1.0f;
	m_lightColor = QColor( 255, 255, 255 );
	m_chromeEnabled = false;

	// Update C renderer lighting state
	// Light position above and in front of model for good illumination
	if ( m_renderInstance ) {
		r_qt_set_lighting_enabled( m_renderInstance, true );
		r_qt_set_light_position( m_renderInstance, 0.0f, 10.0f, 15.0f );
		r_qt_set_light_color( m_renderInstance, 1.0f, 1.0f, 1.0f );
		r_qt_set_ambient_intensity( m_renderInstance, m_ambientIntensity );
	}
	update();
}

// ────────────────────────────────────────────────────────────────────────────
// Scale controls
// ────────────────────────────────────────────────────────────────────────────

void ModelViewport::setMeshScale( float scale ) {
	m_meshScale = qBound( 0.1f, scale, 10.0f );
	update();
}

float ModelViewport::getMeshScale() const {
	return m_meshScale;
}

void ModelViewport::setBoneScale( float scale ) {
	m_boneScale = qBound( 0.1f, scale, 10.0f );
	update();
}

float ModelViewport::getBoneScale() const {
	return m_boneScale;
}

// ────────────────────────────────────────────────────────────────────────────
// Mirror controls
// ────────────────────────────────────────────────────────────────────────────

void ModelViewport::setMirrorX( bool enabled ) {
	m_mirrorX = enabled;
	update();
}

void ModelViewport::setMirrorY( bool enabled ) {
	m_mirrorY = enabled;
	update();
}

void ModelViewport::setMirrorZ( bool enabled ) {
	m_mirrorZ = enabled;
	update();
}

bool ModelViewport::isMirrorX() const {
	return m_mirrorX;
}

bool ModelViewport::isMirrorY() const {
	return m_mirrorY;
}

bool ModelViewport::isMirrorZ() const {
	return m_mirrorZ;
}

// ────────────────────────────────────────────────────────────────────────────
// FOV control
// ────────────────────────────────────────────────────────────────────────────

void ModelViewport::setFov( float degrees ) {
	m_fov = qBound( 30.0f, degrees, 120.0f );
	// Update projection matrix with new FOV
	makeCurrent();
	float aspect = (float)width() / (float)height();
	float nearPlane = 0.01f;
	float farPlane = 1000.0f;
	glm_perspective( glm_rad( m_fov ), aspect, nearPlane, farPlane, m_projMatrix );
	doneCurrent();
	update();
}

float ModelViewport::getFov() const {
	return m_fov;
}

// ────────────────────────────────────────────────────────────────────────────
// Weapon origin mode
// ────────────────────────────────────────────────────────────────────────────

void ModelViewport::setWeaponOriginMode( bool enabled ) {
	m_weaponOriginMode = enabled;
	if ( enabled ) {
		// Save current camera state before entering weapon origin mode
		m_savedFov = m_fov;
		m_savedCameraPitch = m_cameraPitch;
		m_savedCameraYaw = m_cameraYaw;
		m_savedCameraDistance = m_cameraDistance;
		m_savedCameraTarget[0] = m_cameraTarget[0];
		m_savedCameraTarget[1] = m_cameraTarget[1];
		m_savedCameraTarget[2] = m_cameraTarget[2];
		m_savedModelOffset[0] = m_modelOffset[0];
		m_savedModelOffset[1] = m_modelOffset[1];
		m_savedModelOffset[2] = m_modelOffset[2];

		// Position camera for first-person weapon view (like HLMV)
		// FOV 90 is the Half-Life default for first-person
		m_fov = 90.0f;

		// Camera positioned at origin looking forward (down Y axis in HL coords)
		// This simulates the player's eye position
		m_cameraPitch = 0.0f;   // Looking straight ahead
		m_cameraYaw = 0.0f;     // Facing forward
		m_cameraDistance = 0.0f; // Camera AT origin (eye position)
		m_cameraTarget[0] = 0.0f;
		m_cameraTarget[1] = 10.0f;  // Look forward into the scene
		m_cameraTarget[2] = 0.0f;

		// Reset model offset - weapon should be at its natural position
		m_modelOffset[0] = 0.0f;
		m_modelOffset[1] = 0.0f;
		m_modelOffset[2] = 0.0f;

		// Update projection matrix with new FOV
		resizeGL( width(), height() );
	} else {
		// Restore previous camera state when leaving weapon origin mode
		m_fov = m_savedFov;
		m_cameraPitch = m_savedCameraPitch;
		m_cameraYaw = m_savedCameraYaw;
		m_cameraDistance = m_savedCameraDistance;
		m_cameraTarget[0] = m_savedCameraTarget[0];
		m_cameraTarget[1] = m_savedCameraTarget[1];
		m_cameraTarget[2] = m_savedCameraTarget[2];
		m_modelOffset[0] = m_savedModelOffset[0];
		m_modelOffset[1] = m_savedModelOffset[1];
		m_modelOffset[2] = m_savedModelOffset[2];

		// Update projection matrix with restored FOV
		resizeGL( width(), height() );
	}
	update();
}

bool ModelViewport::isWeaponOriginMode() const {
	return m_weaponOriginMode;
}

void ModelViewport::resetCamera() {
	m_cameraPitch = 0.33f;
	m_cameraYaw = 0.0f;
	m_cameraDistance = 35.0f;
	m_cameraTarget[0] = 0.0f;
	m_cameraTarget[1] = 0.0f;
	m_cameraTarget[2] = 0.0f;
	update();
}

void ModelViewport::resetModelPosition() {
	m_modelOffset[0] = 0.0f;
	m_modelOffset[1] = 0.0f;
	m_modelOffset[2] = 0.0f;
	update();
}

void ModelViewport::getModelOffset( float &x, float &y, float &z ) const {
	x = m_modelOffset[0];
	y = m_modelOffset[1];
	z = m_modelOffset[2];
}

void ModelViewport::resetModelRotation() {
	m_modelRotationX = 0.0f;
	m_modelRotationY = 0.0f;
	m_modelRotationZ = 0.0f;
	update();
}

void ModelViewport::setModelRotationX( float radians ) {
	m_modelRotationX = radians;
	update();
}

void ModelViewport::setModelRotationY( float radians ) {
	m_modelRotationY = radians;
	update();
}

void ModelViewport::setModelRotationZ( float radians ) {
	m_modelRotationZ = radians;
	update();
}

float ModelViewport::getModelRotationX() const {
	return m_modelRotationX;
}

float ModelViewport::getModelRotationY() const {
	return m_modelRotationY;
}

float ModelViewport::getModelRotationZ() const {
	return m_modelRotationZ;
}

void ModelViewport::setShowGizmo( bool show ) {
	m_showGizmo = show;
	if ( !show ) {
		m_gizmoHoveredAxis = -1;
		m_gizmoActiveAxis  = -1;
	}
	update();
}

bool ModelViewport::isGizmoVisible() const {
	return m_showGizmo;
}

// ── Gizmo helpers ─────────────────────────────────────────────────────────

/*
 * buildRayFromMouse
 *   Unprojects a viewport pixel into a world-space ray.
 *   origin[] = camera world position
 *   dir[]    = normalised ray direction
 */
void ModelViewport::buildRayFromMouse( int mx, int my,
                                       float origin[3], float dir[3] ) const
{
	// NDC (-1..1)
	float ndcX =  ( 2.0f * mx / (float)width()  ) - 1.0f;
	float ndcY = -( 2.0f * my / (float)height() ) + 1.0f;

	// Clip-space ray pointing into screen
	vec4 clipRay = { ndcX, ndcY, -1.0f, 1.0f };

	// Copy matrices to mutable locals (glm_mat4_inv doesn't take const)
	mat4 proj, view;
	memcpy( proj, m_projMatrix, sizeof( mat4 ) );
	memcpy( view, m_viewMatrix, sizeof( mat4 ) );

	// View space
	mat4 invProj;
	glm_mat4_inv( proj, invProj );
	vec4 viewRay;
	glm_mat4_mulv( invProj, clipRay, viewRay );
	viewRay[2] = -1.0f;
	viewRay[3] =  0.0f;  // direction, not position

	// World space
	mat4 invView;
	glm_mat4_inv( view, invView );
	vec4 worldRay;
	glm_mat4_mulv( invView, viewRay, worldRay );

	dir[0] = worldRay[0];
	dir[1] = worldRay[1];
	dir[2] = worldRay[2];
	// Normalise
	float len = sqrtf( dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2] );
	if ( len > 1e-6f ) { dir[0] /= len; dir[1] /= len; dir[2] /= len; }

	// Camera world position = 4th column of the inverse view matrix
	origin[0] = invView[3][0];
	origin[1] = invView[3][1];
	origin[2] = invView[3][2];
}

/*
 * rayPlaneIntersect
 *   Returns true and fills hit[] with the intersection point if the ray
 *   hits the plane (defined by a normal and a point on the plane).
 *   Only positive t values (in front of the camera) are accepted.
 */
bool ModelViewport::rayPlaneIntersect( const float origin[3], const float dir[3],
                                       const float planeNorm[3], const float planePoint[3],
                                       float hit[3] ) const
{
	float denom = planeNorm[0]*dir[0] + planeNorm[1]*dir[1] + planeNorm[2]*dir[2];
	if ( fabsf( denom ) < 1e-6f ) return false;  // parallel

	float d = ( planeNorm[0] * ( planePoint[0] - origin[0] ) +
	            planeNorm[1] * ( planePoint[1] - origin[1] ) +
	            planeNorm[2] * ( planePoint[2] - origin[2] ) ) / denom;
	if ( d < 0.0f ) return false;  // behind camera

	hit[0] = origin[0] + dir[0] * d;
	hit[1] = origin[1] + dir[1] * d;
	hit[2] = origin[2] + dir[2] * d;
	return true;
}

/*
 * gizmoHitTest
 *   Returns the axis index (0=X, 1=Y, 2=Z) of the ring under the mouse,
 *   or -1 if none.  Uses a tolerance of ~12% of the ring radius.
 */
int ModelViewport::gizmoHitTest( int mx, int my ) const
{
	if ( !m_showGizmo ) return -1;

	float origin[3], dir[3];
	buildRayFromMouse( mx, my, origin, dir );

	const float *c   = m_gizmoCenter;
	const float  R   = m_gizmoRadius;
	const float  tol = R * 0.12f;   // pick tolerance ≈ 12% of ring radius

	// Plane normals for each ring
	static const float normals[3][3] = {
		{ 1.0f, 0.0f, 0.0f },   // X ring – YZ plane
		{ 0.0f, 1.0f, 0.0f },   // Y ring – XZ plane
		{ 0.0f, 0.0f, 1.0f }    // Z ring – XY plane
	};

	int   bestAxis = -1;
	float bestDist = 1e9f;

	for ( int axis = 0; axis < 3; ++axis ) {
		float hit[3];
		if ( !rayPlaneIntersect( origin, dir, normals[axis], c, hit ) ) continue;

		// Distance from hit to ring centre in the ring's plane
		float dx = hit[0] - c[0];
		float dy = hit[1] - c[1];
		float dz = hit[2] - c[2];
		float dist2D = 0.0f;
		switch ( axis ) {
		case 0: dist2D = sqrtf( dy*dy + dz*dz ); break;   // YZ
		case 1: dist2D = sqrtf( dx*dx + dz*dz ); break;   // XZ
		case 2: dist2D = sqrtf( dx*dx + dy*dy ); break;   // XY
		}

		float err = fabsf( dist2D - R );
		if ( err < tol && err < bestDist ) {
			bestDist = err;
			bestAxis = axis;
		}
	}

	return bestAxis;
}

void ModelViewport::setWireframeMode( bool enabled ) {
	m_wireframeMode = enabled;
	update();
}

void ModelViewport::setWireframeOverlay( bool enabled ) {
	m_wireframeOverlay = enabled;
	update();
}

void ModelViewport::setShowGrid( bool show ) {
	m_showGrid = show;
	update();
}

void ModelViewport::setShowGround( bool show ) {
	m_showGround = show;
	update();
}

void ModelViewport::setShowAxes( bool show ) {
	m_showAxes = show;
	update();
}

void ModelViewport::setShowBones( bool show ) {
	m_showBones = show;
	update();
}

void ModelViewport::setShowHitboxes( bool show ) {
	m_showHitboxes = show;
	update();
}

void ModelViewport::setShowAttachments( bool show ) {
	m_showAttachments = show;
	update();
}

void ModelViewport::setFlatShading( bool enabled ) {
	m_flatShading = enabled;
	if ( m_renderInstance ) {
		r_qt_set_flat_shading( m_renderInstance, enabled );
	}
	update();
}

// ═══════════════════════════════════════════════════════════════════════════
// OVERLAY RENDERING - Bones, Hitboxes, Attachments
// ═══════════════════════════════════════════════════════════════════════════

void ModelViewport::renderBoneSkeleton() {
	if ( !m_overlayInitialized || !m_renderInstance || !m_model || !m_model->header ) return;

	studiohdr_t *hdr = m_model->header;
	if ( hdr->numbones <= 0 ) return;

	mstudiobone_t *bones = (mstudiobone_t *)( m_model->data + hdr->boneindex );

	// Build the same model matrix as in paintGL
	mat4 modelMatrix;
	Math_Mat4_Identity( modelMatrix );

	float modelScale = 0.1f * m_meshScale;
	mat4 S = GLM_MAT4_IDENTITY_INIT;
	glm_scale( S, (vec3){ modelScale, modelScale, modelScale } );
	glm_mat4_mul( S, modelMatrix, modelMatrix );

	if ( m_mirrorX || m_mirrorY || m_mirrorZ ) {
		mat4 M = GLM_MAT4_IDENTITY_INIT;
		glm_scale( M, (vec3){ m_mirrorX ? -1.0f : 1.0f, m_mirrorY ? -1.0f : 1.0f, m_mirrorZ ? -1.0f : 1.0f } );
		glm_mat4_mul( M, modelMatrix, modelMatrix );
	}

	float groundOffset = 0.0f;
	if ( m_model->header->numseq > 0 ) {
		mstudioseqdesc_t *sequences = (mstudioseqdesc_t *)( m_model->data + m_model->header->seqindex );
		groundOffset = -sequences[0].bbmin[2] * modelScale;
	}

	if ( !m_weaponOriginMode ) {
		mat4 RyFace = GLM_MAT4_IDENTITY_INIT;
		glm_rotate( RyFace, -GLM_PI * 0.5f, (vec3){ 0, 1, 0 } );
		glm_mat4_mul( RyFace, modelMatrix, modelMatrix );

		mat4 T = GLM_MAT4_IDENTITY_INIT;
		glm_translate( T, (vec3){ 0.0f, groundOffset, 0.0f } );
		glm_mat4_mul( T, modelMatrix, modelMatrix );
	} else {
		mat4 RyWeapon = GLM_MAT4_IDENTITY_INIT;
		glm_rotate( RyWeapon, GLM_PI, (vec3){ 0, 1, 0 } );
		glm_mat4_mul( RyWeapon, modelMatrix, modelMatrix );

		mat4 TWeapon = GLM_MAT4_IDENTITY_INIT;
		glm_translate( TWeapon, (vec3){ 0.0f, 6.4f, 0.0f } );
		glm_mat4_mul( TWeapon, modelMatrix, modelMatrix );
	}

	if ( m_modelRotationX != 0.0f || m_modelRotationY != 0.0f || m_modelRotationZ != 0.0f ) {
		mat4 R = GLM_MAT4_IDENTITY_INIT;
		glm_rotate( R, m_modelRotationY, (vec3){ 0.0f, 1.0f, 0.0f } );
		glm_rotate( R, m_modelRotationX, (vec3){ 1.0f, 0.0f, 0.0f } );
		glm_rotate( R, m_modelRotationZ, (vec3){ 0.0f, 0.0f, 1.0f } );
		glm_mat4_mul( R, modelMatrix, modelMatrix );
	}

	mat4 TOffset = GLM_MAT4_IDENTITY_INIT;
	glm_translate( TOffset, (vec3){ m_modelOffset[0], m_modelOffset[1], m_modelOffset[2] } );
	glm_mat4_mul( TOffset, modelMatrix, modelMatrix );

	// Use modern OpenGL with shader
	glDisable( GL_DEPTH_TEST );
	glUseProgram( m_overlayShader );

	// Set uniforms
	if ( m_overlayModelLoc != -1 ) {
		glUniformMatrix4fv( m_overlayModelLoc, 1, GL_FALSE, (const float *)modelMatrix );
	}
	if ( m_overlayViewLoc != -1 ) {
		glUniformMatrix4fv( m_overlayViewLoc, 1, GL_FALSE, (const float *)m_viewMatrix );
	}
	if ( m_overlayProjLoc != -1 ) {
		glUniformMatrix4fv( m_overlayProjLoc, 1, GL_FALSE, (const float *)m_projMatrix );
	}

	// Build bone line vertex data (position + color)
	// Each vertex: x, y, z, r, g, b
	float lineVertices[MAXSTUDIOBONES * 2 * 6];  // Max bones * 2 verts per line * 6 floats
	int numLineVerts = 0;

	for ( int i = 0; i < hdr->numbones; i++ ) {
		if ( bones[i].parent >= 0 && bones[i].parent < hdr->numbones ) {
			mat4 *boneMatrix = &m_renderInstance->bone_transformations[i];
			mat4 *parentMatrix = &m_renderInstance->bone_transformations[bones[i].parent];

			// Parent vertex (yellow)
			lineVertices[numLineVerts * 6 + 0] = ( *parentMatrix )[3][0];
			lineVertices[numLineVerts * 6 + 1] = ( *parentMatrix )[3][1];
			lineVertices[numLineVerts * 6 + 2] = ( *parentMatrix )[3][2];
			lineVertices[numLineVerts * 6 + 3] = 1.0f;  // R
			lineVertices[numLineVerts * 6 + 4] = 1.0f;  // G
			lineVertices[numLineVerts * 6 + 5] = 0.0f;  // B
			numLineVerts++;

			// Child vertex (yellow)
			lineVertices[numLineVerts * 6 + 0] = ( *boneMatrix )[3][0];
			lineVertices[numLineVerts * 6 + 1] = ( *boneMatrix )[3][1];
			lineVertices[numLineVerts * 6 + 2] = ( *boneMatrix )[3][2];
			lineVertices[numLineVerts * 6 + 3] = 1.0f;  // R
			lineVertices[numLineVerts * 6 + 4] = 1.0f;  // G
			lineVertices[numLineVerts * 6 + 5] = 0.0f;  // B
			numLineVerts++;
		}
	}

	// Upload and draw bone lines
	if ( numLineVerts > 0 ) {
		glBindVertexArray( m_boneLineVAO );
		glBindBuffer( GL_ARRAY_BUFFER, m_boneLineVBO );
		glBufferSubData( GL_ARRAY_BUFFER, 0, numLineVerts * 6 * sizeof( float ), lineVertices );

		glLineWidth( 2.0f * m_boneScale );
		glDrawArrays( GL_LINES, 0, numLineVerts );
	}

	// Build bone point vertex data (joints)
	float pointVertices[MAXSTUDIOBONES * 6];  // Max bones * 6 floats per vertex
	int numPointVerts = 0;

	for ( int i = 0; i < hdr->numbones; i++ ) {
		mat4 *boneMatrix = &m_renderInstance->bone_transformations[i];

		pointVertices[numPointVerts * 6 + 0] = ( *boneMatrix )[3][0];
		pointVertices[numPointVerts * 6 + 1] = ( *boneMatrix )[3][1];
		pointVertices[numPointVerts * 6 + 2] = ( *boneMatrix )[3][2];
		pointVertices[numPointVerts * 6 + 3] = 1.0f;  // R (red joints)
		pointVertices[numPointVerts * 6 + 4] = 0.0f;  // G
		pointVertices[numPointVerts * 6 + 5] = 0.0f;  // B
		numPointVerts++;
	}

	// Upload and draw bone points
	if ( numPointVerts > 0 ) {
		glBindVertexArray( m_bonePointVAO );
		glBindBuffer( GL_ARRAY_BUFFER, m_bonePointVBO );
		glBufferSubData( GL_ARRAY_BUFFER, 0, numPointVerts * 6 * sizeof( float ), pointVertices );

		// Enable shader-controlled point size and set via uniform
		glEnable( GL_PROGRAM_POINT_SIZE );
		if ( m_overlayPointSizeLoc != -1 ) {
			glUniform1f( m_overlayPointSizeLoc, 6.0f * m_boneScale );
		}
		glDrawArrays( GL_POINTS, 0, numPointVerts );
		glDisable( GL_PROGRAM_POINT_SIZE );
	}

	// Restore state
	glBindVertexArray( 0 );
	glEnable( GL_DEPTH_TEST );
}

void ModelViewport::drawWireframeBox( const float boneMatrix[3][4], const float bbmin[3], const float bbmax[3] ) {
	// Transform 8 corners of the box by the bone matrix
	float corners[8][3];
	int idx = 0;

	for ( int iz = 0; iz < 2; iz++ ) {
		for ( int iy = 0; iy < 2; iy++ ) {
			for ( int ix = 0; ix < 2; ix++ ) {
				float local[3] = {
					ix ? bbmax[0] : bbmin[0],
					iy ? bbmax[1] : bbmin[1],
					iz ? bbmax[2] : bbmin[2]
				};

				// Transform by bone matrix (3x4 affine matrix)
				corners[idx][0] = boneMatrix[0][0] * local[0] + boneMatrix[0][1] * local[1] +
								  boneMatrix[0][2] * local[2] + boneMatrix[0][3];
				corners[idx][1] = boneMatrix[1][0] * local[0] + boneMatrix[1][1] * local[1] +
								  boneMatrix[1][2] * local[2] + boneMatrix[1][3];
				corners[idx][2] = boneMatrix[2][0] * local[0] + boneMatrix[2][1] * local[1] +
								  boneMatrix[2][2] * local[2] + boneMatrix[2][3];
				idx++;
			}
		}
	}

	// Draw 12 edges of the box
	// Bottom face (z=0): 0-1, 1-3, 3-2, 2-0
	// Top face (z=1): 4-5, 5-7, 7-6, 6-4
	// Vertical edges: 0-4, 1-5, 2-6, 3-7
	static const int edges[12][2] = {
		{ 0, 1 }, { 1, 3 }, { 3, 2 }, { 2, 0 }, // Bottom
		{ 4, 5 }, { 5, 7 }, { 7, 6 }, { 6, 4 }, // Top
		{ 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 }  // Vertical
	};

	glBegin( GL_LINES );
	for ( int e = 0; e < 12; e++ ) {
		glVertex3fv( corners[edges[e][0]] );
		glVertex3fv( corners[edges[e][1]] );
	}
	glEnd();
}

void ModelViewport::renderHitboxes() {
	if ( !m_overlayInitialized || !m_renderInstance || !m_model || !m_model->header ) return;

	studiohdr_t *hdr = m_model->header;
	if ( hdr->numhitboxes <= 0 ) return;

	mstudiobbox_t *hitboxes = (mstudiobbox_t *)( m_model->data + hdr->hitboxindex );

	// Build the same model matrix as in paintGL
	mat4 modelMatrix;
	Math_Mat4_Identity( modelMatrix );

	float modelScale = 0.1f * m_meshScale;
	mat4 S = GLM_MAT4_IDENTITY_INIT;
	glm_scale( S, (vec3){ modelScale, modelScale, modelScale } );
	glm_mat4_mul( S, modelMatrix, modelMatrix );

	if ( m_mirrorX || m_mirrorY || m_mirrorZ ) {
		mat4 M = GLM_MAT4_IDENTITY_INIT;
		glm_scale( M, (vec3){ m_mirrorX ? -1.0f : 1.0f, m_mirrorY ? -1.0f : 1.0f, m_mirrorZ ? -1.0f : 1.0f } );
		glm_mat4_mul( M, modelMatrix, modelMatrix );
	}

	float groundOffset = 0.0f;
	if ( m_model->header->numseq > 0 ) {
		mstudioseqdesc_t *sequences = (mstudioseqdesc_t *)( m_model->data + m_model->header->seqindex );
		groundOffset = -sequences[0].bbmin[2] * modelScale;
	}

	if ( !m_weaponOriginMode ) {
		mat4 RyFace = GLM_MAT4_IDENTITY_INIT;
		glm_rotate( RyFace, -GLM_PI * 0.5f, (vec3){ 0, 1, 0 } );
		glm_mat4_mul( RyFace, modelMatrix, modelMatrix );

		mat4 T = GLM_MAT4_IDENTITY_INIT;
		glm_translate( T, (vec3){ 0.0f, groundOffset, 0.0f } );
		glm_mat4_mul( T, modelMatrix, modelMatrix );
	} else {
		mat4 RyWeapon = GLM_MAT4_IDENTITY_INIT;
		glm_rotate( RyWeapon, GLM_PI, (vec3){ 0, 1, 0 } );
		glm_mat4_mul( RyWeapon, modelMatrix, modelMatrix );

		mat4 TWeapon = GLM_MAT4_IDENTITY_INIT;
		glm_translate( TWeapon, (vec3){ 0.0f, 6.4f, 0.0f } );
		glm_mat4_mul( TWeapon, modelMatrix, modelMatrix );
	}

	if ( m_modelRotationX != 0.0f || m_modelRotationY != 0.0f || m_modelRotationZ != 0.0f ) {
		mat4 R = GLM_MAT4_IDENTITY_INIT;
		glm_rotate( R, m_modelRotationY, (vec3){ 0.0f, 1.0f, 0.0f } );
		glm_rotate( R, m_modelRotationX, (vec3){ 1.0f, 0.0f, 0.0f } );
		glm_rotate( R, m_modelRotationZ, (vec3){ 0.0f, 0.0f, 1.0f } );
		glm_mat4_mul( R, modelMatrix, modelMatrix );
	}

	mat4 TOffset = GLM_MAT4_IDENTITY_INIT;
	glm_translate( TOffset, (vec3){ m_modelOffset[0], m_modelOffset[1], m_modelOffset[2] } );
	glm_mat4_mul( TOffset, modelMatrix, modelMatrix );

	// Use modern OpenGL with shader
	glDisable( GL_DEPTH_TEST );
	glUseProgram( m_overlayShader );

	// Set uniforms
	if ( m_overlayModelLoc != -1 ) {
		glUniformMatrix4fv( m_overlayModelLoc, 1, GL_FALSE, (const float *)modelMatrix );
	}
	if ( m_overlayViewLoc != -1 ) {
		glUniformMatrix4fv( m_overlayViewLoc, 1, GL_FALSE, (const float *)m_viewMatrix );
	}
	if ( m_overlayProjLoc != -1 ) {
		glUniformMatrix4fv( m_overlayProjLoc, 1, GL_FALSE, (const float *)m_projMatrix );
	}

	// Build hitbox line vertex data
	// Each hitbox has 12 edges * 2 verts * 6 floats = 144 floats
	// Max 128 hitboxes = 18432 floats
	static const int edges[12][2] = {
		{ 0, 1 }, { 1, 3 }, { 3, 2 }, { 2, 0 }, // Bottom
		{ 4, 5 }, { 5, 7 }, { 7, 6 }, { 6, 4 }, // Top
		{ 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 }  // Vertical
	};

	float *hitboxVertices = new float[hdr->numhitboxes * 24 * 6];  // 12 edges * 2 verts * 6 floats
	int numHitboxVerts = 0;

	for ( int i = 0; i < hdr->numhitboxes; i++ ) {
		mstudiobbox_t *box = &hitboxes[i];

		if ( box->bone < 0 || box->bone >= hdr->numbones ) continue;

		mat4 *boneMat4 = &m_renderInstance->bone_transformations[box->bone];

		// Compute 8 corners of the hitbox
		float corners[8][3];
		int idx = 0;
		for ( int iz = 0; iz < 2; iz++ ) {
			for ( int iy = 0; iy < 2; iy++ ) {
				for ( int ix = 0; ix < 2; ix++ ) {
					float local[3] = {
						ix ? box->bbmax[0] : box->bbmin[0],
						iy ? box->bbmax[1] : box->bbmin[1],
						iz ? box->bbmax[2] : box->bbmin[2]
					};

					// Transform by bone matrix (column-major mat4)
					corners[idx][0] = ( *boneMat4 )[0][0] * local[0] + ( *boneMat4 )[1][0] * local[1] +
									  ( *boneMat4 )[2][0] * local[2] + ( *boneMat4 )[3][0];
					corners[idx][1] = ( *boneMat4 )[0][1] * local[0] + ( *boneMat4 )[1][1] * local[1] +
									  ( *boneMat4 )[2][1] * local[2] + ( *boneMat4 )[3][1];
					corners[idx][2] = ( *boneMat4 )[0][2] * local[0] + ( *boneMat4 )[1][2] * local[1] +
									  ( *boneMat4 )[2][2] * local[2] + ( *boneMat4 )[3][2];
					idx++;
				}
			}
		}

		// Add 12 edges (24 vertices)
		for ( int e = 0; e < 12; e++ ) {
			// First vertex of edge (green)
			hitboxVertices[numHitboxVerts * 6 + 0] = corners[edges[e][0]][0];
			hitboxVertices[numHitboxVerts * 6 + 1] = corners[edges[e][0]][1];
			hitboxVertices[numHitboxVerts * 6 + 2] = corners[edges[e][0]][2];
			hitboxVertices[numHitboxVerts * 6 + 3] = 0.0f;  // R
			hitboxVertices[numHitboxVerts * 6 + 4] = 1.0f;  // G (green)
			hitboxVertices[numHitboxVerts * 6 + 5] = 0.0f;  // B
			numHitboxVerts++;

			// Second vertex of edge (green)
			hitboxVertices[numHitboxVerts * 6 + 0] = corners[edges[e][1]][0];
			hitboxVertices[numHitboxVerts * 6 + 1] = corners[edges[e][1]][1];
			hitboxVertices[numHitboxVerts * 6 + 2] = corners[edges[e][1]][2];
			hitboxVertices[numHitboxVerts * 6 + 3] = 0.0f;  // R
			hitboxVertices[numHitboxVerts * 6 + 4] = 1.0f;  // G (green)
			hitboxVertices[numHitboxVerts * 6 + 5] = 0.0f;  // B
			numHitboxVerts++;
		}
	}

	// Upload and draw hitbox lines
	if ( numHitboxVerts > 0 ) {
		glBindVertexArray( m_hitboxVAO );
		glBindBuffer( GL_ARRAY_BUFFER, m_hitboxVBO );
		glBufferSubData( GL_ARRAY_BUFFER, 0, numHitboxVerts * 6 * sizeof( float ), hitboxVertices );

		glLineWidth( 1.0f );
		glDrawArrays( GL_LINES, 0, numHitboxVerts );
	}

	delete[] hitboxVertices;

	// Restore state
	glBindVertexArray( 0 );
	glEnable( GL_DEPTH_TEST );
}

void ModelViewport::renderAttachmentPoints() {
	if ( !m_overlayInitialized || !m_renderInstance || !m_model || !m_model->header ) return;

	studiohdr_t *hdr = m_model->header;
	if ( hdr->numattachments <= 0 ) return;

	mstudioattachment_t *attachments = (mstudioattachment_t *)( m_model->data + hdr->attachmentindex );

	// Build the same model matrix as in paintGL
	mat4 modelMatrix;
	Math_Mat4_Identity( modelMatrix );

	float modelScale = 0.1f * m_meshScale;
	mat4 S = GLM_MAT4_IDENTITY_INIT;
	glm_scale( S, (vec3){ modelScale, modelScale, modelScale } );
	glm_mat4_mul( S, modelMatrix, modelMatrix );

	if ( m_mirrorX || m_mirrorY || m_mirrorZ ) {
		mat4 M = GLM_MAT4_IDENTITY_INIT;
		glm_scale( M, (vec3){ m_mirrorX ? -1.0f : 1.0f, m_mirrorY ? -1.0f : 1.0f, m_mirrorZ ? -1.0f : 1.0f } );
		glm_mat4_mul( M, modelMatrix, modelMatrix );
	}

	float groundOffset = 0.0f;
	if ( m_model->header->numseq > 0 ) {
		mstudioseqdesc_t *sequences = (mstudioseqdesc_t *)( m_model->data + m_model->header->seqindex );
		groundOffset = -sequences[0].bbmin[2] * modelScale;
	}

	if ( !m_weaponOriginMode ) {
		mat4 RyFace = GLM_MAT4_IDENTITY_INIT;
		glm_rotate( RyFace, -GLM_PI * 0.5f, (vec3){ 0, 1, 0 } );
		glm_mat4_mul( RyFace, modelMatrix, modelMatrix );

		mat4 T = GLM_MAT4_IDENTITY_INIT;
		glm_translate( T, (vec3){ 0.0f, groundOffset, 0.0f } );
		glm_mat4_mul( T, modelMatrix, modelMatrix );
	} else {
		mat4 RyWeapon = GLM_MAT4_IDENTITY_INIT;
		glm_rotate( RyWeapon, GLM_PI, (vec3){ 0, 1, 0 } );
		glm_mat4_mul( RyWeapon, modelMatrix, modelMatrix );

		mat4 TWeapon = GLM_MAT4_IDENTITY_INIT;
		glm_translate( TWeapon, (vec3){ 0.0f, 6.4f, 0.0f } );
		glm_mat4_mul( TWeapon, modelMatrix, modelMatrix );
	}

	if ( m_modelRotationX != 0.0f || m_modelRotationY != 0.0f || m_modelRotationZ != 0.0f ) {
		mat4 R = GLM_MAT4_IDENTITY_INIT;
		glm_rotate( R, m_modelRotationY, (vec3){ 0.0f, 1.0f, 0.0f } );
		glm_rotate( R, m_modelRotationX, (vec3){ 1.0f, 0.0f, 0.0f } );
		glm_rotate( R, m_modelRotationZ, (vec3){ 0.0f, 0.0f, 1.0f } );
		glm_mat4_mul( R, modelMatrix, modelMatrix );
	}

	mat4 TOffset = GLM_MAT4_IDENTITY_INIT;
	glm_translate( TOffset, (vec3){ m_modelOffset[0], m_modelOffset[1], m_modelOffset[2] } );
	glm_mat4_mul( TOffset, modelMatrix, modelMatrix );

	// Use modern OpenGL with shader
	glDisable( GL_DEPTH_TEST );
	glUseProgram( m_overlayShader );

	// Set uniforms
	if ( m_overlayModelLoc != -1 ) {
		glUniformMatrix4fv( m_overlayModelLoc, 1, GL_FALSE, (const float *)modelMatrix );
	}
	if ( m_overlayViewLoc != -1 ) {
		glUniformMatrix4fv( m_overlayViewLoc, 1, GL_FALSE, (const float *)m_viewMatrix );
	}
	if ( m_overlayProjLoc != -1 ) {
		glUniformMatrix4fv( m_overlayProjLoc, 1, GL_FALSE, (const float *)m_projMatrix );
	}

	// Build attachment point vertex data (position + color)
	float pointVertices[32 * 6];  // Max 32 attachments * 6 floats
	int numPointVerts = 0;

	for ( int i = 0; i < hdr->numattachments && numPointVerts < 32; i++ ) {
		mstudioattachment_t *att = &attachments[i];
		if ( att->bone < 0 || att->bone >= hdr->numbones ) continue;

		mat4 *boneMat4 = &m_renderInstance->bone_transformations[att->bone];

		// Transform attachment origin by bone matrix
		float worldPos[3];
		worldPos[0] = ( *boneMat4 )[0][0] * att->org[0] + ( *boneMat4 )[1][0] * att->org[1] +
					  ( *boneMat4 )[2][0] * att->org[2] + ( *boneMat4 )[3][0];
		worldPos[1] = ( *boneMat4 )[0][1] * att->org[0] + ( *boneMat4 )[1][1] * att->org[1] +
					  ( *boneMat4 )[2][1] * att->org[2] + ( *boneMat4 )[3][1];
		worldPos[2] = ( *boneMat4 )[0][2] * att->org[0] + ( *boneMat4 )[1][2] * att->org[1] +
					  ( *boneMat4 )[2][2] * att->org[2] + ( *boneMat4 )[3][2];

		// Orange point
		pointVertices[numPointVerts * 6 + 0] = worldPos[0];
		pointVertices[numPointVerts * 6 + 1] = worldPos[1];
		pointVertices[numPointVerts * 6 + 2] = worldPos[2];
		pointVertices[numPointVerts * 6 + 3] = 1.0f;   // R (orange)
		pointVertices[numPointVerts * 6 + 4] = 0.5f;   // G
		pointVertices[numPointVerts * 6 + 5] = 0.0f;   // B
		numPointVerts++;
	}

	// Draw attachment points
	if ( numPointVerts > 0 ) {
		glBindVertexArray( m_attachmentPointVAO );
		glBindBuffer( GL_ARRAY_BUFFER, m_attachmentPointVBO );
		glBufferSubData( GL_ARRAY_BUFFER, 0, numPointVerts * 6 * sizeof( float ), pointVertices );

		if ( m_overlayPointSizeLoc != -1 ) {
			glUniform1f( m_overlayPointSizeLoc, 10.0f );
		}
		glEnable( GL_PROGRAM_POINT_SIZE );
		glDrawArrays( GL_POINTS, 0, numPointVerts );
		glDisable( GL_PROGRAM_POINT_SIZE );
	}

	// Build attachment axes line data
	float axisLen = 5.0f;
	float lineVertices[32 * 6 * 2 * 6];  // Max 32 attachments * 3 axes * 2 verts * 6 floats
	int numLineVerts = 0;

	for ( int i = 0; i < hdr->numattachments && i < 32; i++ ) {
		mstudioattachment_t *att = &attachments[i];
		if ( att->bone < 0 || att->bone >= hdr->numbones ) continue;

		mat4 *boneMat4 = &m_renderInstance->bone_transformations[att->bone];

		float worldPos[3];
		worldPos[0] = ( *boneMat4 )[0][0] * att->org[0] + ( *boneMat4 )[1][0] * att->org[1] +
					  ( *boneMat4 )[2][0] * att->org[2] + ( *boneMat4 )[3][0];
		worldPos[1] = ( *boneMat4 )[0][1] * att->org[0] + ( *boneMat4 )[1][1] * att->org[1] +
					  ( *boneMat4 )[2][1] * att->org[2] + ( *boneMat4 )[3][1];
		worldPos[2] = ( *boneMat4 )[0][2] * att->org[0] + ( *boneMat4 )[1][2] * att->org[1] +
					  ( *boneMat4 )[2][2] * att->org[2] + ( *boneMat4 )[3][2];

		// X axis (red)
		lineVertices[numLineVerts * 6 + 0] = worldPos[0];
		lineVertices[numLineVerts * 6 + 1] = worldPos[1];
		lineVertices[numLineVerts * 6 + 2] = worldPos[2];
		lineVertices[numLineVerts * 6 + 3] = 1.0f; lineVertices[numLineVerts * 6 + 4] = 0.0f; lineVertices[numLineVerts * 6 + 5] = 0.0f;
		numLineVerts++;
		lineVertices[numLineVerts * 6 + 0] = worldPos[0] + axisLen;
		lineVertices[numLineVerts * 6 + 1] = worldPos[1];
		lineVertices[numLineVerts * 6 + 2] = worldPos[2];
		lineVertices[numLineVerts * 6 + 3] = 1.0f; lineVertices[numLineVerts * 6 + 4] = 0.0f; lineVertices[numLineVerts * 6 + 5] = 0.0f;
		numLineVerts++;

		// Y axis (green)
		lineVertices[numLineVerts * 6 + 0] = worldPos[0];
		lineVertices[numLineVerts * 6 + 1] = worldPos[1];
		lineVertices[numLineVerts * 6 + 2] = worldPos[2];
		lineVertices[numLineVerts * 6 + 3] = 0.0f; lineVertices[numLineVerts * 6 + 4] = 1.0f; lineVertices[numLineVerts * 6 + 5] = 0.0f;
		numLineVerts++;
		lineVertices[numLineVerts * 6 + 0] = worldPos[0];
		lineVertices[numLineVerts * 6 + 1] = worldPos[1] + axisLen;
		lineVertices[numLineVerts * 6 + 2] = worldPos[2];
		lineVertices[numLineVerts * 6 + 3] = 0.0f; lineVertices[numLineVerts * 6 + 4] = 1.0f; lineVertices[numLineVerts * 6 + 5] = 0.0f;
		numLineVerts++;

		// Z axis (blue)
		lineVertices[numLineVerts * 6 + 0] = worldPos[0];
		lineVertices[numLineVerts * 6 + 1] = worldPos[1];
		lineVertices[numLineVerts * 6 + 2] = worldPos[2];
		lineVertices[numLineVerts * 6 + 3] = 0.0f; lineVertices[numLineVerts * 6 + 4] = 0.0f; lineVertices[numLineVerts * 6 + 5] = 1.0f;
		numLineVerts++;
		lineVertices[numLineVerts * 6 + 0] = worldPos[0];
		lineVertices[numLineVerts * 6 + 1] = worldPos[1];
		lineVertices[numLineVerts * 6 + 2] = worldPos[2] + axisLen;
		lineVertices[numLineVerts * 6 + 3] = 0.0f; lineVertices[numLineVerts * 6 + 4] = 0.0f; lineVertices[numLineVerts * 6 + 5] = 1.0f;
		numLineVerts++;
	}

	// Draw attachment axes
	if ( numLineVerts > 0 ) {
		glBindVertexArray( m_attachmentLineVAO );
		glBindBuffer( GL_ARRAY_BUFFER, m_attachmentLineVBO );
		glBufferSubData( GL_ARRAY_BUFFER, 0, numLineVerts * 6 * sizeof( float ), lineVertices );

		glLineWidth( 2.0f );
		glDrawArrays( GL_LINES, 0, numLineVerts );
	}

	// Restore state
	glBindVertexArray( 0 );
	glEnable( GL_DEPTH_TEST );
}

// ═══════════════════════════════════════════════════════════════════════════
// OVERLAY RENDERING SYSTEM (Modern OpenGL for bones, hitboxes, etc.)
// ═══════════════════════════════════════════════════════════════════════════

void ModelViewport::initOverlayRendering() {
	// Load the axes shader (position + color)
	char *vert_source = read_shader_source( "axes.vert" );
	char *frag_source = read_shader_source( "axes.frag" );

	if ( !vert_source || !frag_source ) {
		qWarning() << "Failed to load overlay shaders";
		if ( vert_source ) free( vert_source );
		if ( frag_source ) free( frag_source );
		return;
	}

	GLuint vert_shader = compile_shader( vert_source, GL_VERTEX_SHADER );
	GLuint frag_shader = compile_shader( frag_source, GL_FRAGMENT_SHADER );
	m_overlayShader = create_shader_program( vert_shader, frag_shader );

	free( vert_source );
	free( frag_source );

	if ( m_overlayShader == 0 ) {
		qWarning() << "Failed to create overlay shader program";
		return;
	}

	// Get uniform locations
	m_overlayModelLoc = glGetUniformLocation( m_overlayShader, "model" );
	m_overlayViewLoc = glGetUniformLocation( m_overlayShader, "view" );
	m_overlayProjLoc = glGetUniformLocation( m_overlayShader, "projection" );
	m_overlayPointSizeLoc = glGetUniformLocation( m_overlayShader, "u_pointSize" );

	// Create VAO/VBO for bone lines
	// Max bones = 128, each bone can have a line to parent = 128 lines * 2 verts * 6 floats
	glGenVertexArrays( 1, &m_boneLineVAO );
	glBindVertexArray( m_boneLineVAO );

	glGenBuffers( 1, &m_boneLineVBO );
	glBindBuffer( GL_ARRAY_BUFFER, m_boneLineVBO );
	glBufferData( GL_ARRAY_BUFFER, MAXSTUDIOBONES * 2 * 6 * sizeof( float ), nullptr, GL_DYNAMIC_DRAW );

	// Position attribute (location 0)
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( float ), (void *)0 );
	glEnableVertexAttribArray( 0 );
	// Color attribute (location 1)
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( float ), (void *)( 3 * sizeof( float ) ) );
	glEnableVertexAttribArray( 1 );

	glBindVertexArray( 0 );

	// Create VAO/VBO for bone points (joints)
	glGenVertexArrays( 1, &m_bonePointVAO );
	glBindVertexArray( m_bonePointVAO );

	glGenBuffers( 1, &m_bonePointVBO );
	glBindBuffer( GL_ARRAY_BUFFER, m_bonePointVBO );
	glBufferData( GL_ARRAY_BUFFER, MAXSTUDIOBONES * 6 * sizeof( float ), nullptr, GL_DYNAMIC_DRAW );

	// Position attribute (location 0)
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( float ), (void *)0 );
	glEnableVertexAttribArray( 0 );
	// Color attribute (location 1)
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( float ), (void *)( 3 * sizeof( float ) ) );
	glEnableVertexAttribArray( 1 );

	glBindVertexArray( 0 );

	// Create VAO/VBO for hitboxes (max 128 hitboxes * 24 lines * 2 verts)
	glGenVertexArrays( 1, &m_hitboxVAO );
	glBindVertexArray( m_hitboxVAO );

	glGenBuffers( 1, &m_hitboxVBO );
	glBindBuffer( GL_ARRAY_BUFFER, m_hitboxVBO );
	glBufferData( GL_ARRAY_BUFFER, 128 * 24 * 2 * 6 * sizeof( float ), nullptr, GL_DYNAMIC_DRAW );

	// Position attribute (location 0)
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( float ), (void *)0 );
	glEnableVertexAttribArray( 0 );
	// Color attribute (location 1)
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( float ), (void *)( 3 * sizeof( float ) ) );
	glEnableVertexAttribArray( 1 );

	glBindVertexArray( 0 );

	// Create VAO/VBO for attachment points (max 32 attachments)
	glGenVertexArrays( 1, &m_attachmentPointVAO );
	glBindVertexArray( m_attachmentPointVAO );

	glGenBuffers( 1, &m_attachmentPointVBO );
	glBindBuffer( GL_ARRAY_BUFFER, m_attachmentPointVBO );
	glBufferData( GL_ARRAY_BUFFER, 32 * 6 * sizeof( float ), nullptr, GL_DYNAMIC_DRAW );

	// Position attribute (location 0)
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( float ), (void *)0 );
	glEnableVertexAttribArray( 0 );
	// Color attribute (location 1)
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( float ), (void *)( 3 * sizeof( float ) ) );
	glEnableVertexAttribArray( 1 );

	glBindVertexArray( 0 );

	// Create VAO/VBO for attachment axes lines (max 32 attachments * 3 axes * 2 verts)
	glGenVertexArrays( 1, &m_attachmentLineVAO );
	glBindVertexArray( m_attachmentLineVAO );

	glGenBuffers( 1, &m_attachmentLineVBO );
	glBindBuffer( GL_ARRAY_BUFFER, m_attachmentLineVBO );
	glBufferData( GL_ARRAY_BUFFER, 32 * 6 * 2 * 6 * sizeof( float ), nullptr, GL_DYNAMIC_DRAW );

	// Position attribute (location 0)
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( float ), (void *)0 );
	glEnableVertexAttribArray( 0 );
	// Color attribute (location 1)
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( float ), (void *)( 3 * sizeof( float ) ) );
	glEnableVertexAttribArray( 1 );

	glBindVertexArray( 0 );

	m_overlayInitialized = true;
	qDebug() << "Overlay rendering initialized";
}

void ModelViewport::cleanupOverlayRendering() {
	if ( !m_overlayInitialized ) return;

	if ( m_boneLineVAO ) {
		glDeleteVertexArrays( 1, &m_boneLineVAO );
		m_boneLineVAO = 0;
	}
	if ( m_boneLineVBO ) {
		glDeleteBuffers( 1, &m_boneLineVBO );
		m_boneLineVBO = 0;
	}
	if ( m_bonePointVAO ) {
		glDeleteVertexArrays( 1, &m_bonePointVAO );
		m_bonePointVAO = 0;
	}
	if ( m_bonePointVBO ) {
		glDeleteBuffers( 1, &m_bonePointVBO );
		m_bonePointVBO = 0;
	}
	if ( m_hitboxVAO ) {
		glDeleteVertexArrays( 1, &m_hitboxVAO );
		m_hitboxVAO = 0;
	}
	if ( m_hitboxVBO ) {
		glDeleteBuffers( 1, &m_hitboxVBO );
		m_hitboxVBO = 0;
	}
	if ( m_attachmentPointVAO ) {
		glDeleteVertexArrays( 1, &m_attachmentPointVAO );
		m_attachmentPointVAO = 0;
	}
	if ( m_attachmentPointVBO ) {
		glDeleteBuffers( 1, &m_attachmentPointVBO );
		m_attachmentPointVBO = 0;
	}
	if ( m_attachmentLineVAO ) {
		glDeleteVertexArrays( 1, &m_attachmentLineVAO );
		m_attachmentLineVAO = 0;
	}
	if ( m_attachmentLineVBO ) {
		glDeleteBuffers( 1, &m_attachmentLineVBO );
		m_attachmentLineVBO = 0;
	}
	if ( m_overlayShader ) {
		glDeleteProgram( m_overlayShader );
		m_overlayShader = 0;
	}

	m_overlayInitialized = false;
}
