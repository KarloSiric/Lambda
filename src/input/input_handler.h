#ifndef INPUHANDLER_H
#define INPUHANDLER_H 


#include <stdbool.h>
#include <GLFW/glfw3.h>

#include "studio.h"
#include "mdl/mdl_animations.h"
#include "mdl/mdl_loader.h"


typedef struct {
    float *rotation_x;
    float *rotation_y;
    float *zoom;
    bool *wireframe_enabled;
} input_camera_state_t;


typedef struct {
    mdl_animation_state_t *anim_state;
    mdl_seqgroup_blob_t *seqgroups;
    studiohdr_t *header;
    unsigned char *data;
    int num_seqgroups;
    bool *animation_enabled;
    bool *model_processed;  // For forcing model reprocessing 
} input_animation_state_t;


void Input_ProcessGameInput(
    GLFWwindow *window,
    input_camera_state_t *camera_state,
    input_animation_state_t *anim_state
);




#endif
