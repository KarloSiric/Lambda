/*
* @Author: karlosiric
* @Date:   2025-07-18 12:28:34
* @Last Modified by:   karlosiric
* @Last Modified time: 2025-07-31 16:08:59
*/

#include "mdl_loader.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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

triangle_data_t *mdl_read_triangles_for_models(FILE *file, mstudiomodel_t *model) {
    printf("  DEBUG: Triangle function called for model '%s'\n", model->name);

    if (model->nummesh == 0) {
        printf("  Model '%s' has no meshes\n", model->name);
        return NULL;
    }

    long file_size = mdl_get_file_size(file);


    fseek(file, model->meshindex, SEEK_SET);    
    mstudiomesh_t *meshes = (mstudiomesh_t *)malloc(model->nummesh * sizeof(mstudiomesh_t));
    if (!meshes) {
        fprintf(stderr, "ERROR - Failed to allocate enough space for meshes.\n");
        return NULL;
    }

    size_t meshes_read = fread(meshes, sizeof(mstudiomesh_t), model->nummesh, file);
    if (meshes_read != (size_t)model->nummesh) {
        fprintf(stderr, "ERROR - Failed to read meshes.\n");
        free(meshes);
        return NULL;
    }

    int total_triangles = 0;
    for (int i = 0; i < model->nummesh; i++) {
        long triangle_data_start = meshes[i].triindex;
        long triangle_data_size = meshes[i].numtris * sizeof(mstudiotrivert_t) * 3;

        if (triangle_data_start + triangle_data_size > file_size) {
            printf("  WARNING: Mesh[%d] triangle data extends beyond file (starts at %ld, needs %ld bytes, file is %ld bytes long.\n",
                    i, triangle_data_start, triangle_data_size, file_size);
            printf("  Adjusting triangle count for this mesh\n");

            long available_bytes = file_size - triangle_data_start;
            int max_triangles = available_bytes / (sizeof(mstudiotrivert_t) * 3);
            if (max_triangles < 0) {
                max_triangles = 0;
            }

            printf("  Mesh claimed %d triangles but can only read %d\n", meshes[i].numtris, max_triangles);
            meshes[i].numtris = max_triangles;
        }

        total_triangles += meshes[i].numtris;
    }

    if (total_triangles == 0) {
        printf("  Model '%s' has no triangles\n", model->name);
        free(meshes);
        return NULL;
    }

    triangle_data_t *triangles = malloc(total_triangles * sizeof(triangle_data_t));
    if (!triangles) {
        fprintf(stderr, "ERROR - Failed to allocate enough space for triangles.\n");
        free(meshes);
        return NULL;
    }

    int triangles_index = 0;
    int successful_triangles = 0;

    for (int i = 0; i < model->nummesh; i++) {
        
        if (meshes[i].numtris > 0) {
            fseek(file, meshes[i].triindex, SEEK_SET);

            for (int j = 0; j < meshes[i].numtris; j++) {

                if (ftell(file) + sizeof(mstudiotrivert_t) * 3 > file_size) {
                    printf("  WARNING: Would read past end of file at triangle %d, stopping.\n",
                            triangles_index);
                    continue;
                }


                size_t tri_vertices_read = fread(&triangles[triangles_index].triverts, sizeof(mstudiotrivert_t), 3, file);
                if (tri_vertices_read != 3) {
                    if (feof(file)) {
                        printf("  WARNING: Reached the end of file at triangle %d (read %zu/3 vertices)\n",
                                triangles_index, tri_vertices_read);
                    } else {
                        printf("  WARNING: Failed to read complete triangle %d (read %zu/3 vertices)\n",
                                triangles_index, tri_vertices_read);
                    }
                    fprintf(stderr, "ERROR - Failed to read triangle %d\n", triangles_index);

                    free(triangles);
                    free(meshes);
                    break;
                }

                bool valid_triangle = true;
                for (int v = 0; v < 3; v++) {
                    if (triangles[triangles_index].triverts[v].vertindex >= model->numverts) {
                        printf("  WARNING: Triangle %d has invalid vertex index %d (max: %d)\n",
                                triangles_index, triangles[triangles_index].triverts[v].vertindex, model->numverts);
                        valid_triangle = false;
                        break;
                    }
                }

                if (valid_triangle) {
                    successful_triangles++;
                    triangles_index++;
                } else {
                    printf("  Skipping invalid triangle %d\n",
                            triangles_index);
                }
            }
        }
    } 

    free(meshes);

    if (successful_triangles == 0) {
        printf("  Model '%s' has no valid triangles after corruption filtering.\n",
                model->name);
        free(triangles);
        return NULL;
    }

    printf("  SUCCESS: Read %d triangles for model '%s' \n", total_triangles, model->name);

    return triangles;
}


