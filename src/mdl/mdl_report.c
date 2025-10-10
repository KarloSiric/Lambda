/*======================================================================
   File: mdl_report.c
   Project: shaders
   Author: karlosiric <email@example.com>
   Created: 2025-10-09 23:02:35
   Last Modified by: karlosiric
   Last Modified: 2025-10-10 16:25:12
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

// This function already prints things for the
void print_complete_model_analysis(
    const char *, const studiohdr_t *, const studiohdr_t *, const unsigned char *, const unsigned char * );

#define DUMP_I32( L, V ) printf( "  %-22s %d\n", ( L ), ( int ) ( V ) )
#define DUMP_HEX( L, V ) printf( "  %-22s 0x%08X\n", ( L ), ( unsigned ) ( V ) )
#define DUMP_STR( L, V ) printf( "  %-22s %s\n", ( L ), ( V ) )
#define DUMP_V3( L, V )                                                                                                \
    printf( "  %-22s (%.3f, %.3f, %.3f)\n", ( L ), ( double ) ( V )[0], ( double ) ( V )[1], ( double ) ( V )[2] )
#define DUMP_OFF( L, BASE, OFF )                                                                                       \
    printf(                                                                                                            \
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

void print_studio_header_file( const char *title, const studiohdr_t *header )
{
    if ( !header )
    {
        printf( "%s\n  (null)\n", title ? title : "STUDIO HEADER" );
        return;
    }

    const unsigned char *base = ( const unsigned char * ) header;

    printf( "RAW STUDIO HEADER (all fields)\n" );
    printf( "%s\n", RULER );
    if ( title )
    {
        printf( "  %s\n\n", title );
    }

    DUMP_HEX( "id", header->id );
    DUMP_I32( "version", header->version );
    DUMP_STR( "name", header->name );
    DUMP_I32( "length", header->length );

    // Positions and bounds for each model
    DUMP_V3( "eyeposition", header->eyeposition );
    DUMP_V3( "min", header->min );
    DUMP_V3( "max", header->max );
    DUMP_V3( "bbmin", header->bbmin );
    DUMP_V3( "bbmax", header->bbmax );

    DUMP_HEX( "flags", header->flags );

    DUMP_I32( "numbones", header->numbones );
    DUMP_OFF( "boneindes", base, header->boneindex );

    DUMP_I32( "numbonecontrollers", header->numbonecontrollers );
    DUMP_OFF( "bonecontrollerindex", base, header->bonecontrollerindex );

    // Hitboxes
    DUMP_I32( "numhitboxes", header->numhitboxes );
    DUMP_OFF( "hitboxindex", base, header->hitboxindex );

    // Sequences + groups
    DUMP_I32( "numseq", header->numseq );
    DUMP_OFF( "seqindex", base, header->seqindex );
    DUMP_I32( "numseqgroups", header->numseqgroups );
    DUMP_OFF( "seqgroupindex", base, header->seqgroupindex );

    // Textures (often zero in main header; real values in texture header)
    DUMP_I32( "numtextures", header->numtextures );
    DUMP_OFF( "textureindex", base, header->textureindex );
    DUMP_OFF( "texturedataindex", base, header->texturedataindex );

    // Skins
    DUMP_I32( "numskinref", header->numskinref );
    DUMP_I32( "numskinfamilies", header->numskinfamilies );
    DUMP_OFF( "skinindex", base, header->skinindex );

    // Bodyparts
    DUMP_I32( "numbodyparts", header->numbodyparts );
    DUMP_OFF( "bodypartindex", base, header->bodypartindex );

    // Attachments
    DUMP_I32( "numattachments", header->numattachments );
    DUMP_OFF( "attachmentindex", base, header->attachmentindex );

    // Sounds (often unused)
    DUMP_I32( "soundtable", header->soundtable );
    DUMP_OFF( "soundindex", base, header->soundindex );
    DUMP_I32( "soundgroups", header->soundgroups );
    DUMP_OFF( "soundgroupindex", base, header->soundgroupindex );

    // Transitions
    DUMP_I32( "numtransitions", header->numtransitions );
    DUMP_OFF( "transitionindex", base, header->transitionindex );

    printf( "%s\n", RULER );
}



