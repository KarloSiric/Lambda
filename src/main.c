/*
 * Main application implementation - FIXED version with proper UV extraction
 */

#include "main.h"
#include "mdl/mdl_info.h"
#include "mdl/mdl_loader.h"
#include "studio.h"
#include "graphics/renderer.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else
#include <GL/gl3.h>
#endif

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
    
    // Extract and upload textures to OpenGL (AFTER OpenGL context is created)
    unsigned int *gl_texture_ids = NULL;
    if (texture_header && texture_data) {
        printf("\n=== EXTRACTING AND UPLOADING TEXTURES TO OPENGL ===\n");
        printf("Found %d textures to extract\n", texture_header->numtextures);
        
        // Allocate array for OpenGL texture IDs
        gl_texture_ids = malloc(texture_header->numtextures * sizeof(unsigned int));
        glGenTextures(texture_header->numtextures, gl_texture_ids);
        
        printf("Generated texture IDs: ");
        for (int i = 0; i < texture_header->numtextures && i < 5; i++) {
            printf("%u ", gl_texture_ids[i]);
        }
        printf("...\n");
        
        // Extract and upload all textures
        for (int i = 0; i < texture_header->numtextures; i++) {
            unsigned char *rgb_data = NULL;
            int tex_width, tex_height;
            
            mdl_result_t result = extract_texture_rgb(texture_header, texture_data, 
                                                      i, &rgb_data, 
                                                      &tex_width, &tex_height);
            
            if (result == MDL_SUCCESS && rgb_data) {
                // Upload to OpenGL
                glBindTexture(GL_TEXTURE_2D, gl_texture_ids[i]);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 
                            0, GL_RGB, GL_UNSIGNED_BYTE, rgb_data);
                
                // Set texture parameters
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                
                printf("  Texture %d uploaded to OpenGL (ID: %u)\n", i, gl_texture_ids[i]);
                
                free(rgb_data);
            } else {
                printf("  Failed to extract texture %d\n", i);
            }
        }
        printf("=================================\n\n");
    } else {
        printf("\nNo texture file loaded - running without textures\n\n");
    }
    
    // NEW APPROACH: Extract vertices WITH proper UVs directly
    printf("\n=== EXTRACTING MODEL WITH PROPER UVS ===\n");
    
    mstudiobodyparts_t *bodyparts = (mstudiobodyparts_t *)(main_data + main_header->bodypartindex);
    
    // First pass: count expected triangles to allocate proper buffer size
    int total_triangles = 0;
    for (int bp = 0; bp < main_header->numbodyparts; bp++) {
        mstudiobodyparts_t *bodypart = &bodyparts[bp];
        if (bodypart->nummodels > 0) {
            mstudiomodel_t *models = (mstudiomodel_t *)(main_data + bodypart->modelindex);
            mstudiomodel_t *model = &models[0];
            if (model->nummesh > 0) {
                mstudiomesh_t *meshes = (mstudiomesh_t *)(main_data + model->meshindex);
                for (int m = 0; m < model->nummesh; m++) {
                    total_triangles += meshes[m].numtris;
                }
            }
        }
    }
    
    printf("Total expected triangles: %d\n", total_triangles);
    
    // Each triangle needs 3 unique vertices (for proper per-vertex UVs)
    int max_vertices = total_triangles * 3;
    
    // Allocate final arrays
    float *all_vertices = malloc(max_vertices * 3 * sizeof(float));
    float *all_texcoords = malloc(max_vertices * 2 * sizeof(float));
    
    if (!all_vertices || !all_texcoords) {
        printf("ERROR: Failed to allocate memory for vertices/texcoords\n");
        free(main_data);
        if (texture_data) free(texture_data);
        return (1);
    }
    
    int total_vertex_count = 0;
    
    // Extract all meshes with proper UVs
    for (int bp = 0; bp < main_header->numbodyparts; bp++) {
        mstudiobodyparts_t *bodypart = &bodyparts[bp];
        
        if (bodypart->nummodels > 0) {
            mstudiomodel_t *models = (mstudiomodel_t *)(main_data + bodypart->modelindex);
            mstudiomodel_t *model = &models[0];
            
            printf("\nProcessing bodypart %d: %s\n", bp, bodypart->name);
            printf("  Model: %s (%d meshes)\n", model->name, model->nummesh);
            
            if (model->numverts > 0 && model->nummesh > 0) {
                vec3_t *hl_vertices = (vec3_t *)(main_data + model->vertindex);
                mstudiomesh_t *meshes = (mstudiomesh_t *)(main_data + model->meshindex);
                
                for (int m = 0; m < model->nummesh; m++) {
                    printf("  Processing mesh %d: %d triangles\n", m, meshes[m].numtris);
                    
                    // Extract vertices with proper UVs
                    float *mesh_verts = NULL;
                    float *mesh_uvs = NULL;
                    int mesh_vert_count = 0;
                    
                    mdl_result_t result = extract_triangles_with_uvs(&meshes[m], main_data,
                                                                    hl_vertices, model->numverts,
                                                                    &mesh_verts, &mesh_uvs, 
                                                                    &mesh_vert_count);
                    
                    if (result == MDL_SUCCESS && mesh_verts && mesh_vert_count > 0) {
                        printf("    Extracted %d vertices with UVs\n", mesh_vert_count);
                        
                        // Copy to global arrays
                        for (int v = 0; v < mesh_vert_count && total_vertex_count < max_vertices; v++) {
                            all_vertices[total_vertex_count * 3 + 0] = mesh_verts[v * 3 + 0];
                            all_vertices[total_vertex_count * 3 + 1] = mesh_verts[v * 3 + 1];
                            all_vertices[total_vertex_count * 3 + 2] = mesh_verts[v * 3 + 2];
                            
                            all_texcoords[total_vertex_count * 2 + 0] = mesh_uvs[v * 2 + 0];
                            all_texcoords[total_vertex_count * 2 + 1] = mesh_uvs[v * 2 + 1];
                            
                            total_vertex_count++;
                        }
                        
                        free(mesh_verts);
                        free(mesh_uvs);
                    } else {
                        printf("    WARNING: Failed to extract mesh %d\n", m);
                    }
                }
            }
        }
    }
    
    printf("\n=== FINAL STATISTICS ===\n");
    printf("Total vertices extracted: %d\n", total_vertex_count);
    printf("Total triangles: %d\n", total_vertex_count / 3);
    
    if (total_vertex_count > 0) {
        // For simple triangle list, indices are just 0, 1, 2, 3, 4, 5...
        unsigned short *indices = malloc(total_vertex_count * sizeof(unsigned short));
        for (int i = 0; i < total_vertex_count; i++) {
            indices[i] = i;
        }
        
        // Send to OpenGL
        setup_model_vertices_with_indices_and_texcoords(all_vertices, total_vertex_count, 
                                                       indices, total_vertex_count,
                                                       all_texcoords);
        
        free(indices);
        printf(" Model loaded with proper UVs!\n");
        
        // Set the first texture as current for rendering
        if (gl_texture_ids && texture_header->numtextures > 0) {
            set_current_texture(gl_texture_ids[0]);
            printf(" Texture set for rendering (ID: %u)\n", gl_texture_ids[0]);
        }
    } else {
        printf("ERROR: No vertices extracted!\n");
    }
    
    free(all_vertices);
    free(all_texcoords);
    
    render_loop();
    
    cleanup_renderer();

    // Clean up memory
    free(main_data);
    if (texture_data) free(texture_data);
    if (gl_texture_ids) free(gl_texture_ids);
    
    printf("\n Program terminated successfully\n");
    return 0;
}
