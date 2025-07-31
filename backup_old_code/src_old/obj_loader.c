/*
* @Author: karlosiric
* @Date:   2025-07-17 10:35:50
* @Last Modified by:   karlosiric
* @Last Modified time: 2025-07-17 12:23:46
*/

#include "obj_loader.h"
#include "shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Model load_obj_file(const char *filepath) {
    FILE *file = fopen(filepath, "r");

    if (!file) {
        fprintf(stderr, "Failed to open the OBJ file!\n");
        return (Model) {NULL, 0};
    }

    float *vertex_buffer = NULL;
    size_t capacity = 0;
    size_t count = 0;

    char line[128];
    while(fgets(line, sizeof(line), file)) {
        if (line[0] == 'v' && line[1] == ' ') {
            float x,y,z;
            if (sscanf(line, "v %f %f %f", &x, &y, &z) == 3) {
                if (count + 3 > capacity) {
                    capacity = (capacity == 0) ? 9 : capacity * 2;
                    vertex_buffer = realloc(vertex_buffer, capacity * sizeof(float));
                }

                vertex_buffer[count++] = x;
                vertex_buffer[count++] = y;
                vertex_buffer[count++] = z;
            }
        }
    }

    fclose(file);

    Model model;
    model.vertices = vertex_buffer;
    model.vertex_count = count;

    return model;
}

void free_model(Model *model) {
    if (model->vertices) {
        free(model->vertices);
        model->vertices = NULL;
        model->vertex_count = 0;
    }
}




