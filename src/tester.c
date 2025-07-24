/*
* @Author: karlosiric
* @Date:   2025-07-22 11:49:29
* @Last Modified by:   karlosiric
* @Last Modified time: 2025-07-23 14:51:42
*/

#include "mdl_loader.h"
#include <stdlib.h>
#include <stdbool.h>

int main(void) {

    FILE *file = fopen("models/HL1_Original/scientist.mdl", "rb");
    if (!file) {
        fprintf(stderr, "ERROR - Failed to open the file.\n");
        return (-1);
    }

    studiohdr_t *header = mdl_read_header(file);
    if (header) {
        printf("\n=== MDL HEADER INFO ===\n");
        printf("Name: '%s'\n", header->name);
        printf("Version: %d\n", header->version);
        printf("Bodyparts: %d\n", header->numbodyparts);
        printf("Textures: %d\n", header->numtextures);
    }

    mstudiobodypart_t *bodyparts = mdl_read_bodyparts(file, header);
    if (bodyparts) {
        printf("\n=== BODYPARTS INFO ===\n");
        for (int i = 0; i < header->numbodyparts; i++) {
            printf("  Bodypart[%d]: '%s' (%d models)\n",
                i, bodyparts[i].name, bodyparts[i].nummodels);
        }
    } else {
        printf("FAILED to read bodyparts\n");
    }

    for (int i = 0; i < header->numbodyparts; i++) {
        printf("  \nReading models for bodypart '%s' \n", bodyparts[i].name);

        mstudiomodel_t *models = mdl_read_models_for_bodyparts(file, &bodyparts[i]);
        if (models) {
            for (int m = 0; m < bodyparts[i].nummodels; m++) {
                printf("  Model[%d]: '%s' (%d vertices, %d meshes)\n", 
                   m, models[m].name, models[m].numverts, models[m].nummesh);


                vec3_t *vertices = mdl_read_vertices(file, &models[m]);
                if (vertices) {
                    printf("  First vertex: (%.2f, %.2f, %.2f)\n",
                            vertices[0].x, vertices[0].y, vertices[0].z);
                    printf("  Last vertex: (%.2f, %.2f, %.2f)\n",
                            vertices[models[m].numverts - 1].x,
                            vertices[models[m].numverts - 1].y,
                            vertices[models[m].numverts - 1].z);
                    free(vertices);
                }

                triangle_data_t *triangles = mdl_read_triangles_for_models(file, &models[m]);
                if (triangles) {
                    int total_triangles = 0;

                    fseek(file, models[m].meshindex, SEEK_SET);
                    mstudiomesh_t *meshes = malloc(models[m].nummesh * sizeof(mstudiomesh_t));
                    if (meshes) {
                        fread(meshes, sizeof(mstudiomesh_t), models[i].nummesh, file);

                        for (int mesh = 0; mesh < models[m].nummesh; mesh++) {
                            total_triangles += meshes[mesh].numtris;
                        }
                        free(meshes);
                    }

                    printf("   TRIANGLE DATA (showing first 10 triangles): \n");
                    int triangles_to_show = (total_triangles > 10) ? 10 : total_triangles;
                    for (int t = 0; t < triangles_to_show; t++) {
                        printf("   Triangle[%d]: indices [%d %d %d]\n",
                                t, 
                                triangles[t].triverts[0].vertindex,
                                triangles[t].triverts[1].vertindex,
                                triangles[t].triverts[2].vertindex);

                        bool valid = true;
                        for (int tri = 0; tri < 3; tri++) {
                            if (triangles[t].triverts[tri].vertindex >= models[m].numverts) {
                                valid = false;
                                break;
                            }
                        }

                        if (valid) {
                            printf("VALID!\n");
                        } else {
                            printf("INVALID!\n");
                        }
                    }

                    printf("    SUMMARY: %d total triangles, max vertex index should be < %d\n",
                            total_triangles, models[m].numverts);

                    free(triangles);
                } else {
                    printf("    No Triangles read for this model.\n");
                }


            } 
            free(models);
        }
    }

    free(header);
    free(bodyparts);

    fclose(file);
    return (0);

}



