/*======================================================================
 *  File: mdl_messages.c
 *  Project: shaders
 *  Author: karlosiric <email@example.com>
 *  Created: 2025-09-27 15:08:00
 *  Last Modified by: karlosiric
 *  Last Modified: 2025-09-29 17:47:28
 *----------------------------------------------------------------------
 *  Description:
 *      
 *----------------------------------------------------------------------
 *  License: 
 *  Company: 
 *  Version: 0.1.0
 *======================================================================
 */

#include "mdl_messages.h"

#include <stdio.h>
#include <string.h>

static const char *k_result_names[MDL_RESULT_COUNT] = {
    [MDL_SUCCESS]                 = "SUCCESS",
    [MDL_ERROR_FILE_NOT_FOUND]    = "FILE_NOT_FOUND",
    [MDL_ERROR_INVALID_PARAMETER] = "INVALID_PARAMETER",
    [MDL_ERROR_MEMORY_ALLOCATION] = "MEMORY_ALLOCATION",
    [MDL_ERROR_FILE_TOO_SMALL]    = "FILE_TOO_SMALL",

    [MDL_ERROR_INVALID_MAGIC]      = "INVALID_MAGIC",
    [MDL_ERROR_INVALID_VERSION]    = "INVALID_VERSION",
    [MDL_INFO_SEQUENCE_GROUP_FILE] = "SEQUENCE_GROUP_FILE",
    [MDL_INFO_TEXTURE_MODEL_FILE]  = "TEXTURE_MODEL_FILE",

    [MDL_ERROR_BONE_COUNT_INVALID]       = "BONE_COUNT_INVALID",
    [MDL_ERROR_BODYPART_COUNT_INVALID]   = "BODYPART_COUNT_INVALID",
    [MDL_ERROR_MODEL_INDEX_INVALID]      = "MODEL_INDEX_INVALID",
    [MDL_ERROR_MESH_INDEX_INVALID]       = "MESH_INDEX_INVALID",
    [MDL_ERROR_VERT_INDEX_INVALID]       = "VERT_INDEX_INVALID",
    [MDL_ERROR_NORM_INDEX_INVALID]       = "NORM_INDEX_INVALID",
    [MDL_ERROR_TRICMD_BROKEN]            = "TRICMD_BROKEN",
    [MDL_ERROR_BONE_PARENT_OUT_OF_RANGE] = "BONE_PARENT_OUT_OF_RANGE",
    [MDL_ERROR_TOO_MANY_BONES]           = "TOO_MANY_BONES",
    [MDL_ERROR_TOO_MANY_VERTS]           = "TOO_MANY_VERTS",

    [MDL_ERROR_MISSING_TEXTURE_FILE]       = "MISSING_TEXTURE_FILE",
    [MDL_ERROR_TEXTURE_FORMAT_UNSUPPORTED] = "TEXTURE_FORMAT_UNSUPPORTED",
    [MDL_ERROR_SKIN_FAMILY_OUT_OF_RANGE]   = "SKIN_FAMILY_OUT_OF_RANGE",

    [MDL_ERROR_SEQGROUP_NOT_FOUND] = "SEQGROUP_NOT_FOUND",
    [MDL_ERROR_ANIM_BLOCK_BROKEN]  = "ANIM_BLOCK_BROKEN",

    [MDL_ERROR_NO_TEXTURES_IN_FILE] = "NO_TEXTURES",

    [MDL_ERROR_NOT_IMPLEMENTED] = "NOT_IMPLEMENTED",
};

