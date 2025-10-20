#ifndef CORE_APP_H
#define CORE_APP_H 


#include "../utils/args.h"

typedef struct {
    bool running;
    bool initialized;
} app_state_t;


int app_run(const app_state_t *args);

int app_init(const app_state_t *args);

void app_shutdown();




#endif
