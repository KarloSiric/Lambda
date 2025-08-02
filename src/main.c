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
        printf("Usage: %s <model.mdl\n>", argv[0]);
        printf("Example: %s assets/models/headcrab.mdl\n", argv[0]);
        return (1);
    }

    printf("Testing MDL Loader with: %s\n", argv[1]);
    unsigned char *file_data = NULL;
    size_t file_size = 0;

    printf("Loading file ... \n");
    mdl_result_t result = read_mdl_file(argv[1], &file_data, &file_size);
    if (result != MDL_SUCCESS) {
        printf("Failed to load file! Error code: %d\n", 
                result);
        return (1);
    }

    printf("File loaded successfully! Size: %zu bytes\n",
            file_size);
    
    studiohdr_t *header = NULL;
    mdl_result_t parser = parse_mdl_header(file_data, &header);

    if (parser != MDL_SUCCESS) {
        printf("Failed to parse the header data! Error code: %d\n", parser);
        free(file_data);
        return (1);
    }

    printf("Header parsed successfully!\n");

    printf("Model info:\n");
    printf("  Magic: %.4s\n", (char *)&header->id);
    printf("  Version: %d\n", header->version);
    printf("  Name: %s\n", header->name);
    printf("  File size: %d\n", header->length);

    printf("  Number of bones: %d\n", header->numbones);
    printf("  Number of bodyparts: %d\n", header->numbodyparts);
    printf("  Number of textures: %d\n", header->numtextures);
    printf("  Number of sequences: %d\n", header->numseq);

    print_bodypart_info(header, file_data);

    printf("Testing texture filename generation: \n");
    char *data_name = generate_texture_filename("scientist.mdl");
    char *data_name2 = generate_texture_filename("barney.mdl");

    printf("  scientist.mdl -> %s\n", data_name);
    printf("  barney.mdl -> %s\n", data_name2);

    free(data_name);
    free(data_name2);


    free(file_data);
    printf("\nModel analysis completed\n");


    return (0);
}
