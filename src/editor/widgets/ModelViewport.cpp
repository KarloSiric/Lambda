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
#include "math_vector.h"
#include "mdl_loader.h"
#include "r_draw.h"
#include "util_messages.h"
#include <QDebug>
#include <QtCore/qdebug.h>
#include <QtCore/qlogging.h>
#include <QtCore/qnamespace.h>
#include <QtCore/qpoint.h>
#include <QtCore/qstringview.h>
#include <cmath>

ModelViewport::ModelViewport( QWidget *parent )
	: QOpenGLWidget( parent ),
	  m_model( nullptr ),
	  m_renderInstance( nullptr ),
	  m_animationPlaying( false ),
	  m_animationTimer( nullptr ),
	  m_cameraPitch( 0.3f ),
	  m_cameraYaw( 0.5f ),
      m_cameraDistance( 50.0f ),
      m_cameraTarget{ 0.0f, 0.0f, 0.0f },
      m_showGrid( true ),
      m_wireframeMode( false ),
      m_groundHeight( 0.0f ) {

	mdl_animation_init( &m_animState );

	// Create the new animationTimer so 60 fps ~16.66 ms ~16 ms
	m_animationTimer = new QTimer( this );
	m_animationTimer->setInterval( 16 );

	connect( m_animationTimer, &QTimer::timeout, this, &ModelViewport::onAnimationTick );

	//  Look at the origin
	math_vec3_t target = { 0.0f, 0.0f, 0.0f };

	// 100 units away the distance
	Camera_Init( &m_camera, target, 50.0f );

    // Set camera angles for isometric view
    m_camera.angles_deg[0] = 17.2f;  // Pitch: look down 20 degrees
    m_camera.angles_deg[1] = 28.6f;  // Yaw: rotate 45 degrees
    m_camera.angles_deg[2] = 0.0f;   // Roll: no rotation

	qDebug() << "ModelViewport Created";
}

ModelViewport::~ModelViewport() {
	makeCurrent();

	if ( m_model ) {
		free_model( m_model );
		m_model = nullptr;
	}

	if ( m_renderInstance ) {
		r_qt_destroy_instance( m_renderInstance );
		m_renderInstance = nullptr;
	}

	doneCurrent();

	qDebug() << "ModelViewport Destroyed";
}

void ModelViewport::initializeGL( void ) {
	// Open OpenGL function pointers ( This is Qt's version of glewInit )

	initializeOpenGLFunctions();

	qDebug() << "OpenGL Initialized";
	qDebug() << "OpenGL Version: " << (const char *)glGetString( GL_VERSION );
	qDebug() << "GLSL Version: " << (const char *)glGetString( GL_SHADING_LANGUAGE_VERSION );
	qDebug() << "Renderer: " << (const char *)glGetString( GL_RENDERER );


    glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );

    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LESS );

    // Enable face culling for performance (cull back-facing triangles)
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glFrontFace( GL_CW );

    // Create per-viewport rendering instance
    m_renderInstance = r_qt_create_instance();
    if ( !m_renderInstance ) {
        qCritical() << "ERROR: Failed to create Qt rendering instance!";
    } else {
        qDebug() << "Qt rendering instance created successfully!";
    }

    r_grid_init( 1000.0f, 10.0f );
    r_ground_init( 1000.0f );
    r_axes_init( 100.0f );

    qDebug( ) << "Grid/Ground/Axes initialized!";

}

