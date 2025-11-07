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
 *   Purpose: Camera for the look and feel of the application
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include "r_camera.h"

void Camera_Init( r_camera_t *camera, math_vec3_t target_position, float initial_distance ) {
	Math_Vec3Copy( target_position, camera->target );

	camera->distance = initial_distance;

	camera->angles_deg[0] = 0.0f;
	camera->angles_deg[1] = 0.0f;
	camera->angles_deg[2] = 0.0f;

	camera->movement_speed = 5.0f;
	camera->rotation_speed = 45.0f;
	camera->zoom_speed = 1.1f;

	camera->forward[0] = 0.0f;
	camera->forward[1] = 0.0f;
	camera->forward[2] = -1.0f;

	camera->right[0] = 1.0f;
	camera->right[1] = 0.0f;
	camera->right[2] = 0.0f;

	camera->up[0] = 0.0f;
	camera->up[1] = 1.0f;
	camera->up[2] = 0.0f;

	camera->position[0] = camera->target[0] - camera->forward[0] * camera->distance;
	camera->position[1] = camera->target[1] - camera->forward[1] * camera->distance;
	camera->position[2] = camera->target[2] - camera->forward[2] * camera->distance;

	Math_Mat4_Identity( camera->view_matrix );
	camera->dirty = true;
}

void Camera_Rotate( r_camera_t *camera, float delta_pitch, float delta_yaw, float delta_time ) {
	float actual_pitch_value, actual_yaw_value;

	actual_pitch_value = delta_pitch * camera->rotation_speed * delta_time;
	actual_yaw_value = delta_yaw * camera->rotation_speed * delta_time;

	camera->angles_deg[0] += actual_pitch_value;
	camera->angles_deg[1] += actual_yaw_value;

	// need to clamp things so
	if ( camera->angles_deg[0] > 89.0f ) {
		camera->angles_deg[0] = 89.0f;
	}

	if ( camera->angles_deg[0] < -89.0f ) {
		camera->angles_deg[0] = -89.0f;
	}

	if ( camera->angles_deg[1] >= 360.0f ) {
		camera->angles_deg[1] -= 360.0f;
	}

	if ( camera->angles_deg[1] < 0.0f ) {
		camera->angles_deg[1] += 360.0f;
	}

	camera->dirty = true;
}

void Camera_Zoom( r_camera_t *camera, float scroll_delta ) {
	float multiplier;

	if ( scroll_delta > 0.0f ) {
		multiplier = 1.0f / camera->zoom_speed;
	} else {
		multiplier = camera->zoom_speed;
	}

	camera->distance *= multiplier;

	// clamping values now
	if ( camera->distance < 0.1f ) {
		camera->distance = 0.1f;
	}
	if ( camera->distance > 1000.0f ) {
		camera->distance = 1000.0f;
	}

	camera->dirty = true;
}

void Camera_UpdateTransforms( r_camera_t *camera ) {
	if ( !camera->dirty ) {
		return;
	}

	// now we convert the camera angles to the direction vectors
	Math_AngleVectors( camera->angles_deg, &camera->forward, &camera->right, &camera->up );

	camera->position[0] = camera->target[0] - camera->forward[0] * camera->distance;
	camera->position[1] = camera->target[1] - camera->forward[1] * camera->distance;
	camera->position[2] = camera->target[2] - camera->forward[2] * camera->distance;

	Math_Mat4_LookAt( camera->position, camera->target, camera->up, camera->view_matrix );

	camera->dirty = false;
}

const math_mat4_t *Camera_GetViewMatrix( r_camera_t *camera ) {
	if ( !camera->dirty ) {
		return &camera->view_matrix;
	}

	Camera_UpdateTransforms( camera );

	return &camera->view_matrix;
}
