/*======================================================================
 *  File: utils.c
 *  Project: shaders
 *  Author: karlosiric <email@example.com>
 *  Created: 2025-07-31 21:32:03
 *  Last Modified by: karlosiric
 *  Last Modified: 2025-09-27 16:47:36
 *----------------------------------------------------------------------
 *  Description:
 *      
 *----------------------------------------------------------------------
 *  License: 
 *  Company: 
 *  Version: 0.1.0
 *======================================================================
 */
#include "utils.h"


// Derive the base model path from a sequence group file
bool mdl_derive_base_path(const char *seq_path, char *out, size_t out_size) {
    if (!seq_path || !out) return false;

    // Copy the path
    strncpy(out, seq_path, out_size);
    out[out_size - 1] = '\0';

    // Find the .mdl extension
    char *ext = strrchr(out, '.');
    if (!ext || strcasecmp(ext, ".mdl") != 0) {
        return false; // Not an .mdl
    }

    // Walk backwards from extension and strip digits
    char *p = ext - 1;
    while (p >= out && isdigit((unsigned char)*p)) {
        *p = '\0';
        p--;
    }

    // Re-append .mdl
    strcat(out, ".mdl");
    return true;
}

