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
#include <QDebug>
#include <QtCore/qdebug.h>
#include <QtCore/qlogging.h>

ModelViewport::ModelViewport( QWidget *parent )
	: QOpenGLWidget( parent ),
	  m_model( nullptr ),
	  m_animationPlaying( false ),
	  m_animationTimer( nullptr ),
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
	Camera_Init( &m_camera, target, 100.0f );

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
	qDebug() << "Viewport Resized:" << width << "x" << height;
}

void ModelViewport::paintGL() {
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void ModelViewport::onAnimationTick() {
}

void ModelViewport::mousePressEvent( QMouseEvent *event ) {
	m_lastMousePos = event->pos();
}

void ModelViewport::mouseMoveEvent( QMouseEvent *event ) {
	m_lastMousePos = event->pos();
}

void ModelViewport::mouseReleaseEvent( QMouseEvent *event ) {
}

void ModelViewport::wheelEvent( QWheelEvent *event ) {
}
