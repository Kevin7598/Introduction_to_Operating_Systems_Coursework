#define _GNU_SOURCE
#include "parser.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utility.h"
#include "error_handler.h"

char *preprocess(const char *line) {
  char* processed_line = malloc(sizeof(char) * (2 * strlen(line) + 1));
  if (!processed_line) {
    perror("malloc failed");
    exit(1);
  }
  unsigned int pos = 0;
  for (unsigned int i = 0; i < strlen(line); i++) {
    if (line[i] == '>' || line[i] == '<') {
      if (i == 0) {
        if (!is_special_char(line[i + 1])) {
          processed_line[pos++] = line[i];
          processed_line[pos] = ' ';
        } else {
          processed_line[pos] = line[i];
        }
      } else if (i == strlen(line) - 1) {
        if (!is_special_char(line[i - 1])) {
          processed_line[pos++] = ' ';
          processed_line[pos] = line[i];
        } else {
          processed_line[pos] = line[i];
        }
      } else if (!is_special_char(line[i - 1]) || !is_special_char(line[i + 1])) {
        if (!is_special_char(line[i - 1]) && !is_special_char(line[i + 1])) {
          processed_line[pos++] = ' ';
          processed_line[pos++] = line[i];
          processed_line[pos] = ' ';
        } else if (!is_special_char(line[i - 1])) {
          processed_line[pos++] = ' ';
          processed_line[pos] = line[i];
        } else if (!is_special_char(line[i + 1])) {
          processed_line[pos++] = line[i];
          processed_line[pos] = ' ';
        }
      } else {
        processed_line[pos] = line[i];
      }
    } else {
      processed_line[pos] = line[i];
    }
    pos++;
  }
  processed_line[pos] = '\0';
  return processed_line;
}

bool handle_redirection(char **target, bool *append, ErrorType err_dup, bool set_append) {
  // If target is already set, it's a duplicate redirection.
  if (*target) {
    print_error(err_dup, NULL);
    return false;
  }

  // Get the next token and check if it's valid.
  char *next = next_token_checked();
  if (!next) return false;

  // Set the target and append flag.
  *target = my_strdup(next);
  if (append) *append = set_append;
  return true;
}

bool tokenize(char *line, char **args, char **infile, char **outfile,
              bool *append) {
  int i = 0;

  // Tokenize the line using whitespace as delimiters.
  char *token = strtok(line, " \t\n");

  // Process each kind of token.
  // If it's a redirection, handle it accordingly.
  // If there is an error, return false.
  while (token) {
    if (strcmp(token, ">") == 0) {
      if (!handle_redirection(outfile, NULL, ERR_DUP_OUT, false)) return false;
    } else if (strcmp(token, ">>") == 0) {
      if (!handle_redirection(outfile, append, ERR_DUP_OUT, true)) return false;
    } else if (strcmp(token, "<") == 0) {
      if (!handle_redirection(infile, NULL, ERR_DUP_IN, false)) return false;
    } else {
      args[i++] = my_strdup(token);
    }
    token = strtok(NULL, " \t\n");
  }

  // If no command is found, it's an error.
  if (i == 0) {
    print_error(ERR_MISSING_PROGRAM, NULL);
    return false;
  }

  // Null-terminate the args array.
  args[i] = NULL;
  return true;
}

int parse_pipeline(char *line, stage stages[]) {
  char *proc = preprocess(line);

  // First check for syntax errors with pipes.
  if (!check_syntax(proc)) {
    print_error(ERR_SYNTAX, "|");
    free(proc);
    return 0;
  }
  int count = 0;
  char *saveptr = NULL;

  // Split the line into parts using '|' as the delimiter.
  char *part = strtok_r(proc, "|", &saveptr);
  while (part && count < 64) {
    // Check if the part is empty or only contains spaces
    char *trimmed = part;
    while (*trimmed == ' ' || *trimmed == '\t') trimmed++;
    if (strlen(trimmed) == 0) {
      print_error(ERR_MISSING_PROGRAM, NULL);
      free(proc);
      return 0;
    }

    // Tokenize each part into arguments and handle redirections.
    // If there's an error, free the allocated memory and return 0.
    if (!tokenize(part, stages[count].argv, &stages[count].infile,
             &stages[count].outfile, &stages[count].append)) {
      free(proc);
      return 0;
    }

    // Check for duplicate input/output redirections across stages.
    if (count > 0 && stages[count].infile) {
      print_error(ERR_DUP_IN, NULL);
      free(proc);
      return 0;
    }
    if (saveptr && *saveptr != '\0' && stages[count].outfile) {
      print_error(ERR_DUP_OUT, NULL);
      free(proc);
      return 0;
    }

    // Go to the next part.
    count++;
    part = strtok_r(NULL, "|", &saveptr);
  }

  // If the last character is a pipe, it's an error.
  if (line[strlen(line)-1] == '|') {
    print_error(ERR_MISSING_PROGRAM, NULL);
    free(proc);
    return 0;
  }

  free(proc);
  return count;
}

void free_stages(stage *st, int n) {
  for (int i = 0; i < n; i++) {
    for (int j = 0; st[i].argv[j]; j++) free(st[i].argv[j]);
    if (st[i].infile) free(st[i].infile);
    if (st[i].outfile) free(st[i].outfile);
  }
}
