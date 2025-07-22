/*
* @Author: karlosiric
* @Date:   2025-07-18 12:28:34
* @Last Modified by:   karlosiric
* @Last Modified time: 2025-07-22 15:08:41
*/

#include "mdl_loader.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

studiohdr_t *mdl_read_header(FILE *file) {
    rewind(file);

    studiohdr_t *header = (studiohdr_t *)malloc(sizeof(studiohdr_t));
    if (!header) {
        fprintf(stderr, "ERROR - Failed to allocate memory for the header!\n");
        return NULL;
    }

    size_t bytes_read = fread(header, sizeof(studiohdr_t), 1, file);
    if (bytes_read < 1) {
        fprintf(stderr, "ERROR - Failed to read header\n");
        free(header);
        return NULL;
    }

    if (header->id != IDSTUDIOHEADER) {
        fprintf(stderr, "ERROR - Invalid magic number. Expected 0x%08X, got 0x%08X\n",
                IDSTUDIOHEADER, header->id);
        free(header);
        return NULL;
    }

    printf("SUCCESS: Read valid MDL header for '%s'\n",
            header->name);

    return header;
}

mstudiobodypart_t *mdl_read_bodyparts(FILE *file, studiohdr_t *header) {

    fseek(file, header->bodypartindex, SEEK_SET);

    mstudiobodypart_t *bodyparts = (mstudiobodypart_t *)malloc(header->numbodyparts * sizeof(mstudiobodypart_t));

    if (!bodyparts) {
        fprintf(stderr, "ERROR - Failed to allocate enough memory for all bodyparts!\n");
        return NULL; 
    }

    size_t bytes_read = fread(bodyparts, sizeof(mstudiobodypart_t), header->numbodyparts, file);
    size_t bodyparts_bytes = sizeof(header->numbodyparts);
    if (!bytes_read) {
        fprintf(stderr, "ERROR - Failed to read %d bodyparts, Expected %zu and got %zu bytes.\n",
                header->numbodyparts, bodyparts_bytes, bytes_read);
        free(bodyparts);
        return NULL;
    }

    printf("SUCCESS: Read %d bodyparts, containing %zu bytes.\n",
            header->numbodyparts, bytes_read);
    return bodyparts;
}
