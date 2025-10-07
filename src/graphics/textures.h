#ifndef TEXTURE_H
#define TEXTURE_H 


#pragma once
#include <stdbool.h>
#include <stddef.h>
#include "../studio.h"
#include "../utils/mdl_messages.h"

typedef struct {
    unsigned int gl_id;          // 0 meaning it is not created 
    int width;
    int height;
    char name[64];
    int flags;
} mdl_gl_texture_t;


typedef struct {
    mdl_gl_texture_t *textures;
    int count;
} mdl_texture_set_t;



const studiohdr_t *mdl_pick_texture_header(const studiohdr_t *main_header,
                                           const studiohdr_t *text_header);

mdl_result_t mdl_load_textures(const studiohdr_t *main_header, const unsigned char *texture_data,
                               mdl_texture_set_t *out_set);


void mdl_free_texture(mdl_texture_set_t *set);


bool mdl_pal8_to_rgba(const unsigned char *indices, int w, int h, const unsigned char *pallette_rgb,
                      size_t pallette_size, unsigned char *dst);



#endif
