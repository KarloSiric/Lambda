/*
* @Author: karlosiric
* @Date:   2025-08-06 07:37:32
* @Last Modified by:   karlosiric
* @Last Modified time: 2025-09-15 19:51:50
*/

#include "mdl_info.h"
#include "mdl_loader.h"
#include <stdio.h>

void print_complete_model_analysis(const char *filename, studiohdr_t *main_header, studiohdr_t *texture_header, unsigned char *main_data, unsigned char *texture_data) {

    printf("Testing complete model + texure loading %s\n", filename);
    printf("SUCCESS: Model loaded completely!\n\n");

    printf("=== MAIN MODEL INFO ===\n");
    printf("  Name: %s\n", main_header->name);
    printf("  File Size: %d bytes\n", main_header->length);
    printf("  Bones: %d\n", main_header->numbones);
    printf("  Bodyparts: %d\n", main_header->numbodyparts);
    printf("  Sequences: %d\n", main_header->numseq);

    print_texture_info(texture_header, texture_data);

    print_bodypart_info(main_header, main_data);

    mstudiobone_t *bones = NULL;
    mdl_result_t bone_result = parse_bone_hierarchy(main_header, main_data, &bones);

    if (bone_result == MDL_SUCCESS && bones) {
        print_bone_info(bones, main_header->numbones);
    } else {
        printf("\nFailed to parse bone hierarchy (ERROR: %d)\n",
                bone_result);
    }

    mstudioseqdesc_t *sequences = NULL;
    mdl_result_t sequence_result =  parse_animation_sequences(main_header, main_data, &sequences);
    if (sequence_result == MDL_SUCCESS && sequences) {
        print_sequence_info(sequences, main_header->numseq);
    } else {
        fprintf(stderr, "Failed to parse animations!\n");
    }

    printf("\n=== Detailed Model Analysis ===\n");
    mstudiobodyparts_t *bodyparts = (mstudiobodyparts_t *)(main_data + main_header->bodypartindex);
    for (int bodypart_index = 0; bodypart_index < main_header->numbodyparts; bodypart_index++) {
        printf("bodypart_index: %d: %s (%d models)\n", bodypart_index, bodyparts[bodypart_index].name, bodyparts[bodypart_index].nummodels);

        mstudiomodel_t *models = (mstudiomodel_t *)(main_data + bodyparts[bodypart_index].modelindex);

        for (int model_index = 0; model_index < bodyparts[bodypart_index].nummodels; model_index++) {
            print_model_info(&models[model_index], bodypart_index, model_index);

            mstudiomesh_t *meshes = NULL;
            mdl_result_t mesh_result = parse_mesh_data(&models[model_index], main_data, &meshes);
            if (mesh_result == MDL_SUCCESS && meshes) {
                print_mesh_data(meshes, &models[model_index], models[model_index].nummesh);
                print_simple_triangle_info(&models[model_index], bodypart_index, model_index);
            } else {
                printf("   Failed to parse meshes for model: %s\n", models[model_index].name);
            }

            vec3_t *vertices = NULL;
            if (parse_vertex_data(&models[model_index], main_data, &vertices) == MDL_SUCCESS) {
                if (vertices && models[model_index].numverts > 0) {
                    printf("  Vertices:\n");
                    printf("    First: (%.2f, %.2f, %.2f)\n",
                            vertices[0][0], vertices[0][1], vertices[0][2]);
                }
                if (models[model_index].numverts > 1) {
                    printf("    Last: (%.2f, %.2f, %.2f)\n",
                            vertices[models[model_index].numverts - 1][0],
                            vertices[models[model_index].numverts - 1][1],
                            vertices[models[model_index].numverts - 1][2]);
                }
            }
        }
    }

    printf("Complete model analysis completed!\n");
}

