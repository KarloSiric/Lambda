#ifndef CORE_APP_H
#define CORE_APP_H

#include "util/util_args.h"
#include "mdl/mdl_loader.h"
#include "r/r_gl_platform.h"

#include <stdbool.h>

#define APP_INIT_ERROR       -1          // error initialization just stop...
#define APP_INIT_EXIT_SUCCESS 1          // quick exit but not an error initialization
#define APP_INIT_SUCCESS      0          // normal successfull initialization

typedef struct {
	bool running;
	bool initialized;

	GLFWwindow *window;
	int window_width;
	int window_height;
	bool fullscreen;
    
    app_args_t *args;

	mdl_model_t *model;
	char model_path[260]; // Path to loaded model

	int bodypart_selections[32]; // Current submodel for each bodypart
	int num_bodyparts;

	int current_sequence;
	int current_frame;
	float animation_time;
	bool animation_playing;
	bool animation_looping;
	float animation_speed; // Playback speed multiplier

	bool show_bones;
	bool show_hitboxes;
	bool show_attachments;
	bool show_bounding_boxes;
	bool wireframe_mode;
	int wireframe_type; // Simple, by material, by bone, etc.
	int shading_mode; // Flat, smooth, matcap, unlit
	int background_mode; // Solid, gradient, grid
	float background_color[3]; // RGB

	int selected_bone_index;
	int selected_hitbox_index;
	int selected_attachment_index;
	int selected_sequence_index;
	int selected_texture_index;
	int hovered_vertex_index;

	// TODO: Will be a camera_t structure later
	float camera_position[3];
	float camera_target[3];
	float camera_distance;
	float camera_pitch;
	float camera_yaw;

	// TODO: Will be input_state_t structure later
	bool keys_pressed[512];
	bool mouse_buttons[8];
	float mouse_x, mouse_y;
	float mouse_delta_x, mouse_delta_y;

	bool show_info_panel;
	bool show_debug_overlay;
	bool mouse_over_ui;

	bool edit_mode;
	bool model_modified;
	// TODO: Undo/redo stacks later

} app_state_t;

int app_init( app_args_t *args );

int app_run( void );

void app_shutdown( void );

app_state_t *app_get_state( void );

#endif
