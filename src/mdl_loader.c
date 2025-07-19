/*
* @Author: karlosiric
* @Date:   2025-07-18 12:28:34
* @Last Modified by:   karlosiric
* @Last Modified time: 2025-07-20 00:26:26
*/

#include "mdl_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int read_data(FILE *file, void *buffer, size_t size, const char *description) {
    size_t bytes_read = fread(buffer, 1, size, file);
    if (bytes_read != size) {
        fprintf(stderr, "Failed to read %s: expected %zu bytes and got %zu bytes\n", 
            description, size, bytes_read);
        return (0);
    }

    return (1);
}

void print_mdl_info(const char *filepath) {
    FILE *file = fopen(filepath, "rb");
    if (!file) {
        fprintf(stderr, "Error opening file: '%s'\n", filepath);
        return;
    }

    printf("\nFILE opened successfully!\n\n");
    printf("==== MDL FILE INFORMATION ====\n");
    printf("FILE: '%s'\n", filepath);

    mdl_header_s mdl_header;

    printf("Printing position before reading: %ld\n", ftell(file));

    if (!read_data(file, &mdl_header.id, sizeof(mdl_header.id), "Magic ID")) {
        fclose(file);
        return;
    }

    printf("Printing position after ID: %ld\n", ftell(file));

    if (!read_data(file, &mdl_header.version, sizeof(mdl_header.version), "Version ID")) {
        fclose(file);
        return;
    }

    if (!read_data(file, mdl_header.name, sizeof(mdl_header.name), "Name")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.length, sizeof(mdl_header.length), "Data Length")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.eyeposition, sizeof(mdl_header.eyeposition), "Eye position")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.min, sizeof(mdl_header.min), "hull min")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.max, sizeof(mdl_header.max), "hull max")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.bbmin, sizeof(mdl_header.bbmin), "view bbmin")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.bbmax, sizeof(mdl_header.bbmax), "view bbmax")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.flags, sizeof(mdl_header.flags), "flags")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.numbones, sizeof(mdl_header.numbones), "bone count")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.boneindex, sizeof(mdl_header.boneindex), "bone offset")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.numbonecontrollers, sizeof(mdl_header.numbonecontrollers), "bone controller count")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.bonecontrollerindex, sizeof(mdl_header.bonecontrollerindex), "bone controller offset")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.numhitboxes, sizeof(mdl_header.numhitboxes), "hitbox count")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.hitboxindex, sizeof(mdl_header.hitboxindex), "hitbox offset")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.numseq, sizeof(mdl_header.numseq), "seq count")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.seqindex, sizeof(mdl_header.seqindex), "seq offset")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.numseqgroups, sizeof(mdl_header.numseqgroups), "seqgroup count")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.seqgroupindex, sizeof(mdl_header.seqgroupindex), "seqgroup offset")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.numtextures, sizeof(mdl_header.numtextures), "texture count")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.textureindex, sizeof(mdl_header.textureindex), "texture offset")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.texturedataindex, sizeof(mdl_header.texturedataindex), "texturedata offset")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.numskinref, sizeof(mdl_header.numskinref), "skin ref count")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.numskinfamilies, sizeof(mdl_header.numskinfamilies), "skin families count")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.skinindex, sizeof(mdl_header.skinindex), "skin offset")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.numbodyparts, sizeof(mdl_header.numbodyparts), "bodypart count")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.bodypartindex, sizeof(mdl_header.bodypartindex), "bodypart offset")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.numattachments, sizeof(mdl_header.numattachments), "attachment count")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.attachmentindex, sizeof(mdl_header.attachmentindex), "attachment offset")) {
        fclose(file);
        return;
    }

    

    
    printf("Current position after complete header: %ld\n\n", ftell(file));

    printf("==== COMPLETE MDL HEADER INFORMATION ====\n");
    printf("MAGIC ID: %.4s\n", (char *)&mdl_header.id);
    printf("VERSION: %d\n", mdl_header.version);
    printf("Name: %s\n", mdl_header.name);
    printf("Data Length: %d bytes\n", mdl_header.length);
    printf("Eye Position: (%.2f, %.2f, %.2f)\n", 
           mdl_header.eyeposition.x, mdl_header.eyeposition.y, mdl_header.eyeposition.z);
    printf("Hull Min: (%.2f, %.2f, %.2f)\n", 
           mdl_header.min.x, mdl_header.min.y, mdl_header.min.z);
    printf("Hull Max: (%.2f, %.2f, %.2f)\n", 
           mdl_header.max.x, mdl_header.max.y, mdl_header.max.z);
    printf("BB Min: (%.2f, %.2f, %.2f)\n", 
           mdl_header.bbmin.x, mdl_header.bbmin.y, mdl_header.bbmin.z);
    printf("BB Max: (%.2f, %.2f, %.2f)\n", 
           mdl_header.bbmax.x, mdl_header.bbmax.y, mdl_header.bbmax.z);
    printf("Flags: 0x%08X\n", mdl_header.flags);
    printf("Bone Count: %d\n", mdl_header.numbones);
    printf("Bone Offset: %d\n", mdl_header.boneindex);
    
    printf("\n==== KEY VERTEX DATA LOCATIONS ====\n");
    printf("Bodypart Count: %d\n", mdl_header.numbodyparts);
    printf("Bodypart Offset: %d\n", mdl_header.bodypartindex);
    printf("Texture Count: %d\n", mdl_header.numtextures);
    printf("Sequence Count: %d\n", mdl_header.numseq);
    
    printf("\n==== NEXT STEP: FIND VERTICES! ====\n");
    printf("Now jumping to bodypart offset %d to find vertex data...\n", mdl_header.bodypartindex);

    fseek(file, mdl_header.bodypartindex, SEEK_SET);
    mdl_bodypart_s bodyparts[mdl_header.numbodyparts];
    for (int i = 0; i < mdl_header.numbodyparts; i++) {
        
        long next_bodypart_pos = mdl_header.bodypartindex + ((i + 1) * sizeof(mdl_bodypart_s));

        if (!read_data(file, &bodyparts[i], sizeof(mdl_bodypart_s), "bodypart")) {
            fprintf(stderr, "Cannot read bodypart[%d] at position %ld\n", i, ftell(file));
            return;
        }

        printf("\n==== EXPLORING ALL MODELS ====\n");
        printf("\n---- Bodypart[%d]: '%s' ----\n", i, bodyparts[i].name);
        printf("Bodypart[%d] -> name: '%s' \n", i, bodyparts[i].name);
        printf("Bodypart[%d] -> nummodels: %d \n", i, bodyparts[i].nummodels);
        printf("Bodypart[%d] -> base: %d \n", i, bodyparts[i].base);
        printf("Bodypart[%d] -> modelindex: %d \n", i, bodyparts[i].modelindex);

        for (int j = 0; j < bodyparts[i].nummodels; j++) {
            printf("\n---- MODEL[%d][%d] ----\n", i, j);

            int model_offset = bodyparts[i].modelindex + (j * sizeof(mdl_model_s));
            fseek(file, model_offset, SEEK_SET);
            mdl_model_s model;

            if (!read_data(file, &model, sizeof(mdl_model_s), "model")) {
                continue;
            }

            printf("  MODEL[%d][%d] name: '%s'  \n", i, j, model.name);
            printf("  MODEL[%d][%d] vertindex: %d  \n", i, j, model.vertindex);
            printf("  MODEL[%d][%d] vertinfoindex: %d  \n", i, j, model.vertinfoindex);

            //TODO -> here we need to extract the vertices data that is necessary for our OpenGL
            if (model.numverts > 0) {
                fseek(file, model.vertindex, SEEK_SET);

                mdl_vertex_s vertices[model.numverts];
                fread(vertices, sizeof(mdl_vertex_s), model.numverts, file);

                printf("  Printing %d vertices for the MODEL[%d][%d]: '%s'  \n",
                        model.numverts, i, j, model.name);

                for (int k = 0; k < model.numverts; k++) {
                    printf("   Vertex[%d]: (x=%.2f, y=%.2f, z=%.2f2)\n", k,
                            vertices[k].x, vertices[k].y, vertices[k].z);
                }
                printf("\n");
            }
        }

        if (i < mdl_header.numbodyparts - 1) {
            fseek(file, next_bodypart_pos, SEEK_SET);
        }
    }

    fclose(file);

}

