/*
 * OpenGL renderer for MDL models
 * Handles vertex buffers, shaders, and drawing
 */

#ifndef RENDERER_H
#define RENDERER_H


#include "../studio.h"
#include "GLFW/glfw3.h"
#include <stdbool.h>


#define ASPECT_RATIO          ((float)(16.0f / 9.0f))
#define WIDTH                 ((int) 800)
#define HEIGHT                ((int) WIDTH * (ASPECT_RATIO))

int init_renderer(int width, int height, const char *title);
void cleanup_renderer(void);

void render_loop(void);
bool should_close_window(void);

void render_model(studiohdr_t *header, unsigned char *data);
void set_wireframe_mode(bool enabled);
void setup_model_vertices(float *vertices, int count);
void setup_model_vertices_with_indices(float *vertices, int vertex_count, unsigned short *indices, int index_count);

void clear_screen(void);

#endif // RENDERER_H
