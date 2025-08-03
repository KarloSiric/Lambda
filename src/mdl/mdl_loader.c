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
  

