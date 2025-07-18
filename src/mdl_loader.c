/*
* @Author: karlosiric
* @Date:   2025-07-18 12:28:34
* @Last Modified by:   karlosiric
* @Last Modified time: 2025-07-18 13:28:12
*/

#include "mdl_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int read_data(FILE *file, void *buffer, size_t size, const char *description) {
    size_t bytes_read = fread(buffer, 1, size, file);
    if (bytes_read != size) {
        fprintf(stderr, "Failed to read %s: expected %zu bytes and got %zu bytes\n", 
            description, size, bytes_read);
    }
        return (0);

    return (1);
}

void print_mdl_info(const char *filepath) {
    FILE *file = fopen(filepath, "rb");
    if (!file) {
        fprintf(stderr, "Error opening file: '%s'\n", filepath);
        return;
    }

    printf("FILE opened successfully!\n");
    printf("Printing position before reading: %ld\n", ftell(file));

    printf("==== MDL FILE INFORMATION ====\n");
    printf("FILE: '%s'\n", filepath);

    mdl_header_s mdl_header;

    printf("Printing position before reading: %ld\n", ftell(file));

    if (!read_data(file, &mdl_header.id, sizeof(mdl_header.id), "Magic ID")) {
        fclose(file);
        return;
    }

    printf("Printing position after ID: %ld\n", ftell(file));

    if (!read_data(file, &mdl_header.version, sizeof(mdl_header.version), "Version ID")) {
        fclose(file);
        return;
    }

    if (!read_data(file, mdl_header.name, sizeof(mdl_header.name), "Name")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.dataLength, sizeof(mdl_header.dataLength), "Data Length")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.eyeposition, sizeof(mdl_header.eyeposition), "Eye position")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.illumposition, sizeof(mdl_header.illumposition), "Illum position")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.hull_min, sizeof(mdl_header.hull_min), "hull min")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.hull_max, sizeof(mdl_header.hull_max), "hull max")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.view_bbmin, sizeof(mdl_header.view_bbmin), "view bbmin")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.view_bbmax, sizeof(mdl_header.view_bbmax), "view bbmax")) {
        fclose(file);
        return;
    }




    printf("Current position: %ld\n", ftell(file));


    printf("Printing position after NAME: %ld\n", ftell(file));

    printf("MAGIC ID: %.4s\n", (char *)&mdl_header.id);
    printf("VERSION: %d\n", mdl_header.version);
    printf("Name: %s\n", mdl_header.name);
    printf("Data Length: %d bytes\n", mdl_header.dataLength);
    printf("Eye Position: (%.2f, %.2f, %.2f)\n", 
           mdl_header.eyeposition.x, mdl_header.eyeposition.y, mdl_header.eyeposition.z);
    printf("Illum Position: (%.2f, %.2f, %.2f)\n", 
           mdl_header.illumposition.x, mdl_header.illumposition.y, mdl_header.illumposition.z);
    printf("Hull Min: (%.2f, %.2f, %.2f)\n", 
           mdl_header.hull_min.x, mdl_header.hull_min.y, mdl_header.hull_min.z);
    printf("Hull Max: (%.2f, %.2f, %.2f)\n", 
           mdl_header.hull_max.x, mdl_header.hull_max.y, mdl_header.hull_max.z);




    fclose(file);

}
