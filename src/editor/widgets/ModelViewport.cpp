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
	  m_animationTimer( nullptr ),
	  m_cameraPitch( 0.33f ),
	  m_cameraYaw( 0.0f ), // ~14° tilt down - matches CLI default
	  m_cameraDistance( 35.0f ), // Straight front view - model faces camera
	  m_cameraTarget{ 0.0f, 0.0f, 0.0f }, // ~25 units distance - shows full model
	  m_frameCount( 0 ),
	  m_lastFpsTime( 0 ),
	  m_currentFps( 0.0f ),
	  m_lastAnimTime( 0 ),
	  m_showGrid( true ),
	  m_wireframeMode( false ),
	  m_groundHeight( 0.0f ),
	  m_statusBar( nullptr ) {
	mdl_animation_init( &m_animState );

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
}

void ModelViewport::resizeGL( int width, int height ) {
	glViewport( 0, 0, width, height );

	float aspect = (float)width / (float)height;
	float fov = 50.0f;
	float nearPlane = 0.01f;
	float farPlane = 1000.0f;

	glm_perspective( glm_rad( fov ), aspect, nearPlane, farPlane, m_projMatrix );
}

void ModelViewport::paintGL() {
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

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
	math_vec3_t camPos;
	camPos[0] = m_cameraTarget[0] + offset[0];
	camPos[1] = m_cameraTarget[1] + offset[1];
	camPos[2] = m_cameraTarget[2] + offset[2];

	// Optional: Clamp camera Y above ground
	if ( camPos[1] < m_groundHeight + 0.5f ) {
		camPos[1] = m_groundHeight + 0.5f;
	}

	math_vec3_t target;
	Math_Vec3Copy( m_cameraTarget, target );

	math_vec3_t up = { 0.0f, 1.0f, 0.0f }; // World up

	// Create view matrix - camera looks at target from offset position
	Math_Mat4_LookAt( camPos, target, up, m_viewMatrix );

	// Draw grid and axes at Y=0 (ALWAYS when enabled, even without model)
	if ( m_showGrid ) {
		r_grid_draw( m_viewMatrix, m_projMatrix, 0.0f );
		r_axes_draw( m_viewMatrix, m_projMatrix, 0.0f );
	}

	// Build model matrix - Add proper transformations
	mat4 modelMatrix;
	Math_Mat4_Identity( modelMatrix );

	// CRITICAL: Apply transformations in REVERSE order (last operation first)
	// This matches CLI: Scale → Rotate → Translate

	// Step 1: Scale down (HL units are huge - 0.1x scale)
	float modelScale = 0.1f;
	mat4 S = GLM_MAT4_IDENTITY_INIT;
	glm_scale( S, (vec3){ modelScale, modelScale, modelScale } );
	glm_mat4_mul( S, modelMatrix, modelMatrix );

	// Step 2: Rotate model to face camera (blue Z axis)
	// Half-Life models face +Y after axis remap, need -90° Y rotation to face +Z
	mat4 RyFace = GLM_MAT4_IDENTITY_INIT;
	glm_rotate( RyFace, -GLM_PI * 0.5f, (vec3){ 0, 1, 0 } );
	glm_mat4_mul( RyFace, modelMatrix, modelMatrix );

	// Step 3: Ground alignment - translate model up so feet touch Y=0
	// Use sequence bounding box bbmin.z (HL Z = up, becomes GL Y)
	float groundOffset = 0.0f;
	if ( m_model && m_model->header && m_model->data && m_model->header->numseq > 0 ) {
		mstudioseqdesc_t *sequences = (mstudioseqdesc_t *)( m_model->data + m_model->header->seqindex );
		groundOffset = -sequences[0].bbmin[2] * modelScale; // Use sequence 0 bbox
	}
	mat4 T = GLM_MAT4_IDENTITY_INIT;
	glm_translate( T, (vec3){ 0.0f, groundOffset, 0.0f } );
	glm_mat4_mul( T, modelMatrix, modelMatrix );

	// Render the model if loaded
	if ( m_renderInstance && m_model && m_model->header && m_model->data ) {
		r_qt_render_with_matrices( m_renderInstance, m_viewMatrix, m_projMatrix, modelMatrix );
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
}

void ModelViewport::onAnimationTick() {
	// Only advance animation if playing and model loaded
	if ( m_animationPlaying && m_model && m_model->header ) {
		// Calculate delta time (timer fires every 16ms = 0.016 seconds)
        
        qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
        float deltaTime = ( m_lastAnimTime > 0 ) 
                            ? ( currentTime - m_lastAnimTime ) / 1000.0f
                            : 0.016f;
        m_lastAnimTime = currentTime;
        
        // TODO(karlo): clamping values so we dont have any huge jumps
        if ( deltaTime > 0.05f )
        {
            deltaTime = 0.05f;
        }
         
		// Get the current sequence to check FPS
		if ( m_animState.current_sequence >= 0 &&
			 m_animState.current_sequence < m_model->header->numseq ) {

			// Update animation frame (arg order: state, delta_time, header, data, seqgroups)
			mdl_animation_update( &m_animState, deltaTime, m_model->header, m_model->data, m_model->seqgroups );

			// Sync animation state to render instance for bone calculation
			if ( m_renderInstance ) {
				r_qt_set_animation_state( m_renderInstance, &m_animState );
				r_qt_set_animation_enabled( m_renderInstance, true );
			}

			// Emit frame changed signal for UI updates
			emit frameChanged( m_animState.current_frame );
		}
	}

	// Always redraw - needed for FPS counter, camera movement, and mouse tracking
	update();
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
	// @Note: here we are adding button mouse support

	// Storing the current mouse button that is pressed
	m_activeButton = event->button();

	// Storing the last mouse position
	m_lastMousePos = event->pos();

	event->accept();
}

void ModelViewport::mouseMoveEvent( QMouseEvent *event ) {
	// Get current mouse position and calculate delta
	QPoint currentPos = event->pos();
	QPoint delta = currentPos - m_lastMousePos;

	// HLAM-style controls: Left+Shift OR Middle button = PAN, Left alone = ORBIT
	bool isPanning = ( m_activeButton == Qt::LeftButton && ( event->modifiers() & Qt::ShiftModifier ) ) || ( m_activeButton == Qt::MiddleButton );

	if ( m_activeButton == Qt::LeftButton && !isPanning ) {
		// ORBIT MODE: Rotate camera around target (Left button without Shift)
		float sensitivity = 0.005f;

		m_cameraYaw += delta.x() * sensitivity;
		m_cameraPitch -= delta.y() * sensitivity;

		// Clamp pitch to prevent gimbal lock (~90 degrees = pi/2 radians)
		float maxPitch = 1.57f;
		if ( m_cameraPitch > maxPitch ) m_cameraPitch = maxPitch;
		if ( m_cameraPitch < -maxPitch ) m_cameraPitch = -maxPitch;
	} else if ( isPanning ) {
		// PAN MODE: Move camera target (Left+Shift OR Middle button)
		// This is CRITICAL - allows adjusting view height to frame models properly!
		float panSpeed = 0.01f;

		// Calculate camera-relative right vector (perpendicular to view direction)
		math_vec3_t right;
		right[0] = cosf( m_cameraYaw );
		right[1] = 0.0f;
		right[2] = -sinf( m_cameraYaw );

		// World up vector (Y-axis in OpenGL)
		math_vec3_t up;
		up[0] = 0.0f;
		up[1] = 1.0f;
		up[2] = 0.0f;

		// Horizontal drag (X) = move along right vector
		m_cameraTarget[0] -= right[0] * delta.x() * panSpeed;
		m_cameraTarget[1] -= right[1] * delta.x() * panSpeed;
		m_cameraTarget[2] -= right[2] * delta.x() * panSpeed;

		// Vertical drag (Y) = move along up vector (THIS IS KEY FOR HEIGHT ADJUSTMENT!)
		m_cameraTarget[0] += up[0] * delta.y() * panSpeed;
		m_cameraTarget[1] += up[1] * delta.y() * panSpeed;
		m_cameraTarget[2] += up[2] * delta.y() * panSpeed;
	}

	m_lastMousePos = currentPos;

	// Trigger repaint when camera changes
	update();
	event->accept();
}

void ModelViewport::mouseReleaseEvent( QMouseEvent *event ) {
	// We here release the button, we clear the active button that was being used/pressed

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

	// Trigger repaint when zoom changes
	update();

	event->accept();
}

void ModelViewport::setStatusBar( StatusBarWidget *statusBar ) {
	m_statusBar = statusBar;
}

void ModelViewport::enterEvent( QEnterEvent *event ) {
	// Mouse entered viewport - timer in MainWindow will handle updates
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
	// Base movement speed (units per second)
	float baseSpeed = 10.0f;
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

	// Calculate camera forward and right vectors based on yaw
	// Forward = direction camera is looking (horizontal plane only)
	math_vec3_t forward;
	forward[0] = sinf( m_cameraYaw );   // X
	forward[1] = 0.0f;                   // Y (no vertical in forward)
	forward[2] = cosf( m_cameraYaw );   // Z

	// Right = perpendicular to forward (90 degrees clockwise)
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

	// NOTE: Camera does NOT auto-adjust on model load
	// Camera stays at its default position, model appears at origin
	// User can manually adjust camera with mouse/keyboard

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

	// Calculate model dimensions
	float sizeX = bbmax[0] - bbmin[0];
	float sizeY = bbmax[1] - bbmin[1];
	float sizeZ = bbmax[2] - bbmin[2];

	// ============================================================================
	// GROUND POSITIONING: Place at model's actual lowest point
	// ============================================================================
	// Ground at model's lowest Z point (after coordinate transform to OpenGL Y)
	m_groundHeight = bbmin[2] * modelScale;

	// Camera target: Vertical center of bounding box
	// In HL coords: {0, 0, min.z + size.z/2}
	math_vec3_t hlTarget;
	hlTarget[0] = 0.0f;
	hlTarget[1] = 0.0f;
	hlTarget[2] = bbmin[2] + ( sizeZ / 2.0f ); // Vertical center

	// Transform to OpenGL coordinates (HL Z → OpenGL Y)
	m_cameraTarget[0] = hlTarget[1] * modelScale; // X = 0
	m_cameraTarget[1] = hlTarget[2] * modelScale; // Y = vertical center
	m_cameraTarget[2] = -hlTarget[0] * modelScale; // Z = 0

	// distance = max(dx, dy, dz) - largest bounding box dimension, NO multiplier!
	float maxDim = sizeX;
	if ( sizeY > maxDim ) maxDim = sizeY;
	if ( sizeZ > maxDim ) maxDim = sizeZ;

	// Apply ONLY the model scale (0.1), no additional padding/multipliers!
	m_cameraDistance = maxDim * modelScale;

	// Clamp to prevent extreme values
	if ( m_cameraDistance < 1.0f ) m_cameraDistance = 1.0f;
	if ( m_cameraDistance > 100.0f ) m_cameraDistance = 100.0f;

	m_cameraPitch = 0.45f; // ~17 degrees - look down at model
	m_cameraYaw = 0.785f; // 45 degrees - 3/4 view angle

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

void ModelViewport::resetCamera() {
	m_cameraPitch = 0.33f;
	m_cameraYaw = 0.0f;
	m_cameraDistance = 35.0f;
	m_cameraTarget[0] = 0.0f;
	m_cameraTarget[1] = 0.0f;
	m_cameraTarget[2] = 0.0f;
	update();
}

void ModelViewport::setWireframeMode( bool enabled ) {
	m_wireframeMode = enabled;
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
