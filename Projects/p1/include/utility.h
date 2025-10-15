#ifndef INCLUDE_UTILITY_H_
#define INCLUDE_UTILITY_H_

#include <stdbool.h>

/**
* Check if a character is a special character (>, <, or space).
@return true if the character is special, false otherwise.
*/
bool is_special_char(char c);

/** Duplicate a string.
* @param s The string to duplicate.
* @return A pointer to the duplicated string, or NULL if memory allocation fails.
*/
char* my_strdup(const char* s);

/**
* Check for syntax errors in the command line.
* Specifically, it checks for consecutive special characters (< |, << |, or > |) without
* non-special characters in between.
* @param line The command line to check.
* @return true if the syntax is correct, false if there are errors.
*/
bool check_syntax(const char *line);

/**
* Get the next token from the command line and check for syntax errors.
* Specifically, it checks if the next token is valid and not a special character.
* @return The next token if valid, NULL if there is a syntax error.
*/
char *next_token_checked(char **saveptr);

#endif  // INCLUDE_UTILITY_H_
