#ifndef R_GRID_H
#define R_GRID_H

#include <stdbool.h>
#include <cglm/cglm.h>

void r_grid_init( float size, float spacing );
void r_grid_draw( mat4 view, mat4 projection, float ground_y );
void r_grid_toggle( void );
bool r_grid_is_enabled( void );

// Ground plane (solid colored rectangle under the grid)
void r_ground_init( float size );
void r_ground_draw( mat4 view, mat4 projection, float ground_y );

// Origin axes (X/Y/Z colored lines)
void r_axes_init( float size );
void r_axes_draw( mat4 view, mat4 projection, float ground_y );

#endif