static const char *k_result_defaults[MDL_RESULT_COUNT]
    = { [MDL_SUCCESS]                 = "Success.",
        [MDL_ERROR_FILE_NOT_FOUND]    = "File not found.",
        [MDL_ERROR_INVALID_PARAMETER] = "Invalid parameter passed to function.",
        [MDL_ERROR_MEMORY_ALLOCATION] = "Failed to allocate required memory.",
        [MDL_ERROR_FILE_TOO_SMALL]    = "File is too small to contain a valid MDL.",

        [MDL_ERROR_INVALID_MAGIC]      = "Invalid MDL header magic (not IDST/IDSQ).",
        [MDL_ERROR_INVALID_VERSION]    = "Unsupported MDL version.",
        [MDL_INFO_SEQUENCE_GROUP_FILE] = "This is a sequence-group MDL (animations only). Open the base model instead.",
        [MDL_INFO_TEXTURE_MODEL_FILE]  = "This is a texture-model MDL. Use it as a companion, not as the main model.",

        [MDL_ERROR_BONE_COUNT_INVALID]       = "Invalid bone count or exceeds limits.",
        [MDL_ERROR_BODYPART_COUNT_INVALID]   = "Invalid bodypart count or exceeds limits.",
        [MDL_ERROR_MODEL_INDEX_INVALID]      = "Corrupt model index/offset.",
        [MDL_ERROR_MESH_INDEX_INVALID]       = "Corrupt mesh index/offset.",
        [MDL_ERROR_VERT_INDEX_INVALID]       = "Corrupt vertex index/offset.",
        [MDL_ERROR_NORM_INDEX_INVALID]       = "Corrupt normal index/offset.",
        [MDL_ERROR_TRICMD_BROKEN]            = "Triangle command stream is malformed.",
        [MDL_ERROR_BONE_PARENT_OUT_OF_RANGE] = "Bone parent index is out of range.",
        [MDL_ERROR_TOO_MANY_BONES]           = "Bone count exceeds engine limit.",
        [MDL_ERROR_TOO_MANY_VERTS]           = "Vertex count exceeds engine limit.",

        [MDL_ERROR_MISSING_TEXTURE_FILE]       = "Main MDL has no textures; companion texture MDL not found.",
        [MDL_ERROR_TEXTURE_FORMAT_UNSUPPORTED] = "Unsupported texture format (expected paletted 8bpp).",
        [MDL_ERROR_SKIN_FAMILY_OUT_OF_RANGE]   = "Requested skin family index is out of range.",

        [MDL_ERROR_SEQGROUP_NOT_FOUND] = "Sequence group file not found for this sequence.",
        [MDL_ERROR_ANIM_BLOCK_BROKEN]  = "Animation block is malformed.",

        [MDL_ERROR_NO_TEXTURES_IN_FILE] = "No textures were found in the given file.",

        [MDL_ERROR_NOT_IMPLEMENTED] = "Feature recognized but not implemented." };

const char *mdl_result_name( mdl_result_t r )
{
    if ( ( unsigned ) r < ( unsigned ) MDL_RESULT_COUNT && k_result_names[r] )
        return k_result_names[r];
    return "UNKNOWN";
}

const char *mdl_result_default_text( mdl_result_t r )
{
    if ( ( unsigned ) r < ( unsigned ) MDL_RESULT_COUNT && k_result_defaults[r] )
        return k_result_defaults[r];
    return "Unknown result.";
}

size_t mdl_format_message( char *out, size_t cap, mdl_result_t r, const mdl_msg_ctx_t *ctx )
{
    if ( !out || cap == 0 )
        return 0;
    const char *name = mdl_result_name( r );

    // Common fields (safe fallbacks)
    const char *path     = ( ctx && ctx->path ) ? ctx->path : "<unknown>";
    const char *base     = ( ctx && ctx->base_path ) ? ctx->base_path : NULL;
    int         version  = ctx ? ctx->version : -1;
    int         expected = ctx ? ctx->expected : -1;
    int         got      = ctx ? ctx->got : -1;

    // Tailor a few high-value messages; otherwise use the default text.
    switch ( r )
    {
    case MDL_INFO_SEQUENCE_GROUP_FILE:
        if ( base )
        {
            return ( size_t ) snprintf(
                out,
                cap,
                "INFO [%s]: '%s' is a sequence-group MDL (animations only). "
                "Open the base model instead: %s",
                name,
                path,
                base );
        }
        return ( size_t ) snprintf(
            out,
            cap,
            "INFO [%s]: '%s' is a sequence-group MDL (animations only). "
            "Open the base model (same name without trailing digits).",
            name,
            path );

    case MDL_ERROR_INVALID_MAGIC:
        return ( size_t ) snprintf( out, cap, "ERROR [%s]: '%s' is not an IDST/IDSQ MDL (invalid magic).", name, path );

    case MDL_ERROR_INVALID_VERSION:
        if ( version >= 0 )
        {
            return ( size_t ) snprintf(
                out, cap, "ERROR [%s]: '%s' has unsupported MDL version %d.", name, path, version );
        }
        break;

    case MDL_ERROR_MISSING_TEXTURE_FILE:
        return ( size_t ) snprintf(
            out, cap, "WARN  [%s]: '%s' has no embedded textures and companion texture MDL is missing.", name, path );

    case MDL_ERROR_BONE_PARENT_OUT_OF_RANGE:
        return ( size_t ) snprintf(
            out, cap, "ERROR [%s]: '%s' bone parent index out of range (got=%d, max=%d).", name, path, got, expected );

    default:
        break;
    }

    // Fallback generic
    return ( size_t ) snprintf( out, cap, "%s: %s", mdl_result_name( r ), mdl_result_default_text( r ) );
}

// Simple stderr printer
void mdl_print_message( mdl_result_t r, const mdl_msg_ctx_t *ctx )
{
    char line[1024];
    mdl_format_message( line, sizeof line, r, ctx );
    fprintf( stderr, "%s\n", line );
}
