/*======================================================================
 *  File: mdl_loader.c
 *  Project: shaders
 *  Author: karlosiric <email@example.com>
 *  Created: 2025-09-22 23:59:53
 *  Last Modified by: karlosiric
 *  Last Modified: 2025-10-09 22:58:55
 *----------------------------------------------------------------------
 *  Description:
 *
 *----------------------------------------------------------------------
 *  License:
 *  Company:
 *  Version: 0.1.0
 *======================================================================
 */

#include "mdl_loader.h"

#include "../studio.h"
#include "../utils/mdl_messages.h"
#include "../utils/utils.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

mdl_result_t validate_mdl_magic( unsigned magic )
{
    if ( magic == IDSTUDIOHEADER )
        return MDL_SUCCESS;
    if ( magic == IDSEQGRPHEADER )
        return MDL_INFO_SEQUENCE_GROUP_FILE;
    return MDL_ERROR_INVALID_MAGIC;
}

mdl_result_t validate_mdl_version( int version )
{
    if ( version == STUDIO_VERSION || version == STUDIO_VERSION_2 )
        return MDL_SUCCESS;
    else
        return MDL_ERROR_INVALID_VERSION;
}

mdl_result_t read_mdl_file( const char *filename, unsigned char **file_data, size_t *file_size )
{
    FILE *file = fopen( filename, "rb" );
    if ( !file )
    {
        fprintf( stderr, "ERROR - Failed to open the file '%s'. Invalid file name, file not found!\n", filename );
        return MDL_ERROR_FILE_NOT_FOUND;
    }

    fseek( file, 0, SEEK_END );
    size_t bytes_size = ftell( file );
    rewind( file );

    *file_size = bytes_size;

    *file_data = malloc( bytes_size * sizeof( unsigned char ) );
    if ( !*file_data )
    {
        fprintf( stderr, "ERROR - Failed to allocate enough memory for the file data buffer.\n" );
        fclose( file );
        return MDL_ERROR_MEMORY_ALLOCATION;
    }

    fread( *file_data, 1, bytes_size, file );
    fclose( file );
    return MDL_SUCCESS;
}

mdl_result_t parse_mdl_header( const unsigned char *file_data, studiohdr_t **header )
{
    if ( !file_data || !header )
        return MDL_ERROR_INVALID_PARAMETER;

    *header = ( studiohdr_t * ) file_data;

    mdl_result_t magic = validate_mdl_magic( ( *header )->id );
    if ( magic != MDL_SUCCESS )
    {
        return magic;    // could be MDL_INFO_SEQUENCE_GROUP_FILE or INVALID_MAGIC
    }

    mdl_result_t version = validate_mdl_version( ( *header )->version );
    if ( version != MDL_SUCCESS )
        return version;

    return MDL_SUCCESS;
}

void print_bodypart_info( studiohdr_t *header, unsigned char *file_data )
{
    if ( header->numbodyparts == 0 )
    {
        printf( "  No bodyparts found.\n" );
        return;
    }

    printf( "\nDetailed Bodypart Information:\n" );
    // FIXED: Use correct structure name
    mstudiobodyparts_t *bodyparts = ( mstudiobodyparts_t * ) ( file_data + header->bodypartindex );

    for ( int i = 0; i < header->numbodyparts; i++ )
    {
        printf( "   [%d] Bodypart: %s (%d models)\n", i, bodyparts[i].name, bodyparts[i].nummodels );

        mstudiomodel_t *models = ( mstudiomodel_t * ) ( file_data + bodyparts[i].modelindex );

        for ( int j = 0; j < bodyparts[i].nummodels; j++ )
        {
            printf( "      Model [%d]: %s\n", j, models[j].name );
            printf( "          Vertices: %d\n", models[j].numverts );
            printf( "          Meshes: %d\n", models[j].nummesh );
        }
        printf( "\n" );
    }
}

