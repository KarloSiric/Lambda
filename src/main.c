/*
 * Main application implementation
 * We'll write this together step by step
 */

#include "main.h"
#include "mdl/mdl_loader.h"
#include "studio.h"
#include <stdio.h>
#include <stdlib.h>

// Code will go here as we write it together


int main(int argc, char const *argv[])
{
    if (argc != 2) {
        printf("Usage: %s <model.mdl>\n", argv[0]);
        return (1);
    }

    printf("Testing complete model+texture loading: %s\n", argv[1]);
    
    // Variables for both model and texture data
    studiohdr_t *main_header = NULL;
    studiohdr_t *texture_header = NULL;
    unsigned char *main_data = NULL;
    unsigned char *texture_data = NULL;

    // Test your new function!
    mdl_result_t result = load_model_with_textures(argv[1], &main_header, &texture_header, &main_data, &texture_data);
    
    if (result != MDL_SUCCESS) {
        printf("Failed to load model! Error code: %d\n", result);
        return (1);
    }

    printf("SUCCESS! Model loaded completely!\n\n");
    
    // Display main model info
    printf("=== MAIN MODEL INFO ===\n");
    printf("  Name: %s\n", main_header->name);
    printf("  File size: %d bytes\n", main_header->length);
    printf("  Bones: %d\n", main_header->numbones);
    printf("  Bodyparts: %d\n", main_header->numbodyparts);
    printf("  Sequences: %d\n", main_header->numseq);
    
    // Display texture info
    print_texture_info(texture_header, texture_data);
    
    // Display bodypart details
    print_bodypart_info(main_header, main_data);

    // Clean up memory
    free(main_data);
    if (texture_data) {
        free(texture_data);
    }
    
    printf("\nComplete model analysis finished!\n");
    return (0);
}
