/*
* @Author: karlosiric
* @Date:   2025-07-18 12:28:34
* @Last Modified by:   karlosiric
* @Last Modified time: 2025-07-31 17:27:58
*/

#define GL_SILENCE_DEPRECATION
#include "mdl_loader.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <OpenGL/gl3.h>

studiohdr_t *mdl_read_header(FILE *file) {
    rewind(file);

    studiohdr_t *header = (studiohdr_t *)malloc(sizeof(studiohdr_t));
    if (!header) {
        fprintf(stderr, "ERROR - Failed to allocate memory for the header!\n");
        return NULL;
    }

    size_t bytes_read = fread(header, sizeof(studiohdr_t), 1, file);
    if (bytes_read < 1) {
        fprintf(stderr, "ERROR - Failed to read header\n");
        free(header);
        return NULL;
    }

    if (header->id != IDSTUDIOHEADER) {
        fprintf(stderr, "ERROR - Invalid magic number. Expected 0x%08X, got 0x%08X\n",
                IDSTUDIOHEADER, header->id);
        free(header);
        return NULL;
    }

    printf("SUCCESS: Read valid MDL header for '%s'\n",
            header->name);

    return header;
}

mstudiobodypart_t *mdl_read_bodyparts(FILE *file, studiohdr_t *header) {

    fseek(file, header->bodypartindex, SEEK_SET);

    mstudiobodypart_t *bodyparts = (mstudiobodypart_t *)malloc(header->numbodyparts * sizeof(mstudiobodypart_t));

    if (!bodyparts) {
        fprintf(stderr, "ERROR - Failed to allocate enough memory for all bodyparts!\n");
        return NULL; 
    }

    size_t bytes_read = fread(bodyparts, sizeof(mstudiobodypart_t), header->numbodyparts, file);
    size_t bodyparts_bytes = header->numbodyparts * sizeof(mstudiobodypart_t);
    if (bytes_read != (size_t)header->numbodyparts) {
        fprintf(stderr, "ERROR - Failed to read %d bodyparts, Expected %zu and got %zu bytes.\n",
                header->numbodyparts, bodyparts_bytes, bytes_read);
        free(bodyparts);
        return NULL;
    }

    printf("SUCCESS: Read %d bodyparts, containing %zu bytes.\n",
            header->numbodyparts, bytes_read);
    return bodyparts;
}

mstudiomodel_t *mdl_read_models_for_bodyparts(FILE *file, mstudiobodypart_t *bodypart) {
    // TOOD Karlo: Need to read all given models for that bodypart

    fseek(file, bodypart->modelindex, SEEK_SET);

    mstudiomodel_t *models = malloc(bodypart->nummodels * sizeof(mstudiomodel_t));
    if (!models) {
        fprintf(stderr, "ERROR - Failed to allocate enough space for %d models.\n", bodypart->nummodels);
        return NULL;
    }

    size_t items_read = fread(models, sizeof(mstudiomodel_t), bodypart->nummodels, file);
    if (items_read != (size_t)bodypart->nummodels) {
        fprintf(stderr, "ERROR - Failed to read models. Expected %d and got %zu.\n",
                bodypart->nummodels, items_read);
        free(models);
        return NULL;
    }

    printf("  SUCCESS: Read %d models for bodypart: '%s' \n",
            bodypart->nummodels, bodypart->name);
    return models;
}

vec3_t *mdl_read_vertices(FILE *file, mstudiomodel_t *model) {
    fseek(file, model->vertindex, SEEK_SET);

    vec3_t *vertices = malloc(model->numverts * sizeof(vec3_t));
    if (!vertices) {
        fprintf(stderr, "ERROR - Failed to allocate enough space for %d vertices of model '%s' \n",
                model->numverts, model->name);
        return NULL;
    }

    size_t vertices_read = fread(vertices, sizeof(vec3_t), model->numverts, file);
    if (vertices_read != (size_t)model->numverts) {
        fprintf(stderr, "ERROR - Failed to read vertices. Expected %d and got %zu. \n",
                model->numverts, vertices_read);
        free(vertices);
        return NULL;
    }

    printf("  SUCCESS: Read %d vertices for model '%s'.\n",
            model->numverts, model->name);
    return vertices;
}

