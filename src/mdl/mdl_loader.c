/*
 * MDL loader implementation
 * We'll write this together following Valve's approach
 */

#include "mdl_loader.h"
#include "../studio.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


mdl_result_t validate_mdl_magic(int magic) {
    if (magic == IDSTUDIOHEADER) {
        return MDL_SUCCESS;
    } else {
        return MDL_ERROR_INVALID_MAGIC;
    }
}

mdl_result_t validate_mdl_version(int version) {
    if (version == STUDIO_VERSION) {
        return MDL_SUCCESS;
    } else {
        return MDL_ERROR_INVALID_VERSION;
    }
}

mdl_result_t read_mdl_file(const char *filename, unsigned char **file_data, size_t *file_size) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "ERROR - Failed to open the file '%s'. Invalid file name, file not found!\n",
                filename);
        return MDL_ERROR_FILE_NOT_FOUND;
    }

    fseek(file, 0, SEEK_END);
    size_t bytes_size = ftell(file);
    rewind(file);

    *file_size = bytes_size;

    *file_data = malloc(bytes_size * sizeof(unsigned char));
    if (!*file_data) {
        fprintf(stderr, "ERROR - Failed to allocate enough memory for the file data buffer.\n");
        return MDL_ERROR_MEMORY_ALLOCATION;
    }

    fread(*file_data, 1, bytes_size, file);

    fclose(file);

    return MDL_SUCCESS; 
}

mdl_result_t parse_mdl_header(const unsigned char *file_data, studiohdr_t **header) {
    if (file_data == NULL) {
        fprintf(stderr, "ERROR - Data passed is NULL, doesn't contain any valid value!\n");
        return MDL_ERROR_INVALID_PARAMETER;
    }

    if (header == NULL) {
        fprintf(stderr, "ERROR - Header address passed to the function is NULL.\n");
        return MDL_ERROR_INVALID_PARAMETER;
    }   

    *header = (studiohdr_t *)file_data;       
    mdl_result_t magic = validate_mdl_magic((*header)->id);
    if (magic != MDL_SUCCESS) {
        fprintf(stderr, "ERROR - Not the correct mdl header ID!\n");
        return MDL_ERROR_INVALID_MAGIC;
    }

    mdl_result_t version = validate_mdl_version((*header)->version);
    if (version != MDL_SUCCESS) {
        fprintf(stderr, "ERROR - Not the correct mdl version!\n");
        return MDL_ERROR_INVALID_VERSION;
    }

    return MDL_SUCCESS;
}

void print_bodypart_info(studiohdr_t *header, unsigned char *file_data) {
    if (header->numbodyparts == 0) {
        printf("  No bodyparts found.\n");
    }

    printf("\nDetailed Bodypart Information:\n");
    mstudiobodypart_t *bodyparts = (mstudiobodypart_t *)(file_data + header->bodypartindex);

    for (int i = 0; i < header->numbodyparts; i++) {
        printf("   [%d] Bodypart: %s (%d models)\n",
                i, bodyparts[i].name, bodyparts[i].nummodels);

        mstudiomodel_t *models = (mstudiomodel_t *)(file_data + bodyparts[i].modelindex);

        for (int j = 0; j < bodyparts[i].nummodels; j++) {
            printf("      Model [%d]: %s\n", j, models[j].name);
            printf("          Vertices: %d\n", models[j].numverts);
            printf("          Meshes: %d\n", models[j].nummesh);
        }

        printf("\n");
    }
}

char *generate_texture_filename(const char *model_filename) {
    size_t original_filename_length = strlen(model_filename);
    char *texture_filename = malloc(original_filename_length + 2);
    if (!texture_filename) {
        fprintf(stderr, "ERROR - Failed to allocate enough space for texture filename!\n");
        return NULL;
    }

    strcpy(texture_filename, model_filename);
    char *dot_position = strstr(texture_filename, ".mdl");

    if (dot_position) {
        memmove(dot_position + 1, dot_position, strlen(dot_position) + 1);
        *dot_position = 't';
    }
    return texture_filename;
}

