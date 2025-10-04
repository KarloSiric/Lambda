/*======================================================================
 *  File: bodypart_manager.h
 *  Description: Manages Half-Life model bodypart selection and visualization
 *              Handles which model variations are shown for each bodypart
 *======================================================================*/

#ifndef BODYPART_MANAGER_H
#define BODYPART_MANAGER_H

#include "../studio.h"
#include <stdbool.h>

// Structure to track current bodypart configuration
typedef struct {
    int bodygroup;           // Packed value encoding all bodypart selections
    int current_bodypart;    // Currently selected bodypart for editing (0-31)
    int submodel_indices[MAXSTUDIOBODYPARTS]; // Current submodel for each bodypart
    bool debug_mode;         // Show debug info on screen
} bodypart_state_t;

// Initialize bodypart manager
void bodypart_init(void);

// Set the current model header (call when loading a new model)
void bodypart_set_model(studiohdr_t *header, unsigned char *data);

// Get the current bodygroup value
int bodypart_get_bodygroup(void);

// Set the bodygroup value (for loading saved states)
void bodypart_set_bodygroup(int bodygroup);

// Calculate which model index to use for a specific bodypart
int bodypart_get_model_index(int bodypart_index);

// Cycle to next submodel for current bodypart
void bodypart_next_submodel(void);

// Cycle to previous submodel for current bodypart  
void bodypart_prev_submodel(void);

// Select next bodypart for editing
void bodypart_select_next(void);

// Select previous bodypart for editing
void bodypart_select_prev(void);

// Reset all bodyparts to default (first submodel)
void bodypart_reset_all(void);

// Print current bodypart configuration to console
void bodypart_print_info(void);

// Get debug string for on-screen display
const char* bodypart_get_debug_string(void);

// Check if we should render a specific model
bool bodypart_should_render_model(int bodypart_index, int model_index);

#endif // BODYPART_MANAGER_H
