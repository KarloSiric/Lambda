#include "mdl_loader.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#define DUP    _dup
#define DUP2   _dup2
#define FILENO _fileno
#else
#include <unistd.h>
#define DUP    dup
#define DUP2   dup2
#define FILENO fileno
#endif

#include "../studio.h"

void print_complete_model_analysis(
    FILE *output,
    const char *filename,
    studiohdr_t *main_header,
    studiohdr_t *texture_header,
    unsigned char *main_data,
    unsigned char *texture_data);

void print_studio_header_file(FILE *output, const char *title, const studiohdr_t *header);

// Redirect stdout to a file while calling your existing analysis function.
// Returns 0 on success.
inline int mdl_write_report_via_stdout_redirect(
    const char          *out_path,
    const char          *model_path,
    const studiohdr_t   *hdr,
    const studiohdr_t   *thdr,
    const unsigned char *data,
    const unsigned char *tdata )
{
    if ( !out_path || !hdr )
        return -1;

    FILE *fp = fopen( out_path, "wb" );
    if ( !fp )
        return -2;

    // Save current stdout FD
    int old_fd = DUP( FILENO( stdout ) );
    if ( old_fd < 0 )
    {
        fclose( fp );
        return -3;
    }

    // Flush stdout before hijacking
    fflush( stdout );

    // Point stdout to our file
    if ( DUP2( FILENO( fp ), FILENO( stdout ) ) < 0 )
    {
        fclose( fp );
#ifdef _WIN32
        _close( old_fd );
#else
        close( old_fd );
#endif
        return -4;
    }

    // Call your existing function — write directly to file
    print_complete_model_analysis( fp, model_path, hdr, thdr, data, tdata );

    // Flush and restore stdout
    fflush( stdout );
    DUP2( old_fd, FILENO( stdout ) );
#ifdef _WIN32
    _close( old_fd );
#else
    close( old_fd );
#endif

    fclose( fp );
    return 0;
}