mdl_complete_model_s load_mdl_file(const char *filepath) {
    mdl_complete_model_s result = {0};
    FILE *file = fopen(filepath, "rb");
    if (!file) {
        fprintf(stderr, "ERROR - Failed to open filepath: '%s' \n", filepath);
        return result;
    }

    // TODO: Reading the whole header file since it is packed tightly
    mdl_header_s mdl_header;
    if (!read_data(file, &mdl_header, sizeof(mdl_header_s), "complete header")) {
        fclose(file);
        return result;
    }

    fseek(file, mdl_header.bodypartindex, SEEK_SET);
    mdl_bodypart_s bodyparts[mdl_header.numbodyparts];
    for (int i = 0; i < mdl_header.numbodyparts; i++) {
        if (!read_data(file, &bodyparts[i], sizeof(mdl_bodypart_s), "bodypart")) {
            fclose(file);
            return result;
        }
    }

    int total_models = 0;
    for (int i = 0; i < mdl_header.numbodyparts; i++) {
        total_models += bodyparts[i].nummodels;
    }

    printf("Total models to load: %d\n", total_models);

    result.models = malloc(total_models * sizeof(single_model_s));
    if (!result.models) {
        fprintf(stderr, "Error allocating memory for the models!\n");
        fclose(file);
        return result;
    }

    result.total_model_count = total_models;
    result.bodypart_count = mdl_header.numbodyparts;
    strncpy(result.filename, filepath, 63);
    result.filename[63] = '\0';

    // TODO: We need to fill the data (so we take on big loop)
    int current_index = 0;
    for (int i = 0; i < mdl_header.numbodyparts; i++) {
        for (int j = 0; j < bodyparts[i].nummodels; j++) {
            // TODO: Fill in the data that is needed here:
            long model_offset = bodyparts[i].modelindex + (j * sizeof(mdl_model_s));
            fseek(file, model_offset, SEEK_SET);
            mdl_model_s model;
            if (!read_data(file, &model, sizeof(mdl_model_s), "model")) {
                continue;
            }

            printf("  Model[%d][%d] has %d meshes\n", i, j, model.nummesh);
            printf("  Mesh data starts at file position: %d\n", model.meshindex);

            if (model.nummesh > 0) {
                printf("  Reading meshes with correct structure.\n");

                fseek(file, model.meshindex, SEEK_SET);

                for (int m = 0; m < model.nummesh; m++) {
                    mdl_mesh_s mesh;
                    if (read_data(file, &mesh, sizeof(mdl_mesh_s), "mesh")) {
                        printf("  Mesh[%d]: %d triangles at offset %d.\n",
                                m, mesh.numtris, mesh.triindex);

                        if (mesh.numtris > 0) {
                            long current_tris_pos = ftell(file);
                            fseek(file, mesh.triindex, SEEK_SET);

                            printf("      First triangles:\n");
                            for (int t = 0; t < 3 && t < mesh.numtris; t++) {
                                mdl_triangle_s triangle;
                                if (read_data(file, &triangle, sizeof(mdl_triangle_s), "triangle")) {
                                    printf("        Triangle[%d]: vertices %d, %d, %d\n", 
                                           t, triangle.vertindex[0], triangle.vertindex[1], triangle.vertindex[2]);
                                }
                            }

                            fseek(file, current_tris_pos, SEEK_SET);
                        }
                    }
                }

                printf("\n");
            }

            result.models[current_index].vertex_count = model.numverts;
            strncpy(result.models[current_index].model_name, model.name, 63);
            strncpy(result.models[current_index].bodypart_name, bodyparts[i].name, 63);
            result.models[current_index].model_id = j;
            result.models[current_index].bodypart_id = i;

            if (model.numverts > 0) {
                result.models[current_index].vertices = malloc(model.numverts * 3 * sizeof(float));

                fseek(file, model.vertindex, SEEK_SET);
                mdl_vertex_s vertices[model.numverts];
                fread(vertices, sizeof(mdl_vertex_s), model.numverts, file);

                for (int k = 0; k < model.numverts; k++) {
                    result.models[current_index].vertices[k * 3 + 0] = vertices[k].x;
                    result.models[current_index].vertices[k * 3 + 1] = vertices[k].y;
                    result.models[current_index].vertices[k * 3 + 2] = vertices[k].z;
                }
            } else {
                result.models[current_index].vertices = NULL;
            }

            printf("Loaded successfully: %s (%d vertices) \n",
                    result.models[current_index].model_name,
                    result.models[current_index].vertex_count);

            current_index++;
        }
    }

    return result;
}

