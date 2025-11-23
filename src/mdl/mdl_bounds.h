#ifndef MDL_BOUNDS_H
#define MDL_BOUNDS_H 


#include "studio.h"
#include <stdbool.h>


typedef struct {
   
   vec3_t min;
   vec3_t max;
   bool valid;
    
} mdl_bounds_t;


void mdl_bounds_calculate( const studiohdr_t *header, const unsigned char *data, mdl_bounds_t *bounds );

float mdl_bounds_get_height( const mdl_bounds_t *bounds );

float mdl_bounds_get_width( const mdl_bounds_t *bounds );

float mdl_bounds_get_depth( const mdl_bounds_t *bounds );


#endif
