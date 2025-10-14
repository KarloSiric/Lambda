#ifndef RENDERER_H
#define RENDERER_H

#include "../studio.h"
#include "../mdl/mdl_loader.h"  // <-- Need this for mdl_seqgroup_blob_t
#include "GLFW/glfw3.h"
#include <stdbool.h>

extern GLFWwindow *window;

#define ASPECT_RATIO ((float)(16.0f / 9.0f))
#define WIDTH ((int)1400)
#define HEIGHT ((int)(WIDTH / ASPECT_RATIO))

// Initialization and cleanup
int  init_renderer(int width, int height, const char *title);
void cleanup_renderer(void);

// Main loop
void render_loop(void);
bool should_close_window(void);

// Rendering functions
void render_model(studiohdr_t *header, unsigned char *data);
void set_wireframe_mode(bool enabled);
void set_current_texture(unsigned int texture_id);

// ==================================================================
// UPDATED: Now accepts seqgroups!
// ==================================================================
void set_model_data(
    studiohdr_t *header,
    unsigned char *data,
    studiohdr_t *tex_header,
    unsigned char *tex_data,
    mdl_seqgroup_blob_t *seqgroups,     // <-- NEW!
    int num_seqgroups                    // <-- NEW!
);

void clear_screen(void);

// Internal helpers (exposed for debugging)
void UpdateBonesForCurrentFrame(void);
void ProcessModelForRendering(void);
void AddVertexToBuffer(int vIndex, int nIndex, short s, short t, float texW, float texH);

#endif // RENDERER_H
