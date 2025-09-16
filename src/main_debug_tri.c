/*
 * Debug version to examine triangle command data
 */

#include "main.h"
#include "mdl/mdl_info.h"
#include "mdl/mdl_loader.h"
#include "studio.h"
#include "graphics/renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char const *argv[])
{    
    studiohdr_t *main_header = NULL;
    studiohdr_t *texture_header = NULL;
    unsigned char *main_data = NULL;
    unsigned char *texture_data = NULL;

    if (argc != 2) {
        printf("Usage: %s <model.mdl>\n", argv[0]);
        return (1);
    }

    mdl_result_t result = load_model_with_textures(argv[1], &main_header, &texture_header, &main_data, &texture_data);

    if (result != MDL_SUCCESS) {
        printf("Failed to load model! Error code: %d\n", result);
        return (1);
    }

    printf("\n=== TRIANGLE COMMAND DEBUG ===\n");
    
    // Look at first bodypart only
    mstudiobodypart_t *bodyparts = (mstudiobodypart_t *)(main_data + main_header->bodypartindex);
    mstudiobodypart_t *bodypart = &bodyparts[0];
    
    if (bodypart->nummodels > 0) {
        mstudiomodel_t *models = (mstudiomodel_t *)(main_data + bodypart->modelindex);
        mstudiomodel_t *model = &models[0];
        
        printf("Model: %s\n", model->name);
        printf("Vertices: %d\n", model->numverts);
        printf("Meshes: %d\n", model->nummesh);
        
        mstudiomesh_t *meshes = (mstudiomesh_t *)(main_data + model->meshindex);
        
        // Just examine first mesh
        if (model->nummesh > 0) {
            mstudiomesh_t *mesh = &meshes[0];
            printf("\nMesh 0:\n");
            printf("  Expected triangles: %d\n", mesh->numtris);
            printf("  triindex offset: 0x%X (%d bytes)\n", mesh->triindex, mesh->triindex);
            
            // Try reading triangle commands as offset from FILE start
            printf("\n  Trying triindex as FILE offset:\n");
            unsigned char *tri_data = main_data + mesh->triindex;
            int *cmd = (int *)tri_data;
            
            printf("  First int value: %d\n", cmd[0]);
            if (abs(cmd[0]) < 100) {  // Reasonable vertex count
                printf("  Looks like a vertex count!\n");
                
                // Read first few commands
                int total_tris = 0;
                for (int i = 0; i < 5 && cmd[0] != 0; i++) {
                    int count = *cmd++;
                    if (count == 0) break;
                    
                    int abs_count = abs(count);
                    printf("  Command %d: %s with %d vertices\n", 
                           i, count < 0 ? "FAN" : "STRIP", abs_count);
                    
                    // Show first few vertex indices
                    short *verts = (short *)cmd;
                    printf("    First vertices: ");
                    for (int v = 0; v < 3 && v < abs_count; v++) {
                        printf("[idx=%d, norm=%d, s=%d, t=%d] ", 
                               verts[v*4], verts[v*4+1], verts[v*4+2], verts[v*4+3]);
                    }
                    printf("\n");
                    
                    total_tris += (abs_count - 2);
                    cmd = (int *)((char *)cmd + abs_count * 8);  // Skip vertex data
                }
                printf("  Total triangles from commands: %d (expected %d)\n", total_tris, mesh->numtris);
            }
            
            // Try as offset from MESH
            printf("\n  Trying triindex as MESH offset:\n");
            tri_data = (unsigned char *)mesh + mesh->triindex;
            cmd = (int *)tri_data;
            
            printf("  First int value: %d\n", cmd[0]);
            if (abs(cmd[0]) < 100) {
                printf("  Looks like a vertex count!\n");
            }
        }
    }
    
    free(main_data);
    if (texture_data) free(texture_data);
    
    return (0);
}
