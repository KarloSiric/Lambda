/*======================================================================
 *  File: textures.c
 *  Project: shaders
 *  Author: karlosiric <email@example.com>
 *  Created: 2025-09-27 14:30:32
 *  Last Modified by: karlosiric
 *  Last Modified: 2025-09-27 22:52:16
 *----------------------------------------------------------------------
 *  Description:
 *      
 *----------------------------------------------------------------------
 *  License: 
 *  Company: 
 *  Version: 0.1.0
 *======================================================================
 */

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