mdl_result_t load_model_with_textures(const char *model_path, studiohdr_t **main_header, studiohdr_t **texture_header, unsigned char **main_data, unsigned char **texture_data) {

    size_t main_size;
    mdl_result_t result = read_mdl_file(model_path, main_data, &main_size);
    if (result != MDL_SUCCESS) {
        return result;
    }

    result = parse_mdl_header(*main_data, main_header);
    if (result != MDL_SUCCESS) {
        free(*main_data);
        return result;
    }

    char *texture_path = generate_texture_filename(model_path);
    if (!texture_path) {
        free(*main_data);
        return MDL_ERROR_MEMORY_ALLOCATION;
    }

    size_t texture_size;
    mdl_result_t texture_result = read_mdl_file(texture_path, texture_data, &texture_size);

    if (texture_result == MDL_SUCCESS) {
        texture_result = parse_mdl_header(*texture_data, texture_header);
        if (texture_result != MDL_SUCCESS) {
            free(*texture_data);
            *texture_data = NULL;
            *texture_header = NULL;
        }
    } else {
        *texture_data = NULL;
        *texture_header = NULL;
    }

    free(texture_path);
    return MDL_SUCCESS;

}

void print_texture_info(studiohdr_t *texture_header, unsigned char *texture_data) {
    if (!texture_data || !texture_header) {
        printf("\nTexture Information: No texture file found.\n");
        return;
    }

    printf("\nTexture Information:\n");
    printf("  Texture file size: %d bytes\n", texture_header->length);
    printf("  Number of textures: %d\n", texture_header->numtextures);


    if (texture_header->numtextures > 0) {
        mstudiotexture_t *textures = (mstudiotexture_t *)(texture_data + texture_header->textureindex);
        for (int i = 0; i < texture_header->numtextures; i++) {
            printf("  [%d] Name: %s\n", i, textures[i].name);
            printf("      Width %d, Height %d\n", textures[i].width, textures[i].height);
            printf("      Flags: %d\n", textures[i].flags);
        }
    }

}

mdl_result_t parse_bone_hierarchy(studiohdr_t *header, unsigned char *data, mstudiobone_t **bones) {
    if (!header || !data || !bones) {
        fprintf(stderr, "ERROR - Invalid parameters passed to the function call!\n");
        return MDL_ERROR_INVALID_PARAMETER;
    }

    if (header->numbones == 0) {
        *bones = NULL;
        return MDL_SUCCESS;
    }

    *bones = (mstudiobone_t *)(data + header->boneindex);

    return MDL_SUCCESS;
}



void print_bone_info(mstudiobone_t *bones, int bone_count) {

    if (!bones || bone_count == 0) {
        printf("\nBone Information: No bones found\n");
        return;
    }

    printf("\nBone Hierarchy(%d bones):\n", bone_count);
    for (int i = 0; i < bone_count; i++) {
        printf(" [%d] %s\n", i, bones[i].name);

        if (bones[i].parent == -1) {
            printf("     Parent: ROOT (no parent)\n");
        } else {
            printf("     Parent: [%d] %s\n", bones[i].parent, bones[bones[i].parent].name);
        }

        printf("     Position: (%.2f, %.2f, %.2f)\n",
                bones[i].value[0], bones[i].value[1], bones[i].value[2]);
        printf("      Rotation: (%.2f, %.2f, %.2f)\n", 
                bones[i].value[3], bones[i].value[4], bones[i].value[5]);
        printf("     Scale: (%.2f, %.2f, %.2f)\n",
                bones[i].scale[0], bones[i].scale[1], bones[i].scale[2]);
        printf("      Rot Scale: (%.2f, %.2f, %.2f)\n", 
                bones[i].scale[3], bones[i].scale[4], bones[i].scale[5]);
        printf("\n");
    }
}

