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


static void glfw_cursor_callback( GLFWwindow *window, double xpos, double ypos )
{
    
    ( void )window; // not being used so we silence it
    
    g_input_state.mouse_x = xpos;
    g_input_state.mouse_y = ypos;   
    
}


static void glfw_mouse_button_callback( GLFWwindow *window, int button, int action, int mods )
{
    
    ( void )window;
    ( void )mods;
    
    int button_index = -1;
    
    if ( button == GLFW_MOUSE_BUTTON_LEFT ) {
        button_index = MOUSE_BUTTON_LEFT;
    } else if ( button == GLFW_MOUSE_BUTTON_RIGHT ) {
        button_index = MOUSE_BUTTON_RIGHT;
    } else if ( button == GLFW_MOUSE_BUTTON_MIDDLE ) {
        button_index = MOUSE_BUTTON_MIDDLE;
    }
    
    
    if ( button_index < 0 || button_index >= 3 ) {
        return;
    }
    
    if ( action == GLFW_PRESS ) {
        g_input_state.mouse_buttons_current[button_index] = true;
    } else if ( action == GLFW_RELEASE ) {
        g_input_state.mouse_buttons_current[button_index] = false;
    }  
}


static void glfw_scroll_callback( GLFWwindow *window, double xoffset, double yoffset ) 
{
    
    ( void )window;
    ( void )xoffset;
    
    g_input_state.scroll_delta += yoffset;
 
}


static int glfw_key_to_enum( int glfwKey )
{
    
    switch( glfwKey ) {
        case GLFW_KEY_W:
            return KEY_W;
        case GLFW_KEY_A:
            return KEY_A;
        case GLFW_KEY_S:
            return KEY_S;
        case GLFW_KEY_D:
            return KEY_D;
        case GLFW_KEY_SPACE:
            return KEY_SPACE;
        case GLFW_KEY_ESCAPE:
            return KEY_ESCAPE;
        default:
            return -1;
    } 
}


static void glfw_key_callback( GLFWwindow *window, int key, int scancode, int action, int mods )
{
    ( void )window;
    ( void )scancode;
    ( void )mods;
    
    // call our helper function that we need
    
    int our_key = glfw_key_to_enum( key );
    
    if ( our_key < 0 || our_key >= MAX_KEYS ) { 
        return;
    }
    
    // we get here that means we are successful for now
    
    if ( action == GLFW_PRESS ) {
        g_input_state.current_keys[our_key] = true;
    } else if ( action == GLFW_RELEASE ) {
        g_input_state.current_keys[our_key] = false;
    } 
}


void Input_Init( GLFWwindow *window ) {
    
    if ( !window ) {
        return;
    }   
    
    memset( &g_input_state, 0, sizeof( input_state_t ) );
    
    g_input_state.window = window;
    
    // callback functions 
       
    glfwSetKeyCallback( window, glfw_key_callback );
    
    glfwSetCursorPosCallback( window, glfw_cursor_callback );
    
    glfwSetMouseButtonCallback( window, glfw_mouse_button_callback );
    
    glfwSetScrollCallback( window, glfw_scroll_callback );
      
    glfwGetCursorPos( window, &g_input_state.mouse_x, &g_input_state.mouse_y );
    
    g_input_state.mouse_prev_x = g_input_state.mouse_x;
    g_input_state.mouse_prev_y = g_input_state.mouse_y;
    
    g_input_state.initialized = true;
}


void Input_Update( void ) { 
    if (!g_input_state.initialized) {
        return;
    }
    
    // we need to copy values from current keys to previous keys and store that snapshot for later usage
    
    memcpy( g_input_state.previous_keys, g_input_state.current_keys, sizeof( g_input_state.previous_keys ) );
    
    memcpy( g_input_state.mouse_buttons_previous, g_input_state.mouse_buttons_current, sizeof( g_input_state.mouse_buttons_previous ) );
    
    g_input_state.mouse_prev_x = g_input_state.mouse_x;    
    g_input_state.mouse_prev_y = g_input_state.mouse_y;    
    
    g_input_state.scroll_delta = 0.0f;
}


void Input_GetMousePosition( float *x, float *y ) { 
    if ( x ) {
        *x = ( float )g_input_state.mouse_x;
    }
    
    if ( y ) {
        *y = ( float )g_input_state.mouse_y;
    }
}


void Input_GetMouseDelta( float *dx, float *dy ) { 
    if ( dx ) {
        *dx = ( float )( g_input_state.mouse_x - g_input_state.mouse_prev_x );
    }
    
    if ( dy ) {
        *dy = ( float )( g_input_state.mouse_y - g_input_state.mouse_prev_y );
    }
}


bool Input_IsSMouseKeyPressed( input_mouse_button_t button ) {
    if ( button < 0 || button >= 3 ) {
        return false;
    }
    
    bool pressed_now = g_input_state.mouse_buttons_current[button];
    bool pressed_before = g_input_state.mouse_buttons_previous[button];
    
    return ( pressed_now && !pressed_before );
}


bool Input_IsSMouseKeyHeld( input_mouse_button_t button ) {
    if ( button < 0 || button >= 3 ) {
        return false;
    }
    
    bool pressed_now = g_input_state.mouse_buttons_current[button];
    
    return ( pressed_now ); 
}


int  Input_GetScrollDelta( void ) {
    return (int)g_input_state.scroll_delta;
}


bool Input_IsKeyPressed( input_key_t key ) {
    
    // validation
    if ( key < 0 || key >= MAX_KEYS ) {
        return false;
    }   
    
    bool pressed_now = g_input_state.current_keys[key];
    bool pressed_before = g_input_state.previous_keys[key];
    
    return ( pressed_now && !pressed_before ); 
}


bool Input_IsKeyHeld( input_key_t key ) {
    
    // validation
    if ( key < 0 || key >= MAX_KEYS ) {
        return false;
    }
     
    bool pressed_now = g_input_state.current_keys[key];
    bool pressed_before = g_input_state.previous_keys[key];
     
    return ( pressed_now && pressed_before );
}


bool Input_IsKeyReleased( input_key_t key ) {
    
    // validation
    if ( key < 0 || key >= MAX_KEYS ) {
        return false;
    }
    
    bool pressed_now = g_input_state.current_keys[key];
    bool pressed_before = g_input_state.previous_keys[key];
        
    return ( !pressed_now && pressed_before ); 
}

void Input_Shutdown( void ) {
    
    if ( !g_input_state.initialized ) {
        return;
    }
    
    
    if ( g_input_state.window ) {
        glfwSetKeyCallback( g_input_state.window, NULL );
        glfwSetCursorPosCallback( g_input_state.window, NULL );
        glfwSetMouseButtonCallback( g_input_state.window, NULL );
        glfwSetScrollCallback( g_input_state.window, NULL );
    }
    
    
    memset( &g_input_state, 0, sizeof( g_input_state ) );  
}











