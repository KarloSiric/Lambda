/*======================================================================
 *  File: textures.c
 *  Project: shaders
 *  Author: karlosiric <email@example.com>
 *  Created: 2025-09-27 14:30:32
 *  Last Modified by: karlosiric
 *  Last Modified: 2025-09-28 11:53:54
 *----------------------------------------------------------------------
 *  Description:
 *      
 *----------------------------------------------------------------------
 *  License: 
 *  Company: 
 *  Version: 0.1.0
 *======================================================================
 */

#include <OpenGL/gl.h>
#include <stddef.h>
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else
#include <GL/gl3.h>
#endif

#include "textures.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>


static inline const mstudiotexture_t *texture_array(const studiohdr_t *header,
                                                    const unsigned char *data) 
{
    return (const mstudiotexture_t *)(data + header->textureindex);
}


const studiohdr_t *mdl_pick_texture_header(const studiohdr_t *main_header,
                                           const studiohdr_t *text_header)
{

    if (main_header && main_header->numtextures > 0) return main_header;
    if (text_header && text_header->numtextures > 0) return text_header;
    return NULL;

}


bool mdl_pal8_to_rgba(const unsigned char *indices, int w, int h, const unsigned char *pallette_rgb, size_t pallette_size, unsigned char *dst)
{

    if (!indices || !pallette_rgb || !dst || w <= 0 || h <= 0) return false;

    if (pallette_size <= 0 || pallette_size > 256) return false;
    
    const int px = w * h;

    for (int i = 0; i < px; i++) {
        const unsigned idx = indices[i];
        const unsigned p = (idx  < (unsigned)pallette_size) ? idx : 0;
        const unsigned char *rgb = &pallette_rgb[p * 3];    // because we have pallette_size array but * 3 the amount of bytes
    
        dst[i*4 + 0] = rgb[0];
        dst[i*4 + 1] = rgb[1];
        dst[i*4 + 2] = rgb[2];
        dst[i*4 + 3] = 255;                                 // For transparency
    }        

    return true;
}


static bool parse_paletted_block(const unsigned char *text_struct_base,
                                 int width, int height, int index_offset,
                                 const unsigned char **out_indices,
                                 int *out_count,
                                 const unsigned char **out_palette,
                                 int *out_pal_size,
                                 const unsigned char *file_start,
                                 size_t file_size) 
{

    /* Writing guards for checking safely everything 
     * once we load and read the .mdl file 
     * TODO(karlo): Implement even stronger checking bounds and offsets.
     */

    if (!text_struct_base || !file_start || file_size == 0) return false;
    if (width <= 0 || height <= 0)                          return false;

    if ((size_t)width > SIZE_MAX / (size_t)height)          return false;

    const ptrdiff_t base_off = text_struct_base - file_start;
    if (base_off < 0 || (size_t)base_off >= file_size)      return false;

    if (index_offset < 0)                                   return false;

    const size_t pix_rel = (size_t)index_offset;
    if (pix_rel > file_size - (size_t)base_off)             return false;

    const size_t pix_off = (size_t)base_off + pix_rel;
    const size_t pixels_len = (size_t)width * (size_t)height;
    if (pix_off > file_size - (size_t)base_off)             return false;

    const unsigned char *indices = file_start + pix_off;

    // Palette data follows the pixels data
    const size_t pal_size_off = pix_off + pixels_len;   

    const unsigned char *palette = NULL;

    int pal_sz = 0;

    if (pal_size_off + 2 <= file_size) {
        uint16_t sz_le = 0;
        memcpy(&sz_le, file_start + pal_size_off, 2);
        const int sz = (int)sz_le;

        const size_t pal_bytes_off = pal_size_off + 2;
        const size_t pal_bytes_len = (size_t)sz * 3;

        if (sz > 0 && sz <= 256 && pal_bytes_len <= file_size - pal_bytes_off) {
            palette = file_start + pal_bytes_off;
            pal_sz  = sz;
        }
    }

    // Layout B (fallback): no size field, assume 256 entries right after pixels
    if (!palette) {
        const size_t need = 256u * 3u;
        if (need <= file_size - pal_size_off) {
            palette = file_start + pal_size_off;
            pal_sz  = 256;
        } else {
            return false;
        }
    }

    *out_indices  = indices;
    *out_count    = (int)pixels_len;
    *out_palette  = palette;
    *out_pal_size = pal_sz;
    return true;

}


mdl_result_t mdl_load_textures(const studiohdr_t *header, 
                               const unsigned char *file_data, 
                               mdl_texture_set_t *out_set)
{

    if (!out_set) {
        return MDL_ERROR_INVALID_PARAMETER;
    }

    out_set->textures = NULL;
    out_set->count = 0;

    if (!header || !file_data) {
        return MDL_ERROR_MISSING_TEXTURE_FILE;
    } 
    if (header->numtextures <= 0)        {
        return MDL_ERROR_NO_TEXTURES_IN_FILE;
    } 

    const size_t file_size = (size_t)header->length;
    const size_t text_arr_off = (size_t)header->textureindex;

    if (text_arr_off >= file_size) {
        return MDL_ERROR_FILE_TOO_SMALL;
    }

    const mstudiotexture_t *textures = (const mstudiotexture_t *)(file_data + text_arr_off);

    const int n_textures = header->numtextures;

    mdl_gl_texture_t *items = (mdl_gl_texture_t *)calloc((size_t)n_textures, sizeof(*items));

    if (!items) {
        return MDL_ERROR_MEMORY_ALLOCATION;     
    }                           

    for (int i = 0; i < n_textures; i++) {

        const mstudiotexture_t *T = &textures[i];
        const unsigned char *indices = NULL;
        const unsigned char *palette = NULL;

        int count = 0, pal_size = 0;

        const unsigned char *text_struct_base = (const unsigned char *)T;

        const bool ok = parse_paletted_block(text_struct_base, T->width, T->height, 
                                            T->index, &indices, &count, &palette, 
                                            &pal_size, file_data, file_size);

        if (!ok) {

            items[i].gl_id = 0;
            items[i].width = items[i].height = 0;
            items[i].name[0] = '\0';
            continue;
        }

        const int w = T->width, h = T->height;
        unsigned char* rgba = (unsigned char*)malloc((size_t)w * (size_t)h * 4u);
        if (!rgba) { free(items); return MDL_ERROR_MEMORY_ALLOCATION; }

        if (!mdl_pal8_to_rgba(indices, w, h, palette, pal_size, rgba)) {
            free(rgba);
            items[i].gl_id = 0;
            items[i].width = items[i].height = 0;
            items[i].name[0] = '\0';
            continue;
        }

        // Upload to GL
        GLuint tex = 0;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, rgba);
        glBindTexture(GL_TEXTURE_2D, 0);
        free(rgba);

        items[i].gl_id  = tex;
        items[i].width  = w;
        items[i].height = h;
        strncpy(items[i].name, T->name, sizeof(items[i].name)-1);
        items[i].name[sizeof(items[i].name)-1] = '\0';
    }

    out_set->textures = items;
    out_set->count = n_textures;
    return MDL_SUCCESS;

}



void mdl_free_texture(mdl_texture_set_t *set) {

    if (!set || !set->textures) {
        return;
    }

    for (int i = 0; i < set->count; i++) {
        if (set->textures[i].gl_id) {
            GLuint id = set->textures[i].gl_id;
            glDeleteTextures(1, &id);
        }
    }

    free(set->textures);
    set->textures = NULL;
    set->count = 0;

}