mdl_result_t parse_animation_sequences(studiohdr_t *header, unsigned char *data, mstudioseqdesc_t **sequences) {

    if (!header || !data || !sequences) {
        fprintf(stderr, "ERROR - Invalid parameters passed to parse_animation_sequences()!\n");
        return MDL_ERROR_INVALID_PARAMETER;
    }

    if (header->numseq == 0) {
        *sequences = NULL;
        return MDL_SUCCESS;
    }

    *sequences = (mstudioseqdesc_t *)(data + header->seqindex);

    return MDL_SUCCESS;
}

void print_sequence_info(mstudioseqdesc_t *sequences, int sequence_count) {
    if (!sequences || sequence_count == 0) {
        printf("\nAnimation Sequences: No sequence found\n");
        return;
    }

    printf("\nAnimation Sequences: (%d sequences):\n", sequence_count);
    for (int i = 0; i < sequence_count; i++) {
        printf(" [%d] %s\n", i, sequences[i].label);

        printf("    Frames: %d @ %.1f fps\n", 
                sequences[i].numframes, sequences[i].fps);
        printf("    Activity: %d (weight: %d)\n",
                sequences[i].activity, sequences[i].actweight);
        printf("    Events: %d\n", sequences[i].numevents);
        printf("    Flags: 0x%x", sequences[i].flags);

        if (sequences[i].flags & 0x01) printf(" [LOOPING]");
        if (sequences[i].flags & 0x08) printf(" [ACTIVITY]");

        printf("\n");

        printf("    Motion: type=%d, bone=%d\n",
                sequences[i].motiontype, sequences[i].motionbone);

        printf("    Bounding box: (%.1f, %.1f, %.1f) to (%.1f, %.1f, %.1f)\n",
            sequences[i].bbmin[0], sequences[i].bbmin[1], sequences[i].bbmin[2],
            sequences[i].bbmax[0], sequences[i].bbmax[1], sequences[i].bbmax[2]);
        printf("    Blends: %d\n", sequences[i].numblends);
        printf("\n");
    }
}


void print_model_info(mstudiomodel_t *model, int bodypart_index, int model_index) {
    if (!model) {
        printf("Model [%d][%d]: NULL\n",
                bodypart_index, model_index);
        return;
    }

    printf("   Model[%d][%d]: %s\n",
            bodypart_index, model_index, model->name);
    printf("     Type: %d\n", model->type);
    printf("        Bounding radius: %.2f\n", model->boundingradius);
    printf("        Vertices: %d\n", model->numverts);
    printf("        Meshes: %d\n", model->nummesh);
    printf("        Normals: %d\n", model->numnorms);
    
    // Show mesh and vertex indices for debugging
    printf("        Vertex index: 0x%X\n", model->vertindex);
    printf("        Mesh index: 0x%X\n", model->meshindex);
    printf("\n");

}

mdl_result_t parse_mesh_data(mstudiomodel_t *model, unsigned char *data, mstudiomesh_t **meshes) {
    if (!model || !data || !meshes) {
        fprintf(stderr, "ERROR - Invalid parameters passed to parse_mesh_data()!\n");
        return MDL_ERROR_INVALID_PARAMETER;
    }

    if (model->nummesh == 0) {
        *meshes = NULL;
        return MDL_SUCCESS;
    }

    *meshes = (mstudiomesh_t *)(data + model->meshindex);
    
    return MDL_SUCCESS;
}

void print_mesh_data(mstudiomesh_t *meshes, mstudiomodel_t *model, int mesh_count) {
 if (!meshes || mesh_count == 0) {
        printf("    No meshes found for model: %s\n", model ? model->name : "Unknown");
        return;
    }

    printf("    Meshes for Model: %s\n", model->name);
    for (int i = 0; i < mesh_count; i++) {
        printf("      Mesh %d:\n", i);
        printf("        Triangles: %d\n", meshes[i].numtris);
        printf("        Texture ref: %d\n", meshes[i].skinref);
        printf("        Normals: %d\n", meshes[i].numnorms);
        printf("\n");    
    } 
}

