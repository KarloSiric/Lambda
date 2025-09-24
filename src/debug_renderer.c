/*======================================================================
 *  File: debug_renderer.c
 *  Purpose: Debug and fix rendering issues in the Half-Life Model Viewer
 *  This file contains diagnostic functions to identify problems
 *======================================================================
 */

#include "studio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

// Color codes for terminal output
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define RESET "\033[0m"

// Debug structures to verify data extraction
typedef struct {
    int vertex_count;
    int normal_count;
    int triangle_count;
    bool has_valid_normals;
    bool has_valid_uvs;
    float normal_avg_length;
    float vertex_bounds[6]; // min_x, min_y, min_z, max_x, max_y, max_z
} ModelDiagnostics;

// Function to validate header
void debug_validate_header(studiohdr_t *header) {
    printf("\n" BLUE "=== HEADER VALIDATION ===" RESET "\n");
    
    if (header->id != IDSTUDIOHEADER) {
        printf(RED "✗ Invalid magic number: 0x%X (expected 0x%X)\n" RESET, 
               header->id, IDSTUDIOHEADER);
        return;
    }
    printf(GREEN "✓ Valid magic number: IDST\n" RESET);
    
    if (header->version != STUDIO_VERSION) {
        printf(RED "✗ Invalid version: %d (expected %d)\n" RESET, 
               header->version, STUDIO_VERSION);
        return;
    }
    printf(GREEN "✓ Valid version: %d\n" RESET, header->version);
    
    printf("Model name: '%s'\n", header->name);
    printf("File size: %d bytes\n", header->length);
    printf("Components:\n");
    printf("  Bones: %d\n", header->numbones);
    printf("  Bodyparts: %d\n", header->numbodyparts);
    printf("  Textures: %d\n", header->numtextures);
    printf("  Sequences: %d\n", header->numseq);
}

// Function to check normal extraction
void debug_check_normals(studiohdr_t *header, unsigned char *data, ModelDiagnostics *diag) {
    printf("\n" BLUE "=== NORMAL EXTRACTION TEST ===" RESET "\n");
    
    mstudiobodypart_t *bodyparts = (mstudiobodypart_t *)(data + header->bodypartindex);
    
    for (int bp = 0; bp < header->numbodyparts && bp < 1; bp++) {
        printf("Bodypart %d: '%s'\n", bp, bodyparts[bp].name);
        
        if (bodyparts[bp].nummodels == 0) {
            printf(YELLOW "  Warning: No models in this bodypart\n" RESET);
            continue;
        }
        
        mstudiomodel_t *models = (mstudiomodel_t *)(data + bodyparts[bp].modelindex);
        mstudiomodel_t *model = &models[0];
        
        printf("  Model 0: '%s'\n", model->name);
        printf("    Vertices: %d\n", model->numverts);
        printf("    Normals: %d\n", model->numnorms);
        printf("    Vertex offset: 0x%X\n", model->vertindex);
        printf("    Normal offset: 0x%X\n", model->normindex);
        
        // CRITICAL FIX: Correct normal extraction
        if (model->numnorms > 0 && model->normindex > 0) {
            vec3_t *normals = (vec3_t *)(data + model->normindex);
            
            float total_length = 0.0f;
            int valid_normals = 0;
            
            // Check first 10 normals
            printf("  First 10 normals:\n");
            for (int i = 0; i < 10 && i < model->numnorms; i++) {
                float length = sqrt(normals[i][0] * normals[i][0] + 
                                  normals[i][1] * normals[i][1] + 
                                  normals[i][2] * normals[i][2]);
                
                printf("    [%d]: (%.3f, %.3f, %.3f) length=%.3f", 
                       i, normals[i][0], normals[i][1], normals[i][2], length);
                
                if (length > 0.5f && length < 1.5f) {
                    printf(GREEN " ✓\n" RESET);
                    valid_normals++;
                } else {
                    printf(RED " ✗ (abnormal)\n" RESET);
                }
                
                total_length += length;
            }
            
            diag->normal_count = model->numnorms;
            diag->has_valid_normals = (valid_normals > 5);
            diag->normal_avg_length = total_length / 10.0f;
            
            if (diag->has_valid_normals) {
                printf(GREEN "  ✓ Normals appear valid\n" RESET);
            } else {
                printf(RED "  ✗ Normals may be invalid or incorrectly extracted\n" RESET);
            }
        } else {
            printf(RED "  ✗ No normals found!\n" RESET);
            diag->has_valid_normals = false;
        }
    }
}