void free_mdl_file(mdl_complete_model_s *model) {
    if (model->models) {
        for (int i = 0; i < model->total_model_count; i++) {
            if (model->models[i].vertices) {
                free(model->models[i].vertices);
            }
        }
        free(model->models);
        model->models = NULL;
        model->total_model_count = 0;
    }   
}

void testing_mdl_file(const char *filepath) {
    printf("==== TESTING MDL FILE ====\n");

    mdl_complete_model_s scientist = load_mdl_file(filepath);

    if (scientist.total_model_count > 0) {
        printf("SUCCESS! Loaded %d models from '%s' \n",
                scientist.total_model_count, scientist.filename);

        for (int i = 0; i < scientist.total_model_count; i++) {
            printf("MODEL[%d]: '%s' (%d vertices) - Bodypart: %s [%d][%d]\n",
                    i, 
                    scientist.models[i].model_name, 
                    scientist.models[i].vertex_count,
                    scientist.models[i].bodypart_name,
                    scientist.models[i].bodypart_id,
                    scientist.models[i].model_id);
        }

        if (scientist.models[0].vertex_count > 0) {
            printf("First 3 vertices of first model:\n");
            for (int i = 0; i < 3; i++) {
                printf("   Vertex[%d]: (%.2f, %.2f, %.2f)\n",
                        i, 
                        scientist.models[0].vertices[i * 3 + 0],
                        scientist.models[0].vertices[i * 3 + 1], 
                        scientist.models[0].vertices[i * 3 + 2]);
            }
        }

        free_mdl_file(&scientist);
        printf("Memory freed successfully!\n");
    } else {
        printf("Failed to load MDL file!\n");
    }

    printf("Test Completed!\n");
}













