#ifndef CORE_APP_INIT_H
#define CORE_APP_INIT_H 


#include "utils/args.h"
#include "mdl/mdl_loader.h"

int app_init_logger(const app_args_t *args);


int app_load_model(const char *model_path, mdl_model_t **model_out);


int app_init_renderer(int width, int height, const char *title);


#endif