// Function to validate triangle commands
void debug_validate_triangle_commands(studiohdr_t *header, unsigned char *data, ModelDiagnostics *diag) {
    printf("\n" BLUE "=== TRIANGLE COMMAND VALIDATION ===" RESET "\n");
    
    mstudiobodypart_t *bodyparts = (mstudiobodypart_t *)(data + header->bodypartindex);
    mstudiomodel_t *models = (mstudiomodel_t *)(data + bodyparts[0].modelindex);
    mstudiomodel_t *model = &models[0];
    mstudiomesh_t *meshes = (mstudiomesh_t *)(data + model->meshindex);
    
    int total_triangles = 0;
    int valid_commands = 0;
    int invalid_commands = 0;
    
    printf("Processing %d meshes:\n", model->nummesh);
    
    for (int m = 0; m < model->nummesh && m < 3; m++) {
        printf("\nMesh %d:\n", m);
        printf("  Expected triangles: %d\n", meshes[m].numtris);
        printf("  Triangle command offset: 0x%X\n", meshes[m].triindex);
        
        short *commands = (short *)(data + meshes[m].triindex);
        int mesh_triangles = 0;
        int cmd_count = 0;
        
        while (*commands != 0 && cmd_count < 100) { // Safety limit
            int cmd = *commands++;
            int num_verts = abs(cmd);
            bool is_fan = (cmd < 0);
            
            printf("  Command %d: %s with %d vertices\n", 
                   cmd_count, is_fan ? "FAN" : "STRIP", num_verts);
            
            if (num_verts < 3 || num_verts > 100) {
                printf(RED "    ✗ Invalid vertex count!\n" RESET);
                invalid_commands++;
                break;
            }
            
            // Check vertex data
            for (int v = 0; v < num_verts; v++) {
                short vert_idx = commands[0];
                short norm_idx = commands[1];
                short s = commands[2];
                short t = commands[3];
                
                if (v < 3) { // Only print first 3
                    printf("    Vert[%d]: v=%d n=%d uv=(%d,%d)", 
                           v, vert_idx, norm_idx, s, t);
                    
                    if (vert_idx < 0 || vert_idx >= model->numverts) {
                        printf(RED " ✗ Invalid vertex index\n" RESET);
                    } else if (norm_idx < 0 || norm_idx >= model->numnorms) {
                        printf(RED " ✗ Invalid normal index\n" RESET);
                    } else {
                        printf(GREEN " ✓\n" RESET);
                        valid_commands++;
                    }
                }
                
                commands += 4;
            }
            
            // Calculate triangles from this command
            if (is_fan) {
                mesh_triangles += (num_verts - 2);
            } else {
                mesh_triangles += (num_verts - 2);
            }
            
            cmd_count++;
        }
        
        printf("  Mesh %d: Generated %d triangles (expected %d)\n", 
               m, mesh_triangles, meshes[m].numtris);
        
        if (abs(mesh_triangles - meshes[m].numtris) < 5) {
            printf(GREEN "  ✓ Triangle count matches\n" RESET);
        } else {
            printf(YELLOW "  ⚠ Triangle count mismatch\n" RESET);
        }
        
        total_triangles += mesh_triangles;
    }
    
    diag->triangle_count = total_triangles;
    printf("\nTotal triangles extracted: %d\n", total_triangles);
    
    if (valid_commands > invalid_commands * 10) {
        printf(GREEN "✓ Triangle commands appear valid\n" RESET);
    } else {
        printf(RED "✗ Triangle command parsing may have issues\n" RESET);
    }
}

