/*
 * Debug version - just show vertices as points
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

    printf("\n=== MODEL OVERVIEW ===\n");
    printf("Model: %s\n", main_header->name);
    printf("Bodyparts: %d\n", main_header->numbodyparts);
    
    // Just load vertices - no triangles for now
    int total_vertices = 0;
    
    // Count vertices
    for (int bp = 0; bp < main_header->numbodyparts; bp++) {
        mstudiobodypart_t *bodyparts = (mstudiobodypart_t *)(main_data + main_header->bodypartindex);
        mstudiobodypart_t *bodypart = &bodyparts[bp];
        
        if (bodypart->nummodels > 0) {
            mstudiomodel_t *models = (mstudiomodel_t *)(main_data + bodypart->modelindex);
            mstudiomodel_t *model = &models[0];
            
            printf("Bodypart %d (%s): %d vertices\n", bp, bodypart->name, model->numverts);
            total_vertices += model->numverts;
        }
    }
    
    printf("Total vertices: %d\n\n", total_vertices);
    
    // Initialize renderer
    if (init_renderer(WIDTH, HEIGHT, "Half-Life Model Viewer - DEBUG") != 0) {
        printf("Failed to initialize renderer!\n");
        free(main_data);
        if (texture_data) free(texture_data);
        return (1);
    }
    
    if (total_vertices > 0) {
        float *all_vertices = malloc(total_vertices * 3 * sizeof(float));
        int vertex_offset = 0;
        
        // Load ALL vertices
        for (int bp = 0; bp < main_header->numbodyparts; bp++) {
            mstudiobodypart_t *bodyparts = (mstudiobodypart_t *)(main_data + main_header->bodypartindex);
            mstudiobodypart_t *bodypart = &bodyparts[bp];
            
            if (bodypart->nummodels > 0) {
                mstudiomodel_t *models = (mstudiomodel_t *)(main_data + bodypart->modelindex);
                mstudiomodel_t *model = &models[0];
                
                if (model->numverts > 0) {
                    vec3_t *vertices = (vec3_t *)(main_data + model->vertindex);
                    
                    // Find bounds
                    float min_vals[3] = {999999, 999999, 999999};
                    float max_vals[3] = {-999999, -999999, -999999};
                    
                    for (int i = 0; i < model->numverts; i++) {
                        for (int j = 0; j < 3; j++) {
                            if (vertices[i][j] < min_vals[j]) min_vals[j] = vertices[i][j];
                            if (vertices[i][j] > max_vals[j]) max_vals[j] = vertices[i][j];
                        }
                    }
                    
                    printf("Bodypart %s bounds:\n", bodypart->name);
                    printf("  X: [%.1f to %.1f]\n", min_vals[0], max_vals[0]);
                    printf("  Y: [%.1f to %.1f]\n", min_vals[1], max_vals[1]);
                    printf("  Z: [%.1f to %.1f]\n", min_vals[2], max_vals[2]);
                    
                    // Calculate scale to fit in -1 to 1
                    float ranges[3] = {
                        max_vals[0] - min_vals[0],
                        max_vals[1] - min_vals[1],
                        max_vals[2] - min_vals[2]
                    };
                    float max_range = ranges[0];
                    if (ranges[1] > max_range) max_range = ranges[1];
                    if (ranges[2] > max_range) max_range = ranges[2];
                    
                    float scale = 1.8f / max_range;  // Scale to fit
                    float center[3] = {
                        (min_vals[0] + max_vals[0]) * 0.5f,
                        (min_vals[1] + max_vals[1]) * 0.5f,
                        (min_vals[2] + max_vals[2]) * 0.5f
                    };
                    
                    printf("  Scale factor: %.4f\n", scale);
                    printf("  Center: (%.1f, %.1f, %.1f)\n\n", center[0], center[1], center[2]);
                    
                    // Copy and transform vertices
                    for (int i = 0; i < model->numverts; i++) {
                        all_vertices[(vertex_offset + i) * 3 + 0] = (vertices[i][0] - center[0]) * scale;
                        all_vertices[(vertex_offset + i) * 3 + 1] = (vertices[i][1] - center[1]) * scale;
                        all_vertices[(vertex_offset + i) * 3 + 2] = (vertices[i][2] - center[2]) * scale;
                    }
                    
                    vertex_offset += model->numverts;
                }
            }
        }
        
        // Just load vertices as points - no triangles
        setup_model_vertices(all_vertices, total_vertices);
        printf("Loaded %d vertices as points\n", total_vertices);
        printf("\nPress W to toggle display mode, ESC to exit\n");
        
        free(all_vertices);
    }
    
    render_loop();
    cleanup_renderer();
    
    free(main_data);
    if (texture_data) free(texture_data);
    
    return (0);
}
