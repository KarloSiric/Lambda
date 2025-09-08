/*
 * MDL loader implementation
 * We'll write this together following Valve's approach
 */

#include "mdl_loader.h"
#include "../studio.h"
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
