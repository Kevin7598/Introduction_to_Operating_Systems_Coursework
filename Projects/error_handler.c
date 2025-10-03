#include "error_handler.h"
#include <stdio.h>

void print_error(ErrorType type, const char *arg) {
    switch (type) {
        case ERR_CMD_NOT_FOUND:
            fprintf(stderr, "%s: command not found\n", arg);
            break;
        case ERR_NO_SUCH_FILE:
            fprintf(stderr, "%s: No such file or directory\n", arg);
            break;
        case ERR_PERMISSION_DENIED:
            fprintf(stderr, "%s: Permission denied\n", arg);
            break;
        case ERR_DUP_IN:
            fprintf(stderr, "error: duplicated input redirection\n");
            break;
        case ERR_DUP_OUT:
            fprintf(stderr, "error: duplicated output redirection\n");
            break;
        case ERR_SYNTAX:
            fprintf(stderr, "syntax error near unexpected token `%s'\n", arg);
            break;
        case ERR_MISSING_PROGRAM:
            fprintf(stderr, "error: missing program\n");
            break;
        case ERR_CD_FAIL:
            fprintf(stderr, "%s: No such file or directory\n", arg);
            break;
        default: break;
    }
}
