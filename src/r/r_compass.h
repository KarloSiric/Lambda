/*
 * ═══════════════════════════════════════════════════════════════════════════
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ═══════════════════════════════════════════════════════════════════════════
 *
 *   r_compass.h  –  Orientation compass widget
 *                   Draws a small 3D XYZ axis indicator in a viewport corner.
 *
 * ═══════════════════════════════════════════════════════════════════════════
 */

#ifndef R_COMPASS_H
#define R_COMPASS_H

#include <cglm/cglm.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Call once after the OpenGL context is current. */
void r_compass_init( void );

/* Call when the context is about to be destroyed. */
void r_compass_cleanup( void );

/*
 * r_compass_draw
 *   view      – camera view matrix (only rotation is used)
 *   vpWidth   – current viewport width  in pixels
 *   vpHeight  – current viewport height in pixels
 *
 *   Draws a 90×90 px orientation widget in the bottom-right corner.
 */
void r_compass_draw( mat4 view, int vpWidth, int vpHeight );

#ifdef __cplusplus
}
#endif

#endif /* R_COMPASS_H */
