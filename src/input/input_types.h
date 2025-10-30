#ifndef INPUTTYPES_H
#define INPUTTYPES_H

#define MAX_KEYS 32

typedef enum {
	KEY_W = 0,
	KEY_A,
	KEY_S,
	KEY_D,
	KEY_SPACE,
	KEY_ESCAPE,

	// .. etc adding more later

} input_key_t;

typedef enum {

	MOUSE_BUTTON_LEFT = 0,
	MOUSE_BUTTON_RIGHT,
	MOUSE_BUTTON_MIDDLE

} input_mouse_button_t;








#endif
