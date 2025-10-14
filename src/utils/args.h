#ifndef ARGS_H
#define ARGS_H

#include <stdbool.h>

typedef enum { 
    DUMP_NONE = 0, 
    DUMP_BASIC, 
    DUMP_EXTENDED 
} dump_level_t;

typedef struct {
    const char  *model_path;
    dump_level_t dump_level;
    bool         dump_only;
    bool         quiet;
    bool         show_help;

} app_args_t;

int parse_arguments( int argc, const char *argv[], app_args_t *args );

void print_usage( const char *program_name );

void print_banner(void);

#endif
