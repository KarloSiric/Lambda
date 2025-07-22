/*
* @Author: karlosiric
* @Date:   2025-07-22 11:49:29
* @Last Modified by:   karlosiric
* @Last Modified time: 2025-07-22 12:15:27
*/

#include "mdl_loader.h"
#include <stdlib.h>

int main(void) {

    FILE *file = fopen("models/HL1_Original/barney.mdl", "rb");
    if (!file) {
        fprintf(stderr, "ERROR - Failed to open the file.\n");
        return (-1);
    }

    studiohdr_t *header = mdl_read_header(file);
    if (header) {
        printf("\n=== MDL HEADER INFO ===\n");
        printf("Name: '%s'\n", header->name);
        printf("Version: %d\n", header->version);
        printf("Bodyparts: %d\n", header->numbodyparts);
        printf("Textures: %d\n", header->numtextures);

        free(header);
    }

    fclose(file);
    return (0);
}


