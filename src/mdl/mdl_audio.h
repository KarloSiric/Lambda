#ifndef MDL_AUDIO_H
#define MDL_AUDIO_H 


#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif


bool mdl_audio_init( void );


void mdl_audio_shutdown( void );


void mdl_audio_configure_for_model( const char *model_path );


void mdl_audio_set_sound_directory( const char *sound_dir );


bool mdl_audio_play_event_sound( const char *relative_path );


bool mdl_audio_is_ready( void );


#endif
