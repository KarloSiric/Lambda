/*
 * Test program for UV extraction from Half-Life MDL files
 * Compile with: gcc -o test_uv test_uv_extraction.c src/mdl/mdl_loader.c -Isrc
 */

#include <stdio.h>
#include <stdlib.h>
#include "studio.h"
#include "mdl/mdl_loader.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <model.mdl>\n", argv[0]);
        return 1;
    }
    
    // Load the model
    unsigned char *main_data = NULL;
    size_t main_size;
    
    mdl_result_t result = read_mdl_file(argv[1], &main_data, &main_size);
    if (result != MDL_SUCCESS) {
        printf("Failed to read file\n");
        return 1;
    }
    
    studiohdr_t *header = (studiohdr_t *)main_data;
    
    printf("Testing UV extraction for: %s\n", header->name);
    printf("Bodyparts: %d\n", header->numbodyparts);
    
    // Get first bodypart
    mstudiobodyparts_t *bodyparts = (mstudiobodyparts_t *)(main_data + header->bodypartindex);
    mstudiomodel_t *models = (mstudiomodel_t *)(main_data + bodyparts[0].modelindex);
    mstudiomodel_t *model = &models[0];
    
    printf("\nFirst model: %s\n", model->name);
    printf("Vertices: %d\n", model->numverts);
    printf("Meshes: %d\n", model->nummesh);
    
    if (model->nummesh > 0) {
        mstudiomesh_t *meshes = (mstudiomesh_t *)(main_data + model->meshindex);
        vec3_t *vertices = (vec3_t *)(main_data + model->vertindex);
        
        printf("\nTesting first mesh:\n");
        printf("  Triangles: %d\n", meshes[0].numtris);
        
        // Test the new UV extraction
        float *out_verts = NULL;
        float *out_uvs = NULL;
        int out_count = 0;
        
        result = extract_triangles_with_uvs(&meshes[0], main_data,
                                           vertices, model->numverts,
                                           &out_verts, &out_uvs, 
                                           &out_count);
        
        if (result == MDL_SUCCESS && out_verts && out_uvs) {
            printf("\n✅ UV EXTRACTION SUCCESSFUL!\n");
            printf("  Extracted %d vertices with UVs (%d triangles)\n", 
                   out_count, out_count / 3);
            
            // Print first few triangles with UV coordinates
            printf("\nFirst 3 triangles (9 vertices) with UVs:\n");
            for (int i = 0; i < 9 && i < out_count; i++) {
                printf("  V[%d]: pos(%.2f, %.2f, %.2f) uv(%.3f, %.3f)\n",
                       i, 
                       out_verts[i*3+0], out_verts[i*3+1], out_verts[i*3+2],
                       out_uvs[i*2+0], out_uvs[i*2+1]);
                
                if ((i+1) % 3 == 0) printf("  ---\n");
            }
            
            // Check UV range
            float min_u = 999, max_u = -999;
            float min_v = 999, max_v = -999;
            
            for (int i = 0; i < out_count; i++) {
                float u = out_uvs[i*2+0];
                float v = out_uvs[i*2+1];
                
                if (u < min_u) min_u = u;
                if (u > max_u) max_u = u;
                if (v < min_v) min_v = v;
                if (v > max_v) max_v = v;
            }
            
            printf("\n UV RANGE ANALYSIS:\n");
            printf("  U: [%.3f to %.3f]\n", min_u, max_u);
            printf("  V: [%.3f to %.3f]\n", min_v, max_v);
            
            if (min_u >= 0 && max_u <= 1 && min_v >= 0 && max_v <= 1) {
                printf("    UVs are properly normalized (0-1 range)\n");
            } else {
                printf("    UVs are outside 0-1 range - may need adjustment\n");
            }
            
            free(out_verts);
            free(out_uvs);
        } else {
            printf("\n UV EXTRACTION FAILED\n");
        }
    }
    
    free(main_data);
    
    printf("\n=== TEST COMPLETE ===\n");
    return 0;
}
