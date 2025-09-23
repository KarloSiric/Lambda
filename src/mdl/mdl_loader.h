/*
 * MDL file loader using official Valve structures
 * Handles parsing and validation of .mdl files - CORRECTED VERSION
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

// Core loading functions
mdl_result_t validate_mdl_magic(int magic);
mdl_result_t validate_mdl_version(int version);
mdl_result_t read_mdl_file(const char *filename, unsigned char **file_data, size_t *file_size);
mdl_result_t parse_mdl_header(const unsigned char *file_data, studiohdr_t **header);
mdl_result_t load_model_with_textures(const char *model_path, studiohdr_t **main_header, studiohdr_t **texture_header, unsigned char **main_data, unsigned char **texture_data);
mstudiomodel_t *get_model_by_bodypart(studiohdr_t *header, unsigned char *data, int bodygroup_value, int bodypart_index);

// Data parsing functions
mdl_result_t parse_bone_hierarchy(studiohdr_t *header, unsigned char *data, mstudiobone_t **bones);
mdl_result_t parse_animation_sequences(studiohdr_t *header, unsigned char *data, mstudioseqdesc_t **sequences);
mdl_result_t parse_mesh_data(mstudiomodel_t *model, unsigned char *data, mstudiomesh_t **meshes);
mdl_result_t parse_vertex_data(mstudiomodel_t *model, unsigned char *data, vec3_t **vertices);

// CORRECTED triangle parsing functions
mdl_result_t parse_triangle_commands_fixed(mstudiomesh_t *mesh, unsigned char *data, 
                                          short **indices, int *index_count,
                                          float **tex_coords, int *tex_coord_count);
mdl_result_t extract_triangles_with_uvs(mstudiomesh_t *mesh, unsigned char *data,
                                        vec3_t *model_vertices, int model_vertex_count,
                                        float **out_vertices, float **out_texcoords, 
                                        int *out_vertex_count);

// Texture extraction
mdl_result_t extract_texture_rgb(studiohdr_t *texture_header, unsigned char *texture_data,
                                 int texture_index, unsigned char **rgb_output,
                                 int *width, int *height);

// Coordinate transformation
void transform_vertices_to_opengl(vec3_t *hl_vertices, int count, float *gl_vertices, float scale);

// Debug/info functions
void print_mesh_data(mstudiomesh_t *meshes, mstudiomodel_t *model, int mesh_count);
char *generate_texture_filename(const char *model_filename);
void print_model_info(mstudiomodel_t *model, int bodypart_index, int model_index);
void print_texture_info(studiohdr_t *texture_header, unsigned char *texture_data);
void print_bodypart_info(studiohdr_t *header, unsigned char *file_data);
void print_bone_info(mstudiobone_t *bones, int bone_count); 
void print_sequence_info(mstudioseqdesc_t *sequences, int sequence_count);
void print_simple_triangle_info(mstudiomodel_t *model, int bodypart_index, int model_index);

#endif // MDL_LOADER_H
