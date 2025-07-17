#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H 

#include <stddef.h>

typedef struct {
    float *vertices;
    size_t vertex_count;
} Model;

Model load_obj_file(const char *filepath);
void free_model(Model *model);

#endif
