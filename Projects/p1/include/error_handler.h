#ifndef INCLUDE_ERROR_HANDLER_H_
#define INCLUDE_ERROR_HANDLER_H_

#include <stdbool.h>

typedef enum {
    ERR_NONE,
    ERR_CMD_NOT_FOUND,
    ERR_NO_SUCH_FILE,
    ERR_PERMISSION_DENIED,
    ERR_DUP_IN,
    ERR_DUP_OUT,
    ERR_SYNTAX,
    ERR_MISSING_PROGRAM,
    ERR_CD_FAIL
} ErrorType;

/**
* Print an error message based on the error type and an argument.
* @param type The type of error.
* @param arg An argument to include in the error message.
*/
void print_error(ErrorType type, const char *arg);

#endif  // INCLUDE_ERROR_HANDLER_H_
