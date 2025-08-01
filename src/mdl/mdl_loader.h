/*
 * MDL file loader using official Valve structures
 * Handles parsing and validation of .mdl files
 */

#ifndef MDL_LOADER_H
#define MDL_LOADER_H

#include "../studio.h"
#include <stdio.h>
#include <stddef.h>

typedef enum
{
    MDL_SUCCESS = 0,
    MDL_ERROR_FILE_NOT_FOUND,
    MDL_ERROR_INVALID_MAGIC,
    MDL_ERROR_INVALID_VERSION,
    MDL_ERROR_FILE_TOO_SMALL
} mdl_result_t;



mdl_result_t validate_mdl_magic(int magic);
mdl_result_t validate_mdl_version(int version);
mdl_result_t read_mdl_file(const char *filename, unsigned char **file_data, size_t *file_size);


#endif // MDL_LOADER_H
