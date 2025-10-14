/*======================================================================
   File: mdl_info.c
   Project: shaders
   Author: karlosiric <email@example.com>
   Created: 2025-10-09 18:40:38
   Last Modified by: karlosiric
   Last Modified: 2025-10-14 19:44:53
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
    FILE              *output,      // <-- ADD THIS PARAMETER!
    const char        *filename,
    const studiohdr_t       *main_header,
    const studiohdr_t       *texture_header,
    const unsigned char     *main_data,
    const unsigned char     *texture_data )
{
    /* Pretty rulers */
    static const char *RULER      = "──────────────────────────────────────────────────────────────────────────────";
    static const char *RULER_THIN = "────────────────────────────────────────";

    // Change ALL printf to fprintf(output, ...)
    fprintf( output, "\n%s\n", RULER );
    fprintf( output, " Half-Life MDL Analysis Report\n" );
    fprintf( output, " File: %s\n", filename );
    fprintf( output, "%s\n\n", RULER );

    fprintf( output, "STATUS: %-10s  %s\n\n", "SUCCESS", "Model loaded completely" );

    fprintf( output, "MAIN MODEL INFO\n" );
    fprintf( output, "%s\n", RULER_THIN );
    fprintf( output, "  %-14s %s\n", "Name:", main_header->name );
    fprintf( output, "  %-14s %d bytes\n", "File Size:", main_header->length );
    fprintf( output, "  %-14s %d\n", "Bones:", main_header->numbones );
    fprintf( output, "  %-14s %d\n", "Bodyparts:", main_header->numbodyparts );
    fprintf( output, "  %-14s %d\n", "Sequences:", main_header->numseq );
    fprintf( output, "\n" );

    /* --- Textures (delegated) ------------------------------------------------ */
    print_texture_info( output, texture_header, texture_data );  // Pass output here too!
    fprintf( output, "\n" );

    /* --- Bodyparts summary (delegated) -------------------------------------- */
    print_bodypart_info( output, main_header, main_data );  // Pass output here too!
    fprintf( output, "\n" );

    /* --- Bones --------------------------------------------------------------- */
    mstudiobone_t *bones       = NULL;
    mdl_result_t   bone_result = parse_bone_hierarchy( main_header, main_data, &bones );
    if ( bone_result == MDL_SUCCESS && bones )
    {
        fprintf( output, "BONE HIERARCHY (%d bones)\n", main_header->numbones );
        fprintf( output, "%s\n", RULER_THIN );
        print_bone_info( output, bones, main_header->numbones );  // Pass output!
        fprintf( output, "\n" );
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
        fprintf( output, "ANIMATION SEQUENCES (%d sequences)\n", main_header->numseq );
        fprintf( output, "%s\n", RULER_THIN );
        print_sequence_info( output, sequences, main_header->numseq );  // Pass output!
        fprintf( output, "\n" );
    }
    else
    {
        fprintf( stderr, "ERROR: Failed to parse animations (code %d)\n\n", sequence_result );
    }

    /* --- Deep dive per bodypart/model/mesh ---------------------------------- */
    fprintf( output, "DETAILED MODEL ANALYSIS\n" );
    fprintf( output, "%s\n", RULER );

    mstudiobodyparts_t *bodyparts = ( mstudiobodyparts_t * ) ( main_data + main_header->bodypartindex );

    for ( int bp = 0; bp < main_header->numbodyparts; ++bp )
    {
        const mstudiobodyparts_t *B = &bodyparts[bp];
        fprintf( output, "\n[Bodypart %d] %-20s  (models: %d)\n", bp, B->name, B->nummodels );
        fprintf( output, "%s\n", RULER_THIN );

        mstudiomodel_t *models = ( mstudiomodel_t * ) ( main_data + B->modelindex );

        for ( int mi = 0; mi < B->nummodels; ++mi )
        {
            mstudiomodel_t *M = &models[mi];

            /* High-level model info (delegated) */
            print_model_info( output, M, bp, mi );  // Pass output!

            /* Meshes (parse + print) */
            mstudiomesh_t *meshes      = NULL;
            mdl_result_t   mesh_result = parse_mesh_data( M, main_data, &meshes );
            if ( mesh_result == MDL_SUCCESS && meshes )
            {
                print_mesh_data( output, meshes, M, M->nummesh );  // Pass output!
                print_simple_triangle_info( output, M, bp, mi );   // Pass output!
            }
            else
            {
                fprintf( output, "  %-12s %s\n", "Meshes:", "FAILED to parse" );
                fprintf( output, "    Model: %s\n", M->name[0] ? M->name : "(unnamed)" );
            }

            /* Vertex peek (first / last) */
            vec3_t *vertices = NULL;
            if ( parse_vertex_data( M, main_data, &vertices ) == MDL_SUCCESS )
            {
                if ( vertices && M->numverts > 0 )
                {
                    fprintf( output, "  %-12s\n", "Vertices:" );
                    fprintf( output, "    First: (%.2f, %.2f, %.2f)\n", vertices[0][0], vertices[0][1], vertices[0][2] );
                }
                if ( M->numverts > 1 )
                {
                    fprintf(
                        output,
                        "    Last:  (%.2f, %.2f, %.2f)\n",
                        vertices[M->numverts - 1][0],
                        vertices[M->numverts - 1][1],
                        vertices[M->numverts - 1][2] );
                }
            }
        }
    }

    fprintf(output, "\n%s\n", RULER);
    fprintf(output, " Complete model analysis completed\n");
    fprintf(output, "%s\n\n", RULER);
}