mdl_mesh_render_data_s *mdl_read_triangle_commands(FILE *file, mstudiomodel_t *model) {
    printf("  Reading triangle commands for model '%s'\n", model->name);
    
    if (model->nummesh == 0) {
        printf("  Model '%s' has no meshes\n", model->name);
        return NULL;
    }

    // Read mesh headers first
    fseek(file, model->meshindex, SEEK_SET);
    mstudiomesh_t *meshes = malloc(model->nummesh * sizeof(mstudiomesh_t));
    if (!meshes) return NULL;
    
    fread(meshes, sizeof(mstudiomesh_t), model->nummesh, file);
    
    mdl_mesh_render_data_s *render_data = malloc(sizeof(mdl_mesh_render_data_s));
    if (!render_data) {
        free(meshes);
        return NULL;
    }
    
    // Initialize render data
    render_data->triangle_commands = NULL;
    render_data->command_count = 0;
    render_data->vertices_flat = NULL;
    render_data->texcoords_flat = NULL;
    render_data->indices_flat = NULL;
    render_data->vertex_count = 0;
    render_data->texcoord_count = 0;
    render_data->index_count = 0;
    
    // Estimate maximum triangles needed
    int max_indices = 0;
    for (int i = 0; i < model->nummesh; i++) {
        max_indices += meshes[i].numtris * 3;
    }
    
    if (max_indices == 0) {
        free(meshes);
        free(render_data);
        return NULL;
    }
    
    render_data->indices_flat = malloc(max_indices * sizeof(int));
    render_data->texcoords_flat = malloc(max_indices * 2 * sizeof(float));
    
    // Process each mesh
    for (int mesh_idx = 0; mesh_idx < model->nummesh; mesh_idx++) {
        mstudiomesh_t *mesh = &meshes[mesh_idx];
        
        printf("    Processing mesh %d: %d triangles\n", mesh_idx, mesh->numtris);
        
        // Seek to triangle command data
        fseek(file, mesh->triindex, SEEK_SET);
        
        int command;
        while (fread(&command, sizeof(int), 1, file) && command != 0) {
            
            int num_verts = abs(command);
            bool is_fan = (command < 0);
            
            printf("      Command: %s with %d vertices\n", 
                   is_fan ? "TRIANGLE_FAN" : "TRIANGLE_STRIP", num_verts);
            
            // Read vertex data for this primitive
            mstudiotrivert_t *triverts = malloc(num_verts * sizeof(mstudiotrivert_t));
            if (!triverts) continue;
            
            fread(triverts, sizeof(mstudiotrivert_t), num_verts, file);
            
            // Convert triangle strips/fans to individual triangles
            if (is_fan) {
                // Triangle fan: vertices 0,1,2 then 0,2,3 then 0,3,4 etc.
                for (int i = 2; i < num_verts; i++) {
                    if (render_data->index_count < max_indices - 3) {
                        render_data->indices_flat[render_data->index_count++] = triverts[0].vertindex;
                        render_data->indices_flat[render_data->index_count++] = triverts[i-1].vertindex; 
                        render_data->indices_flat[render_data->index_count++] = triverts[i].vertindex;
                        
                        // Store texture coordinates
                        int tex_idx = render_data->texcoord_count;
                        render_data->texcoords_flat[tex_idx++] = triverts[0].s / 256.0f;
                        render_data->texcoords_flat[tex_idx++] = triverts[0].t / 256.0f;
                        render_data->texcoords_flat[tex_idx++] = triverts[i-1].s / 256.0f;
                        render_data->texcoords_flat[tex_idx++] = triverts[i-1].t / 256.0f;
                        render_data->texcoords_flat[tex_idx++] = triverts[i].s / 256.0f;
                        render_data->texcoords_flat[tex_idx++] = triverts[i].t / 256.0f;
                        render_data->texcoord_count = tex_idx;
                    }
                }
            } else {
                // Triangle strip: 0,1,2 then 1,3,2 (note winding) then 2,3,4 etc.
                for (int i = 2; i < num_verts; i++) {
                    if (render_data->index_count < max_indices - 3) {
                        if (i % 2 == 0) {
                            // Even triangle - normal winding
                            render_data->indices_flat[render_data->index_count++] = triverts[i-2].vertindex;
                            render_data->indices_flat[render_data->index_count++] = triverts[i-1].vertindex;
                            render_data->indices_flat[render_data->index_count++] = triverts[i].vertindex;
                        } else {
                            // Odd triangle - reverse winding for correct orientation
                            render_data->indices_flat[render_data->index_count++] = triverts[i-1].vertindex;
                            render_data->indices_flat[render_data->index_count++] = triverts[i-2].vertindex;
                            render_data->indices_flat[render_data->index_count++] = triverts[i].vertindex;
                        }
                        
                        // Store texture coordinates
                        int tex_idx = render_data->texcoord_count;
                        if (i % 2 == 0) {
                            render_data->texcoords_flat[tex_idx++] = triverts[i-2].s / 256.0f;
                            render_data->texcoords_flat[tex_idx++] = triverts[i-2].t / 256.0f;
                            render_data->texcoords_flat[tex_idx++] = triverts[i-1].s / 256.0f;
                            render_data->texcoords_flat[tex_idx++] = triverts[i-1].t / 256.0f;
                            render_data->texcoords_flat[tex_idx++] = triverts[i].s / 256.0f;
                            render_data->texcoords_flat[tex_idx++] = triverts[i].t / 256.0f;
                        } else {
                            render_data->texcoords_flat[tex_idx++] = triverts[i-1].s / 256.0f;
                            render_data->texcoords_flat[tex_idx++] = triverts[i-1].t / 256.0f;
                            render_data->texcoords_flat[tex_idx++] = triverts[i-2].s / 256.0f;
                            render_data->texcoords_flat[tex_idx++] = triverts[i-2].t / 256.0f;
                            render_data->texcoords_flat[tex_idx++] = triverts[i].s / 256.0f;
                            render_data->texcoords_flat[tex_idx++] = triverts[i].t / 256.0f;
                        }
                        render_data->texcoord_count = tex_idx;
                    }
                }
            }
            
            free(triverts);
        }
    }
    
    free(meshes);
    
    printf("  SUCCESS: Converted to %d triangles (%d indices)\n", 
           render_data->index_count / 3, render_data->index_count);
    
    return render_data;
}


