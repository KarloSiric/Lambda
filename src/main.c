/*
 * Main application implementation
 * Properly uses triangle indices for correct model rendering
 */

#include "main.h"
#include "mdl/mdl_info.h"
#include "mdl/mdl_loader.h"
#include "studio.h"
#include "graphics/renderer.h"
#include <stdio.h>
#include <stdlib.h>

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
    if (init_renderer(WIDTH, HEIGHT, "Half-Life Model Viewer") != 0) {
        printf("Failed to initialize renderer!\n");
        free(main_data);
        if (texture_data) free(texture_data);
        return (1);
    }
    
    // PROPERLY EXTRACT VERTICES AND TRIANGLES WITH CORRECT CONNECTIVITY
    printf("\n=== EXTRACTING MODEL WITH PROPER TRIANGLE CONNECTIVITY ===\n");
    
    // Count total vertices and triangles across all bodyparts
    int total_vertices = 0;
    int total_triangles = 0;
    
    mstudiobodyparts_t *bodyparts = (mstudiobodyparts_t *)(main_data + main_header->bodypartindex);
    
    // First pass: count vertices and triangles
    for (int bp = 0; bp < main_header->numbodyparts; bp++) {
        mstudiobodyparts_t *bodypart = &bodyparts[bp];
        
        if (bodypart->nummodels > 0) {
            mstudiomodel_t *models = (mstudiomodel_t *)(main_data + bodypart->modelindex);
            mstudiomodel_t *model = &models[0]; // First model of bodypart
            
            total_vertices += model->numverts;
            
            // Count triangles from all meshes
            if (model->nummesh > 0) {
                mstudiomesh_t *meshes = (mstudiomesh_t *)(main_data + model->meshindex);
                for (int m = 0; m < model->nummesh; m++) {
                    total_triangles += meshes[m].numtris;
                }
            }
        }
    }
    
    printf("Total vertices: %d\n", total_vertices);
    printf("Total expected triangles: %d\n", total_triangles);
    
    if (total_vertices > 0 && total_triangles > 0) {
        // Allocate arrays for all vertices and indices
        float *all_vertices = malloc(total_vertices * 3 * sizeof(float));
        int max_indices = total_triangles * 3;  // Max possible indices
        unsigned short *all_indices = malloc(max_indices * sizeof(unsigned short));
        
        int vertex_offset = 0;
        int index_offset = 0;
        int current_vertex_base = 0;  // Track base vertex index for each model
        
        // Second pass: load vertices and extract triangle indices
        for (int bp = 0; bp < main_header->numbodyparts; bp++) {
            mstudiobodyparts_t *bodypart = &bodyparts[bp];
            
            if (bodypart->nummodels > 0) {
                mstudiomodel_t *models = (mstudiomodel_t *)(main_data + bodypart->modelindex);
                mstudiomodel_t *model = &models[0];
                
                printf("\nProcessing bodypart %d: %s\n", bp, bodypart->name);
                printf("  Model has %d vertices, %d meshes\n", model->numverts, model->nummesh);
                
                // Load vertices with coordinate transformation
                if (model->numverts > 0) {
                    vec3_t *hl_vertices = (vec3_t *)(main_data + model->vertindex);
                    
                    // Transform Half-Life coordinates to OpenGL
                    transform_vertices_to_opengl(hl_vertices, model->numverts, 
                                               &all_vertices[vertex_offset * 3], 0.01f);  // Much smaller scale for scientist model
                    
                    vertex_offset += model->numverts;
                    
                    // Process each mesh to extract triangle indices
                    if (model->nummesh > 0) {
                        mstudiomesh_t *meshes = (mstudiomesh_t *)(main_data + model->meshindex);
                        
                        for (int m = 0; m < model->nummesh; m++) {
                            printf("  Processing mesh %d: %d triangles expected\n", m, meshes[m].numtris);
                            
                            short *mesh_indices = NULL;
                            int mesh_index_count = 0;
                            
                            // Parse triangle commands for this mesh
                            mdl_result_t parse_result = parse_triangle_commands_fixed(&meshes[m], main_data, 
                                                                                    &mesh_indices, &mesh_index_count);
                            
                            if (parse_result == MDL_SUCCESS && mesh_indices && mesh_index_count > 0) {
                                printf("    Successfully extracted %d indices (%d triangles)\n", 
                                       mesh_index_count, mesh_index_count / 3);
                                
                                // Copy indices, adjusting for current vertex base
                                for (int i = 0; i < mesh_index_count && index_offset < max_indices; i++) {
                                    all_indices[index_offset++] = (unsigned short)(mesh_indices[i] + current_vertex_base);
                                }
                                
                                free(mesh_indices);
                            } else {
                                printf("    WARNING: Failed to parse triangle commands for mesh %d\n", m);
                            }
                        }
                    }
                    
                    current_vertex_base += model->numverts;  // Update base for next model
                }
            }
        }
        
        printf("\n=== LOADING INTO OPENGL ===\n");
        printf("Loaded %d vertices\n", vertex_offset);
        printf("Loaded %d indices (%d triangles)\n", index_offset, index_offset / 3);
        
        // Send to OpenGL with proper triangle connectivity
        if (index_offset > 0) {
            setup_model_vertices_with_indices(all_vertices, vertex_offset, all_indices, index_offset);
            printf("✅ Model loaded with proper triangle connectivity!\n");
        } else {
            // Fallback if no indices were extracted
            printf("⚠️ No triangle indices extracted, using vertex-only mode\n");
            setup_model_vertices(all_vertices, vertex_offset);
        }
        
        free(all_vertices);
        free(all_indices);
    } else {
        printf("ERROR: Model has no geometry to render!\n");
    }
    
    render_loop();

    cleanup_renderer();

    // Clean up memory
    free(main_data);
    if (texture_data) {
        free(texture_data);
    }
    
    printf("\n✅ Program terminated successfully\n");
    return (0);
}