char *generate_texture_filename( const char *model_filename )
{
    size_t original_filename_length = strlen( model_filename );
    char  *texture_filename         = malloc( original_filename_length + 2 );
    if ( !texture_filename )
    {
        fprintf( stderr, "ERROR - Failed to allocate enough space for texture filename!\n" );
        return NULL;
    }

    strcpy( texture_filename, model_filename );
    char *dot_position = strstr( texture_filename, ".mdl" );

    if ( dot_position )
    {
        memmove( dot_position + 1, dot_position, strlen( dot_position ) + 1 );
        // *dot_position = 't';
        strcpy( dot_position, "t.mdl" );
    }
    return texture_filename;
}

mdl_result_t load_model_with_textures(
    const char     *model_path,
    studiohdr_t   **main_header,
    studiohdr_t   **texture_header,
    unsigned char **main_data,
    unsigned char **texture_data )
{
    size_t       main_size;
    mdl_result_t r = read_mdl_file( model_path, main_data, &main_size );
    if ( r != MDL_SUCCESS )
    {
        mdl_print_message( r, &( mdl_msg_ctx_t ) { .path = model_path } );
        return r;
    }

    r = parse_mdl_header( *main_data, main_header );
    if ( r != MDL_SUCCESS )
    {
        if ( r == MDL_INFO_SEQUENCE_GROUP_FILE )
        {
            char base[1024];
            bool ok = mdl_derive_base_path( model_path, base, sizeof base );
            mdl_print_message( r, &( mdl_msg_ctx_t ) { .path = model_path, .base_path = ok ? base : NULL } );
        }
        else if ( r == MDL_ERROR_INVALID_VERSION )
        {
            mdl_print_message(
                r, &( mdl_msg_ctx_t ) { .path = model_path, .version = ( int ) ( *main_header )->version } );
        }
        else
        {
            mdl_print_message( r, &( mdl_msg_ctx_t ) { .path = model_path } );
        }
        free( *main_data );
        *main_data   = NULL;
        *main_header = NULL;
        return r;
    }

    // If main has no textures, try companion t.mdl
    if ( ( *main_header )->numtextures == 0 )
    {
        char *texture_path = generate_texture_filename( model_path );
        if ( !texture_path )
        {
            free( *main_data );
            *main_data   = NULL;
            *main_header = NULL;
            return MDL_ERROR_MEMORY_ALLOCATION;
        }

        size_t       tex_size;
        mdl_result_t tr = read_mdl_file( texture_path, texture_data, &tex_size );
        if ( tr == MDL_SUCCESS )
        {
            tr = parse_mdl_header( *texture_data, texture_header );
            if ( tr != MDL_SUCCESS )
            {
                mdl_print_message( tr, &( mdl_msg_ctx_t ) { .path = texture_path } );
                free( *texture_data );
                *texture_data   = NULL;
                *texture_header = NULL;
                // Not fatal; continue without textures
            }
        }
        else
        {
            mdl_print_message( MDL_ERROR_MISSING_TEXTURE_FILE, &( mdl_msg_ctx_t ) { .path = model_path } );
            *texture_data   = NULL;
            *texture_header = NULL;
        }

        free( texture_path );
    }
    else
    {
        *texture_data   = NULL;
        *texture_header = NULL;
    }

    // Optional: on success, emit a friendly banner (console or GUI string)
    fprintf( stderr, "SUCCESS: Model loaded completely!\n" );

    return MDL_SUCCESS;
}

void print_texture_info( studiohdr_t *texture_header, unsigned char *texture_data )
{
    if ( !texture_data || !texture_header )
    {
        printf( "\nTexture Information: No texture file found.\n" );
        return;
    }

    printf( "\nTexture Information:\n" );
    printf( "  Texture file size: %d bytes\n", texture_header->length );
    printf( "  Number of textures: %d\n\n\n", texture_header->numtextures );

    if ( texture_header->numtextures > 0 )
    {
        mstudiotexture_t *textures = ( mstudiotexture_t * ) ( texture_data + texture_header->textureindex );
        for ( int i = 0; i < texture_header->numtextures; i++ )
        {
            int struct_textures_location = texture_header->textureindex + ( i * sizeof( mstudiotexture_t ) );
            printf( "  TextureStruct[%d] Offset 0x%08X\n",i,  struct_textures_location );
            printf( "  [%d] Name: %s\n", i, textures[i].name );
            printf( "      Width %d, Height %d\n", textures[i].width, textures[i].height );
            printf( "      Flags: %d\n", textures[i].flags );
            printf( "      Index Offset: %d (HEX 0x%08X)\n", textures[i].index, textures[i].index );
            printf( " \n ");
        }
    }
}

