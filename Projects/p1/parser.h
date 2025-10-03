#ifndef PARSER_H
#define PARSER_H

#define MAXLINE 1024
#define MAXARGS 64

#include <stdbool.h>
#include "error_handler.h"

typedef struct {
  char *argv[MAXARGS];
  char *infile;
  char *outfile;
  bool append;
} stage;

/**
* Preprocess the input line to ensure spaces around special characters.
* @param line The input command line.
* @return A newly allocated string with spaces around special characters.
*         The caller is responsible for freeing the returned string.
*/
char *preprocess(const char *line);

/**
* Handle redirection for input/output files.
* @param target Pointer to the target file string (infile or outfile).
* @param append Pointer to a boolean indicating if appending is needed (for outfile).
* @param err_dup ErrorType to report if there is a duplication error.
* @param set_append Boolean indicating if appending should be set to true.
* @return true if successful, false if there was an error (e.g., duplication).
*/
bool handle_redirection(char **target, bool *append, ErrorType err_dup, bool set_append);

/**
* Tokenize the command line into arguments and handle redirections.
* @param line The input command line.
* @param args Array to store the command arguments.
* @param infile Pointer to store the input file name (if any).
* @param outfile Pointer to store the output file name (if any).
* @param append Pointer to a boolean indicating if appending is needed (for outfile).
* @return true if tokenization is successful, false if there was an error.
*/
bool tokenize(char *line, char **args, char **infile, char **outfile,
              bool *append);

/**
* Parse the command line into stages separated by pipes.
* @param line The input command line.
* @param stages Array to store the parsed stages.
* @return The number of stages parsed.
*/
int parse_pipeline(char *line, stage stages[]);

/**
* Free the memory allocated for stages.
* @param st Array of stages to free.
* @param n Number of stages.
*/
void free_stages(stage *st, int n);

#endif
