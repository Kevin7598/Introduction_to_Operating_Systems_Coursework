#include "utility.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "error_handler.h"

bool is_special_char(char c) { return (c == '>' || c == '<' || c == ' '); }

char* my_strdup(const char* s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char* copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

bool check_syntax(const char *line) {
    bool last_was_pipe = false;
    // Go through each character in the line, and first detect special characters
    // and then check for syntax errors with pipes
    for (unsigned int i = 0; i < strlen(line); i++) {
        if (is_special_char(line[i]) && line[i] != ' ') {
            last_was_pipe = true;
        } else if (line[i] == '|' && last_was_pipe) {
            return false;
        } else if (line[i] != ' ' && line[i] != '\t' && line[i] != '\n') {
            last_was_pipe = false;
        }
    }
    return true;
}

char *next_token_checked(void) {
    char *next = strtok(NULL, " \t\n");
    if (!next || strcmp(next, ">") == 0 || strcmp(next, ">>") == 0 ||
        strcmp(next, "<") == 0 || strcmp(next, "|") == 0) {
        print_error(ERR_SYNTAX, next ? next : "newline");
        return NULL;
    }
    return next;
}
