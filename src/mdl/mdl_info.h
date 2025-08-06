#ifndef MDL_INFO_H
#define MDL_INFO_H 


#include "../studio.h"

void print_complete_model_analysis(const char *filename, studiohdr_t *main_header,
                                   studiohdr_t *texture_header, unsigned char *main_data,
                                   unsigned char *texture_data);

#endif
