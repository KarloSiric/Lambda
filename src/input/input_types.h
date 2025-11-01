#ifndef INPUTTYPES_H
#define INPUTTYPES_H

#define MAX_KEYS 32

typedef enum {
	KEY_W = 0,
	KEY_A,
	KEY_S,
	KEY_D,
	KEY_Q,
	KEY_E,
	KEY_R,
	KEY_F,
	KEY_P,
	KEY_L,
	KEY_I,
	KEY_SPACE,
	KEY_ESCAPE,
	KEY_0,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_UP,
	KEY_DOWN,

	// .. etc adding more later

	KEY_COUNT

} input_key_t;

typedef enum {

	MOUSE_BUTTON_LEFT = 0,
	MOUSE_BUTTON_RIGHT,
	MOUSE_BUTTON_MIDDLE

} input_mouse_button_t;

#endif
