

#ifndef RENDERER_H
#define RENDERER_H

#include "../studio.h"
#include "GLFW/glfw3.h"
#include <stdbool.h>

extern GLFWwindow *window;

#define ASPECT_RATIO          ((float)(16.0f / 9.0f))
#define WIDTH                 ((int) 800)
#define HEIGHT                ((int) (800 / ASPECT_RATIO))

int init_renderer(int width, int height, const char *title);
void cleanup_renderer(void);

void render_loop(void);
bool should_close_window(void);

void render_model(studiohdr_t *header, unsigned char *data);
void set_wireframe_mode(bool enabled);
void set_current_texture(unsigned int texture_id);
void set_model_data(studiohdr_t *header, unsigned char *data);

void clear_screen(void);

void InitializeBoneSystem(studiohdr_t *header, unsigned char *data, int bodypart_index);
void SetUpBones(void);
void ProcessModelForRendering(void);
void AddVertexToBuffer(int vertex_index, int normal_index, float u, float v);

#endif // RENDERER_H
