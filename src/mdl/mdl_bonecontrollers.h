#ifndef BONECONTROLLERS_H
#define BONECONTROLLERS_H 


#include "studio.h"
#include "math_types.h"
#include <stdbool.h>


#pragma once

// @Note: THese are bonecontroller types

#define STUDIO_X         0     // X Position
#define STUDIO_Y         1     // Y Position
#define STUDIO_Z         2     // Z Position
#define STUDIO_XR        3     // X Rotation
#define STUDIO_YR        4     // Y Rotation
#define STUDIO_ZR        5     // Z Rotation

// @Note: These are bonecontroller index

#define STUDIO_MOUTH     4     // Mouth open/close
#define STUDIO_EYES      5     // Eye tracking


typedef struct 
{ 
    
    float values[MAXSTUDIOCONTROLLERS];
    
} mdl_bonecontrollers_state_t;



void mdl_bonecontroller_init( mdl_bonecontrollers_state_t *ctrl_state );

bool mdl_bonecontroller_set_value ( mdl_bonecontrollers_state_t *ctrl_state, int controller, float value );

float mdl_bonecontroller_get_value ( const mdl_bonecontrollers_state_t *ctrl_state, int controller );

void mdl_bonecontroller_apply( const mdl_bonecontrollers_state_t *ctrl_state, const studiohdr_t *header,
                                const unsigned char *data, math_mat4_t *bone_matrices );



#endif /* BONECONTROLLERS_H */