long mdl_get_file_size(FILE *file) {
    long current_pos = ftell(file);
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, current_pos, SEEK_SET);
    return size;
}

// =================================
// BONE SYSTEM FUNCTIONS
// =================================

mstudiobone_t *mdl_read_bones(FILE *file, studiohdr_t *header) {
    if (header->numbones == 0) {
        printf("Model has no bones\n");
        return NULL;
    }
    
    printf("Reading %d bones...\n", header->numbones);
    
    fseek(file, header->boneindex, SEEK_SET);
    mstudiobone_t *bones = malloc(header->numbones * sizeof(mstudiobone_t));
    if (!bones) return NULL;
    
    fread(bones, sizeof(mstudiobone_t), header->numbones, file);
    
    printf("Successfully read %d bones\n", header->numbones);
    return bones;
}

mstudioboneweight_t *mdl_read_vertex_bone_weights(FILE *file, mstudiomodel_t *model) {
    // Vertex bone info is stored at vertinfoindex
    fseek(file, model->vertinfoindex, SEEK_SET);
    
    mstudioboneweight_t *bone_weights = malloc(model->numverts * sizeof(mstudioboneweight_t));
    if (!bone_weights) return NULL;
    
    fread(bone_weights, sizeof(mstudioboneweight_t), model->numverts, file);
    
    printf("  SUCCESS: Read bone weights for %d vertices\n", model->numverts);
    return bone_weights;
}

void mdl_setup_bones(bone_transform_state_t *state, mstudiobone_t *bones, int bone_count) {
    // Initialize with identity matrices (flattened 4x4)
    for (int i = 0; i < bone_count; i++) {
        // Identity matrix
        for (int j = 0; j < 16; j++) {
            state->bone_matrices[i][j] = (j % 5 == 0) ? 1.0f : 0.0f;
        }
    }
    
    // Apply bone hierarchy transformations
    for (int i = 0; i < bone_count; i++) {
        mstudiobone_t *bone = &bones[i];
        
        // For now, just use default pose (no animation)
        // TODO: Add animation frame interpolation
        
        // Set translation in matrix (last column)
        state->bone_matrices[i][12] = bone->value[0]; // X
        state->bone_matrices[i][13] = bone->value[1]; // Y  
        state->bone_matrices[i][14] = bone->value[2]; // Z
        
        // TODO: Add rotation from bone->value[3,4,5]
        // TODO: Apply parent transformations
    }
    
    state->bone_count = bone_count;
}

