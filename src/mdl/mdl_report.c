/*======================================================================
 *  File: mdl_report.c
 *  Project: shaders
 *  Author: karlosiric <email@example.com>
 *  Created: 2025-10-08 17:41:20
 *  Last Modified by: karlosiric
 *  Last Modified: 2025-10-08 19:30:08
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



