/*
 * ═══════════════════════════════════════════════════════════════════════════
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ═══════════════════════════════════════════════════════════════════════════
 *
 *   Copyright (c) 1996-2002, Valve LLC. All rights reserved.
 *
 *   This product contains software technology licensed from Id
 *   Software, Inc. ("Id Technology"). Id Technology (c) 1996 Id Software, Inc.
 *   All Rights Reserved.
 *
 *   Use, distribution, and modification of this source code and/or resulting
 *   object code is restricted to non-commercial enhancements to products from
 *   Valve LLC. All other use, distribution, or modification is prohibited
 *   without written permission from Valve LLC.
 *
 * ───────────────────────────────────────────────────────────────────────────
 *   Author: Karlo Siric
 *   Purpose: Command-Line Argument Parser Implementation
 * ═══════════════════════════════════════════════════════════════════════════
 */


#include "textures.h"

#include "../graphics/gl_platform.h"
#include "../utils/logger.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline const mstudiotexture_t *texture_array( const studiohdr_t *header, const unsigned char *data )
{
    return ( const mstudiotexture_t * ) ( data + header->textureindex );
}

const studiohdr_t *mdl_pick_texture_header( const studiohdr_t *main_header, const studiohdr_t *text_header )
{
    if ( main_header && main_header->numtextures > 0 )
        return main_header;
    if ( text_header && text_header->numtextures > 0 )
        return text_header;
    return NULL;
}

bool mdl_pal8_to_rgba(
    const unsigned char *indices,
    int                  w,
    int                  h,
    const unsigned char *palette_rgb,
    size_t               palette_size,
    unsigned char       *dst )
{
    if ( !indices || !palette_rgb || !dst || w <= 0 || h <= 0 )
        return false;
    if ( palette_size == 0 || palette_size > 256 )
        return false;

    const int px = w * h;

    // MDL palettes are effectively RGB (R,G,B).
    // Do NOT swap unless you verify otherwise.
    for ( int i = 0; i < px; ++i )
    {
        const unsigned idx  = indices[i];
        const unsigned p    = ( idx < ( unsigned ) palette_size ) ? idx : 0;
        const unsigned base = p * 3;

        dst[i * 4 + 0] = palette_rgb[base + 0];       // R
        dst[i * 4 + 1] = palette_rgb[base + 1];       // G
        dst[i * 4 + 2] = palette_rgb[base + 2];       // B
        dst[i * 4 + 3] = ( idx == 255 ) ? 0 : 255;    // 255 is transparent in many GoldSrc MDLs
    }
    return true;
}

static bool parse_paletted_block(
    const unsigned char  *text_struct_base,
    int                   width,
    int                   height,
    int                   index_offset,
    const unsigned char **out_indices,
    int                  *out_count,
    const unsigned char **out_palette,
    int                  *out_pal_size,
    const unsigned char  *file_start,
    size_t                file_size )
{
    if ( !text_struct_base || !file_start || !out_indices || !out_palette || !out_count || !out_pal_size || width <= 0
         || height <= 0 || index_offset < 0 )
        return false;

    const ptrdiff_t base_off = text_struct_base - file_start;
    if ( base_off < 0 || ( size_t ) base_off >= file_size )
        return false;

    const size_t pix_off   = ( size_t ) base_off + ( size_t ) index_offset;
    const size_t pixels_sz = ( size_t ) width * ( size_t ) height;

    if ( pix_off > file_size || pixels_sz > file_size - pix_off )
        return false;

    const size_t after_pixels = pix_off + pixels_sz;

    // MDL format: uint16 palette size (little endian), then palette (pal_size *
    if ( after_pixels + 2 > file_size )
        return false;

    uint16_t pal_count_le = 0;
    memcpy( &pal_count_le, file_start + after_pixels, 2 );

    if ( pal_count_le == 0 || pal_count_le > 256 )
        return false;

    const size_t pal_bytes_off = after_pixels + 2;
    const size_t pal_bytes_len = ( size_t ) pal_count_le * 3u;

    if ( pal_bytes_off > file_size || pal_bytes_len > file_size - pal_bytes_off )
        return false;

    *out_indices  = file_start + pix_off;
    *out_count    = ( int ) pixels_sz;
    *out_palette  = file_start + pal_bytes_off;
    *out_pal_size = ( int ) pal_count_le;

    return true;
}

