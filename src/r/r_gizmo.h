/*
 * ═══════════════════════════════════════════════════════════════════════════
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ═══════════════════════════════════════════════════════════════════════════
 *
 *   r_gizmo.h  –  Rotation gizmo: three coloured rings (X/Y/Z axes)
 *                 drawn around the model.  Hit-detection and drag math
 *                 live in ModelViewport.cpp; this module only renders.
 *
 * ═══════════════════════════════════════════════════════════════════════════
 */

#ifndef R_GIZMO_H
#define R_GIZMO_H

#include <cglm/cglm.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Call once after the OpenGL context is current. */
void r_gizmo_init( void );

/* Call when the context is about to be destroyed. */
void r_gizmo_cleanup( void );

/*
 * r_gizmo_draw
 *   view        – camera view matrix
 *   projection  – camera projection matrix
 *   center      – world-space centre of the rings (model pivot)
 *   radius      – ring radius in world units
 *   hovered     – axis index currently under the mouse (-1 = none, 0=X 1=Y 2=Z)
 *   active      – axis index being dragged        (-1 = none, 0=X 1=Y 2=Z)
 */
void r_gizmo_draw( mat4 view, mat4 projection,
                   vec3 center, float radius,
                   int hovered, int active );

#ifdef __cplusplus
}
#endif

#endif /* R_GIZMO_H */