// Function to check UV coordinates
void debug_check_uv_coordinates(studiohdr_t *header, unsigned char *data, ModelDiagnostics *diag) {
    printf("\n" BLUE "=== UV COORDINATE ANALYSIS ===" RESET "\n");
    
    mstudiobodypart_t *bodyparts = (mstudiobodypart_t *)(data + header->bodypartindex);
    mstudiomodel_t *models = (mstudiomodel_t *)(data + bodyparts[0].modelindex);
    mstudiomodel_t *model = &models[0];
    mstudiomesh_t *meshes = (mstudiomesh_t *)(data + model->meshindex);
    
    printf("Checking UV ranges for first mesh:\n");
    
    if (model->nummesh > 0) {
        short *commands = (short *)(data + meshes[0].triindex);
        float min_u = 1000.0f, max_u = -1000.0f;
        float min_v = 1000.0f, max_v = -1000.0f;
        int uv_count = 0;
        
        int cmd = *commands++;
        if (cmd != 0) {
            int num_verts = abs(cmd);
            
            for (int v = 0; v < num_verts && v < 20; v++) {
                commands += 2; // Skip vertex and normal indices
                short s = commands[0];
                short t = commands[1];
                commands += 2;
                
                float u = (float)s;
                float v = (float)t;
                
                if (u < min_u) min_u = u;
                if (u > max_u) max_u = u;
                if (v < min_v) min_v = v;
                if (v > max_v) max_v = v;
                
                uv_count++;
                
                if (v < 5) {
                    printf("  UV[%d]: raw=(%d, %d) normalized=(%.3f, %.3f)\n",
                           v, s, t, u/256.0f, v/256.0f);
                }
            }
        }
        
        printf("\nUV Range Analysis:\n");
        printf("  Raw U range: %.1f to %.1f\n", min_u, max_u);
        printf("  Raw V range: %.1f to %.1f\n", min_v, max_v);
        printf("  Normalized U: %.3f to %.3f\n", min_u/256.0f, max_u/256.0f);
        printf("  Normalized V: %.3f to %.3f\n", min_v/256.0f, max_v/256.0f);
        
        // Check if UVs are in a reasonable range
        if (max_u > 0 && max_v > 0 && max_u < 10000 && max_v < 10000) {
            printf(GREEN "✓ UV coordinates appear valid\n" RESET);
            diag->has_valid_uvs = true;
        } else {
            printf(RED "✗ UV coordinates may be invalid\n" RESET);
            diag->has_valid_uvs = false;
        }
    }
}

// Function to check vertex bounds
void debug_check_vertex_bounds(studiohdr_t *header, unsigned char *data, ModelDiagnostics *diag) {
    printf("\n" BLUE "=== VERTEX BOUNDS CHECK ===" RESET "\n");
    
    mstudiobodypart_t *bodyparts = (mstudiobodypart_t *)(data + header->bodypartindex);
    mstudiomodel_t *models = (mstudiomodel_t *)(data + bodyparts[0].modelindex);
    mstudiomodel_t *model = &models[0];
    
    if (model->numverts > 0) {
        vec3_t *vertices = (vec3_t *)(data + model->vertindex);
        
        float min_x = vertices[0][0], max_x = vertices[0][0];
        float min_y = vertices[0][1], max_y = vertices[0][1];
        float min_z = vertices[0][2], max_z = vertices[0][2];
        
        for (int i = 0; i < model->numverts; i++) {
            if (vertices[i][0] < min_x) min_x = vertices[i][0];
            if (vertices[i][0] > max_x) max_x = vertices[i][0];
            if (vertices[i][1] < min_y) min_y = vertices[i][1];
            if (vertices[i][1] > max_y) max_y = vertices[i][1];
            if (vertices[i][2] < min_z) min_z = vertices[i][2];
            if (vertices[i][2] > max_z) max_z = vertices[i][2];
        }
        
        diag->vertex_bounds[0] = min_x;
        diag->vertex_bounds[1] = min_y;
        diag->vertex_bounds[2] = min_z;
        diag->vertex_bounds[3] = max_x;
        diag->vertex_bounds[4] = max_y;
        diag->vertex_bounds[5] = max_z;
        
        printf("Vertex bounds:\n");
        printf("  X: %.2f to %.2f (width: %.2f)\n", min_x, max_x, max_x - min_x);
        printf("  Y: %.2f to %.2f (depth: %.2f)\n", min_y, max_y, max_y - min_y);
        printf("  Z: %.2f to %.2f (height: %.2f)\n", min_z, max_z, max_z - min_z);
        
        float width = max_x - min_x;
        float height = max_z - min_z;
        
        if (width > 0 && height > 0 && width < 1000 && height < 1000) {
            printf(GREEN "✓ Vertex bounds appear reasonable\n" RESET);
        } else {
            printf(YELLOW "⚠ Vertex bounds may need scaling adjustment\n" RESET);
        }
        
        // Print first few vertices
        printf("\nFirst 5 vertices:\n");
        for (int i = 0; i < 5 && i < model->numverts; i++) {
            printf("  [%d]: (%.2f, %.2f, %.2f)\n", 
                   i, vertices[i][0], vertices[i][1], vertices[i][2]);
        }
    }
}

