/*
* @Author: karlosiric
* @Date:   2025-07-22 11:49:29
* @Last Modified by:   karlosiric
* @Last Modified time: 2025-07-23 09:48:09
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
    }

    mstudiobodypart_t *bodyparts = mdl_read_bodyparts(file, header);
    if (bodyparts) {
        printf("\n=== BODYPARTS INFO ===\n");
        for (int i = 0; i < header->numbodyparts; i++) {
            printf("  Bodypart[%d]: '%s' (%d models)\n",
                i, bodyparts[i].name, bodyparts[i].nummodels);
        }
        free(bodyparts);
    } else {
        printf("FAILED to read bodyparts\n");
    }

    free(header);

    fclose(file);
    return (0);
}


