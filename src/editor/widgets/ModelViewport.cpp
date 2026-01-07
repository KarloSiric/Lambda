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
#include <QtCore/qlogging.h>

ModelViewport::ModelViewport( QWidget *parent ) 
    : QOpenGLWidget( parent ),
    m_model( nullptr ),
    m_animationPlaying( false ),
    m_animationTimer( nullptr ),
    m_showGrid( true ),
    m_wireframeMode( false )
{
    mdl_animation_init( &m_animState );
    
    // Create the new animationTimer so 60 fps ~16.66 ms ~16 ms
    m_animationTimer = new QTimer( this );
    m_animationTimer->setInterval( 16 );
    
    connect( m_animationTimer, &QTimer::timeout, this, &ModelViewport::onAnimationTick);   
    
    //  Look at the origin
    math_vec3_t target = { 0.0f, 0.0f, 0.0f };
    
    // 100 units away the distance
    Camera_Init( &m_camera, target, 100.0f );
    
    qDebug() << "ModelViewport Created";    
    
}

ModelViewport::~ModelViewport( ) 
{
    makeCurrent( );
    
    if ( m_model ) {
        free_model( m_model );
        m_model = nullptr;
    }
    doneCurrent( );
    
    qDebug( ) << "ModelViewport Destroyed";
}


void ModelViewport::initializeGL( void )
{
    
    // Open OpenGL function pointers ( This is Qt's version of glewInit )
    
    initializeOpenGLFunctions( );
    
    qDebug( ) << "OpenGL Initialized";   
    
}




    