mdl_result_t parse_bone_hierarchy( studiohdr_t *header, unsigned char *data, mstudiobone_t **bones )
{
    if ( !header || !data || !bones )
    {
        fprintf( stderr, "ERROR - Invalid parameters passed to the function call!\n" );
        return MDL_ERROR_INVALID_PARAMETER;
    }

    if ( header->numbones == 0 )
    {
        *bones = NULL;
        return MDL_SUCCESS;
    }

    *bones = ( mstudiobone_t * ) ( data + header->boneindex );
    return MDL_SUCCESS;
}

void print_bone_info( mstudiobone_t *bones, int bone_count )
{
    if ( !bones || bone_count == 0 )
    {
        printf( "\nBone Information: No bones found\n" );
        return;
    }

    printf( "\nBone Hierarchy(%d bones):\n", bone_count );
    for ( int i = 0; i < bone_count; i++ )
    {
        printf( " [%d] %s\n", i, bones[i].name );

        if ( bones[i].parent == -1 )
        {
            printf( "     Parent: ROOT (no parent)\n" );
        }
        else
        {
            printf( "     Parent: [%d] %s\n", bones[i].parent, bones[bones[i].parent].name );
        }

        printf( "     Position: (%.2f, %.2f, %.2f)\n", bones[i].value[0], bones[i].value[1], bones[i].value[2] );
        printf( "      Rotation: (%.2f, %.2f, %.2f)\n", bones[i].value[3], bones[i].value[4], bones[i].value[5] );
        printf( "     Scale: (%.2f, %.2f, %.2f)\n", bones[i].scale[0], bones[i].scale[1], bones[i].scale[2] );
        printf( "      Rot Scale: (%.2f, %.2f, %.2f)\n", bones[i].scale[3], bones[i].scale[4], bones[i].scale[5] );
        printf( "\n" );
    }
}

mdl_result_t parse_animation_sequences( studiohdr_t *header, unsigned char *data, mstudioseqdesc_t **sequences )
{
    if ( !header || !data || !sequences )
    {
        fprintf( stderr, "ERROR - Invalid parameters passed to parse_animation_sequences()!\n" );
        return MDL_ERROR_INVALID_PARAMETER;
    }

    if ( header->numseq == 0 )
    {
        *sequences = NULL;
        return MDL_SUCCESS;
    }

    *sequences = ( mstudioseqdesc_t * ) ( data + header->seqindex );
    return MDL_SUCCESS;
}

void print_sequence_info( mstudioseqdesc_t *sequences, int sequence_count )
{
    if ( !sequences || sequence_count == 0 )
    {
        printf( "\nAnimation Sequences: No sequence found\n" );
        return;
    }

    printf( "\nAnimation Sequences: (%d sequences):\n", sequence_count );
    for ( int i = 0; i < sequence_count; i++ )
    {
        printf( " [%d] %s\n", i, sequences[i].label );
        printf( "    Frames: %d @ %.1f fps\n", sequences[i].numframes, sequences[i].fps );
        printf( "    Activity: %d (weight: %d)\n", sequences[i].activity, sequences[i].actweight );
        printf( "    Events: %d\n", sequences[i].numevents );
        printf( "    Flags: 0x%x", sequences[i].flags );

        if ( sequences[i].flags & 0x01 )
            printf( " [LOOPING]" );
        if ( sequences[i].flags & 0x08 )
            printf( " [ACTIVITY]" );
        printf( "\n" );

        printf( "    Motion: type=%d, bone=%d\n", sequences[i].motiontype, sequences[i].motionbone );
        printf(
            "    Bounding box: (%.1f, %.1f, %.1f) to (%.1f, %.1f, %.1f)\n",
            sequences[i].bbmin[0],
            sequences[i].bbmin[1],
            sequences[i].bbmin[2],
            sequences[i].bbmax[0],
            sequences[i].bbmax[1],
            sequences[i].bbmax[2] );
        printf( "    Blends: %d\n", sequences[i].numblends );
        printf( "\n" );
    }
}