mdl_result_t parse_vertex_data(mstudiomodel_t *model, unsigned char *data, vec3_t **vertices) {
    if (!model || !data || !vertices) {
        fprintf(stderr, "ERROR - Invalid parameters passed to parse_vertex_data()!\n");
        return MDL_ERROR_INVALID_PARAMETER;
    }   

    if (model->numverts == 0) {
        *vertices = NULL;
        return MDL_SUCCESS;
    }

    *vertices = (vec3_t *)(data + model->vertindex);

    return MDL_SUCCESS;
}

// Simple triangle index generation for basic wireframe
mdl_result_t create_simple_triangle_indices(int vertex_count, unsigned short **indices, int *index_count) {
    if (!indices || !index_count || vertex_count <= 0) {
        return MDL_ERROR_INVALID_PARAMETER;
    }
    
    // Create triangles from consecutive vertices (not perfect but works)
    *index_count = (vertex_count / 3) * 3;
    if (*index_count == 0) {
        *indices = NULL;
        return MDL_SUCCESS;
    }
    
    *indices = malloc(*index_count * sizeof(unsigned short));
    if (!*indices) {
        return MDL_ERROR_MEMORY_ALLOCATION;
    }
    
    for (int i = 0; i < *index_count; i++) {
        (*indices)[i] = i;
    }
    
    return MDL_SUCCESS;
}

void print_simple_triangle_info(mstudiomodel_t *model, int bodypart_index, int model_index) {
    if (!model) {
        printf("    No model data for triangle testing.\n");
        return;
    }
    
    printf("    Simple Triangle Index Test:\n");
    printf("      Model[%d][%d]: %s has %d vertices\n", 
           bodypart_index, model_index, model->name, model->numverts);
    
    if (model->numverts == 0) {
        printf("      No vertices to create triangles from.\n");
        return;
    }
    
    unsigned short *simple_indices = NULL;
    int index_count = 0;
    
    mdl_result_t result = create_simple_triangle_indices(model->numverts, &simple_indices, &index_count);
    
    if (result == MDL_SUCCESS && simple_indices) {
        printf("      SUCCESS: Created %d triangle indices\n", index_count);
        printf("      Triangles: %d (from %d vertices)\n", index_count / 3, model->numverts);
        
        // Print first few indices
        if (index_count > 0) {
            printf("      First indices: ");
            int print_count = index_count < 12 ? index_count : 12;
            for (int j = 0; j < print_count; j++) {
                printf("%d ", simple_indices[j]);
                if ((j + 1) % 3 == 0) printf("| ");
            }
            printf("\n");
        }
        
        free(simple_indices);
    } else {
        printf("      FAILED: Error %d creating simple indices\n", result);
    }
    printf("\n");
}

/* TODO(Karlo): Adding the extraction of the mesh connectivity 
                Need to know how meshes are being connected properly.
*/

