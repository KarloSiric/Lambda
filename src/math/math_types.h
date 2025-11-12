#ifndef MATH_TYPES_H
#define MATH_TYPES_H 

#include <cglm/cglm.h>
#include <cglm/struct.h>
#include <cglm/vec3.h>
#include <cglm/mat3x4.h>
#include <cglm/mat4.h>
#include <stdbool.h>
#include <math.h>
#include <stdalign.h>


#define MATH_PI             3.14159265358979323846f
#define MATH_PI_2           ( MATH_PI / 2.0f )
#define MATH_PI_4           ( MATH_PI / 4.0f )
#define MATH_2PI            ( MATH_PI * 2.0f )


#define MATH_EPSILON        0.00001f
#define MATH_EQUAL_EPSILON  0.001f


#define MATH_DEG2RAD        ( MATH_PI / 180.0f )
#define MATH_RAD2DEG        ( 180.0f / MATH_PI )


typedef vec4                math_vec4_t;
typedef vec3                math_vec3_t;
typedef versor              math_quat_t;
typedef mat4                math_mat4_t;
typedef mat3                math_mat3_t;


typedef struct {
    
    alignas( 16 ) float mat[3][4];
    
} math_mat3x4_t;


#endif
