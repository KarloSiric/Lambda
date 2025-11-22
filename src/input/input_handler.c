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
 *   Purpose: Game-specific input handling
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include "input_handler.h"
#include "input.h"
#include "input_types.h"
#include "mdl/mdl_animations.h"
#include "mdl/mdl_audio.h"
#include "mdl/mdl_bodypart.h"
#include "r/r_draw.h"
#include "studio.h"

#include <stdio.h>

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#endif

// Helper to check if sequence is available
static bool is_sequence_available( int seq_index, studiohdr_t *header, unsigned char *data, mdl_seqgroup_blob_t *seqgroups, int num_seqgroups ) {
	if ( !header || !data || seq_index < 0 || seq_index >= header->numseq ) {
		return false;
	}

	mstudioseqdesc_t *sequences = (mstudioseqdesc_t *)( data + header->seqindex );
	mstudioseqdesc_t *seq = &sequences[seq_index];
	int seqgroup = seq->seqgroup;

	// Sequence group 0 is always available (embedded in main file)
	if ( seqgroup == 0 ) {
		return true;
	}

	// Check if external sequence group is loaded
	if ( !seqgroups || seqgroup >= num_seqgroups ) {
		return false;
	}

	return ( seqgroups[seqgroup].data != NULL );
}

void Input_ProcessGameInput(
	GLFWwindow *window,
	input_camera_state_t *camera_state,
	input_animation_state_t *anim_state ) {

	if ( !camera_state ) {
		return;
	}

	// ═══════════════════════════════════════════════════════════
	// EXIT
	// ═══════════════════════════════════════════════════════════
	if ( Input_IsKeyPressed( KEY_ESCAPE ) ) {
		glfwSetWindowShouldClose( window, GLFW_TRUE );
	}

	// ═══════════════════════════════════════════════════════════
	// CAMERA - Keyboard rotation
	// ═══════════════════════════════════════════════════════════
	if ( Input_IsKeyHeld( KEY_W ) ) {
		*camera_state->rotation_x -= 0.1f;
	}
	if ( Input_IsKeyHeld( KEY_S ) ) {
		*camera_state->rotation_x += 0.1f;
	}
	if ( Input_IsKeyHeld( KEY_A ) ) {
		*camera_state->rotation_y -= 0.1f;
	}
	if ( Input_IsKeyHeld( KEY_D ) ) {
		*camera_state->rotation_y += 0.1f;
	}

	// ═══════════════════════════════════════════════════════════
	// CAMERA - Zoom
	// ═══════════════════════════════════════════════════════════
	if ( Input_IsKeyHeld( KEY_Q ) ) {
		*camera_state->zoom *= 1.1f;
		if ( *camera_state->zoom > 2.0f ) {
			*camera_state->zoom = 2.0f;
		}
	}
	if ( Input_IsKeyHeld( KEY_E ) ) {
		*camera_state->zoom *= 0.9f;
		if ( *camera_state->zoom < 0.1f ) {
			*camera_state->zoom = 0.1f;
		}
	}

	// Scroll zoom
	int scroll = Input_GetScrollDelta();
	if ( scroll != 0 ) {
		*camera_state->zoom *= ( 1.0f + scroll * 0.1f );
		if ( *camera_state->zoom < 0.01f ) {
			*camera_state->zoom = 0.01f;
		}
		if ( *camera_state->zoom > 2.0f ) {
			*camera_state->zoom = 2.0f;
		}
	}

	// ═══════════════════════════════════════════════════════════
	// CAMERA - Mouse drag rotation
	// ═══════════════════════════════════════════════════════════
	if ( Input_IsSMouseKeyHeld( MOUSE_BUTTON_LEFT ) ) {
		float dx, dy;
		Input_GetMouseDelta( &dx, &dy );
		*camera_state->rotation_y += dx * 0.01f;
		*camera_state->rotation_x -= dy * 0.01f;
	}

	// ═══════════════════════════════════════════════════════════
	// CAMERA - Reset view
	// ═══════════════════════════════════════════════════════════
	if ( Input_IsKeyPressed( KEY_R ) ) {
		*camera_state->rotation_x = 0.0f;
		*camera_state->rotation_y = 0.0f;
		*camera_state->zoom = 0.15f;
	}

	// ═══════════════════════════════════════════════════════════
	// RENDER MODE - Wireframe/Points
	// ═══════════════════════════════════════════════════════════
	if ( Input_IsKeyPressed( KEY_F ) ) {
		*camera_state->wireframe_enabled = !( *camera_state->wireframe_enabled );
		if ( *camera_state->wireframe_enabled ) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		} else {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}
	}

	if ( Input_IsKeyPressed( KEY_P ) ) {
		glPolygonMode( GL_FRONT_AND_BACK, GL_POINT );
	}

	// ═══════════════════════════════════════════════════════════
	// ANIMATION CONTROLS
	// ═══════════════════════════════════════════════════════════
	if ( !anim_state || !anim_state->anim_state || !anim_state->header ) {
		return; // No animation state available
	}

	// Toggle animation
	if ( Input_IsKeyPressed( KEY_SPACE ) ) {
		*anim_state->animation_enabled = !( *anim_state->animation_enabled );
	}

	// Toggle looping
	if ( Input_IsKeyPressed( KEY_L ) ) {
		anim_state->anim_state->is_looping = !( anim_state->anim_state->is_looping );
	}

	// Toggle audio
	if ( Input_IsKeyPressed( KEY_M ) ) {
		mdl_audio_toggle_enabled();
	}

	// Reset to frame 0
	if ( Input_IsKeyPressed( KEY_0 ) ) {
		anim_state->anim_state->current_frame = 0.0f;
		if ( anim_state->model_processed ) {
			*anim_state->model_processed = false;
		}
	}

	// Print animation info
	if ( Input_IsKeyPressed( KEY_I ) ) {
		if ( anim_state->header->numseq > 0 ) {
			mstudioseqdesc_t *sequences = (mstudioseqdesc_t *)( anim_state->data + anim_state->header->seqindex );
			mstudioseqdesc_t *seq = &sequences[anim_state->anim_state->current_sequence];

			printf( "\n═══════════════════════════════════════\n" );
			printf( "  ANIMATION INFO\n" );
			printf( "═════════════════════════════════════════\n" );
			printf( "Sequence:       %d/%d\n", anim_state->anim_state->current_sequence, anim_state->header->numseq - 1 );
			printf( "Name:           %s\n", seq->label );
			printf( "Current Frame:  %.2f/%d\n", anim_state->anim_state->current_frame, seq->numframes - 1 );
			printf( "FPS:            %.1f\n", seq->fps );
			printf( "Looping:        %s\n", anim_state->anim_state->is_looping ? "Yes" : "No" );
			printf( "Animation:      %s\n", *anim_state->animation_enabled ? "ENABLED" : "DISABLED" );
			printf( "═══════════════════════════════════════\n\n" );
		}
	}

	// Previous sequence
	if ( Input_IsKeyPressed( KEY_LEFT ) ) {
		if ( anim_state->anim_state->current_sequence > 0 ) {
			int target_seq = anim_state->anim_state->current_sequence - 1;
			int attempts = 0;

			// Find previous available sequence
			while ( target_seq >= 0 && !is_sequence_available( target_seq, anim_state->header, anim_state->data, anim_state->seqgroups, anim_state->num_seqgroups ) && attempts < anim_state->header->numseq ) {
				target_seq--;
				attempts++;
			}

			if ( target_seq >= 0 && is_sequence_available( target_seq, anim_state->header, anim_state->data, anim_state->seqgroups, anim_state->num_seqgroups ) ) {
				// Stop all sounds when switching sequences
				mdl_audio_stop_all_sounds();

				mdl_animation_set_sequence( anim_state->anim_state, target_seq, anim_state->header, anim_state->data, anim_state->seqgroups );
				if ( anim_state->model_processed ) {
					*anim_state->model_processed = false;
				}
			}
		}
	}

	// Next sequence
	if ( Input_IsKeyPressed( KEY_RIGHT ) ) {
		if ( anim_state->anim_state->current_sequence < anim_state->header->numseq - 1 ) {
			int target_seq = anim_state->anim_state->current_sequence + 1;
			int attempts = 0;

			// Find next available sequence
			while ( target_seq < anim_state->header->numseq && !is_sequence_available( target_seq, anim_state->header, anim_state->data, anim_state->seqgroups, anim_state->num_seqgroups ) && attempts < anim_state->header->numseq ) {
				target_seq++;
				attempts++;
			}

			if ( target_seq < anim_state->header->numseq && is_sequence_available( target_seq, anim_state->header, anim_state->data, anim_state->seqgroups, anim_state->num_seqgroups ) ) {
				// Stop all sounds when switching sequences
				mdl_audio_stop_all_sounds();

				mdl_animation_set_sequence( anim_state->anim_state, target_seq, anim_state->header, anim_state->data, anim_state->seqgroups );
				if ( anim_state->model_processed ) {
					*anim_state->model_processed = false;
				}
			}
		}
	}

	// ═══════════════════════════════════════════════════════════
	// BODYPART CONTROLS
	// ═══════════════════════════════════════════════════════════

	// Previous bodypart
	if ( Input_IsKeyPressed( KEY_LEFT_BRACKET ) ) {
		bodypart_select_prev();
		if ( anim_state->model_processed ) {
			*anim_state->model_processed = false;
		}
	}

	// Next bodypart
	if ( Input_IsKeyPressed( KEY_RIGHT_BRACKET ) ) {
		bodypart_select_next();
		if ( anim_state->model_processed ) {
			*anim_state->model_processed = false;
		}
	}

	// Previous submodel
	if ( Input_IsKeyPressed( KEY_MINUS ) ) {
		bodypart_prev_submodel();
		if ( anim_state->model_processed ) {
			*anim_state->model_processed = false;
		}
	}

	// Next submodel
	if ( Input_IsKeyPressed( KEY_EQUAL ) ) {
		bodypart_next_submodel();
		if ( anim_state->model_processed ) {
			*anim_state->model_processed = false;
		}
	}

	// Print bodypart info
	if ( Input_IsKeyPressed( KEY_B ) ) {
		bodypart_print_info();
	}

	// Reset all bodyparts
	if ( Input_IsKeyPressed( KEY_BACKSPACE ) ) {
		bodypart_reset_all();
		if ( anim_state->model_processed ) {
			*anim_state->model_processed = false;
		}
	}

	// ═══════════════════════════════════════════════════════════
	// SKIN FAMILY CONTROLS
	// ═══════════════════════════════════════════════════════════

	// Next skin family
	if ( Input_IsKeyPressed( KEY_UP ) ) {
		next_skin_family();
		if ( anim_state->model_processed ) {
			*anim_state->model_processed = false;
		}
	}

	// Previous skin family
	if ( Input_IsKeyPressed( KEY_DOWN ) ) {
		prev_skin_family();
		if ( anim_state->model_processed ) {
			*anim_state->model_processed = false;
		}
	}
}
