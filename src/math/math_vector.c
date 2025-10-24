/*======================================================================
   File: math_vector.c
   Project: ModelViewer
   Author: karlosiric <email@example.com>
   Created: 2025-10-24 18:13:11
   Last Modified by: karlosiric
   Last Modified: 2025-10-24 18:35:20
   ---------------------------------------------------------------------
   Description:
       
   ---------------------------------------------------------------------
   License: 
   Company: 
   Version: 0.1.0
 ======================================================================
                                                                       */

#include "math_vector.h"
#include "math_types.h"


void Math_Vec3Copy( math_vec3_t src, math_vec3_t dst )
{ 
    glm_vec3_copy( src, dst ); 
}

void Math_Vec3Add( math_vec3_t a, math_vec3_t b, math_vec3_t out )
{
    glm_vec3_add( a, b, out );
}





