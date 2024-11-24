#ifndef _UTILITIES_H
#define _UTILITIES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// exit code with error status
extern void bail_with_error(const char * err_msg);

// exit code with non-error status
extern void bail_no_error(const char * msg);

// print to stderr don't exit
extern void error_no_bail(const char * err_msg);

#endif