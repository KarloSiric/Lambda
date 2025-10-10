/*======================================================================
   File: mdl_info.c
   Project: shaders
   Author: karlosiric <email@example.com>
   Created: 2025-10-09 18:40:38
   Last Modified by: karlosiric
   Last Modified: 2025-10-10 10:17:09
   ---------------------------------------------------------------------
   Description:
       
   ---------------------------------------------------------------------
   License: 
   Company: 
   Version: 0.1.0
 ======================================================================
                                                                       */


#include "mdl_info.h"

#include "mdl_loader.h"

#include <stdio.h>

void print_complete_model_analysis(
    const char    *filename,
    studiohdr_t   *main_header,
    studiohdr_t   *texture_header,
    unsigned char *main_data,
    unsigned char *texture_data )
{
    /* Pretty rulers */
    static const char *RULER      = "──────────────────────────────────────────────────────────────────────────────";
    static const char *RULER_THIN = "────────────────────────────────────────";

    printf( "\n%s\n", RULER );
    printf( " Half-Life MDL Analysis Report\n" );
    printf( " File: %s\n", filename );
    printf( "%s\n\n", RULER );

    /* Quick status */
    printf( "STATUS: %-10s  %s\n\n", "SUCCESS", "Model loaded completely" );

    /* --- Main header summary ------------------------------------------------- */
    printf( "MAIN MODEL INFO\n" );
    printf( "%s\n", RULER_THIN );
    printf( "  %-14s %s\n", "Name:", main_header->name );
    printf( "  %-14s %d bytes\n", "File Size:", main_header->length );
    printf( "  %-14s %d\n", "Bones:", main_header->numbones );
    printf( "  %-14s %d\n", "Bodyparts:", main_header->numbodyparts );
    printf( "  %-14s %d\n", "Sequences:", main_header->numseq );
    printf( "\n" );

    /* --- Textures (delegated) ------------------------------------------------ */
    print_texture_info( texture_header, texture_data );
    printf( "\n" );

    /* --- Bodyparts summary (delegated) -------------------------------------- */
    print_bodypart_info( main_header, main_data );
    printf( "\n" );

    /* --- Bones --------------------------------------------------------------- */
    mstudiobone_t *bones       = NULL;
    mdl_result_t   bone_result = parse_bone_hierarchy( main_header, main_data, &bones );
    if ( bone_result == MDL_SUCCESS && bones )
    {
        printf( "BONE HIERARCHY (%d bones)\n", main_header->numbones );
        printf( "%s\n", RULER_THIN );
        print_bone_info( bones, main_header->numbones );
        printf( "\n" );
    }
    else
    {
        fprintf( stderr, "ERROR: Failed to parse bone hierarchy (code %d)\n\n", bone_result );
    }

    /* --- Sequences ----------------------------------------------------------- */
    mstudioseqdesc_t *sequences       = NULL;
    mdl_result_t      sequence_result = parse_animation_sequences( main_header, main_data, &sequences );
    if ( sequence_result == MDL_SUCCESS && sequences )
    {
        printf( "ANIMATION SEQUENCES (%d sequences)\n", main_header->numseq );
        printf( "%s\n", RULER_THIN );
        print_sequence_info( sequences, main_header->numseq );
        printf( "\n" );
    }
    else
    {
        fprintf( stderr, "ERROR: Failed to parse animations (code %d)\n\n", sequence_result );
    }

    /* --- Deep dive per bodypart/model/mesh ---------------------------------- */
    printf( "DETAILED MODEL ANALYSIS\n" );
    printf( "%s\n", RULER );

    mstudiobodyparts_t *bodyparts = ( mstudiobodyparts_t * ) ( main_data + main_header->bodypartindex );

    for ( int bp = 0; bp < main_header->numbodyparts; ++bp )
    {
        const mstudiobodyparts_t *B = &bodyparts[bp];
        printf( "\n[Bodypart %d] %-20s  (models: %d)\n", bp, B->name, B->nummodels );
        printf( "%s\n", RULER_THIN );

        mstudiomodel_t *models = ( mstudiomodel_t * ) ( main_data + B->modelindex );

        for ( int mi = 0; mi < B->nummodels; ++mi )
        {
            mstudiomodel_t *M = &models[mi];

            /* High-level model info (delegated) */
            print_model_info( M, bp, mi );

            /* Meshes (parse + print) */
            mstudiomesh_t *meshes      = NULL;
            mdl_result_t   mesh_result = parse_mesh_data( M, main_data, &meshes );
            if ( mesh_result == MDL_SUCCESS && meshes )
            {
                print_mesh_data( meshes, M, M->nummesh );
                print_simple_triangle_info( M, bp, mi );
            }
            else
            {
                printf( "  %-12s %s\n", "Meshes:", "FAILED to parse" );
                printf( "    Model: %s\n", M->name[0] ? M->name : "(unnamed)" );
            }

            /* Vertex peek (first / last) */
            vec3_t *vertices = NULL;
            if ( parse_vertex_data( M, main_data, &vertices ) == MDL_SUCCESS )
            {
                if ( vertices && M->numverts > 0 )
                {
                    printf( "  %-12s\n", "Vertices:" );
                    printf( "    First: (%.2f, %.2f, %.2f)\n", vertices[0][0], vertices[0][1], vertices[0][2] );
                }
                if ( M->numverts > 1 )
                {
                    printf(
                        "    Last:  (%.2f, %.2f, %.2f)\n",
                        vertices[M->numverts - 1][0],
                        vertices[M->numverts - 1][1],
                        vertices[M->numverts - 1][2] );
                }
            }
        }
    }

    printf("\n%s\n", RULER);
    printf(" Complete model analysis completed\n");
    printf("%s\n\n", RULER);
}