void print_model_info( mstudiomodel_t *model, int bodypart_index, int model_index )
{
    if ( !model )
    {
        printf( "Model [%d][%d]: NULL\n", bodypart_index, model_index );
        return;
    }

    printf( "   Model[%d][%d]: %s\n", bodypart_index, model_index, model->name );
    printf( "     Type: %d\n", model->type );
    printf( "        Bounding radius: %.2f\n", model->boundingradius );
    printf( "        Vertices: %d\n", model->numverts );
    printf( "        Meshes: %d\n", model->nummesh );
    printf( "        Normals: %d\n", model->numnorms );
    printf( "        Vertex index: 0x%X\n", model->vertindex );
    printf( "        Mesh index: 0x%X\n", model->meshindex );
    printf( "\n" );
}

mdl_result_t parse_mesh_data( mstudiomodel_t *model, unsigned char *data, mstudiomesh_t **meshes )
{
    if ( !model || !data || !meshes )
    {
        fprintf( stderr, "ERROR - Invalid parameters passed to parse_mesh_data()!\n" );
        return MDL_ERROR_INVALID_PARAMETER;
    }

    if ( model->nummesh == 0 )
    {
        *meshes = NULL;
        return MDL_SUCCESS;
    }

    *meshes = ( mstudiomesh_t * ) ( data + model->meshindex );
    return MDL_SUCCESS;
}

void print_mesh_data( mstudiomesh_t *meshes, mstudiomodel_t *model, int mesh_count )
{
    if ( !meshes || mesh_count == 0 )
    {
        printf( "    No meshes found for model: %s\n", model ? model->name : "Unknown" );
        return;
    }

    printf( "    Meshes for Model: %s\n", model->name );
    for ( int i = 0; i < mesh_count; i++ )
    {
        printf( "      Mesh %d:\n", i );
        printf( "        Triangles: %d\n", meshes[i].numtris );
        printf( "        Texture ref: %d\n", meshes[i].skinref );
        printf( "        Normals: %d\n", meshes[i].numnorms );
        printf( "        Triangle cmd offset: 0x%X\n", meshes[i].triindex );
        printf( "\n" );
    }
}

mdl_result_t parse_vertex_data( mstudiomodel_t *model, unsigned char *data, vec3_t **vertices )
{
    if ( !model || !data || !vertices )
    {
        fprintf( stderr, "ERROR - Invalid parameters passed to parse_vertex_data()!\n" );
        return MDL_ERROR_INVALID_PARAMETER;
    }

    if ( model->numverts == 0 )
    {
        *vertices = NULL;
        return MDL_SUCCESS;
    }

    *vertices = ( vec3_t * ) ( data + model->vertindex );
    return MDL_SUCCESS;
}

// Simple triangle index generation for basic wireframe (fallback)
mdl_result_t create_simple_triangle_indices( int vertex_count, short **indices, int *index_count )
{
    if ( !indices || !index_count || vertex_count <= 0 )
    {
        return MDL_ERROR_INVALID_PARAMETER;
    }

    // Create triangles from consecutive vertices (not perfect but works)
    *index_count = ( vertex_count / 3 ) * 3;
    if ( *index_count == 0 )
    {
        *indices = NULL;
        return MDL_SUCCESS;
    }

    *indices = malloc( *index_count * sizeof( short ) );
    if ( !*indices )
    {
        return MDL_ERROR_MEMORY_ALLOCATION;
    }

    for ( int i = 0; i < *index_count; i++ )
    {
        ( *indices )[i] = i;
    }

    return MDL_SUCCESS;
}

