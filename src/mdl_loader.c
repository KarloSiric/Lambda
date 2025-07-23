/*
* @Author: karlosiric
* @Date:   2025-07-18 12:28:34
* @Last Modified by:   karlosiric
* @Last Modified time: 2025-07-23 10:05:18
*/

#include "mdl_loader.h"
#include <cstdio>
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
    size_t bodyparts_bytes = header->numbodyparts * sizeof(mstudiobodypart_t);
    if (bytes_read != (size_t)header->numbodyparts) {
        fprintf(stderr, "ERROR - Failed to read %d bodyparts, Expected %zu and got %zu bytes.\n",
                header->numbodyparts, bodyparts_bytes, bytes_read);
        free(bodyparts);
        return NULL;
    }

    printf("SUCCESS: Read %d bodyparts, containing %zu bytes.\n",
            header->numbodyparts, bytes_read);
    return bodyparts;
}

mstudiomodel_t *mdl_read_models_for_bodyparts(FILE *file, mstudiobodypart_t *bodypart) {
    // TOOD Karlo: Need to read all given models for that bodypart

    fseek(file, bodypart->modelindex, SEEK_SET);

    mstudiomodel_t *models = malloc(bodypart->nummodels * sizeof(mstudiomodel_t));
    if (!models) {
        fprintf(stderr, "ERROR - Failed to allocate enough space for %d models.\n", bodypart->nummodels);
        return NULL;
    }

    size_t items_read = fread(models, sizeof(mstudiomodel_t), bodypart->nummodels, file);
    if (items_read != (size_t)bodypart->nummodels) {
        
    }



}
