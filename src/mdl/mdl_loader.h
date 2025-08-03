/*
 * MDL file loader using official Valve structures
 * Handles parsing and validation of .mdl files
 */

#ifndef MDL_LOADER_H
#define MDL_LOADER_H

#include "../studio.h"
#include <stdio.h>
#include <stddef.h>

typedef enum
{
    MDL_SUCCESS = 0,
    MDL_ERROR_FILE_NOT_FOUND,
    MDL_ERROR_INVALID_MAGIC,
    MDL_ERROR_INVALID_VERSION,
    MDL_ERROR_FILE_TOO_SMALL,
    MDL_ERROR_MEMORY_ALLOCATION,
    MDL_ERROR_INVALID_PARAMETER,
} mdl_result_t;



mdl_result_t validate_mdl_magic(int magic);
mdl_result_t validate_mdl_version(int version);
mdl_result_t read_mdl_file(const char *filename, unsigned char **file_data, size_t *file_size);
mdl_result_t parse_mdl_header(const unsigned char *file_data, studiohdr_t **header);
mdl_result_t load_model_with_textures(const char *model_path, studiohdr_t **main_header, studiohdr_t **texture_header, unsigned char **main_data, unsigned char **texture_data);
mdl_result_t parse_bone_hierarchy(studiohdr_t *header, unsigned char *data, mstudiobone_t **bones);
mdl_result_t parse_animation_sequences(studiohdr_t *header, unsigned char *data, mstudioseqdesc_t **sequences);
mdl_result_t parse_animation_sequences(studiohdr_t *header, unsigned char *data, mstudioseqdesc_t **sequences);


char *generate_texture_filename(const char *model_filename);
void print_texture_info(studiohdr_t *texture_header, unsigned char *texture_data);
void print_bodypart_info(studiohdr_t *header, unsigned char *file_data);
void print_bone_info(mstudiobone_t *bones, int bone_count); 
void print_sequence_info(mstudioseqdesc_t *sequences, int sequence_count);
#endif // MDL_LOADER_H