mdl_result_t parse_triangle_commands(mstudiomesh_t *mesh, unsigned char *data, short **indices, int *index_count) {
    if (!mesh || !data || !indices || !index_count) {
        fprintf(stderr, "ERROR - Invalid parameters passed to parse_triangle_commands()!\n");
        return MDL_ERROR_INVALID_PARAMETER;
    }

    if (mesh->numtris == 0) {
        *indices = NULL;
        *index_count = 0;
        return MDL_SUCCESS;
    }

    // Get pointer to triangle commands
    unsigned char *triangle_commands = data + mesh->triindex;
    short *command_reader = (short *)triangle_commands;
    
    // First pass: count total triangles needed
    int total_triangles = 0;
    short *reader = command_reader;
    
    while (true) {
        short vertex_count = *reader++;
        
        if (vertex_count == 0) {
            break; // End of commands
        }
        
        if (vertex_count < 0) {
            // Triangle fan: N vertices = (N-2) triangles
            vertex_count = -vertex_count;
            total_triangles += (vertex_count - 2);
        } else {
            // Triangle strip: N vertices = (N-2) triangles  
            total_triangles += (vertex_count - 2);
        }
        
        // Skip past the trivert data (each trivert is 4 shorts)
        reader += vertex_count * 4;
    }
    
    if (total_triangles <= 0) {
        *indices = NULL;
        *index_count = 0;
        return MDL_SUCCESS;
    }
    
    // Allocate index array (3 indices per triangle)
    int total_indices = total_triangles * 3;
    *indices = malloc(total_indices * sizeof(short));
    if (!*indices) {
        fprintf(stderr, "ERROR - Failed to allocate memory for triangle indices!\n");
        return MDL_ERROR_MEMORY_ALLOCATION;
    }
    
    // Second pass: generate actual triangle indices
    reader = command_reader;
    int output_index = 0;
    
    while (true) {
        short vertex_count = *reader++;
        
        if (vertex_count == 0) {
            break; // End of commands
        }
        
        // Get pointer to trivert array for this command
        mstudiotrivert_t *triverts = (mstudiotrivert_t *)reader;
        
        if (vertex_count < 0) {
            // TRIANGLE FAN - all triangles share the first vertex
            vertex_count = -vertex_count;
            
            for (int i = 1; i < vertex_count - 1; i++) {
                (*indices)[output_index++] = triverts[0].vertindex;     // Center vertex
                (*indices)[output_index++] = triverts[i].vertindex;     // Current vertex
                (*indices)[output_index++] = triverts[i + 1].vertindex; // Next vertex
            }
        } else {
            // TRIANGLE STRIP - alternating winding order
            for (int i = 0; i < vertex_count - 2; i++) {
                if (i % 2 == 0) {
                    // Even triangle: normal winding
                    (*indices)[output_index++] = triverts[i].vertindex;
                    (*indices)[output_index++] = triverts[i + 1].vertindex;
                    (*indices)[output_index++] = triverts[i + 2].vertindex;
                } else {
                    // Odd triangle: reversed winding to maintain consistent orientation
                    (*indices)[output_index++] = triverts[i].vertindex;
                    (*indices)[output_index++] = triverts[i + 2].vertindex;
                    (*indices)[output_index++] = triverts[i + 1].vertindex;
                }
            }
        }
        
        // Advance past this command's trivert data
        reader += vertex_count * 4;
    }
    
    *index_count = total_indices;
    
    printf("DEBUG: Parsed %d triangles (%d indices) from mesh with %d expected triangles\n", 
           total_triangles, total_indices, mesh->numtris);
    
    return MDL_SUCCESS;
}

void print_triangle_info(mstudiomesh_t *meshes, mstudiomodel_t *model, unsigned char *data, int mesh_count) {
    if (!meshes || mesh_count == 0 || !model || !data) {
        printf("   No triangle data available for testing.\n");
        return;
    }

    printf("   Testing Triangle Commands:\n");

    for (int i = 0; i < mesh_count; i++) {

        printf("        DEBUG: Model vertex count: %d\n", model->numverts);
        printf("        DEBUG: Mesh triindex: 0x%X (decimal: %d)\n", meshes[i].triindex, meshes[i].triindex);
        printf("        DEBUG: Model mesh index: 0x%X\n", model->meshindex);


        printf("     Mesh %d:\n", i);
        printf("       Expected triangles: %d\n", meshes[i].numtris);
        printf("       Triangle commands offset: 0x%X\n", meshes[i].triindex);

        if (meshes[i].numtris == 0) {
            printf("       No triangles to parse.\n");
            continue;
        }

        short *triangle_indices = NULL;
        int index_count = 0;

        mdl_result_t result = parse_triangle_commands(&meshes[i], data, &triangle_indices, &index_count);

        if (result == MDL_SUCCESS && triangle_indices) {
            printf("       SUCCESS: Extracted %d indices\n", index_count);
            printf("       Triangles created: %d (expected %d)\n", index_count / 3 , meshes[i].numtris);

            if (index_count > 0) {
                printf("       First indices: ");
                int print_count = index_count < 12 ? index_count : 12;
                for (int j = 0; j < print_count; j++) {
                    printf("%d ", triangle_indices[j]);
                    if ((j + 1) % 3 == 0) {
                        printf("| ");
                    }
                    printf("\n");
                }
            }
            free(triangle_indices);
        } else {
            printf("       FAILED: Error %d parsing triangle commands\n", result);
        }
        printf("\n");
    }
}
