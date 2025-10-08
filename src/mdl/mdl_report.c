/*======================================================================
 *  File: mdl_report.c
 *  Project: shaders
 *  Author: karlosiric <email@example.com>
 *  Created: 2025-10-08 17:41:20
 *  Last Modified by: karlosiric
 *  Last Modified: 2025-10-08 19:39:54
 *----------------------------------------------------------------------
 *  Description:
 *      
 *----------------------------------------------------------------------
 *  License: 
 *  Company: 
 *  Version: 0.1.0
 *======================================================================
 */


#include "mdl_report.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef _WIN32
    #include <io.h>
    #include <fcntl.h>
    #define DUP _dup
    #define DUP2 _dup2
    #define FILENO _fileno
    #define CLOSE _close
#else 
    #include <unistd.h>
    #define DUP dup
    #define DUP2 dup2
    #define FILENO fileno
    #define CLOSE close
#endif


// This function already prints things for the 
void print_complete_model_analysis(const char *, const studiohdr_t *, const studiohdr_t *, const unsigned char *, const unsigned char *);


#define DUMP_I32(L, V) printf("  %-22s %d\n", (L). (int)(V))
#define DUMP_HEX(L, V) printf("  %-22s 0x%08X\n", (L), (unsigned)(V))
#define DUMP_STR(L, V) printf("  %-22s %s\n", (L), (S))
#define DUMP_V3(L, V) printf("  %-22s (%.3f, %.3f, %.3f)\n", (L), (double)(V)[0], (double)(V)[1], (double)(V)[2])
#define
