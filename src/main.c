/*
 * Main application implementation
 * We'll write this together step by step
 */

#include "main.h"
#include "mdl/mdl_info.h"
#include "mdl/mdl_loader.h"
#include "studio.h"
#include "graphics/renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Code will go here as we write it together

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

    // Debug: Check structure sizes to ensure they're correct
    printf("\n=== STRUCTURE SIZE CHECK ===\n");
    printf("sizeof(mstudiomesh_t) = %zu (should be 20)\n", sizeof(mstudiomesh_t));
    printf("sizeof(studiohdr_t) = %zu (should be 244)\n", sizeof(studiohdr_t));
    printf("sizeof(mstudiotrivert_t) = %zu (should be 8)\n", sizeof(mstudiotrivert_t));
    printf("\n");
    
    print_complete_model_analysis(argv[1], main_header, texture_header, main_data, texture_data);

    printf("Initializing the renderer...\n");
    if (init_renderer(WIDTH, HEIGHT, "Half-Life Model Viewer") != 0) {
        printf("Failed to initialize renderer!\n");
        free(main_data);
        if (texture_data) free(texture_data);
        return (1);
    }
    
    // Extract vertices from ALL bodyparts and load into OpenGL
    printf("\nExtracting ALL model parts for rendering...\n");
    
    // Count total vertices across all bodyparts
    int total_vertices = 0;
    
    for (int bp = 0; bp < main_header->numbodyparts; bp++) {
        mstudiobodypart_t *bodyparts = (mstudiobodypart_t *)(main_data + main_header->bodypartindex);
        mstudiobodypart_t *bodypart = &bodyparts[bp];
        
        if (bodypart->nummodels > 0) {
            mstudiomodel_t *models = (mstudiomodel_t *)(main_data + bodypart->modelindex);
            mstudiomodel_t *first_model = &models[0]; // Use first model of each bodypart
            total_vertices += first_model->numverts;
        }
    }
    
    printf("Total vertices across all bodyparts: %d\n", total_vertices);
    
    if (total_vertices > 0) {
        // Allocate memory for all vertices
        float *all_vertices = malloc(total_vertices * 3 * sizeof(float));
        int vertex_offset = 0;
        int vertex_offset_for_bodypart[3] = {0}; // Track vertex offsets for each bodypart
        
        // Load vertices from all bodyparts
        for (int bp = 0; bp < main_header->numbodyparts; bp++) {
            mstudiobodypart_t *bodyparts = (mstudiobodypart_t *)(main_data + main_header->bodypartindex);
            mstudiobodypart_t *bodypart = &bodyparts[bp];
            
            if (bodypart->nummodels > 0) {
                mstudiomodel_t *models = (mstudiomodel_t *)(main_data + bodypart->modelindex);
                mstudiomodel_t *model = &models[0]; // Use first model
                
                printf("Loading bodypart %d: %d vertices\n", bp, model->numverts);
                vertex_offset_for_bodypart[bp] = vertex_offset; // Store offset for this bodypart
                
                if (model->numverts > 0) {
                    vec3_t *vertices = (vec3_t *)(main_data + model->vertindex);
                    
                    // Copy vertices with better scaling and centering
                    for (int i = 0; i < model->numverts; i++) {
                        all_vertices[(vertex_offset + i) * 3 + 0] = vertices[i][0] * 0.005f;  // X
                        all_vertices[(vertex_offset + i) * 3 + 1] = vertices[i][1] * 0.005f - 0.2f;  // Y - move up
                        all_vertices[(vertex_offset + i) * 3 + 2] = vertices[i][2] * 0.005f;  // Z
                    }
                    
                    vertex_offset += model->numverts;
                }
            }
        }
        
        // First, just try to render vertices as points to ensure they're loading
        printf("\nRendering vertices as POINTS first to verify they're visible...\n");
        
        setup_model_vertices(all_vertices, total_vertices);
        
        printf("Complete model loaded with %d total vertices!\n", total_vertices);
    }
    
    render_loop();

    cleanup_renderer();

    // Clean up memory
    free(main_data);
    if (texture_data) {
        free(texture_data);
    }
    
    return (0);
}
