#define _GNU_SOURCE
#include "../include/parser.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/utility.h"
#include "../include/error_handler.h"

char *preprocess(const char *line) {
  char* processed_line = malloc(sizeof(char) * (2 * strlen(line) + 1));
  if (!processed_line) { 
    perror("malloc"); 
    exit(1); 
  }
  memset(processed_line, 0, 2 * strlen(line) + 1);
  if (!processed_line) {
    perror("malloc failed");
    exit(1);
  }
  unsigned int pos = 0;
  size_t len = strlen(line);
  for (unsigned int i = 0; i < len; i++) {
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


bool handle_redirection(char **target, bool *append, ErrorType err_dup, bool set_append, char **saveptr) {
  // If target is already set, it's a duplicate redirection.
  if (*target) {
    print_error(err_dup, NULL);
    return false;
  }

  // Get the next token and check if it's valid.
  char *next = next_token_checked(saveptr);
  if (!next) return false;

  // Set the target and append flag.
  *target = my_strdup(next);
  if (append) *append = set_append;
  return true;
}

bool tokenize(char *line, char **args, char **infile, char **outfile,
              bool *append) {
  if (!line || !args) return false;

  if (infile) *infile = NULL;
  if (outfile) *outfile = NULL;
  if (append) *append = false;
  int i = 0;
  char *saveptr = NULL;
  char *token = strtok_r(line, " \t\n", &saveptr);

  while (token) {
    if (strcmp(token, ">") == 0) {
      if (!handle_redirection(outfile, NULL, ERR_DUP_OUT, false, &saveptr)) {
        for (int k = 0; k < i; k++) { free(args[k]); args[k] = NULL; }
        return false;
      }
    } else if (strcmp(token, ">>") == 0) {
      if (!handle_redirection(outfile, append, ERR_DUP_OUT, true, &saveptr)) {
        for (int k = 0; k < i; k++) { free(args[k]); args[k] = NULL; }
        return false;
      }
    } else if (strcmp(token, "<") == 0) {
      if (!handle_redirection(infile, NULL, ERR_DUP_IN, false, &saveptr)) {
        for (int k = 0; k < i; k++) { free(args[k]); args[k] = NULL; }
        return false;
      }
    } else {
      args[i++] = my_strdup(token);
      if (!args[i-1]) {
        for (int k = 0; k < i-1; k++) { 
          free(args[k]); 
          args[k] = NULL; 
        }
        return false;
      }
    }
    token = strtok_r(NULL, " \t\n", &saveptr);
  }

  if (i == 0) {
    print_error(ERR_MISSING_PROGRAM, NULL);
    return false;
  }

  args[i] = NULL;
  return true;
}

int parse_pipeline(char *line, stage stages[]) {
  if (!line || !stages) return 0;
  char *proc = preprocess(line);
  if (strlen(proc) == 0) {
    free(proc);
    return 0;
  }
  if (!proc) return 0;

  if (!check_syntax(proc)) {
    print_error(ERR_SYNTAX, "|");
    free(proc);
    return 0;
  }

  int count = 0;
  char *saveptr = NULL;
  char *part = strtok_r(proc, "|", &saveptr);

  while (part && count < 128) {
    /* trim leading whitespace */
    char *trimmed = part;
    while (*trimmed == ' ' || *trimmed == '\t') trimmed++;
    if (strlen(trimmed) == 0) {
      print_error(ERR_MISSING_PROGRAM, NULL);
      free(proc);
      for (int t = 0; t < count; t++) free_stages(&stages[t], 1);
      return 0;
    }

    /* tokenize into stages[count] */
    if (!tokenize(part, stages[count].argv, &stages[count].infile,
                  &stages[count].outfile, &stages[count].append)) {
      free(proc);
      for (int t = 0; t < count; t++) free_stages(&stages[t], 1);
      return 0;
    }

    /* duplicate redirection checks */
    if (count > 0 && stages[count].infile) {
      print_error(ERR_DUP_IN, NULL);
      free(proc);
      for (int t = 0; t <= count; t++) free_stages(&stages[t], 1);
      return 0;
    }
    if (saveptr && *saveptr != '\0' && stages[count].outfile) {
      print_error(ERR_DUP_OUT, NULL);
      free(proc);
      for (int t = 0; t <= count; t++) free_stages(&stages[t], 1);
      return 0;
    }

    count++;
    part = strtok_r(NULL, "|", &saveptr);
  }

  if (line[strlen(line) - 1] == '|') {
    print_error(ERR_MISSING_PROGRAM, NULL);
    free(proc);
    for (int t = 0; t < count; t++) free_stages(&stages[t], 1);
    return 0;
  }

  free(proc);
  return count;
}

void free_stages(stage *st, int n) {
  if (!st) return;
  
  for (int i = 0; i < n; i++) {
    for (int j = 0; st[i].argv[j]; j++) {
      free(st[i].argv[j]);
      st[i].argv[j] = NULL;
    }
    if (st[i].infile) {
      free(st[i].infile);
      st[i].infile = NULL;
    }
    if (st[i].outfile) {
      free(st[i].outfile);
      st[i].outfile = NULL;
    }
    st[i].append = false;
  }
}
