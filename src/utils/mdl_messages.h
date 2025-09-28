#ifndef MDLMESSAGES_H
#define MDLMESSAGES_H 

#pragma once
#include <stddef.h>
#include <stdbool.h>


// --- Magic constants (keep where you define your MDL structs) ---
// --- Already defined in studio.h but still keeping it just for safety fallback ---


#define IDST_MAGIC 0x54534449u  // 'I''D''S''T' little-endian
#define IDSQ_MAGIC 0x51534449u  // 'I''D''S''Q'


// --- Result codes for loader / parser ---
typedef enum {
    // generic
    MDL_SUCCESS = 0,
    MDL_ERROR_FILE_NOT_FOUND,
    MDL_ERROR_INVALID_PARAMETER,
    MDL_ERROR_MEMORY_ALLOCATION,
    MDL_ERROR_FILE_TOO_SMALL,

    // header-level
    MDL_ERROR_INVALID_MAGIC,
    MDL_ERROR_INVALID_VERSION,
    MDL_INFO_SEQUENCE_GROUP_FILE,   // IDSQ: valid but not a standalone “viewable” model
    MDL_INFO_TEXTURE_MODEL_FILE,    // IDST but a “texture-only” MDL opened directly (optional use)

    // structural consistency
    MDL_ERROR_BONE_COUNT_INVALID,
    MDL_ERROR_BODYPART_COUNT_INVALID,
    MDL_ERROR_MODEL_INDEX_INVALID,
    MDL_ERROR_MESH_INDEX_INVALID,
    MDL_ERROR_VERT_INDEX_INVALID,
    MDL_ERROR_NORM_INDEX_INVALID,
    MDL_ERROR_TRICMD_BROKEN,
    MDL_ERROR_BONE_PARENT_OUT_OF_RANGE,
    MDL_ERROR_TOO_MANY_BONES,       // exceeds your MAXSTUDIOBONES
    MDL_ERROR_TOO_MANY_VERTS,       // exceeds your MAXSTUDIOVERTS

    // textures
    MDL_ERROR_MISSING_TEXTURE_FILE, // main has 0 textures but t.mdl missing
    MDL_ERROR_TEXTURE_FORMAT_UNSUPPORTED, // e.g., not pal8
    MDL_ERROR_SKIN_FAMILY_OUT_OF_RANGE,

    // animations
    MDL_ERROR_SEQGROUP_NOT_FOUND,   // seqdesc points to NN but file missing
    MDL_ERROR_ANIM_BLOCK_BROKEN,

    MDL_ERROR_NO_TEXTURES_IN_FILE,

    // Additional features
    MDL_ERROR_NOT_IMPLEMENTED,


    // keep last
    MDL_RESULT_COUNT
} mdl_result_t;


typedef struct {
    const char* path;        // model the user tried to open
    const char* base_path;   // derived base model suggestion (for IDSQ)
    int         version;     // header->version
    int         expected;    // generic slot (e.g., expected version)
    int         got;         // generic slot (e.g., got count)
} mdl_msg_ctx_t;


// Core mapping
const char* mdl_result_name(mdl_result_t r);
const char* mdl_result_default_text(mdl_result_t r);


// Formats a user-facing line into out (returns #bytes written)
size_t mdl_format_message(char* out, size_t cap, mdl_result_t r, const mdl_msg_ctx_t* ctx);


// Convenience logger-style printer (to stderr)
void mdl_print_message(mdl_result_t r, const mdl_msg_ctx_t* ctx);




#endif