long mdl_get_file_size(FILE *file) {
    long current_pos = ftell(file);
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, current_pos, SEEK_SET);
    return size;
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

    mstudiobodypart_t *bodyparts = mdl_read_bodyparts(file, header);
    if (!bodyparts) {
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

    complete_model->models = malloc(total_models * sizeof(single_model_s));
    if (!complete_model->models) {
        fprintf(stderr, "ERROR - Failed to allocate models array\n");
        free(complete_model);
        free(bodyparts);
        free(header);
        fclose(file);
        return NULL;
    }

    complete_model->total_model_count = total_models;
    complete_model->bodypart_count = header->numbodyparts;
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
            printf("  Processing model[%d]: '%s' \n",
                    m, models[m].name);

            single_model_s *current_model = &complete_model->models[model_index];
            strncpy(current_model->model_name, models[m].name, sizeof(current_model->model_name) - 1);
            current_model->model_name[sizeof(current_model->model_name) - 1] = '\0';

            strncpy(current_model->bodypart_name, bodyparts[bp].name, sizeof(current_model->bodypart_name) - 1);
            current_model->bodypart_name[sizeof(current_model->bodypart_name) - 1] = '\0';

            current_model->bodypart_id = bp;
            current_model->model_id = m;

            // TODO(Karlo): Reading vertices now that we need to read the data

            vec3_t *vertices = mdl_read_vertices(file, &models[m]);
            if (!vertices) {
                printf("   WARNING: Failed to read vertices for model: '%s' \n",
                        models[m].name);
                current_model->vertices = NULL;
                current_model->vertex_count = 0;
                current_model->triangle_indices = NULL;
                current_model->triangle_count = 0;

                model_index++;
                continue;
            }

            current_model->vertex_count = models[m].numverts;
            current_model->vertices = malloc(current_model->vertex_count * 3 * sizeof(float));
            if (!current_model->vertices) {
                fprintf(stderr, "   ERROR - Failed to allocate vertex array for model '%s' \n",
                        models[m].name);
                free(vertices);
                current_model->vertex_count = 0;
                current_model->triangle_indices = NULL;
                current_model->triangle_count = 0;
                model_index++;
                continue;
            }

            for (int v = 0; v < current_model->vertex_count; v++) {
                current_model->vertices[v * 3 + 0] = vertices[v].x;
                current_model->vertices[v * 3 + 1] = vertices[v].y;
                current_model->vertices[v * 3 + 2] = vertices[v].z;
            }
            free(vertices);

            triangle_data_t *triangles = mdl_read_triangles_for_models(file, &models[m]);
            if (!triangles) {
                printf("   WARNING: No triangles for model '%s' \n",
                        models[m].name);
                current_model->triangle_indices = NULL;
                current_model->triangle_count = 0;
                model_index++;
                successful_models++;
                continue;
            }

            // Get raw triangle count from mesh data
            int raw_triangle_count = 0;
            fseek(file, models[m].meshindex, SEEK_SET);
            mstudiomesh_t *meshes = malloc(models[m].nummesh * sizeof(mstudiomesh_t));
            if (meshes) {
                fread(meshes, sizeof(mstudiomesh_t), models[m].nummesh, file);
                
                for (int mesh = 0; mesh < models[m].nummesh; mesh++) {
                    raw_triangle_count += meshes[mesh].numtris;
                }
                free(meshes);
            }

            // Count and filter VALID triangles only
            int valid_triangle_count = 0;
            for (int t = 0; t < raw_triangle_count; t++) {
                bool valid = true;
                for (int v = 0; v < 3; v++) {
                    if (triangles[t].triverts[v].vertindex >= current_model->vertex_count) {
                        valid = false;
                        break;
                    }
                }
                if (valid) {
                    valid_triangle_count++;
                }
            }

            current_model->triangle_count = valid_triangle_count;

            if (valid_triangle_count > 0) {
                current_model->triangle_indices = malloc(valid_triangle_count * 3 * sizeof(int));
                if (!current_model->triangle_indices) {
                    fprintf(stderr, "   ERROR: Failed to allocate triangle indices for model '%s' \n",
                            models[m].name);
                    free(triangles);
                    current_model->triangle_count = 0;
                    model_index++;
                    successful_models++;
                    continue;
                }

                // Copy only VALID triangles
                int valid_index = 0;
                for (int t = 0; t < raw_triangle_count && valid_index < valid_triangle_count; t++) {
                    bool valid = true;
                    for (int v = 0; v < 3; v++) {
                        if (triangles[t].triverts[v].vertindex >= current_model->vertex_count) {
                            valid = false;
                            break;
                        }
                    }
                    
                    if (valid) {
                        current_model->triangle_indices[valid_index * 3 + 0] = triangles[t].triverts[0].vertindex;
                        current_model->triangle_indices[valid_index * 3 + 1] = triangles[t].triverts[1].vertindex;
                        current_model->triangle_indices[valid_index * 3 + 2] = triangles[t].triverts[2].vertindex;
                        valid_index++;
                    }
                }
                
                printf("   FILTERED: %d/%d triangles valid for model '%s'\n", 
                       valid_triangle_count, raw_triangle_count, current_model->model_name);
            } else {
                current_model->triangle_indices = NULL;
                current_model->triangle_count = 0;
            }

            free(triangles);

            printf("   SUCCESS: Model '%s' - %d vertices, %d triangles\n",
                    current_model->model_name, current_model->vertex_count, current_model->triangle_count);

            model_index++;
            successful_models++;
        }
        free(models);
    }

    free(bodyparts);
    free(header);
    fclose(file);

    printf("\n=== LOADING COMPLETE ===\n");
    printf("Successfully processed %d/%d models\n",
            successful_models, total_models);

    return complete_model;
}

void mdl_free_complete_model(mdl_complete_model_s *complete_model) {
    if (!complete_model) {
        return;
    }

    if (complete_model->models) {
        for (int i = 0; i < complete_model->total_model_count; i++) {
            if (complete_model->models[i].vertices) {
                free(complete_model->models[i].vertices);
            }
            if (complete_model->models[i].triangle_indices) {
                free(complete_model->models[i].triangle_indices);
            }
        }
        free(complete_model->models);
    }

    free(complete_model);
}