void print_simple_triangle_info( mstudiomodel_t *model, int bodypart_index, int model_index )
{
    if ( !model )
    {
        printf( "    No model data for triangle testing.\n" );
        return;
    }

    printf( "    Simple Triangle Index Test:\n" );
    printf( "      Model[%d][%d]: %s has %d vertices\n", bodypart_index, model_index, model->name, model->numverts );

    if ( model->numverts == 0 )
    {
        printf( "      No vertices to create triangles from.\n" );
        return;
    }

    short *simple_indices = NULL;
    int    index_count    = 0;

    mdl_result_t result = create_simple_triangle_indices( model->numverts, &simple_indices, &index_count );

    if ( result == MDL_SUCCESS && simple_indices )
    {
        printf( "      SUCCESS: Created %d triangle indices\n", index_count );
        printf( "      Triangles: %d (from %d vertices)\n", index_count / 3, model->numverts );

        // Print first few indices
        if ( index_count > 0 )
        {
            printf( "      First indices: " );
            int print_count = index_count < 12 ? index_count : 12;
            for ( int j = 0; j < print_count; j++ )
            {
                printf( "%d ", simple_indices[j] );
                if ( ( j + 1 ) % 3 == 0 )
                    printf( "| " );
            }
            printf( "\n" );
        }

        free( simple_indices );
    }
    else
    {
        printf( "      FAILED: Error %d creating simple indices\n", result );
    }
    printf( "\n" );
}

// TODO(Karlo): Reimplementing extract texture rgb

mdl_result_t extract_texture_rgb(
    studiohdr_t    *texture_header,
    unsigned char  *texture_data,
    int             texture_index,
    unsigned char **rgb_output,
    int            *width,
    int            *height )
{
    if ( !texture_header || !texture_data || !rgb_output || !width || !height )
    {
        return MDL_ERROR_INVALID_PARAMETER;
    }

    if ( texture_index >= texture_header->numtextures )
    {
        return MDL_ERROR_INVALID_PARAMETER;
    }

    mstudiotexture_t *textures = ( mstudiotexture_t * ) ( texture_data + texture_header->textureindex );
    mstudiotexture_t *tex      = &textures[texture_index];

    *width  = tex->width;
    *height = tex->height;

    unsigned char *indexed_pixels = texture_data + texture_header->texturedataindex + tex->index;
    // Get a palette right after the
    unsigned char *palette = indexed_pixels + ( tex->width * tex->height );

    int pixel_count = tex->width * tex->height;

    *rgb_output = malloc( pixel_count * 3 );
    if ( !*rgb_output )
    {
        fprintf( stderr, "ERROR - Failed to allocate space for RGB values.\n" );
        return MDL_ERROR_MEMORY_ALLOCATION;
    }

    for ( int i = 0; i < pixel_count; i++ )
    {
        unsigned char palette_index = indexed_pixels[i];

        ( *rgb_output )[i * 3 + 0] = palette[palette_index * 3 + 0];
        ( *rgb_output )[i * 3 + 1] = palette[palette_index * 3 + 1];
        ( *rgb_output )[i * 3 + 2] = palette[palette_index * 3 + 2];

        // TODO(Karlo): checking for transparent pixel
        if ( palette_index == 255 )
        {
            ( *rgb_output )[i * 3 + 0] = 0;
            ( *rgb_output )[i * 3 + 1] = 0;
            ( *rgb_output )[i * 3 + 2] = 0;
        }
    }

    printf( "Extracted textures %d: %s (%dx%d)\n", texture_index, tex->name, tex->width, tex->height );
    return MDL_SUCCESS;
}