void mdl_transform_vertices_by_bones(bone_transform_state_t *bone_state, vec3_t *original_vertices,
                                    int vertex_count, mstudioboneweight_t *bone_weights, float *output_vertices) {
    
    for (int i = 0; i < vertex_count; i++) {
        vec3_t *original = &original_vertices[i];
        int bone_index = bone_weights[i].bone_index;
        
        // Clamp bone index to valid range
        if (bone_index >= bone_state->bone_count) {
            bone_index = 0;
        }
        
        // Apply bone transformation (simplified)
        float *matrix = bone_state->bone_matrices[bone_index];
        
        // Transform vertex by bone matrix (simplified - just translation for now)
        output_vertices[i * 3 + 0] = original->x + matrix[12];
        output_vertices[i * 3 + 1] = original->y + matrix[13];
        output_vertices[i * 3 + 2] = original->z + matrix[14];
    }
}

void mdl_free_render_data(mdl_mesh_render_data_s *render_data) {
    if (!render_data) return;
    
    if (render_data->triangle_commands) free(render_data->triangle_commands);
    if (render_data->vertices_flat) free(render_data->vertices_flat);
    if (render_data->texcoords_flat) free(render_data->texcoords_flat);
    if (render_data->indices_flat) free(render_data->indices_flat);
    
    free(render_data);
}

// =================================
// TEXTURE SYSTEM FUNCTIONS
// =================================

mdl_texture_t *mdl_read_textures(FILE *file, studiohdr_t *header) {
    if (header->numtextures == 0) {
        printf("Model has no textures\n");
        return NULL;
    }
    
    printf("Loading %d textures...\n", header->numtextures);
    
    mdl_texture_t *textures = malloc(header->numtextures * sizeof(mdl_texture_t));
    if (!textures) return NULL;
    
    // Read texture headers
    fseek(file, header->textureindex, SEEK_SET);
    mstudiotexture_t *tex_headers = malloc(header->numtextures * sizeof(mstudiotexture_t));
    fread(tex_headers, sizeof(mstudiotexture_t), header->numtextures, file);
    
    // Load each texture
    for (int i = 0; i < header->numtextures; i++) {
        mstudiotexture_t *tex_hdr = &tex_headers[i];
        mdl_texture_t *texture = &textures[i];
        
        // Copy basic info
        strncpy(texture->name, tex_hdr->name, sizeof(texture->name));
        texture->flags = tex_hdr->flags;
        texture->width = tex_hdr->width;
        texture->height = tex_hdr->height;
        texture->index = tex_hdr->index;
        
        printf("  Texture %d: '%s' (%dx%d)\n", i, texture->name, 
               texture->width, texture->height);
        
        // Read texture data
        int data_size = texture->width * texture->height;
        texture->data = malloc(data_size);
        
        fseek(file, header->texturedataindex + texture->index, SEEK_SET);
        fread(texture->data, 1, data_size, file);
        
        // Read palette (256 colors * 3 components)
        texture->palette = malloc(256 * 3);
        fread(texture->palette, 1, 256 * 3, file);
        
        // Convert indexed texture to RGB
        unsigned char *rgb_data = mdl_convert_indexed_to_rgb(texture);
        
        // Create OpenGL texture
        texture->gl_texture_id = mdl_create_opengl_texture(rgb_data, texture->width, texture->height);
        
        free(rgb_data);
    }
    
    free(tex_headers);
    printf("Successfully loaded %d textures\n", header->numtextures);
    return textures;
}

