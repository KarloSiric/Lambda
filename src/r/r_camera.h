/*
 * Camera system for 3D navigation
 * Handles view matrices and user input
 */

#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "math/math_types.h"
#include "math/math_matrix.h"
#include "math/math_angles.h"
#include "math/math_vector.h"


typedef struct {         
    math_vec3_t position;
    math_vec3_t angles_deg;       
    
    math_vec3_t target;
    float distance;
    
    float movement_speed;
    float rotation_speed;
    float zoom_speed;
    
    math_vec3_t forward; 
    math_vec3_t right; 
    math_vec3_t up; 
    
    math_mat4_t view_matrix;
    
    bool dirty;
    
} r_camera_t;

void Camera_Init( r_camera_t *camera, math_vec3_t target_position, float initial_distance );

void Camera_Rotate( r_camera_t *camera, float delta_pitch, float delta_yaw, float delta_time );

void Camera_Zoom( r_camera_t *camera, float scroll_delta );

void Camera_UpdateTransforms( r_camera_t *camera );

const math_mat4_t *Camera_GetViewMatrix( r_camera_t *camera );

#endif // CAMERA_H
