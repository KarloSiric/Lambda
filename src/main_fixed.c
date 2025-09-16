/*
 * FIXED VERSION - Properly handles bodypart separation
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

    print_complete_model_analysis(argv[1], main_header, texture_header, main_data, texture_data);

    printf("Initializing the renderer...\n");
    if (init_renderer(WIDTH, HEIGHT, "Half-Life Model Viewer - FIXED") != 0) {
        printf("Failed to initialize renderer!\n");
        free(main_data);
        if (texture_data) free(texture_data);
        return (1);
    }
    
    printf("\n=== FIXED TRIANGLE EXTRACTION ===\n");
    
    // For now, let's just render the FIRST bodypart (the body)
    // This avoids the mixing issue
    
    mstudiobodypart_t *bodyparts = (mstudiobodypart_t *)(main_data + main_header->bodypartindex);
    mstudiobodypart_t *bodypart = &bodyparts[0];  // Just the body
    
    if (bodypart->nummodels > 0) {
        mstudiomodel_t *models = (mstudiomodel_t *)(main_data + bodypart->modelindex);
        mstudiomodel_t *model = &models[0];
        
        printf("Loading ONLY bodypart 0 (%s): %d vertices, %d meshes\n", 
               bodypart->name, model->numverts, model->nummesh);
        
        if (model->numverts > 0) {
            // Load vertices
            vec3_t *vertices = (vec3_t *)(main_data + model->vertindex);
            
            // Find bounds for auto-scaling
            float min_x = 999999, max_x = -999999;
            float min_y = 999999, max_y = -999999;
            float min_z = 999999, max_z = -999999;
            
            for (int i = 0; i < model->numverts; i++) {
                if (vertices[i][0] < min_x) min_x = vertices[i][0];
                if (vertices[i][0] > max_x) max_x = vertices[i][0];
                if (vertices[i][1] < min_y) min_y = vertices[i][1];
                if (vertices[i][1] > max_y) max_y = vertices[i][1];
                if (vertices[i][2] < min_z) min_z = vertices[i][2];
                if (vertices[i][2] > max_z) max_z = vertices[i][2];
            }
            
            float width = max_x - min_x;
            float height = max_y - min_y;
            float depth = max_z - min_z;
            float max_dim = width > height ? (width > depth ? width : depth) : (height > depth ? height : depth);
            
            float scale = 1.8f / max_dim;
            float center_x = (min_x + max_x) * 0.5f;
            float center_y = (min_y + max_y) * 0.5f;
            float center_z = (min_z + max_z) * 0.5f;
            
            printf("Model bounds: X[%.1f,%.1f] Y[%.1f,%.1f] Z[%.1f,%.1f]\n",
                   min_x, max_x, min_y, max_y, min_z, max_z);
            printf("Scaling by %.4f, centering at (%.1f,%.1f,%.1f)\n",
                   scale, center_x, center_y, center_z);
            
            // Allocate and transform vertices
            float *transformed_vertices = malloc(model->numverts * 3 * sizeof(float));
            for (int i = 0; i < model->numverts; i++) {
                transformed_vertices[i * 3 + 0] = (vertices[i][0] - center_x) * scale;
                transformed_vertices[i * 3 + 1] = (vertices[i][1] - center_y) * scale;
                transformed_vertices[i * 3 + 2] = (vertices[i][2] - center_z) * scale;
            }
            
            // Count total indices needed
            int total_indices = 0;
            mstudiomesh_t *meshes = (mstudiomesh_t *)(main_data + model->meshindex);
            
            for (int m = 0; m < model->nummesh; m++) {
                total_indices += meshes[m].numtris * 3;
            }
            
            printf("Expecting approximately %d indices for %d meshes\n", total_indices, model->nummesh);
            
            // Allocate index buffer
            unsigned short *all_indices = malloc(total_indices * sizeof(unsigned short));
            int index_offset = 0;
            
            // Extract triangles from each mesh
            for (int m = 0; m < model->nummesh; m++) {
                printf("  Processing mesh %d: %d triangles\n", m, meshes[m].numtris);
                
                if (meshes[m].numtris > 0) {
                    short *mesh_indices = NULL;
                    int mesh_index_count = 0;
                    
                    mdl_result_t tri_result = parse_triangle_commands(&meshes[m], main_data, 
                                                                     &mesh_indices, &mesh_index_count);
                    
                    if (tri_result == MDL_SUCCESS && mesh_indices && mesh_index_count > 0) {
                        printf("    Extracted %d indices (%d triangles)\n", 
                               mesh_index_count, mesh_index_count / 3);
                        
                        // Copy indices WITHOUT offset since they're all for this model
                        for (int idx = 0; idx < mesh_index_count && index_offset < total_indices; idx++) {
                            // Clamp to valid vertex range
                            short index = mesh_indices[idx];
                            if (index < 0) index = 0;
                            if (index >= model->numverts) index = model->numverts - 1;
                            all_indices[index_offset++] = (unsigned short)index;
                        }
                        
                        free(mesh_indices);
                    }
                }
            }
            
            printf("\nLoading single bodypart with %d vertices and %d indices\n", 
                   model->numverts, index_offset);
            
            // Load into OpenGL
            setup_model_vertices_with_indices(transformed_vertices, model->numverts, 
                                             all_indices, index_offset);
            
            free(transformed_vertices);
            free(all_indices);
            
            printf("✓ Model loaded successfully!\n");
            printf("\nControls:\n");
            printf("  W - Toggle wireframe/points\n");
            printf("  ESC - Exit\n");
        }
    }
    
    render_loop();
    cleanup_renderer();
    
    free(main_data);
    if (texture_data) free(texture_data);
    
    return (0);
}