// Main diagnostic function
void run_model_diagnostics(studiohdr_t *header, unsigned char *data) {
    printf("\n" BLUE "========================================\n");
    printf("     HALF-LIFE MODEL DIAGNOSTICS\n");
    printf("========================================" RESET "\n");
    
    ModelDiagnostics diag = {0};
    
    // Run all diagnostic tests
    debug_validate_header(header);
    debug_check_normals(header, data, &diag);
    debug_validate_triangle_commands(header, data, &diag);
    debug_check_uv_coordinates(header, data, &diag);
    debug_check_vertex_bounds(header, data, &diag);
    
    // Final summary
    printf("\n" BLUE "=== DIAGNOSTIC SUMMARY ===" RESET "\n");
    printf("Vertices extracted: %d\n", diag.vertex_count);
    printf("Normals extracted: %d\n", diag.normal_count);
    printf("Triangles generated: %d\n", diag.triangle_count);
    
    printf("\nStatus:\n");
    if (diag.has_valid_normals) {
        printf(GREEN "  ✓ Normals: VALID (avg length: %.3f)\n" RESET, diag.normal_avg_length);
    } else {
        printf(RED "  ✗ Normals: INVALID or MISSING\n" RESET);
    }
    
    if (diag.has_valid_uvs) {
        printf(GREEN "  ✓ UV Coordinates: VALID\n" RESET);
    } else {
        printf(RED "  ✗ UV Coordinates: INVALID or MISSING\n" RESET);
    }
    
    if (diag.triangle_count > 0) {
        printf(GREEN "  ✓ Triangle extraction: SUCCESS (%d triangles)\n" RESET, diag.triangle_count);
    } else {
        printf(RED "  ✗ Triangle extraction: FAILED\n" RESET);
    }
    
    // Recommendations
    printf("\n" YELLOW "=== RECOMMENDATIONS ===" RESET "\n");
    
    if (!diag.has_valid_normals) {
        printf("1. Fix normal extraction - check model->normindex offset\n");
        printf("   Current approach may have incorrect pointer arithmetic\n");
    }
    
    if (!diag.has_valid_uvs) {
        printf("2. Fix UV normalization - Half-Life uses different scales\n");
        printf("   Try dividing by texture width/height instead of fixed 64\n");
    }
    
    if (diag.vertex_bounds[3] - diag.vertex_bounds[0] > 100) {
        printf("3. Adjust vertex scaling - model may be too large\n");
        printf("   Current scale factor: 0.05f may need adjustment\n");
    }
    
    printf("\n" BLUE "========================================" RESET "\n\n");
}

// Function to generate a fixed renderer
void generate_fixed_renderer_code() {
    printf("\n" GREEN "=== FIXED CODE SNIPPETS ===" RESET "\n");
    
    printf("\n// FIXED: Correct normal extraction\n");
    printf("void AddVertexToBuffer_FIXED(int vertex_index, int normal_index, float u, float v) {\n");
    printf("    if (total_render_vertices >= MAX_RENDER_VERTICES) return;\n");
    printf("    \n");
    printf("    // Get the current model being rendered\n");
    printf("    mstudiobodypart_t *bodyparts = (mstudiobodypart_t *)(global_data + global_header->bodypartindex);\n");
    printf("    mstudiomodel_t *models = (mstudiomodel_t *)(global_data + bodyparts[0].modelindex);\n");
    printf("    mstudiomodel_t *model = &models[0];\n");
    printf("    \n");
    printf("    // FIXED: Correct normal pointer\n");
    printf("    vec3_t *normals = (vec3_t *)(global_data + model->normindex);\n");
    printf("    \n");
    printf("    int base_index = total_render_vertices * 8;\n");
    printf("    \n");
    printf("    // Position (transformed by bones)\n");
    printf("    render_vertex_buffer[base_index + 0] = m_pxformverts[vertex_index][0] * 0.03f;\n");
    printf("    render_vertex_buffer[base_index + 1] = m_pxformverts[vertex_index][2] * 0.03f; // Swap Y/Z\n");
    printf("    render_vertex_buffer[base_index + 2] = -m_pxformverts[vertex_index][1] * 0.03f;\n");
    printf("    \n");
    printf("    // Normal\n");
    printf("    render_vertex_buffer[base_index + 3] = normals[normal_index][0];\n");
    printf("    render_vertex_buffer[base_index + 4] = normals[normal_index][2]; // Swap Y/Z\n");
    printf("    render_vertex_buffer[base_index + 5] = -normals[normal_index][1];\n");
    printf("    \n");
    printf("    // UV coordinates - use proper normalization\n");
    printf("    render_vertex_buffer[base_index + 6] = (float)u / 256.0f;\n");
    printf("    render_vertex_buffer[base_index + 7] = (float)v / 256.0f;\n");
    printf("    \n");
    printf("    total_render_vertices++;\n");
    printf("}\n");
}
