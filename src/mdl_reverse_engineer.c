/*
* @Author: karlosiric
* @Date:   2025-07-18 10:43:05
* @Last Modified by:   karlosiric
* @Last Modified time: 2025-07-18 12:28:03
*/

#include "mdl_reverse_engineer.h"
#include <stdio.h>
#include <stdlib.h>

void reverse_engineer_header(const char *filepath) {

    FILE *file = fopen(filepath, "rb");
    if (!file) {
        fprintf(stderr, "Error opening the file: '%s'\n", filepath);
        return;
    }

    mdl_header_s header;


    fread(header.magic, sizeof(char), sizeof(header.magic), file);
    fread(&header.version, sizeof(int), 1, file);
    fread(header.name, sizeof(char), sizeof(header.name), file);


    printf("==== REVERSE ENGINEERING MDL FILES ====\n");
    printf("==== MDL File Info ====\n");
    printf("Magic: %.4s\n", header.magic);
    printf("Version: %d\n", header.version);

    printf("sizeof(int): %zu\n", sizeof(int));
    printf("Name: %s\n", header.name);

    printf("\nDigging depper to the unknown ...\n");


    // TODO: need to add more things as we progress and discover new things





    fclose(file);
}


/**
 * @brief       Function used for trying to reverse engineer the contents 
 *              of the binary .mdl files used in Half Life 1 game.
 *              
 *              Wanted to try to make a proper terminal hex to ASCII dump
 *              to see what is waht at which position.
 *              
 *              This function is still being worked at so it is not finished
 *              This is just to give me insight into how things work deeply
 *              
 *              Doing this to understand and learn things better.
 *              
 *                            
 *
 * @param[in]  filepath  The filepath
 */

void reverse_engineer_ascii(const char *filepath) {
    FILE *file = fopen(filepath, "rb");
    if(!file) {
        fprintf(stderr, "Error opening the file: '%s'\n", filepath);
        return;
    }

    char buffer[1024];
    size_t bytes_read;
    int position = 0;

    while((bytes_read = fread(buffer, sizeof(char), sizeof(buffer), file)) > 0) {
        for (size_t i = 0; i < bytes_read; i++) {
            if (buffer[i] >= 32 && buffer[i] <= 126) {
                int start = i;
                int length = 0;

                while(i < bytes_read && buffer[i] >= 32 && buffer[i] <= 126) {
                    length++;
                    i++;
                }

                if (length >= 4) {
                    printf("Text at position %d: \"", position + start);
                    for (int j = 0; j < length; j++) {
                        printf("%c", buffer[start + j]);
                    }
                    printf("\"\n");
                }
            }
        }
        position += bytes_read;
    }
}

mdl_model_s load_mdl_file_re(const char *filepath) {
    mdl_model_s model = {NULL, 0};

    // Adding more as we move along and progress forwards

    return model;

}


void free_mdl_model(mdl_model_s *model) {
    if (model->vertices) {
        free(model->vertices);
        model->vertices = NULL;
        model->vertex_count = 0;
    }
}
