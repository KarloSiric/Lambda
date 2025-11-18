/*
 * ═══════════════════════════════════════════════════════════════════════════
 *   Half-Life Model Viewer
 * ═══════════════════════════════════════════════════════════════════════════
 *
 *   Copyright (c) 1996-2002, Valve LLC. All rights reserved.
 *
 *   This product contains software technology licensed from Id
 *   Software, Inc. ("Id Technology"). Id Technology (c) 1996 Id Software, Inc.
 *   All Rights Reserved.
 *
 *   Use, distribution, and modification of this source code and/or resulting
 *   object code is restricted to non-commercial enhancements to products from
 *   Valve LLC. All other use, distribution, or modification is prohibited
 *   without written permission from Valve LLC.
 *
 * ───────────────────────────────────────────────────────────────────────────
 *   Author: Karlo Siric
 *   Purpose: Hitboxes System implementation
 * ═══════════════════════════════════════════════════════════════════════════
 */


#include "mdl_hitboxes.h"
#include "math_vector.h"
#include "math_matrix.h"
#include "util_logger.h"
#include <stdio.h>
#include <string.h>

// PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
//   ACCESS HITBOXES
// PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP

const mstudiobbox_t* mdl_hitbox_get_info( const studiohdr_t *header,
                                           const unsigned char *data,
                                           int index ) {
	// TODO: Implement get_info
	// Very similar to mdl_attachment_get_info()!
	// 1. Check if header and data are NULL
	// 2. Check if index is valid (>= 0 and < header->numhitboxes)
	// 3. Get hitboxes array: (mstudiobbox_t*)(data + header->hitboxindex)
	// 4. Return &hitboxes[index]
}

// PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
//   WORLD SPACE CALCULATIONS
// PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP

void mdl_hitbox_world_bounds( const mstudiobbox_t *hitbox,
                               const math_mat4_t *bone_matrices,
                               math_vec3_t out_min,
                               math_vec3_t out_max ) {
	// TODO: Implement world bounds calculation
	// Similar to mdl_attachment_get_world_position(), but for TWO points (min and max)
	// 1. Check if hitbox, bone_matrices, out_min, and out_max are NULL
	// 2. Get bone index from hitbox->bone
	// 3. Transform hitbox->bbmin by bone matrix ’ out_min
	// 4. Transform hitbox->bbmax by bone matrix ’ out_max
	// Use Math_Vec3TransformMat4() for both (they're positions, not directions)
}

// PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
//   DEBUG AND DIAGNOSTICS
// PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP

void mdl_hitbox_print_dump( const studiohdr_t *header,
                             const unsigned char *data ) {
	// TODO: Implement debug dump
	// Similar to mdl_attachment_dump()
	// 1. Check if header and data are NULL
	// 2. Print header: "==== HITBOXES (X) ===="
	// 3. Get hitboxes array
	// 4. Loop through all hitboxes and print:
	//    - Index
	//    - Bone index
	//    - Group
	//    - Min bounds (bbmin[0], bbmin[1], bbmin[2])
	//    - Max bounds (bbmax[0], bbmax[1], bbmax[2])
}
