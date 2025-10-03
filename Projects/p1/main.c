#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include "built_in.h"
#include "parser.h"
#include "error_handler.h"

#define MAXLINE 1024
#define MAXARGS 64
#define MAXSTAGES 64
#define MAX_PIPE_FDS 128

// Signal handler for SIGINT (Ctrl+C)
void sigint_handler(int sig) {
  errno = 1;
  ssize_t unused = write(STDOUT_FILENO, "\nmumsh $ ", 9);
  (void)unused;
  (void)sig;
}

void execute_pipeline(stage *stages, int num_stage) {
  // If there are no stages, just return.
  if (num_stage <= 0) return;

  // Handle built-in commands if there's only one stage.
  if (num_stage == 1 && stages[0].argv[0] &&
      strcmp(stages[0].argv[0], "cd") == 0) {
    built_in_cd(stages[0].argv);
    return;
  }

  int pipefds[MAX_PIPE_FDS];
  // Create pipes for inter-process communication.
  // Each pipe requires two file descriptors.
  for (int i = 0; i < num_stage - 1; i++) pipe(pipefds + 2 * i);

  // Fork processes for each stage in the pipeline.
  for (int i = 0; i < num_stage; i++) {
    pid_t pid = fork();

    // Child process
    if (pid == 0) {
      // Restore default signal handling for SIGINT in child processes.
      signal(SIGINT, SIG_DFL);

      // Set up input/output redirection for the stage.
      // If it's not the first stage, set stdin to the read end of the previous pipe.
      // If it's not the last stage, set stdout to the write end of the current pipe.
      // Also handle file redirections if specified.
      if (i > 0) dup2(pipefds[2 * (i - 1)], STDIN_FILENO);
      if (stages[i].infile) {
        int fd = open(stages[i].infile, O_RDONLY);
        if (fd < 0) {
          if (errno == ENOENT) print_error(ERR_NO_SUCH_FILE, stages[i].infile);
          else perror("open infile");
          exit(1);
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
      }
      if (i < num_stage - 1) dup2(pipefds[2 * i + 1], STDOUT_FILENO);
      if (stages[i].outfile) {
        int flags =
            O_WRONLY | O_CREAT | (stages[i].append ? O_APPEND : O_TRUNC);
        int fd = open(stages[i].outfile, flags, 0644);
        if (fd < 0) {
          print_error(ERR_PERMISSION_DENIED, stages[i].outfile);
          exit(1);
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
      }

      for (int j = 0; j < 2 * (num_stage - 1); j++) close(pipefds[j]);

      // If it's a built-in command like "pwd", handle it directly.
      if (stages[i].argv[0] && strcmp(stages[i].argv[0], "pwd") == 0) {
        built_in_pwd();
        exit(0);
      }

      // Use execvp to run the command
      execvp(stages[i].argv[0], stages[i].argv);
      print_error(ERR_CMD_NOT_FOUND, stages[i].argv[0]);
      exit(1);
    }
  }

  for (int j = 0; j < 2 * (num_stage - 1); j++) close(pipefds[j]);
  for (int i = 0; i < num_stage; i++) wait(NULL);
}

void run_command(char *line) {
  stage stages[MAXSTAGES];

  // Initialize stages.
  for (int i = 0; i < MAXSTAGES; i++) {
    for (int j = 0; j < MAXARGS; j++) stages[i].argv[j] = NULL;
    stages[i].infile = NULL;
    stages[i].outfile = NULL;
    stages[i].append = false;
  }
  int nstages = parse_pipeline(line, stages);
  if (nstages == 0) return;

  // Handle "exit" command if it's a single stage.
  if (nstages == 1 && stages[0].argv[0] &&
      strcmp(stages[0].argv[0], "exit") == 0) {
    printf("exit\n");
    free_stages(stages, nstages);
    exit(0);
  }

  execute_pipeline(stages, nstages);
  free_stages(stages, nstages);
}

int main(void) {
  errno = 0;
  signal(SIGINT, sigint_handler);

  char line[MAXLINE];

  while (1) {
    // If interrupted by SIGINT, reset errno and continue.
    // Otherwise, print the shell prompt.
    if (errno == 1) {
        errno = 0;
    } else {
      printf("mumsh $ ");
      fflush(stdout);
    }

    // Read a line from standard input.
    if (!fgets(line, MAXLINE, stdin)) {
      // Handle EOF (Ctrl+D) or error.
      // If EOF, print "exit" and terminate the shell.
      // If error, clear the line and continue.
      // Otherwise, continue to the next iteration.
      if (feof(stdin)) {
        printf("exit\n");
        break;
      } else if (errno == EINTR) {
        continue;
      } else {
        line[0] = '\0';
        printf("mumsh $ ");
        fflush(stdout);
        continue;
    }
    }
    // If the line is empty or just a newline, continue to the next iteration.
    // Otherwise, process the command line.
    if (strlen(line) == 0 || strcmp(line, "\n") == 0) continue;
    run_command(line);
  }
  return 0;
}