unsigned char *mdl_convert_indexed_to_rgb(mdl_texture_t *texture) {
    int pixel_count = texture->width * texture->height;
    unsigned char *rgb_data = malloc(pixel_count * 3);
    
    for (int i = 0; i < pixel_count; i++) {
        unsigned char palette_index = texture->data[i];
        
        // Convert from indexed to RGB using palette
        rgb_data[i * 3 + 0] = texture->palette[palette_index * 3 + 0]; // R
        rgb_data[i * 3 + 1] = texture->palette[palette_index * 3 + 1]; // G  
        rgb_data[i * 3 + 2] = texture->palette[palette_index * 3 + 2]; // B
        
        // Handle transparent pixels (index 255 is usually transparent)
        if (palette_index == 255) {
            rgb_data[i * 3 + 0] = 0;
            rgb_data[i * 3 + 1] = 0;
            rgb_data[i * 3 + 2] = 0;
        }
    }
    
    return rgb_data;
}

unsigned int mdl_create_opengl_texture(unsigned char *rgb_data, int width, int height) {
    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    
    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb_data);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    return texture_id;
}

void mdl_free_textures(mdl_texture_t *textures, int count) {
    if (!textures) return;
    
    for (int i = 0; i < count; i++) {
        if (textures[i].data) free(textures[i].data);
        if (textures[i].palette) free(textures[i].palette);
        if (textures[i].gl_texture_id) {
            glDeleteTextures(1, &textures[i].gl_texture_id);
        }
    }
    
    free(textures);
}


