/*
 * MDL loader implementation
 * We'll write this together following Valve's approach
 */

#include "mdl_loader.h"
#include "../studio.h"


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



