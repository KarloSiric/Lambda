#ifndef INPUT_H
#define INPUT_H

#include "input_types.h"
#include <stdbool.h>
#include <GLFW/glfw3.h>

void Input_Init( GLFWwindow *window );

void Input_Update( void );

void Input_Shutdown( void );

bool Input_IsKeyPressed( input_key_t key );

bool Input_IsKeyHeld( input_key_t key );

bool Input_IsKeyReleased( input_key_t key );

void Input_GetMousePosition( float *x, float *y );

void Input_GetMouseDelta( float *dx, float *dy );

bool Input_IsSMouseKeyPressed( input_mouse_button_t button );

bool Input_IsSMouseKeyHeld( input_mouse_button_t button );

int  Input_GetScrollDelta( void );

#endif
