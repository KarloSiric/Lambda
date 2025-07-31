#ifndef MDL_REVERSE_ENGINEER_H
#define MDL_REVERSE_ENGINEER_H




#include <stddef.h>

typedef struct {
    char magic[4];
    int version;
    char name[64];
    // Need to add more later as we find out more


} mdl_header_s;


typedef struct {
    float *vertices;
    size_t vertex_count;
    // Need to add more later as we find out more



} mdl_model_s;





// prototype functions
void reverse_engineer_header(const char *filepath);
void reverse_engineer_ascii(const char *filepath);
mdl_model_s load_mdl_file_re(const char *filepath);
void free_mdl_model(mdl_model_s *model);




#endif
