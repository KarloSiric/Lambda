/*======================================================================
   File: mdl_report.c
   Project: shaders
   Author: karlosiric <email@example.com>
   Created: 2025-10-09 23:02:35
   Last Modified by: karlosiric
   Last Modified: 2025-10-14 19:05:46
   ---------------------------------------------------------------------
   Description:
       
   ---------------------------------------------------------------------
   License: 
   Company: 
   Version: 0.1.0
 ======================================================================
                                                                       */


#include "mdl_report.h"

#include <stddef.h>
#include <stdlib.h>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#define DUP    _dup
#define DUP2   _dup2
#define FILENO _fileno
#define CLOSE  _close
#else
#include <unistd.h>
#define DUP    dup
#define DUP2   dup2
#define FILENO fileno
#define CLOSE  close
#endif

static const char *RULER      = "────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────";
static const char *RULER_THIN = "────────────────────────────────────────────────────────────────────────────────";

#define DUMP_I32( FP, L, V ) fprintf( (FP), "  %-22s %d\n", ( L ), ( int ) ( V ) )
#define DUMP_HEX( FP, L, V ) fprintf( (FP), "  %-22s 0x%08X\n", ( L ), ( unsigned ) ( V ) )
#define DUMP_STR( FP, L, V ) fprintf( (FP), "  %-22s %s\n", ( L ), ( V ) )
#define DUMP_V3( FP, L, V )                                                                                                \
    fprintf( (FP), "  %-22s (%.3f, %.3f, %.3f)\n", ( L ), ( double ) ( V )[0], ( double ) ( V )[1], ( double ) ( V )[2] )
#define DUMP_OFF( FP, L, BASE, OFF )                                                                                       \
    fprintf( (FP),                                                                                                            \
        "  %-22s 0x%X (abs: %p)\n",                                                                                    \
        ( L ),                                                                                                         \
        ( unsigned ) ( OFF ),                                                                                          \
        ( const void * ) ( const unsigned char * ) ( BASE + ( size_t ) ( OFF ) ) )

// Making some small helper functions to help -- HELPERS

static inline const unsigned char *add_off( const void *base, size_t off )
{
    return ( const unsigned char * ) ( base + off );
}

static inline int in_range( const void *base, size_t len, const void *p, size_t need )
{
    const uintptr_t b = ( uintptr_t ) base;
    const uintptr_t e = b + len;
    const uintptr_t q = ( uintptr_t ) p;

    return ( q >= b ) && ( q + need <= e );
}

void print_studio_header_file( FILE *output, const char *title, const studiohdr_t *header )
{
    if ( !output ) output = stdout;
    
    if ( !header )
    {
        fprintf( output, "%s\n  (null)\n", title ? title : "STUDIO HEADER" );
        return;
    }

    const unsigned char *base = ( const unsigned char * ) header;

    fprintf( output, "RAW STUDIO HEADER (all fields)\n" );
    fprintf( output, "%s\n", RULER );
    if ( title )
    {
        fprintf( output, "  %s\n\n", title );
    }

    DUMP_HEX( output, "id", header->id );
    DUMP_I32( output, "version", header->version );
    DUMP_STR( output, "name", header->name );
    DUMP_I32( output, "length", header->length );

    // Positions and bounds for each model
    DUMP_V3( output, "eyeposition", header->eyeposition );
    DUMP_V3( output, "min", header->min );
    DUMP_V3( output, "max", header->max );
    DUMP_V3( output, "bbmin", header->bbmin );
    DUMP_V3( output, "bbmax", header->bbmax );

    DUMP_HEX( output, "flags", header->flags );

    DUMP_I32( output, "numbones", header->numbones );
    DUMP_OFF( output, "boneindes", base, header->boneindex );

    DUMP_I32( output, "numbonecontrollers", header->numbonecontrollers );
    DUMP_OFF( output, "bonecontrollerindex", base, header->bonecontrollerindex );

    // Hitboxes
    DUMP_I32( output, "numhitboxes", header->numhitboxes );
    DUMP_OFF( output, "hitboxindex", base, header->hitboxindex );

    // Sequences + groups
    DUMP_I32( output, "numseq", header->numseq );
    DUMP_OFF( output, "seqindex", base, header->seqindex );
    DUMP_I32( output, "numseqgroups", header->numseqgroups );
    DUMP_OFF( output, "seqgroupindex", base, header->seqgroupindex );

    // Textures (often zero in main header; real values in texture header)
    DUMP_I32( output, "numtextures", header->numtextures );
    DUMP_OFF( output, "textureindex", base, header->textureindex );
    DUMP_OFF( output, "texturedataindex", base, header->texturedataindex );

    // Skins
    DUMP_I32( output, "numskinref", header->numskinref );
    DUMP_I32( output, "numskinfamilies", header->numskinfamilies );
    DUMP_OFF( output, "skinindex", base, header->skinindex );

    // Bodyparts
    DUMP_I32( output, "numbodyparts", header->numbodyparts );
    DUMP_OFF( output, "bodypartindex", base, header->bodypartindex );

    // Attachments
    DUMP_I32( output, "numattachments", header->numattachments );
    DUMP_OFF( output, "attachmentindex", base, header->attachmentindex );

    // Sounds (often unused)
    DUMP_I32( output, "soundtable", header->soundtable );
    DUMP_OFF( output, "soundindex", base, header->soundindex );
    DUMP_I32( output, "soundgroups", header->soundgroups );
    DUMP_OFF( output, "soundgroupindex", base, header->soundgroupindex );

    // Transitions
    DUMP_I32( output, "numtransitions", header->numtransitions );
    DUMP_OFF( output, "transitionindex", base, header->transitionindex );

    fprintf( output, "%s\n", RULER );
}



