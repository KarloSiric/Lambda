#ifndef R_GRID_H
#define R_GRID_H 


#include <stdbool.h>
#include <cglm/cglm.h>


void grid_init( void );


void grid_cleanup( void );


void grid_render( mat4 view, mat4 projection, float ground_z );


void grid_toggle( void );


bool grid_is_enabled( void );


bool grid_set_enabled( bool enabled );


#endif
