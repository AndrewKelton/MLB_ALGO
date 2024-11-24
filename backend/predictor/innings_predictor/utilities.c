#include "utilities.h"

// exit code with error status
extern void bail_with_error(const char * err_msg)
{
    fprintf(stderr, "Error: %s\n", err_msg);
    exit(EXIT_FAILURE);
}

// exit code with non-error status
extern void bail_no_error(const char * err_msg) 
{
    fprintf(stderr, "Error: %s\n", err_msg);
    exit(EXIT_SUCCESS);
}

extern void error_no_bail(const char * err_msg)
{
    fprintf(stderr, "Error: %s\n", err_msg);
}