mdl_complete_model_s *mdl_load_complete_file(const char *filename) {
    printf("\n=== LOADING COMPLETE MDL FILE: %s ===\n", filename);

    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "ERROR - Failed to open the file '%s' \n", filename);
        return NULL;
    }

    studiohdr_t *header = mdl_read_header(file);
    if (!header) {
        fclose(file);
        return NULL;
    }

    // Read all major components
    mstudiobodypart_t *bodyparts = mdl_read_bodyparts(file, header);
    mstudiobone_t *bones = mdl_read_bones(file, header);
    mdl_texture_t *textures = mdl_read_textures(file, header);
    
    if (!bodyparts) {
        printf("WARNING: No bodyparts found\n");
        free(header);
        fclose(file);
        return NULL;
    }

    int total_models = 0;
    for (int m = 0; m < header->numbodyparts; m++) {
        total_models += bodyparts[m].nummodels;
    }

    printf("Total models to process: %d\n", total_models);

    mdl_complete_model_s *complete_model = malloc(sizeof(mdl_complete_model_s));
    if (!complete_model) {
        fprintf(stderr, "ERROR - Failed to allocate complete model structure\n");
        free(bodyparts);
        free(header);
        fclose(file);
        return NULL;
    }

    // Initialize the complete model structure
    complete_model->models = malloc(total_models * sizeof(single_model_s));
    if (!complete_model->models) {
        fprintf(stderr, "ERROR - Failed to allocate models array\n");
        free(complete_model);
        free(bodyparts);
        free(header);
        fclose(file);
        return NULL;
    }

    // Initialize all pointers to NULL
    for (int i = 0; i < total_models; i++) {
        memset(&complete_model->models[i], 0, sizeof(single_model_s));
    }

    complete_model->total_model_count = total_models;
    complete_model->bone_count = header->numbones;
    complete_model->texture_count = header->numtextures;
    complete_model->bodypart_count = header->numbodyparts;
    complete_model->bones = bones;
    complete_model->textures = textures;
    complete_model->skin_families = NULL; // TODO: implement skin families
    complete_model->skin_family_count = header->numskinfamilies;
    complete_model->skin_ref_count = header->numskinref;
    
    strncpy(complete_model->filename, filename, sizeof(complete_model->filename) - 1);
    complete_model->filename[sizeof(complete_model->filename) - 1] = '\0';

    int model_index = 0;
    int successful_models = 0;

    for (int bp = 0; bp < header->numbodyparts; bp++) {
        printf("\nProcessing bodypart[%d]: '%s' (%d models)\n",
                bp, bodyparts[bp].name, bodyparts[bp].nummodels);

        mstudiomodel_t *models = mdl_read_models_for_bodyparts(file, &bodyparts[bp]);
        if (!models) {
            printf("  WARNING: Failed to read models for bodyparts '%s' \n",
                    bodyparts[bp].name);
            continue;
        }

        for (int m = 0; m < bodyparts[bp].nummodels; m++) {
            printf("  Processing model[%d]: '%s' \n", m, models[m].name);

            single_model_s *current_model = &complete_model->models[model_index];
            
            // Copy model info
            strncpy(current_model->model_name, models[m].name, sizeof(current_model->model_name) - 1);
            current_model->model_name[sizeof(current_model->model_name) - 1] = '\0';
            strncpy(current_model->bodypart_name, bodyparts[bp].name, sizeof(current_model->bodypart_name) - 1);
            current_model->bodypart_name[sizeof(current_model->bodypart_name) - 1] = '\0';
            current_model->bodypart_id = bp;
            current_model->model_id = m;

            // Read vertices
            vec3_t *vertices = mdl_read_vertices(file, &models[m]);
            if (!vertices) {
                printf("   WARNING: Failed to read vertices for model: '%s' \n", models[m].name);
                model_index++;
                continue;
            }

            current_model->vertex_count = models[m].numverts;
            current_model->vertices = malloc(current_model->vertex_count * 3 * sizeof(float));
            if (!current_model->vertices) {
                fprintf(stderr, "   ERROR - Failed to allocate vertex array for model '%s' \n", models[m].name);
                free(vertices);
                model_index++;
                continue;
            }

            // Convert vertices to flat array
            for (int v = 0; v < current_model->vertex_count; v++) {
                current_model->vertices[v * 3 + 0] = vertices[v].x;
                current_model->vertices[v * 3 + 1] = vertices[v].y;
                current_model->vertices[v * 3 + 2] = vertices[v].z;
            }

            // Read bone weights
            current_model->bone_weights = mdl_read_vertex_bone_weights(file, &models[m]);
            
            // Set up bone transformations
            if (bones && current_model->bone_weights) {
                mdl_setup_bones(&current_model->bone_state, bones, header->numbones);
                
                // Allocate transformed vertices
                current_model->transformed_vertices = malloc(current_model->vertex_count * 3 * sizeof(float));
                if (current_model->transformed_vertices) {
                    mdl_transform_vertices_by_bones(&current_model->bone_state, vertices, 
                                                   current_model->vertex_count, 
                                                   current_model->bone_weights, 
                                                   current_model->transformed_vertices);
                }
            }

            // Read triangle commands (improved system)
            current_model->render_data = mdl_read_triangle_commands(file, &models[m]);
            if (current_model->render_data) {
                current_model->triangle_count = current_model->render_data->index_count / 3;
                current_model->triangle_indices = current_model->render_data->indices_flat;
                current_model->texcoords = current_model->render_data->texcoords_flat;
                current_model->texcoord_count = current_model->render_data->texcoord_count / 2;
            } else {
                current_model->triangle_count = 0;
                current_model->triangle_indices = NULL;
                current_model->texcoords = NULL;
                current_model->texcoord_count = 0;
            }

            printf("   SUCCESS: Model '%s' - %d vertices, %d triangles\n",
                    current_model->model_name, current_model->vertex_count, current_model->triangle_count);

            free(vertices); // Free original vertices, we have copies
            model_index++;
            successful_models++;
        }
        free(models);
    }

    free(bodyparts);
    free(header);
    fclose(file);

    printf("\n=== LOADING COMPLETE ===\n");
    printf("Successfully processed %d/%d models\n", successful_models, total_models);
    printf("Loaded %d bones and %d textures\n", complete_model->bone_count, complete_model->texture_count);

    return complete_model;
}

void mdl_free_complete_model(mdl_complete_model_s *complete_model) {
    if (!complete_model) {
        return;
    }

    if (complete_model->models) {
        for (int i = 0; i < complete_model->total_model_count; i++) {
            single_model_s *model = &complete_model->models[i];
            
            if (model->vertices) {
                free(model->vertices);
            }
            if (model->transformed_vertices) {
                free(model->transformed_vertices);
            }
            if (model->bone_weights) {
                free(model->bone_weights);
            }
            if (model->render_data) {
                mdl_free_render_data(model->render_data);
            }
            // Note: texcoords and triangle_indices are owned by render_data
        }
        free(complete_model->models);
    }
    
    if (complete_model->bones) {
        free(complete_model->bones);
    }
    
    if (complete_model->textures) {
        mdl_free_textures(complete_model->textures, complete_model->texture_count);
    }
    
    if (complete_model->skin_families) {
        free(complete_model->skin_families);
    }

    free(complete_model);
}
