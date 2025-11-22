/*
 * ═══════════════════════════════════════════════════════════════════════════
 *   Half-Life Model Viewer/Editor ~ Lambda
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
 *   Purpose: Bone Controllers implementation
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include "mdl_bonecontrollers.h"
#include "math/math_matrix.h"
#include "util/util_logger.h"
#include "util/util_messages.h"
#include <math.h>
#include <stdalign.h>
#include <string.h>

void mdl_bonecontroller_init( mdl_bonecontrollers_state_t *ctrl_state ) {
	// @Note: safety check first always
	if ( ctrl_state == NULL ) {
		return;
	}

	memset( ctrl_state, 0, sizeof( mdl_bonecontrollers_state_t ) );

	return;
}

bool mdl_bonecontroller_set_value( mdl_bonecontrollers_state_t *ctrl_state, int controller, float value ) {
	if ( ctrl_state == NULL || controller < 0 || controller >= MAXSTUDIOCONTROLLERS ) {
		return false;
	}

	// @Note: We clamp the value here
	if ( value < 0.0 ) {
		value = 0.0f;
	} else if ( value > 1.0f ) {
		value = 1.0f;
	}

	ctrl_state->values[controller] = value;

	return true;
}

float mdl_bonecontroller_get_value( const mdl_bonecontrollers_state_t *ctrl_state, int controller ) {
	if ( ctrl_state == NULL || controller < 0 || controller >= MAXSTUDIOCONTROLLERS ) {
		return 0.0f;
	}

	return ctrl_state->values[controller];
}

void mdl_bonecontroller_apply( const mdl_bonecontrollers_state_t *ctrl_state,
							   const studiohdr_t *header,
							   const unsigned char *data,
							   math_mat4_t *bone_matrices ) {
	if ( ctrl_state == NULL || header == NULL || data == NULL || bone_matrices == NULL ) {
		LOG_ERRORF( "bonecontroller", "ERROR [%d] - Invalid parameter passed\n", MDL_ERROR_INVALID_PARAMETER );
		return;
	}

	int numcontrollers = header->numbonecontrollers;

	mstudiobonecontroller_t *bone_controllers = (mstudiobonecontroller_t *)( data + header->bonecontrollerindex );

	for ( int i = 0; i < numcontrollers; i++ ) {
		mstudiobonecontroller_t *controller = &bone_controllers[i];

		int bone_index = controller->bone;

		float value = ctrl_state->values[controller->index];

		float angle = controller->start + ( controller->end - controller->start ) * value;

		int type = controller->type & 0x07; // @Note(Karlo): I masked here to ensure it is from 0 - 7

		// Check first the position
		if ( type >= STUDIO_X && type <= STUDIO_Z ) {
			bone_matrices[bone_index][3][type] += angle;
		}

		else if ( type >= STUDIO_XR && type <= STUDIO_ZR ) {
			int axis = type - STUDIO_XR;

			float angle_rad = angle * MATH_DEG2RAD;

			math_mat4_t rotation;
			Math_Mat4_Identity( rotation );

			float c = cosf( angle_rad );
			float s = sinf( angle_rad );

			if ( axis == 0 ) {
				rotation[1][1] = c;
				rotation[1][2] = -s;
				rotation[2][1] = s;
				rotation[2][2] = c;
			}
			if ( axis == 1 ) {
				rotation[0][0] = c;
				rotation[0][2] = s;
				rotation[2][0] = -s;
				rotation[2][2] = c;
			}

			if ( axis == 2 ) {
				rotation[0][0] = c;
				rotation[0][1] = -s;
				rotation[1][0] = s;
				rotation[1][1] = c;
			}

			math_mat4_t result;
			Math_Mat4_Multiply( bone_matrices[bone_index], rotation, result );

			Math_Mat4_Copy( result, bone_matrices[bone_index] );
		}
	}

	return;
}
