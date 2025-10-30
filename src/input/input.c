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
 *   Purpose: Input functions using wrapping for glfw
 * ═══════════════════════════════════════════════════════════════════════════
 */


#include "input.h"
#include "input_types.h"
#include <string.h>


typedef struct {
    GLFWwindow *window;
   
    bool current_keys[MAX_KEYS];
    bool previous_keys[MAX_KEYS];
    
    bool mouse_buttons_current[3];
    bool mouse_buttons_previous[3];
    
    double mouse_x;
    double mouse_y;
    double mouse_prev_x;
    double mouse_prev_y;
    
    float scroll_delta;
    
    bool initialized; 
    
} input_state_t;


static input_state_t g_input_state = {0};


void Input_Init( GLFWwindow *window ) {
    
    if ( !window ) {
        return;
    }   
    
    memset( &g_input_state, 0, sizeof( input_state_t ) );
    
    g_input_state.window = window;
    
    glfwSetKeyCallback( window, glfw_key_callback );
    
    glfwGetCursorPos( window, &g_input_state.mouse_x, &g_input_state.mouse_y );
    
       
    
     
}



