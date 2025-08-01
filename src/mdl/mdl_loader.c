/*
 * MDL loader implementation
 * We'll write this together following Valve's approach
 */

#include "mdl_loader.h"
#include "../studio.h"
#include <stdio.h>


mdl_result_t validate_mdl_magic(int magic) {
    if (magic == IDSTUDIOHEADER) {
        return MDL_SUCCESS;
    } else {
        return MDL_ERROR_INVALID_MAGIC;
    }

}

mdl_result_t validate_mdl_version(int version) {
    if (version == STUDIO_VERSION) {
        return MDL_SUCCESS;
    } else {
        return MDL_ERROR_INVALID_VERSION;
    }

}

mdl_result_t read_mdl_file(const char *filename, unsigned char **file_data, size_t *file_size) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "ERROR - Failed to open the file '%s'. Invalid file name, file not found!\n",
                filename);
        return MDL_ERROR_FILE_NOT_FOUND;
    }





}