void ModelViewport::resizeGL( int width, int height ) {
	glViewport( 0, 0, width, height );

    float aspect = (float)width / (float)height;
    float fov = 50.0f;        // Match Lambda's FOV (was 60)
    float nearPlane = 0.01f;  // Match Lambda (was 1.0f)
    float farPlane = 1000.0f; // Match Lambda (was 10000.0f)

    glm_perspective( glm_rad( fov ), aspect, nearPlane, farPlane, m_projMatrix );

	qDebug() << "Viewport Resized:" << width << "x" << height;
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
    offset[0] = camDist * cosf(pitch) * sinf(yaw);   // X offset
    offset[1] = camDist * sinf(pitch);               // Y offset (vertical)
    offset[2] = camDist * cosf(pitch) * cosf(yaw);   // Z offset

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

    math_vec3_t up = { 0.0f, 1.0f, 0.0f };      // World up

    // Create view matrix - camera looks at target from offset position
    Math_Mat4_LookAt( camPos, target, up, m_viewMatrix );

    // Draw grid and axes at Y=0 (NO ground plane - grid only)
    if ( m_showGrid && m_model && m_model->header )
    {
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

    // Step 2: Rotate model 180° to face toward camera
    // After shader axis remap: HL +Y (forward) → GL -Z
    // Camera looks from +Z toward origin, so model needs to face +Z
    mat4 R = GLM_MAT4_IDENTITY_INIT;
    glm_rotate( R, MATH_PI, (vec3){ 0, 1, 0 } );  // 180° rotation
    glm_mat4_mul( R, modelMatrix, modelMatrix );

    // Step 3: Ground alignment - translate model up so feet touch Y=0
    // Use sequence bounding box bbmin.z (HL Z = up, becomes GL Y)
    float groundOffset = 0.0f;
    if (m_model && m_model->header && m_model->data && m_model->header->numseq > 0) {
        mstudioseqdesc_t* sequences = (mstudioseqdesc_t*)(m_model->data + m_model->header->seqindex);
        groundOffset = -sequences[0].bbmin[2] * modelScale;  // Use sequence 0 bbox
    }
    mat4 T = GLM_MAT4_IDENTITY_INIT;
    glm_translate( T, (vec3){ 0.0f, groundOffset, 0.0f } );
    glm_mat4_mul( T, modelMatrix, modelMatrix );

    // Render the model if loaded
    if ( m_renderInstance && m_model && m_model->header && m_model->data )
    {
        r_qt_render_with_matrices( m_renderInstance, m_viewMatrix, m_projMatrix, modelMatrix );
    }

}

void ModelViewport::onAnimationTick() {
}

void ModelViewport::mousePressEvent( QMouseEvent *event ) {
    
    // @Note: here we are adding button mouse support
    
    // Storing the current mouse button that is pressed
    m_activeButton = event->button( );
    
    // Storing the last mouse position
    m_lastMousePos = event->pos( );
    
    event->accept( ); 
    
}

void ModelViewport::mouseMoveEvent( QMouseEvent *event ) {
    // Get current mouse position and calculate delta
    QPoint currentPos = event->pos();
    QPoint delta = currentPos - m_lastMousePos;

    // HLAM-style controls: Left+Shift OR Middle button = PAN, Left alone = ORBIT
    bool isPanning = ( m_activeButton == Qt::LeftButton && (event->modifiers() & Qt::ShiftModifier) ) ||
                     ( m_activeButton == Qt::MiddleButton );

    if ( m_activeButton == Qt::LeftButton && !isPanning )
    {
        // ORBIT MODE: Rotate camera around target (Left button without Shift)
        float sensitivity = 0.005f;

        m_cameraYaw += delta.x() * sensitivity;
        m_cameraPitch -= delta.y() * sensitivity;

        // Clamp pitch to prevent gimbal lock (~90 degrees = pi/2 radians)
        float maxPitch = 1.57f;
        if ( m_cameraPitch > maxPitch ) m_cameraPitch = maxPitch;
        if ( m_cameraPitch < -maxPitch ) m_cameraPitch = -maxPitch;
    }
    else if ( isPanning )
    {
        // PAN MODE: Move camera target (Left+Shift OR Middle button)
        // This is CRITICAL - allows adjusting view height to frame models properly!
        float panSpeed = 0.01f;

        // Calculate camera-relative right vector (perpendicular to view direction)
        math_vec3_t right;
        right[0] = -cosf( m_cameraYaw );
        right[1] = 0.0f;
        right[2] = sinf( m_cameraYaw );

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
    
    event->accept( );    
    
}

void ModelViewport::wheelEvent( QWheelEvent *event ) {
    
    // @Note: Adding the wheel behavior for the camera movement
    int delta = event->angleDelta().y();
    
    if ( delta > 0 )
    {
        m_cameraDistance *= 0.9f;   // we are reducing the distance by this 90 %
    } 
    else if ( delta < 0 )
    {
        m_cameraDistance *= 1.1f;   // we are increasing the distance by 110 %
    }
    
    
    // @Note: We need to clamp the distance so we dont get too close to the model
    if ( m_cameraDistance < 1.0f )
    {
        m_cameraDistance = 1.0f;   // this is the minimum length
    }
    
    if ( m_cameraDistance > 500.0f )
    {
        m_cameraDistance = 500.0f; // this is the maximum length
    }

    // Trigger repaint when zoom changes
    update();

    event->accept( );

}

bool ModelViewport::loadModel( const QString &modelPath )
{
    
    if ( m_model )
    {
        free_model( m_model );
        m_model = nullptr;
    }
    
    QByteArray pathBytes = modelPath.toUtf8( );
    const char *cPath = pathBytes.constData( );
    
    qDebug( ) << "Loading Model:" << modelPath;
    
    mdl_result_t result = create_mdl_model( cPath, &m_model );
    
    if ( result != MDL_SUCCESS || m_model == nullptr )
    {
        qDebug( ) << "Error: Failed to load the model!";
        m_model = nullptr;
        return ( false ); 
    }
    
    
    qDebug( ) << "Model Loaded Successfully!";
    qDebug( ) << "   Bones: " << m_model->header->numbones;
    qDebug( ) << "   Sequences: " << m_model->header->numseq;

    // Set model data for this viewport's renderer instance
    if ( m_renderInstance ) {
        r_qt_set_model_data( m_renderInstance, m_model->header, m_model->data,
                             m_model->texture_header, m_model->texture_data,
                             m_model->seqgroups, m_model->num_seqgroups );
    } else {
        qCritical() << "ERROR: No render instance available!";
    }

    // Auto-frame camera to show entire model
    frameModel();

    emit modelLoaded( modelPath );
    return ( true );

}

void ModelViewport::frameModel() {
    if ( !m_model || !m_model->header ) {
        return;
    }

    const float modelScale = 0.1f;  // Same scale we use for rendering

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

    // ============================================================================
    // CAMERA SYSTEM: Based on hlmvqt (working Qt 6 Half-Life model viewer)
    // Source: https://github.com/iOrange/hlmvqt/blob/main/renderview.cpp
    // ============================================================================

    // Camera target: Vertical center of bounding box
    // In HL coords: {0, 0, min.z + size.z/2}
    math_vec3_t hlTarget;
    hlTarget[0] = 0.0f;
    hlTarget[1] = 0.0f;
    hlTarget[2] = bbmin[2] + (sizeZ / 2.0f);  // Vertical center

    // Transform to OpenGL coordinates (HL Z → OpenGL Y)
    m_cameraTarget[0] = hlTarget[1] * modelScale;     // X = 0
    m_cameraTarget[1] = hlTarget[2] * modelScale;     // Y = vertical center
    m_cameraTarget[2] = -hlTarget[0] * modelScale;    // Z = 0

    // Camera distance: EXACT formula from hlmvqt
    // distance = max(dx, dy, dz) - largest bounding box dimension, NO multiplier!
    float maxDim = sizeX;
    if (sizeY > maxDim) maxDim = sizeY;
    if (sizeZ > maxDim) maxDim = sizeZ;

    // Apply ONLY the model scale (0.1), no additional padding/multipliers!
    m_cameraDistance = maxDim * modelScale;

    // Clamp to prevent extreme values
    if (m_cameraDistance < 1.0f) m_cameraDistance = 1.0f;
    if (m_cameraDistance > 100.0f) m_cameraDistance = 100.0f;

    // Camera angles: Isometric-style view (looking down at model from angle)
    // hlmvqt uses -90°/-90° rotations, converted to our spherical system:
    m_cameraPitch = 0.3f;      // ~17 degrees - look down at model
    m_cameraYaw = 0.785f;      // 45 degrees - 3/4 view angle

    // Trigger repaint to show new framing
    update();

    qDebug() << "\n========== MODEL FRAMED (hlmvqt formula) ==========";
    qDebug() << "BBox (HL): min=[" << bbmin[0] << bbmin[1] << bbmin[2]
             << "] max=[" << bbmax[0] << bbmax[1] << bbmax[2] << "]";
    qDebug() << "Size (HL):" << sizeX << "×" << sizeY << "×" << sizeZ;
    qDebug() << "Max dimension (HL):" << maxDim;
    qDebug() << "Model scale factor:" << modelScale;
    qDebug() << "Ground height (OpenGL Y):" << m_groundHeight;
    qDebug() << "Camera target (OpenGL): [" << m_cameraTarget[0] << ","
             << m_cameraTarget[1] << "," << m_cameraTarget[2] << "]";
    qDebug() << "Camera distance:" << m_cameraDistance << "(max dimension × scale)";
    qDebug() << "Camera angles: pitch=" << (m_cameraPitch * 57.3f) << "° yaw="
             << (m_cameraYaw * 57.3f) << "°";
    qDebug() << "===================================================\n";
}

bool ModelViewport::hasModelLoaded() {
    return (m_model != nullptr);
}