mdl_result_t mdl_load_textures( const studiohdr_t *header, const unsigned char *file_data, mdl_texture_set_t *out_set )
{
    // debug_texture_data( header, file_data );

    if ( !out_set )
    {
        return MDL_ERROR_INVALID_PARAMETER;
    }

    out_set->textures = NULL;
    out_set->count    = 0;

    if ( !header || !file_data )
    {
        return MDL_ERROR_MISSING_TEXTURE_FILE;
    }
    if ( header->numtextures <= 0 )
    {
        return MDL_ERROR_NO_TEXTURES_IN_FILE;
    }

    const mstudiotexture_t *textures   = ( const mstudiotexture_t * ) ( file_data + header->textureindex );
    const int               n_textures = header->numtextures;

    mdl_gl_texture_t *items = ( mdl_gl_texture_t * ) calloc( ( size_t ) n_textures, sizeof( *items ) );
    if ( !items )
    {
        return MDL_ERROR_MEMORY_ALLOCATION;
    }

    for ( int i = 0; i < n_textures; i++ )
    {
        const mstudiotexture_t *T = &textures[i];

        // T->index is an absolute offset from file start
        const unsigned char *indices = file_data + T->index;

        // Calculate pixel data size
        const int pixel_count = T->width * T->height;

        const unsigned char *palette  = indices + pixel_count;
        const int            pal_size = 256;

        // Allocate RGBA buffer
        unsigned char *rgba = ( unsigned char * ) malloc( ( size_t ) pixel_count * 4u );
        if ( !rgba )
        {
            free( items );
            return MDL_ERROR_MEMORY_ALLOCATION;
        }
        /* NOTE(Karlo):
        // 
        // Convert palette indices to RGBA
        // Need to modify this to detect if it is RGBA or BGR or something else
        // RIght now issue is happening because some models maybe store things
        // differently so it is not quite RGB, most of them are RGB but some are 
        // as it seems quite obvious when I was testing models not quite right so 
        // we need to add that as well.
        // 
        */
        for ( int j = 0; j < pixel_count; j++ )
        {
            unsigned char idx = indices[j];

            // Handle transparency - index 255 is transparent in Half-Life
            if ( idx == 255 )
            {
                rgba[j * 4 + 0] = 0;    // R
                rgba[j * 4 + 1] = 0;    // G
                rgba[j * 4 + 2] = 0;    // B
                rgba[j * 4 + 3] = 0;    // A (transparent)
            }
            else
            {
                // Clamp index to palette size
                if ( idx >= pal_size )
                    idx = 0;

                rgba[j * 4 + 0] = palette[idx * 3 + 0];    // R
                rgba[j * 4 + 1] = palette[idx * 3 + 1];    // G
                rgba[j * 4 + 2] = palette[idx * 3 + 2];    // B
                rgba[j * 4 + 3] = 255;                     // A (opaque)
            }
        }

        // Create OpenGL texture
        GLuint tex = 0;
        glGenTextures( 1, &tex );
        glBindTexture( GL_TEXTURE_2D, tex );

        // Set pixel storage mode
        glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

        // Set texture parameters
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

        // Upload texture data
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, T->width, T->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba );

        // Check for OpenGL errors
        GLenum err = glGetError( );
        if ( err != GL_NO_ERROR )
        {
            LOG_WARNF( "textures", "OpenGL error 0x%x creating texture %s", err, T->name );
        }
        else
        {
            LOG_TRACEF( "textures", "Created GL texture ID %u for %s", tex, T->name );
        }
        // Unbind texture
        glBindTexture( GL_TEXTURE_2D, 0 );

        // Free temporary RGBA buffer
        free( rgba );

        // Store texture info
        items[i].gl_id  = tex;
        items[i].width  = T->width;
        items[i].height = T->height;
        items[i].flags  = T->flags;
        strncpy( items[i].name, T->name, sizeof( items[i].name ) - 1 ); 
        items[i].name[sizeof( items[i].name ) - 1] = '\0';
    }

    out_set->textures = items;
    out_set->count    = n_textures;

    return MDL_SUCCESS;
}

void mdl_free_texture( mdl_texture_set_t *set )
{
    if ( !set || !set->textures )
    {
        return;
    }

    for ( int i = 0; i < set->count; i++ )
    {
        if ( set->textures[i].gl_id )
        {
            GLuint id = set->textures[i].gl_id;
            glDeleteTextures( 1, &id );
        }
    }

    free( set->textures );
    set->textures = NULL;
    set->count    = 0;
}