mdl_result_t extract_triangles_with_uvs(
    mstudiomesh_t *mesh,
    unsigned char *main_data,
    vec3_t        *model_vertices,
    int            model_vertex_count,
    float        **out_vertices,
    float        **out_texcoords,
    int           *out_vertex_count )
{
    if ( !mesh || !main_data || !model_vertices || !out_vertices || !out_texcoords || !out_vertex_count )
    {
        fprintf( stderr, "ERROR - Invalid parameters error for function 'extract_traingles_with_uvs'" );
        return MDL_ERROR_INVALID_PARAMETER;
    }

    *out_vertices     = NULL;
    *out_texcoords    = NULL;
    *out_vertex_count = 0;

    if ( mesh->numtris == 0 )
    {
        return MDL_SUCCESS;
    }

    const short *triangle_commands = ( const short * ) ( main_data + mesh->triindex );

    int max_vertices = mesh->numtris * 3;

    float *temp_vertices  = malloc( max_vertices * 3 * sizeof( float ) );
    float *temp_textcoord = malloc( max_vertices * 2 * sizeof( float ) );

    if ( !temp_vertices || !temp_textcoord )
    {
        free( temp_vertices );
        free( temp_textcoord );
        return MDL_ERROR_MEMORY_ALLOCATION;
    }

    int vertex_count = 0;
    int command;

    while ( ( command = *triangle_commands++ ) != 0 )
    {
        int  num_verts = abs( command );
        bool is_fan    = ( command < 0 );

        const mstudiotrivert_t *vertices_data = ( const mstudiotrivert_t * ) triangle_commands;

        if ( is_fan )
        {
            /* So it goes like this for TRIANGLE FAN: 0, 1, 2 -> 0, 2, 3 -> 0, 3, 4
             * TODO(Karlo): Extracting Triangle Fans
             */

            for ( int i = 2; i < num_verts; i++ )
            {
                if ( vertex_count + 3 <= max_vertices )
                {
                    int indices[3]
                        = { vertices_data[0].vertindex, vertices_data[i - 1].vertindex, vertices_data[i].vertindex };

                    /* VALIDATION OF INDICES */

                    for ( int j = 0; j < 3; j++ )
                    {
                        if ( indices[j] >= model_vertex_count )
                        {
                            fprintf( stderr, "ERROR - Invalid vertex index %d\n", indices[j] );
                            free( temp_vertices );
                            free( temp_textcoord );
                            return MDL_ERROR_INVALID_PARAMETER;
                        }
                    }

                    for ( int j = 0; j < 3; j++ )
                    {
                        float scale_factor                  = 0.01f;
                        int   temp_vertindex                = indices[j];    // Half life system uses y->z
                        temp_vertices[vertex_count * 3 + 0] = model_vertices[temp_vertindex][0] * scale_factor;    // x
                        temp_vertices[vertex_count * 3 + 1] = model_vertices[temp_vertindex][1] * scale_factor;    // z
                        temp_vertices[vertex_count * 3 + 2] = model_vertices[temp_vertindex][2] * scale_factor;    // y
                        vertex_count++;
                    }

                    temp_textcoord[( vertex_count - 3 ) * 2 + 0] = ( float ) vertices_data[0].s / 256.0f;
                    temp_textcoord[( vertex_count - 3 ) * 2 + 1] = ( float ) vertices_data[0].t / 256.0f;
                    temp_textcoord[( vertex_count - 2 ) * 2 + 0] = ( float ) vertices_data[i - 1].s / 256.0f;
                    temp_textcoord[( vertex_count - 2 ) * 2 + 1] = ( float ) vertices_data[i - 1].t / 256.0f;
                    temp_textcoord[( vertex_count - 1 ) * 2 + 0] = ( float ) vertices_data[i].s / 256.0f;
                    temp_textcoord[( vertex_count - 1 ) * 2 + 1] = ( float ) vertices_data[i].t / 256.0f;
                }
            }
        }
        else
        {
            // Triangel Stripping: We have two different winding methods to address for this
            for ( int i = 2; i < num_verts; i++ )
            {
                if ( vertex_count + 3 <= max_vertices )
                {
                    int indices[3];

                    if ( i % 2 == 0 )
                    {
                        // Even triangle - Normal Winding

                        indices[0] = vertices_data[i - 2].vertindex;
                        indices[1] = vertices_data[i - 1].vertindex;
                        indices[2] = vertices_data[i].vertindex;
                    }
                    else
                    {
                        // Odd triangle - Reverse Winding

                        indices[0] = vertices_data[i - 1].vertindex;
                        indices[1] = vertices_data[i - 2].vertindex;
                        indices[2] = vertices_data[i].vertindex;
                    }

                    // VALIDATION

                    for ( int j = 0; j < 3; j++ )
                    {
                        if ( indices[j] >= model_vertex_count )
                        {
                            fprintf( stderr, "ERROR - Invalid vertex index %d\n", indices[j] );
                            free( temp_vertices );
                            free( temp_textcoord );
                            return MDL_ERROR_INVALID_PARAMETER;
                        }
                    }

                    for ( int j = 0; j < 3; j++ )
                    {
                        float scale_factor                  = 0.01f;
                        int   temp_vertindex                = indices[j];
                        temp_vertices[vertex_count * 3 + 0] = model_vertices[temp_vertindex][0] * scale_factor;
                        temp_vertices[vertex_count * 3 + 1] = model_vertices[temp_vertindex][1] * scale_factor;
                        temp_vertices[vertex_count * 3 + 2] = model_vertices[temp_vertindex][2] * scale_factor;
                        vertex_count++;
                    }

                    if ( i % 2 == 0 )
                    {
                        temp_textcoord[( vertex_count - 3 ) * 2 + 0] = ( float ) vertices_data[i - 1].s / 256.0f;
                        temp_textcoord[( vertex_count - 3 ) * 2 + 1] = ( float ) vertices_data[i - 1].t / 256.0f;
                        temp_textcoord[( vertex_count - 2 ) * 2 + 0] = ( float ) vertices_data[i - 2].s / 256.0f;
                        temp_textcoord[( vertex_count - 2 ) * 2 + 1] = ( float ) vertices_data[i - 2].t / 256.0f;
                        temp_textcoord[( vertex_count - 1 ) * 2 + 0] = ( float ) vertices_data[i].s / 256.0f;
                        temp_textcoord[( vertex_count - 1 ) * 2 + 1] = ( float ) vertices_data[i].t / 256.0f;
                    }
                    else
                    {
                        temp_textcoord[( vertex_count - 3 ) * 2 + 0] = ( float ) vertices_data[i - 1].s / 256.0f;
                        temp_textcoord[( vertex_count - 3 ) * 2 + 1] = ( float ) vertices_data[i - 1].t / 256.0f;
                        temp_textcoord[( vertex_count - 2 ) * 2 + 0] = ( float ) vertices_data[i - 2].s / 256.0f;
                        temp_textcoord[( vertex_count - 2 ) * 2 + 1] = ( float ) vertices_data[i - 2].t / 256.0f;
                        temp_textcoord[( vertex_count - 1 ) * 2 + 0] = ( float ) vertices_data[i].s / 256.0f;
                        temp_textcoord[( vertex_count - 1 ) * 2 + 1] = ( float ) vertices_data[i].t / 256.0f;
                    }
                }
            }
        }

        // Needed to move the triangle_commands pointer to a new set of verts to read and its data
        triangle_commands += num_verts * 4;
    }

    *out_vertices  = malloc( vertex_count * 3 * sizeof( float ) );
    *out_texcoords = malloc( vertex_count * 2 * sizeof( float ) );

    if ( !*out_vertices || !*out_texcoords )
    {
        free( *out_vertices );
        free( *out_texcoords );
        free( temp_vertices );
        free( temp_textcoord );
        return MDL_ERROR_MEMORY_ALLOCATION;
    }

    memcpy( *out_vertices, temp_vertices, vertex_count * 3 * sizeof( float ) );
    memcpy( *out_texcoords, temp_textcoord, vertex_count * 2 * sizeof( float ) );

    *out_vertex_count = vertex_count;

    free( temp_vertices );
    free( temp_textcoord );

    return MDL_SUCCESS;
}

mstudiomodel_t *
get_model_by_bodypart( studiohdr_t *header, unsigned char *main_data, int bodygroup_value, int bodypart_index )
{
    if ( !header || !main_data || bodypart_index >= header->numbodyparts )
    {
        fprintf( stderr, "ERROR - Invalid parameters: 'get_model_by_bodypart'\n" );
        return NULL;
    }

    mstudiobodyparts_t *bodyparts = ( mstudiobodyparts_t * ) ( main_data + header->bodypartindex );
    mstudiobodyparts_t *bodypart  = &bodyparts[bodypart_index];

    if ( bodypart->nummodels == 0 )
    {
        fprintf(
            stderr,
            "Bodypart(Bodyparts[%d]) - '%s' has no models attached to it, models %d\n",
            bodypart_index,
            bodypart->name,
            bodypart->nummodels );
        return NULL;
    }

    // Algorithm

    int index              = bodygroup_value / bodypart->base;
    index                  = index % bodypart->nummodels;
    mstudiomodel_t *models = ( mstudiomodel_t * ) ( main_data + bodypart->modelindex );

    return &models[index];
}
