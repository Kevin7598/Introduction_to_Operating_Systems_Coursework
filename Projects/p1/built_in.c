#include "built_in.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "error_handler.h"

#define PATHS_MAX 4096

void built_in_cd(char **args) {
  // No argument, go to HOME.
  if (!args[1]) {
    const char *home = getenv("HOME");
    if (!home) home = "/";
    if (chdir(home) != 0) perror("cd");
    return;
  }
  // Too many arguments, print out error.
  if (args[2]) {
    fprintf(stderr, "cd: too many arguments\n");
    return;
  }
  // otherwise change to the specified directory.
  if (chdir(args[1]) != 0) print_error(ERR_CD_FAIL, args[1]);
}

void built_in_pwd(void) {
  char cwd[PATHS_MAX];
  // Get current working directory and print it.
  if (getcwd(cwd, sizeof(cwd))) {
    printf("%s\n", cwd);
    return;
  }
  perror("pwd");
}
