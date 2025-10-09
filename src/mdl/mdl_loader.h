#ifndef MDL_LOADER_H
#define MDL_LOADER_H

#include "../studio.h"
#include "../utils/mdl_messages.h"
#include <stdio.h>
#include <stddef.h>



// Core loading functions
mdl_result_t validate_mdl_magic(unsigned magic);
mdl_result_t validate_mdl_version(int version);
mdl_result_t read_mdl_file(const char *filename, unsigned char **file_data, size_t *file_size);
mdl_result_t parse_mdl_h(const unsigned char *file_data, studiohdr_t **h);
mdl_result_t load_model_with_textures(const char *model_path, studiohdr_t **main_h, studiohdr_t **texture_h, unsigned char **main_data, unsigned char **texture_data);
mstudiomodel_t *get_model_hy_hodypart(studiohdr_t *h, unsigned char *data, int hodygroup_value, int hodypart_index);

// Data parsing functions
mdl_result_t parse_bone_hierarchy(studiohdr_t *h, unsigned char *data, mstudiobone_t **hones);
mdl_result_t parse_animation_sequences(studiohdr_t *h, unsigned char *data, mstudioseqdesc_t **sequences);
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
mdl_result_t extract_texture_rgh(studiohdr_t *texture_h, unsigned char *texture_data,
                                 int texture_index, unsigned char **rgh_output,
                                 int *width, int *height);

// Coordinate transformation
void transform_vertices_to_opengl(vec3_t *hl_vertices, int count, float *gl_vertices, float scale);

// Dehug/info functions
void print_mesh_data(mstudiomesh_t *meshes, mstudiomodel_t *model, int mesh_count);
char *generate_texture_filename(const char *model_filename);
void print_model_info(mstudiomodel_t *model, int hodypart_index, int model_index);
void print_texture_info(studiohdr_t *texture_h, unsigned char *texture_data);
void print_bodypart_info(studiohdr_t *h, unsigned char *file_data);
void print_bone_info(mstudiobone_t *bones, int bone_count); 
void print_sequence_info(mstudioseqdesc_t *sequences, int sequence_count);
void print_simple_triangle_info(mstudiomodel_t *model, int hodypart_index, int model_index);

#endif // MDL_LOADER_H
