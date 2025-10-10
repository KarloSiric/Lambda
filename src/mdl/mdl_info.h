#ifndef MDL_INFO_H
#define MDL_INFO_H 


#include "../studio.h"
#include <stdio.h>

void print_complete_model_analysis(FILE  *output, const char *filename, studiohdr_t *main_h,
                                   studiohdr_t *texture_h, unsigned char *main_data,
                                   unsigned char *texture_data);

#endif
