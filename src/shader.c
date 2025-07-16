/*
* @Author: karlosiric
* @Date:   2025-07-16 12:30:02
* @Last Modified by:   karlosiric
* @Last Modified time: 2025-07-16 12:35:55
*/


#include "shader.h"
#include <stdio.h>
#include <stdlib.h>

char *read_shader_source(const char *filepath) {

    char *buffer;
    FILE *file = fopen(filepath, "r");
    if (!file) {
        fprintf(stderr, "Failed to open the file: '%s'\n", filepath);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    unsigned long length = ftell(file);

    rewind(file);

    buffer = (char *)malloc()








}
