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


#include "utils.h"

// Derive the base model path from a sequence group file
bool mdl_derive_base_path( const char *seq_path, char *out, size_t out_size )
{
    if ( !seq_path || !out )
        return false;

    // Copy the path
    strncpy( out, seq_path, out_size );
    out[out_size - 1] = '\0';

    // Find the .mdl extension
    char *ext = strrchr( out, '.' );
    if ( !ext || strcasecmp( ext, ".mdl" ) != 0 )
    {
        return false;    // Not an .mdl
    }

    // Walk backwards from extension and strip digits
    char *p = ext - 1;
    while ( p >= out && isdigit( ( unsigned char ) *p ) )
    {
        *p = '\0';
        p--;
    }

    // Re-append .mdl
    strcat( out, ".mdl" );
    return true;
}
