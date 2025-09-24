// test_fixes.c
// Test program to verify the renderer fixes

#include "studio.h"
#include "mdl/mdl_loader.h"
#include <stdio.h>
#include <stdlib.h>

// External diagnostic function
void run_model_diagnostics(studiohdr_t *header, unsigned char *data);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <model.mdl>\n", argv[0]);
        return 1;
    }
    
    printf("Half-Life Model Viewer - Fix Verification\n");
    printf("=========================================\n\n");
    
    // Load the model
    studiohdr_t *main_header = NULL;
    studiohdr_t *texture_header = NULL;
    unsigned char *main_data = NULL;
    unsigned char *texture_data = NULL;
    
    mdl_result_t result = load_model_with_textures(argv[1], &main_header, &texture_header, 
                                                   &main_data, &texture_data);
    
    if (result != MDL_SUCCESS) {
        printf("ERROR: Failed to load model! Error code: %d\n", result);
        return 1;
    }
    
    printf("Model loaded successfully!\n");
    printf("Model name: %s\n", main_header->name);
    printf("File size: %d bytes\n\n", main_header->length);
    
    // Run diagnostics
    run_model_diagnostics(main_header, main_data);
    
    // Test specific data extraction
    printf("\n=== TESTING DATA EXTRACTION ===\n");
    
    // Test 1: Verify bodypart and model access
    mstudiobodypart_t *bodyparts = (mstudiobodypart_t *)(main_data + main_header->bodypartindex);
    printf("\nTest 1 - Bodypart Access:\n");
    printf("  First bodypart: %s\n", bodyparts[0].name);
    printf("  Models in bodypart: %d\n", bodyparts[0].nummodels);
    
    // Test 2: Verify model data
    mstudiomodel_t *models = (mstudiomodel_t *)(main_data + bodyparts[0].modelindex);
    mstudiomodel_t *model = &models[0];
    printf("\nTest 2 - Model Data:\n");
    printf("  Model name: %s\n", model->name);
    printf("  Vertices: %d at offset 0x%X\n", model->numverts, model->vertindex);
    printf("  Normals: %d at offset 0x%X\n", model->numnorms, model->normindex);
    printf("  Meshes: %d at offset 0x%X\n", model->nummesh, model->meshindex);
    
    // Test 3: Verify normal extraction (CRITICAL TEST)
    printf("\nTest 3 - Normal Extraction:\n");
    if (model->normindex > 0 && model->numnorms > 0) {
        vec3_t *normals = (vec3_t *)(main_data + model->normindex);
        
        // Check if normals are valid (should be unit vectors or close to it)
        int valid_count = 0;
        for (int i = 0; i < 5 && i < model->numnorms; i++) {
            float length = sqrt(normals[i][0] * normals[i][0] + 
                              normals[i][1] * normals[i][1] + 
                              normals[i][2] * normals[i][2]);
            
            printf("  Normal[%d]: (%.3f, %.3f, %.3f) length=%.3f", 
                   i, normals[i][0], normals[i][1], normals[i][2], length);
            
            if (length > 0.9f && length < 1.1f) {
                printf(" ✓\n");
                valid_count++;
            } else {
                printf(" ✗\n");
            }
        }
        
        if (valid_count >= 3) {
            printf("  PASS: Normals are correctly extracted\n");
        } else {
            printf("  FAIL: Normal extraction has issues\n");
        }
    } else {
        printf("  FAIL: No normals found\n");
    }
    
    // Test 4: Verify mesh and triangle commands
    printf("\nTest 4 - Triangle Commands:\n");
    mstudiomesh_t *meshes = (mstudiomesh_t *)(main_data + model->meshindex);
    
    for (int m = 0; m < 2 && m < model->nummesh; m++) {
        printf("  Mesh %d:\n", m);
        printf("    Expected triangles: %d\n", meshes[m].numtris);
        printf("    Skin reference: %d\n", meshes[m].skinref);
        
        // Parse first command
        short *commands = (short *)(main_data + meshes[m].triindex);
        int first_cmd = *commands;
        
        if (first_cmd != 0) {
            bool is_fan = (first_cmd < 0);
            int verts = abs(first_cmd);
            printf("    First command: %s with %d vertices\n", 
                   is_fan ? "FAN" : "STRIP", verts);
            
            if (verts >= 3 && verts <= 100) {
                printf("    PASS: Valid triangle command\n");
            } else {
                printf("    FAIL: Invalid triangle command\n");
            }
        }
    }
    
    // Test 5: Verify texture information
    printf("\nTest 5 - Texture Information:\n");
    if (texture_header && texture_data) {
        printf("  Texture file loaded: YES\n");
        printf("  Number of textures: %d\n", texture_header->numtextures);
        
        if (texture_header->numtextures > 0) {
            mstudiotexture_t *textures = (mstudiotexture_t *)(texture_data + texture_header->textureindex);
            printf("  First texture: %s (%dx%d)\n", 
                   textures[0].name, textures[0].width, textures[0].height);
        }
    } else if (main_header->numtextures > 0) {
        printf("  Textures in main file: %d\n", main_header->numtextures);
        mstudiotexture_t *textures = (mstudiotexture_t *)(main_data + main_header->textureindex);
        printf("  First texture: %s (%dx%d)\n", 
               textures[0].name, textures[0].width, textures[0].height);
    } else {
        printf("  WARNING: No texture information found\n");
    }
    
    // Summary
    printf("\n=== SUMMARY ===\n");
    printf("Model structure: ");
    if (model->numverts > 0 && model->numnorms > 0 && model->nummesh > 0) {
        printf("VALID\n");
        printf("\nTo fix the renderer:\n");
        printf("1. Replace AddVertexToBuffer with AddVertexToBuffer_FIXED from renderer_fixes.c\n");
        printf("2. Replace ProcessModelForRendering with ProcessModelForRendering_FIXED\n");
        printf("3. Add InitializeModelPointers function\n");
        printf("4. Update coordinate system conversion (Y↔Z swap)\n");
        printf("5. Fix UV normalization (use /256.0f or texture dimensions)\n");
    } else {
        printf("INVALID - Check model loading\n");
    }
    
    // Cleanup
    free(main_data);
    if (texture_data) free(texture_data);
    
    return 0;
}
