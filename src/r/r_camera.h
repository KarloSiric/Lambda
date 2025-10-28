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
#include "math/math_angles.h"
#include "math/math_vector.h"


typedef struct {     
    math_vec3_t position;
    math_vec3_t angles_deg;       
     
    
    
    
} ;

#endif // CAMERA_H
