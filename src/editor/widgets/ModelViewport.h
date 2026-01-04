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
#include <QtWidgets/qtabwidget.h>


// @Note: C backend includes - everything is coevered in these includes
extern "C" {
    
    #include "mdl/mdl_loader.h"
    #include "mdl/mdl_animations.h"
    #include "r/r_draw.h"
    #include "r/r_grid.h"
    #include "r/r_camera.h"
    #include "math/math_types.h"
    #include <cglm/cglm.h>
    

}

}

class ModelViewport : public QOpenGLWidget , protected QOpenGLFunctions
{
    
    Q_OBJECT

public:
    
    explicit ModelViewport( QWidget *parent = nullptr );
    ~ModelViewport( ) override;
    
    // @Note: Model managment - using C backend
    bool loadModel( const QString &modelPath );
    void closeModel( );
    bool hasModelLoaded( );
    
    // @Note: Model animations managment - uses C backend API
    void setSequence( int sequenceIndex );
    void playAnimation( bool play );
    void setAnimationFrame( float frame );
    int getCurrentSequence( ) const;
    float getCurrentFrame( ) const;
    bool isAnimationPlaying( ) const;
    
    // @Note: Rendering toggles - uses C backend r_draw and r_grid API
    void setWireframeMode( bool enabled );
    void setShowGrid( bool show );
    void setShowGround( bool show );
    void setShowAxes( bool show );
    void setShowBones( bool show );
    void setShowHitboxes( bool show );
    
    // @Note: Camera control - uses C backend r_camera API
    void resetCamera( );
    void setCameraDistance( float distance );
    float getCameraDistance( ) const;
    
    // @Note: Skin family control - uses C backend r_draw API
    void setSkinFamily( int family );
    void nextSkinFamily( );
    void prevSkinFamily( );
    int getCurrentSkinFamily( ) const;
    int getNumSkinFamilies( ) const;
    
signals:
    
    // @Note: What happens when the model is loaded and everything
    void modelLoaded( const QString &path );
    void modelCloses( );
    
    void sequenceChanged( int sequenceIndex );
    void frameChanged( float frame );
    void animationPlayStateChanged( bool playing );
    
protected:

    void initializeGL( ) override;
    void paintGL( ) override;
    void resizeGL( int width, int height ) override;
    
    
    
    
     
    
      
    
    
    
    
};


#endif
