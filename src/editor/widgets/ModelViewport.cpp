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

ModelViewport::ModelViewport( QWidget *parent )
	: QOpenGLWidget( parent ),
	  m_model( nullptr ),
	  m_animationPlaying( false ),
	  m_animationTimer( nullptr ),
	  m_cameraPitch( 0.3f ),
	  m_cameraYaw( 0.5f ),
      m_cameraDistance( 50.0f ),
      m_cameraTarget{ 0.0f, 0.0f, 0.0f },
      m_showGrid( true ),
      m_wireframeMode( false ) {
        
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
    
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glFrontFace( GL_CCW );
    
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

    // @Note: Calculate camera position EXACTLY like Lambda does in r_draw.c (lines 1340-1352)
    //        Lambda uses spherical coordinates, not Quake-style angles!
    
    float camDist = m_cameraDistance;
     
    float pitch = m_cameraPitch;
    float yaw = m_cameraYaw;

    // Spherical to Cartesian conversion (Lambda's formula)
    math_vec3_t camPos;
    camPos[0] = camDist * cosf(pitch) * sinf(yaw);   // X
    camPos[1] = camDist * sinf(pitch);               // Y
    camPos[2] = camDist * cosf(pitch) * cosf(yaw);   // Z

    // Clamp camera Y above ground (Lambda does this)
    if ( camPos[1] < 0.5f ) {
        camPos[1] = 0.5f;
    }

    math_vec3_t target;
    Math_Vec3Copy( m_cameraTarget, target );
    
    math_vec3_t up = { 0.0f, 1.0f, 0.0f };      // World up

    // Create view matrix (same as Lambda)
    Math_Mat4_LookAt( camPos, target, up, m_viewMatrix );

    if ( m_showGrid )
    {
        r_grid_draw( m_viewMatrix, m_projMatrix, 0.0f );
        r_ground_draw( m_viewMatrix, m_projMatrix, 0.0f );
        r_axes_draw( m_viewMatrix, m_projMatrix, 0.0f );
    }

    // @NOTE: Need to render the model here later!
    
    if ( m_model && m_model->header && m_model->data ) 
    {
        
        set_model_data( m_model->header, m_model->data, m_model->texture_header, m_model->texture_data, m_model->seqgroups, m_model->num_seqgroups );
        render_model( m_model->header, m_model->data );
        
    }
     
    update( );
    
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
    // First we get the current mouse position
    QPoint currentPos = event->pos( );
    
    QPoint delta = currentPos - m_lastMousePos;
    
    if ( m_activeButton == Qt::LeftButton )
    {
        // Now we convert the pixel movement to angle change
        // we can adjust the sensitivity
        float sensitivity = 0.005f;
        
        m_cameraYaw += delta.x( ) * sensitivity;
        
        m_cameraPitch -= delta.y( ) * sensitivity;
         
        // We clamp the pitch to prevent camera flipping ( e.g. gimbal lock case )
        // ~90 degerees = pi/2 radians
        float maxPitch = 1.57f; 
        
        if ( m_cameraPitch > maxPitch )
        {
            m_cameraPitch = maxPitch;
        }
        
        if ( m_cameraPitch < - ( maxPitch ) )
        {
            m_cameraPitch = - ( maxPitch );
        }
    }
    
    // @NOTE( Karlo ): Adding support for PANNING!
    else if ( m_activeButton == Qt::MiddleButton )
    {
        float panSpeed = 0.01f; 
        math_vec3_t right, up;
        
        right[0] = -cosf( m_cameraYaw );
        right[1] = 0.0f;
        right[2] = sinf( m_cameraYaw );
        
        up[0] = 0.0f;
        up[1] = 1.0f;
        up[2] = 0.0f;
        
        m_cameraTarget[0] -= right[0] * delta.x( ) * panSpeed; 
        m_cameraTarget[1] -= right[1] * delta.x() * panSpeed;
        m_cameraTarget[2] -= right[2] * delta.x() * panSpeed;

        // Vertical drag (Y) = move along up vector
        m_cameraTarget[0] -= up[0] * delta.y() * panSpeed;
        m_cameraTarget[1] -= up[1] * delta.y() * panSpeed;
        m_cameraTarget[2] -= up[2] * delta.y() * panSpeed;    
        
    }
    
    m_lastMousePos = currentPos;
    
    event->accept( );
    
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
    
    emit modelLoaded( modelPath );
    return ( true );
    
}
