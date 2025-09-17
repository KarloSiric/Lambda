/*
 * MDL loader implementation - CORRECTED VERSION
 * Fixed triangle parsing and structure names based on Valve's format
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
        fclose(file);
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
        return;
    }

    printf("\nDetailed Bodypart Information:\n");
    // FIXED: Use correct structure name
    mstudiobodyparts_t *bodyparts = (mstudiobodyparts_t *)(file_data + header->bodypartindex);

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

mdl_result_t load_model_with_textures(const char *model_path, studiohdr_t **main_header, 
                                     studiohdr_t **texture_header, unsigned char **main_data, 
                                     unsigned char **texture_data) {
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
        printf("Model [%d][%d]: NULL\n", bodypart_index, model_index);
        return;
    }

    printf("   Model[%d][%d]: %s\n", bodypart_index, model_index, model->name);
    printf("     Type: %d\n", model->type);
    printf("        Bounding radius: %.2f\n", model->boundingradius);
    printf("        Vertices: %d\n", model->numverts);
    printf("        Meshes: %d\n", model->nummesh);
    printf("        Normals: %d\n", model->numnorms);
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
        printf("        Triangle cmd offset: 0x%X\n", meshes[i].triindex);
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


// Simple triangle index generation for basic wireframe (fallback)
mdl_result_t create_simple_triangle_indices(int vertex_count, short **indices, int *index_count) {
    if (!indices || !index_count || vertex_count <= 0) {
        return MDL_ERROR_INVALID_PARAMETER;
    }
    
    // Create triangles from consecutive vertices (not perfect but works)
    *index_count = (vertex_count / 3) * 3;
    if (*index_count == 0) {
        *indices = NULL;
        return MDL_SUCCESS;
    }
    
    *indices = malloc(*index_count * sizeof(short));
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
    
    short *simple_indices = NULL;
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

// Parse triangle commands and extract texture coordinates
mdl_result_t parse_triangle_commands_fixed(mstudiomesh_t *mesh, unsigned char *data, 
                                          short **indices, int *index_count,
                                          float **tex_coords, int *tex_coord_count) {
    if (!mesh || !data || !indices || !index_count) {
        fprintf(stderr, "ERROR - Invalid parameters passed to parse_triangle_commands_fixed()!\n");
        return MDL_ERROR_INVALID_PARAMETER;
    }

    if (mesh->numtris == 0) {
        *indices = NULL;
        *index_count = 0;
        if (tex_coords) *tex_coords = NULL;
        if (tex_coord_count) *tex_coord_count = 0;
        return MDL_SUCCESS;
    }

    // Navigate to triangle command data
    short *cmd_ptr = (short *)(data + mesh->triindex);
    
    // First pass: count indices needed
    int total_indices = 0;
    short *current = cmd_ptr;
    
    while (true) {
        short vertex_count = *current++;
        if (vertex_count == 0) break;
        
        int abs_count = (vertex_count < 0) ? -vertex_count : vertex_count;
        if (vertex_count < 0) {
            // Triangle fan: (N-2) triangles
            total_indices += (abs_count - 2) * 3;
        } else {
            // Triangle strip: (N-2) triangles  
            total_indices += (abs_count - 2) * 3;
        }
        
        // Skip over the vertex data (each vertex is 4 shorts: vertindex, normindex, s, t)
        current += abs_count * 4;
    }
    
    // Allocate memory for indices and texture coordinates
    *indices = malloc(total_indices * sizeof(short));
    if (!*indices) {
        return MDL_ERROR_MEMORY_ALLOCATION;
    }
    
    float *texcoords = NULL;
    bool debug_uvs = true;
    if (tex_coords && tex_coord_count) {
        texcoords = malloc(total_indices * 2 * sizeof(float)); // 2 floats per vertex (u,v)
        if (!texcoords) {
            free(*indices);
            *indices = NULL;
            return MDL_ERROR_MEMORY_ALLOCATION;
        }
        *tex_coords = texcoords;
        *tex_coord_count = total_indices * 2;
    }
    
    *index_count = total_indices;
    
    // Second pass: extract indices and texture coordinates
    current = cmd_ptr;
    int output_idx = 0;
    int tex_idx = 0;

    short min_s;
    short min_t;
    short max_t;
    short max_s;
    
    while (true) {
        short vertex_count = *current++;
        if (vertex_count == 0) break;
        
        mstudiotrivert_t *verts = (mstudiotrivert_t *)current;
        
        if (vertex_count < 0) {
            // Triangle fan
            vertex_count = -vertex_count;
            
            // Track UV range
            for (int v = 0; v < vertex_count; v++) {
                if (verts[v].s < min_s) min_s = verts[v].s;
                if (verts[v].s > max_s) max_s = verts[v].s;
                if (verts[v].t < min_t) min_t = verts[v].t;
                if (verts[v].t > max_t) max_t = verts[v].t;
            }
            
            for (int i = 2; i < vertex_count; i++) {
                // Triangle: 0, i-1, i
                (*indices)[output_idx++] = verts[0].vertindex;
                (*indices)[output_idx++] = verts[i-1].vertindex;
                (*indices)[output_idx++] = verts[i].vertindex;
                
                if (texcoords) {
                    // Convert s,t from shorts to normalized floats
                    // Half-Life stores texture coords as shorts, we need to normalize them
                    texcoords[tex_idx++] = verts[0].s / 32768.0f;
                    texcoords[tex_idx++] = verts[0].t / 32768.0f;
                    texcoords[tex_idx++] = verts[i-1].s / 32768.0f;
                    texcoords[tex_idx++] = verts[i-1].t / 32768.0f;
                    texcoords[tex_idx++] = verts[i].s / 32768.0f;
                    texcoords[tex_idx++] = verts[i].t / 32768.0f;
                }
            }
        } else {
            // Triangle strip
            
            // Track UV range
            for (int v = 0; v < vertex_count; v++) {
                if (verts[v].s < min_s) min_s = verts[v].s;
                if (verts[v].s > max_s) max_s = verts[v].s;
                if (verts[v].t < min_t) min_t = verts[v].t;
                if (verts[v].t > max_t) max_t = verts[v].t;
            }
            
            for (int i = 2; i < vertex_count; i++) {
                if (i % 2 == 0) {
                    // Even triangles: i-2, i-1, i
                    (*indices)[output_idx++] = verts[i-2].vertindex;
                    (*indices)[output_idx++] = verts[i-1].vertindex;
                    (*indices)[output_idx++] = verts[i].vertindex;
                    
                    if (texcoords) {
                        texcoords[tex_idx++] = verts[i-2].s / 32768.0f;
                        texcoords[tex_idx++] = verts[i-2].t / 32768.0f;
                        texcoords[tex_idx++] = verts[i-1].s / 32768.0f;
                        texcoords[tex_idx++] = verts[i-1].t / 32768.0f;
                        texcoords[tex_idx++] = verts[i].s / 32768.0f;
                        texcoords[tex_idx++] = verts[i].t / 32768.0f;
                    }
                } else {
                    // Odd triangles (reversed winding): i-1, i-2, i
                    (*indices)[output_idx++] = verts[i-1].vertindex;
                    (*indices)[output_idx++] = verts[i-2].vertindex;
                    (*indices)[output_idx++] = verts[i].vertindex;
                    
                    if (texcoords) {
                        texcoords[tex_idx++] = verts[i-1].s / 32768.0f;
                        texcoords[tex_idx++] = verts[i-1].t / 32768.0f;
                        texcoords[tex_idx++] = verts[i-2].s / 32768.0f;
                        texcoords[tex_idx++] = verts[i-2].t / 32768.0f;
                        texcoords[tex_idx++] = verts[i].s / 32768.0f;
                        texcoords[tex_idx++] = verts[i].t / 32768.0f;
                    }
                }
            }
        }
        
        current += vertex_count * 4;
    }
    
    // Debug UV range
    if (texcoords && debug_uvs) {
        printf("    UV Range Debug: s=[%d to %d], t=[%d to %d]\n", min_s, max_s, min_t, max_t);
        
        // Check if these look like actual texture coordinates or pixels
        if (max_s > 1000 || max_t > 1000) {
            printf("    WARNING: UVs appear to be in pixel space, not normalized!\n");
            printf("    Likely texture size: %dx%d\n", max_s, max_t);
        }
    }
    
    printf("    Parsed %d triangle indices", total_indices);
    if (texcoords) {
        printf(" with texture coordinates");
    }
    printf("\n");
    
    return MDL_SUCCESS;
}

void transform_vertices_to_opengl(vec3_t *hl_vertices, int count, float *gl_vertices, float scale) {
    for (int i = 0; i < count; i++) {
        // Half-Life stores the model lying on the Y axis
        // We need to rotate 90 degrees to stand it up
        // Original: X=side, Y=length (along body), Z=front/back
        // Target: X=side, Y=up, Z=front/back
        
        gl_vertices[i * 3 + 0] = hl_vertices[i][0] * scale;  // X (side to side) stays X
        gl_vertices[i * 3 + 1] = hl_vertices[i][1] * scale;  // Y becomes height (already correct)
        gl_vertices[i * 3 + 2] = hl_vertices[i][2] * scale;  // Z (front/back) stays Z
    }
}

// Extract triangles with proper UV coordinates per vertex

// Extract texture as RGB data from MDL file
mdl_result_t extract_texture_rgb(studiohdr_t *texture_header, unsigned char *texture_data,
                                 int texture_index, unsigned char **rgb_output,
                                 int *width, int *height) {
    
    if (!texture_header || !texture_data || !rgb_output) {
        return MDL_ERROR_INVALID_PARAMETER;
    }
    
    // Get texture metadata array
    mstudiotexture_t *textures = (mstudiotexture_t *)(texture_data + texture_header->textureindex);
    
    if (texture_index >= texture_header->numtextures) {
        return MDL_ERROR_INVALID_PARAMETER;
    }
    
    mstudiotexture_t *tex = &textures[texture_index];
    *width = tex->width;
    *height = tex->height;
    
    // Get indexed pixel data (each pixel is 1 byte)
    unsigned char *indexed_pixels = texture_data + texture_header->texturedataindex + tex->index;
    
    // Get palette (256 colors * 3 bytes RGB = 768 bytes)
    // Palette immediately follows the indexed pixel data
    unsigned char *palette = indexed_pixels + (tex->width * tex->height);
    
    // Allocate RGB output (3 bytes per pixel)
    int pixel_count = tex->width * tex->height;
    *rgb_output = malloc(pixel_count * 3);
    if (!*rgb_output) {
        return MDL_ERROR_MEMORY_ALLOCATION;
    }
    
    // Convert indexed to RGB
    for (int i = 0; i < pixel_count; i++) {
        unsigned char palette_index = indexed_pixels[i];
        
        // Copy RGB values from palette
        (*rgb_output)[i * 3 + 0] = palette[palette_index * 3 + 0];  // R
        (*rgb_output)[i * 3 + 1] = palette[palette_index * 3 + 1];  // G
        (*rgb_output)[i * 3 + 2] = palette[palette_index * 3 + 2];  // B
    }
    
    printf("Extracted texture %d: %s (%dx%d)\n", texture_index, tex->name, tex->width, tex->height);
    return MDL_SUCCESS;
}

mdl_result_t extract_triangles_with_uvs(mstudiomesh_t *mesh, unsigned char *data,
                                        vec3_t *model_vertices, int model_vertex_count,
                                        float **out_vertices, float **out_texcoords, 
                                        int *out_vertex_count) {
    
    if (!mesh || !data || !model_vertices || !out_vertices || !out_texcoords || !out_vertex_count) {
        return MDL_ERROR_INVALID_PARAMETER;
    }
    
    if (mesh->numtris == 0) {
        *out_vertices = NULL;
        *out_texcoords = NULL;
        *out_vertex_count = 0;
        return MDL_SUCCESS;
    }
    
    // Navigate to triangle command data
    short *cmd_ptr = (short *)(data + mesh->triindex);
    
    // First pass: count total vertices needed (3 per triangle)
    int expected_triangles = mesh->numtris;
    int total_vertices = expected_triangles * 3;  // Allocate for expected triangles
    
    // Allocate output arrays
    float *vertices = malloc(total_vertices * 3 * sizeof(float));
    float *texcoords = malloc(total_vertices * 2 * sizeof(float));
    
    if (!vertices || !texcoords) {
        if (vertices) free(vertices);
        if (texcoords) free(texcoords);
        return MDL_ERROR_MEMORY_ALLOCATION;
    }
    
    // Second pass: extract triangles
    short *current = cmd_ptr;
    int out_idx = 0;
    
    while (out_idx < total_vertices) {
        short vertex_count = *current++;
        if (vertex_count == 0) break;
        
        mstudiotrivert_t *verts = (mstudiotrivert_t *)current;
        
        if (vertex_count < 0) {
            // Triangle fan
            vertex_count = -vertex_count;
            for (int i = 2; i < vertex_count; i++) {
                // Triangle fan: pivot is vertex 0
                int indices[3] = { verts[0].vertindex, verts[i-1].vertindex, verts[i].vertindex };
                
                for (int j = 0; j < 3; j++) {
                    int vidx = indices[j];
                    if (vidx >= 0 && vidx < model_vertex_count) {
                        // Transform from Half-Life to OpenGL coordinate system
                        // Half-Life uses: X=right, Y=forward, Z=up (right-handed)
                        // OpenGL uses: X=right, Y=up, Z=backward (right-handed)
                        // Simple transformation: swap Y and Z
                        
                        float hl_x = model_vertices[vidx][0];  // right
                        float hl_y = model_vertices[vidx][1];  // forward  
                        float hl_z = model_vertices[vidx][2];  // up
                        
                        // Direct mapping:
                        // HL X (right) -> GL X (right)
                        // HL Z (up) -> GL Y (up) 
                        // HL Y (forward) -> GL -Z (backward)
                        vertices[out_idx * 3 + 0] = hl_x * 0.02f;   // X stays X
                        vertices[out_idx * 3 + 1] = hl_z * 0.02f;   // Z becomes Y (up)
                        vertices[out_idx * 3 + 2] = -hl_y * 0.02f;  // -Y becomes Z (backward)
                    } else {
                        // Invalid index, use zero
                        vertices[out_idx * 3 + 0] = 0;
                        vertices[out_idx * 3 + 1] = 0;
                        vertices[out_idx * 3 + 2] = 0;
                    }
                    
                    // Copy UV coordinates (s,t are in texture pixel coordinates)
                    // Map j index to the correct vertex in the fan
                    int vert_idx;
                    if (j == 0) vert_idx = 0;        // First vertex (pivot)
                    else if (j == 1) vert_idx = i-1; // Previous vertex
                    else vert_idx = i;                // Current vertex
                    
                    // UV coords are stored as texture pixel coordinates
                    // Need to normalize to 0-1 range for OpenGL
                    // Assuming standard 256x256 texture (most common in Half-Life)
                    // TODO: Get actual texture dimensions from mesh->skinref
                    texcoords[out_idx * 2 + 0] = (float)verts[vert_idx].s / 256.0f;
                    texcoords[out_idx * 2 + 1] = (float)verts[vert_idx].t / 256.0f;
                    out_idx++;
                }
            }
        } else {
            // Triangle strip
            for (int i = 2; i < vertex_count; i++) {
                int indices[3];
                int uv_indices[3];  // Track which vertex to use for UVs
                
                if (i % 2 == 0) {
                    // Even triangle - normal winding
                    indices[0] = verts[i-2].vertindex;
                    indices[1] = verts[i-1].vertindex;
                    indices[2] = verts[i].vertindex;
                    uv_indices[0] = i-2;
                    uv_indices[1] = i-1;
                    uv_indices[2] = i;
                } else {
                    // Odd triangle - reverse winding for correct orientation
                    indices[0] = verts[i-1].vertindex;
                    indices[1] = verts[i-2].vertindex;
                    indices[2] = verts[i].vertindex;
                    uv_indices[0] = i-1;
                    uv_indices[1] = i-2;
                    uv_indices[2] = i;
                }
                
                for (int j = 0; j < 3; j++) {
                    int vidx = indices[j];
                    if (vidx >= 0 && vidx < model_vertex_count) {
                        // Transform from Half-Life to OpenGL coordinate system
                        // Half-Life uses: X=right, Y=forward, Z=up (right-handed)
                        // OpenGL uses: X=right, Y=up, Z=backward (right-handed)
                        // Simple transformation: swap Y and Z
                        
                        float hl_x = model_vertices[vidx][0];  // right
                        float hl_y = model_vertices[vidx][1];  // forward  
                        float hl_z = model_vertices[vidx][2];  // up
                        
                        // Direct mapping:
                        // HL X (right) -> GL X (right)
                        // HL Z (up) -> GL Y (up) 
                        // HL Y (forward) -> GL -Z (backward)
                        vertices[out_idx * 3 + 0] = hl_x * 0.02f;   // X stays X
                        vertices[out_idx * 3 + 1] = hl_z * 0.02f;   // Z becomes Y (up)
                        vertices[out_idx * 3 + 2] = -hl_y * 0.02f;  // -Y becomes Z (backward)
                    } else {
                        vertices[out_idx * 3 + 0] = 0;
                        vertices[out_idx * 3 + 1] = 0;
                        vertices[out_idx * 3 + 2] = 0;
                    }
                    
                    // UV coords are stored as texture pixel coordinates
                    // Need to normalize to 0-1 range for OpenGL
                    // Assuming standard 256x256 texture (most common in Half-Life)
                    // TODO: Get actual texture dimensions from mesh->skinref
                    texcoords[out_idx * 2 + 0] = (float)verts[uv_indices[j]].s / 256.0f;
                    texcoords[out_idx * 2 + 1] = (float)verts[uv_indices[j]].t / 256.0f;
                    out_idx++;
                }
            }
        }
        
        current += vertex_count * 4;
    }
    
    *out_vertices = vertices;
    *out_texcoords = texcoords;
    *out_vertex_count = out_idx;
    
    printf("    Extracted %d vertices with UVs\n", out_idx);
    
    return MDL_SUCCESS;
}
