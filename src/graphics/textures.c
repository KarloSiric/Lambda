/*======================================================================
 *  File: textures.c
 *  Project: shaders
 *  Author: karlosiric <email@example.com>
 *  Created: 2025-09-27 14:30:32
 *  Last Modified by: karlosiric
 *  Last Modified: 2025-10-04 22:10:42
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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline const mstudiotexture_t *texture_array(const studiohdr_t   *header,
                                                    const unsigned char *data)
{
    return (const mstudiotexture_t *)(data + header->textureindex);
}

const studiohdr_t *mdl_pick_texture_header(const studiohdr_t *main_header,
                                           const studiohdr_t *text_header)
{
    if (main_header && main_header->numtextures > 0)
        return main_header;
    if (text_header && text_header->numtextures > 0)
        return text_header;
    return NULL;
}

// --- REPLACE YOUR mdl_pal8_to_rgba WITH THIS ---
bool mdl_pal8_to_rgba(const unsigned char *indices, int w, int h,
                      const unsigned char *palette_rgb, size_t palette_size,
                      unsigned char *dst)
{
    if (!indices || !palette_rgb || !dst || w <= 0 || h <= 0)
        return false;
    if (palette_size == 0 || palette_size > 256)
        return false;

    const int px = w * h;

    // MDL palettes are effectively RGB (R,G,B). Do NOT swap unless you verify otherwise.
    for (int i = 0; i < px; ++i)
    {
        const unsigned idx  = indices[i];
        const unsigned p    = (idx < (unsigned)palette_size) ? idx : 0;
        const unsigned base = p * 3;

        dst[i * 4 + 0] = palette_rgb[base + 0];  // R
        dst[i * 4 + 1] = palette_rgb[base + 1];  // G
        dst[i * 4 + 2] = palette_rgb[base + 2];  // B
        dst[i * 4 + 3] = (idx == 255) ? 0 : 255; // 255 is transparent in many GoldSrc MDLs
    }
    return true;
}

// --- REPLACE your parse_paletted_block WITH THIS STRICT VERSION ---
static bool parse_paletted_block(const unsigned char *text_struct_base,
                                 int width, int height, int index_offset,
                                 const unsigned char **out_indices,
                                 int                  *out_count,
                                 const unsigned char **out_palette,
                                 int                  *out_pal_size,
                                 const unsigned char  *file_start,
                                 size_t                file_size)
{
    if (!text_struct_base || !file_start || !out_indices || !out_palette ||
        !out_count || !out_pal_size || width <= 0 || height <= 0 || index_offset < 0)
        return false;

    const ptrdiff_t base_off = text_struct_base - file_start;
    if (base_off < 0 || (size_t)base_off >= file_size)
        return false;

    const size_t pix_off   = (size_t)base_off + (size_t)index_offset;
    const size_t pixels_sz = (size_t)width * (size_t)height;

    if (pix_off > file_size || pixels_sz > file_size - pix_off)
        return false;

    const size_t after_pixels = pix_off + pixels_sz;

    // MDL format: uint16 palette size (little endian), then palette (pal_size * 3) bytes.
    if (after_pixels + 2 > file_size)
        return false;

    uint16_t pal_count_le = 0;
    memcpy(&pal_count_le, file_start + after_pixels, 2);

    if (pal_count_le == 0 || pal_count_le > 256)
        return false;

    const size_t pal_bytes_off = after_pixels + 2;
    const size_t pal_bytes_len = (size_t)pal_count_le * 3u;

    if (pal_bytes_off > file_size || pal_bytes_len > file_size - pal_bytes_off)
        return false;

    *out_indices  = file_start + pix_off;
    *out_count    = (int)pixels_sz;
    *out_palette  = file_start + pal_bytes_off;
    *out_pal_size = (int)pal_count_le;

    // Debug prints so you can see exactly what is used
    printf("Found palette: %d colors at offset %ld (strict 16-bit count)\n",
           *out_pal_size, (long)(*out_palette - file_start));
    return true;
}



// ADDING FOR DEBUGGING FUNCTION
static void debug_texture_data(const studiohdr_t *header, const unsigned char *file_data)
{
    printf("\n=== TEXTURE DEBUG INFO ===\n");
    printf("header->textureindex = 0x%X\n", header->textureindex);
    printf("header->texturedataindex = 0x%X\n", header->texturedataindex); 
    printf("header->numtextures = %d\n", header->numtextures);
    printf("sizeof(mstudiotexture_t) = %lu\n", sizeof(mstudiotexture_t));
    
    const mstudiotexture_t *textures = (const mstudiotexture_t *)(file_data + header->textureindex);
    
    // Print first texture info
    if (header->numtextures > 0) {
        printf("\nFirst texture:\n");
        printf("  name: %s\n", textures[0].name);
        printf("  width x height: %d x %d\n", textures[0].width, textures[0].height);
        printf("  index (offset): 0x%X\n", textures[0].index);
        
        // Calculate expected data location
        size_t expected_data_start;
        if (header->texturedataindex != 0) {
            expected_data_start = header->texturedataindex;
        } else {
            expected_data_start = header->textureindex + (header->numtextures * sizeof(mstudiotexture_t));
        }
        printf("\nExpected texture data start: 0x%lX\n", expected_data_start);
        
        // Try to read palette size at expected location
        const unsigned char *test_data = file_data + expected_data_start + textures[0].index;
        const unsigned char *pal_ptr = test_data + (textures[0].width * textures[0].height);
        uint16_t test_pal_size;
        memcpy(&test_pal_size, pal_ptr, 2);
        printf("Palette size at expected location: %u\n", test_pal_size);
        
        // Show some hex dump
        printf("\nHex dump at palette location:\n");
        for (int i = 0; i < 32; i++) {
            printf("%02X ", pal_ptr[i]);
            if ((i + 1) % 16 == 0) printf("\n");
        }
        printf("\n");
    }
    printf("=========================\n\n");
}


// MODIFIED
mdl_result_t mdl_load_textures(const studiohdr_t   *header,
                               const unsigned char *file_data,
                               mdl_texture_set_t   *out_set)
{

    debug_texture_data(header, file_data);

    if (!out_set)
    {
        return MDL_ERROR_INVALID_PARAMETER;
    }

    out_set->textures = NULL;
    out_set->count    = 0;

    if (!header || !file_data)
    {
        return MDL_ERROR_MISSING_TEXTURE_FILE;
    }
    if (header->numtextures <= 0)
    {
        return MDL_ERROR_NO_TEXTURES_IN_FILE;
    }

    const mstudiotexture_t *textures = (const mstudiotexture_t *)(file_data + header->textureindex);
    const int n_textures = header->numtextures;

    mdl_gl_texture_t *items = (mdl_gl_texture_t *)calloc((size_t)n_textures, sizeof(*items));
    if (!items)
    {
        return MDL_ERROR_MEMORY_ALLOCATION;
    }

    // CRITICAL FIX: Calculate where texture data actually starts
    const unsigned char *texture_data_base;
    
    if (header->texturedataindex != 0) {
        // Normal case: data is in a separate section
        texture_data_base = file_data + header->texturedataindex;
    } else {
        // T.mdl case: data starts after all texture headers
        // Calculate: header->textureindex + (numtextures * sizeof(mstudiotexture_t))
        size_t texture_headers_size = n_textures * sizeof(mstudiotexture_t);
        texture_data_base = file_data + header->textureindex + texture_headers_size;
        printf("T.mdl detected: texture data starts at offset 0x%lX\n", 
               (unsigned long)(texture_data_base - file_data));
    }

    for (int i = 0; i < n_textures; i++)
    {
        const mstudiotexture_t *T = &textures[i];
        
        // T->index is an offset from texture_data_base
        const unsigned char *indices = texture_data_base + T->index;
        
        // Calculate pixel data size
        const int pixel_count = T->width * T->height;
        
        // Palette immediately follows pixel data
        const unsigned char *palette_ptr = indices + pixel_count;
        
        // Read palette size (16-bit little endian)
        uint16_t pal_size = 0;
        memcpy(&pal_size, palette_ptr, 2);
        
        // Palette RGB data starts after the size
        const unsigned char *palette = palette_ptr + 2;
        
        // Validate palette size
        if (pal_size == 0 || pal_size > 256) {
            printf("ERROR: Texture %d (%s) has invalid palette size %d\n", 
                   i, T->name, pal_size);
            items[i].gl_id = 0;
            items[i].width = 0;
            items[i].height = 0;
            items[i].name[0] = '\0';
            continue;
        }
        
        printf("Loading texture %d: %s (%dx%d), palette has %d colors\n", 
               i, T->name, T->width, T->height, pal_size);
        
        // Allocate RGBA buffer
        unsigned char *rgba = (unsigned char *)malloc((size_t)pixel_count * 4u);
        if (!rgba)
        {
            free(items);
            return MDL_ERROR_MEMORY_ALLOCATION;
        }
        
        // Convert palette indices to RGBA
        for (int j = 0; j < pixel_count; j++)
        {
            unsigned char idx = indices[j];
            
            // Handle transparency - index 255 is transparent in Half-Life
            if (idx == 255)
            {
                rgba[j * 4 + 0] = 0;    // R
                rgba[j * 4 + 1] = 0;    // G
                rgba[j * 4 + 2] = 0;    // B
                rgba[j * 4 + 3] = 0;    // A (transparent)
            }
            else
            {
                // Clamp index to palette size
                if (idx >= pal_size)
                    idx = 0;
                
                rgba[j * 4 + 0] = palette[idx * 3 + 0];  // R
                rgba[j * 4 + 1] = palette[idx * 3 + 1];  // G
                rgba[j * 4 + 2] = palette[idx * 3 + 2];  // B
                rgba[j * 4 + 3] = 255;                   // A (opaque)
            }
        }
        
        // Create OpenGL texture
        GLuint tex = 0;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        
        // Set pixel storage mode
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        
        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        // Upload texture data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, T->width, T->height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, rgba);
        
        // Check for OpenGL errors
        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
        {
            printf("  WARNING: OpenGL Error 0x%x after creating texture %s\n", err, T->name);
        }
        else
        {
            printf("  Successfully created GL texture ID %u for %s\n", tex, T->name);
        }
        
        // Unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);
        
        // Free temporary RGBA buffer
        free(rgba);
        
        // Store texture info
        items[i].gl_id = tex;
        items[i].width = T->width;
        items[i].height = T->height;
        strncpy(items[i].name, T->name, sizeof(items[i].name) - 1);
        items[i].name[sizeof(items[i].name) - 1] = '\0';
    }
    
    out_set->textures = items;
    out_set->count = n_textures;
    
    printf("Texture loading complete: %d textures loaded\n", n_textures);
    return MDL_SUCCESS;
}



void mdl_free_texture(mdl_texture_set_t *set)
{
    if (!set || !set->textures)
    {
        return;
    }

    for (int i = 0; i < set->count; i++)
    {
        if (set->textures[i].gl_id)
        {
            GLuint id = set->textures[i].gl_id;
            glDeleteTextures(1, &id);
        }
    }

    free(set->textures);
    set->textures = NULL;
    set->count    = 0;
}



