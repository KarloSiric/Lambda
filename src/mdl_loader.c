/*
* @Author: karlosiric
* @Date:   2025-07-18 12:28:34
* @Last Modified by:   karlosiric
* @Last Modified time: 2025-07-22 11:04:39
*/

#include "mdl_loader.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int read_data(FILE *file, void *buffer, size_t size, const char *description) {
    size_t bytes_read = fread(buffer, 1, size, file);
    if (bytes_read != size) {
        fprintf(stderr, "Failed to read %s: expected %zu bytes and got %zu bytes\n", 
            description, size, bytes_read);
        return (0);
    }

    return (1);
}

