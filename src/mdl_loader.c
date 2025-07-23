/*
* @Author: karlosiric
* @Date:   2025-07-18 12:28:34
* @Last Modified by:   karlosiric
* @Last Modified time: 2025-07-23 14:12:11
*/

#include "mdl_loader.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    // printf("\n=== PRINTING MODELS ===\n");
    // for (int i = 0; i < bodypart->nummodels; i++) {
    //     printf("MODEL[%d] BELONGS TO BODYPART: '%s' \n",
    //             i, bodypart->name);
    //     printf("  MODEL[%d] -> Name: '%s' \n",
    //             i, models[i].name);
    //     printf("  MODEL[%d] -> Type: %d \n",
    //             i, models[i].type);
    //     printf("  MODEL[%d] -> Nummesh: %d \n",
    //             i, models[i].nummesh);
    //     printf("  MODEL[%d] -> Meshindex: %d \n",
    //             i, models[i].meshindex);
    //     printf("  MODEL[%d] -> Numverts: %d \n",
    //             i, models[i].numverts);
    // }


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
    if (model->nummesh == 0) {
        printf("  Model '%s' has no meshes\n", model->nummesh);
        return NULL;
    }

    fseek(file, model->meshindex, SEEK_SET);    
    mstudiomesh_t *meshes = (mstudiomesh_t *)malloc(model->nummesh * sizeof(mstudiomesh_t));
    if (!meshes) {
        fprintf(stderr, "ERROR - Failed to allocate enough space for meshes.\n");
        return NULL;
    }

    size_t meshes_read = fread(meshes, sizeof(mstudiomesh_t), model->nummesh, file);
    if (meshes_read != model->nummesh) {
        fprintf(stderr, "ERROR - Failed to read meshes.\n");
        free(meshes);
        return NULL;
    }

    int total_triangles = 0;
    for (int i = 0; i < model->nummesh; i++) {
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
    for (int i = 0; i < model->nummesh; i++) {
        
        if (meshes[i].numtris > 0) {
            fseek(file, meshes[i].triindex, SEEK_SET);
            for (int j = 0; j < meshes[i].numtris; j++) {
                size_t tri_vertices_read = fread(&triangles[triangles_index].triverts, sizeof(mstudiotrivert_t), 3, file);
                if (tri_vertices_read != 3) {
                    fprintf(stderr, "ERROR - Failed to read triangle %d\n", triangles_index);
                    free(triangles);
                    free(meshes);
                    return NULL;
                }
                triangles_index++;
            }
        }
    } 

    free(meshes);
    printf("  SUCCESS: Read %d triangles for model '%s' \n", total_triangles, model->name);
    return triangles;
}
