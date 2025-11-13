#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

/* Print the prompt ">>> " and read a line of characters
   from stdin. */
int getcmd(char *buf, int nbuf) {

  // ##### Place your code here
  write(2, ">>>", 4); // prompt user

  memset(buf, 0, nbuf);
  if (gets(buf, nbuf) == 0) { // error catch
    return -1;
  }

  int len = strlen(buf); // to rid of new lines
  if (len > 0 && buf[len-1] == '\n') {
    buf[len-1] = 0;
  }

  int start = 0; // to rid of empty spaces in prompt
  while (buf[start] == ' ' || buf[start] == '\t') start++;
  if (start > 0) {
    memmove(buf, buf + start, strlen(buf + start) + 1);
  }

  return strlen(buf);
}

/*
  A recursive function which parses the command
  at *buf and executes it.
*/
__attribute__((noreturn))
void run_command(char *buf, int nbuf, int *pcp) {

  /* Useful data structures and flags. */
  char *arguments[10];
  int numargs = 0;
  /* Flags to mark word start/end */
  int ws = 1;
  int we = 0;

  /* Flags to mark redirection direction */
  int redirection_left = 0;
  int redirection_right = 0;

  /* File names supplied in the command */
  char *file_name_l = 0;
  char *file_name_r = 0;

  int p[2];
  int pipe_cmd = 0;

  /* Flag to mark sequence command */
  int sequence_cmd = 0;
  char right_cmd[100]; // to hold right side of pipe

  int i = 0;
  /* Parse the command character by character. */
  for (; i < nbuf; i++) {

    /* Parse the current character and set-up various flags:
       sequence_cmd, redirection, pipe_cmd and similar. */

    /* ##### Place your code here. */

    if (buf[i] == ' ') {
      if (!we) {
        buf[i] = 0;
        we = 1;
        ws = 1;
      }
      continue;
    }

    if (buf[i] == ';') {
      buf[1] = 0;
      sequence_cmd = 1;
      break;
    }

    if (buf[i] == '<') {
      buf[i] = 0;
      redirection_left = 1;
      we = 1;
      ws = 1;
      continue;
    }

    if (buf[i] == '>') {
      buf[i] = 0;
      redirection_right = 1;
      we = 1;
      ws = 1;
      continue;
    }

    if (buf[i] == '|') {
      buf[i] = 0;
      pipe_cmd = 1;
      strcpy(right_cmd, &buf[i+1]);
      break;
    }

    if (!(redirection_left || redirection_right)) {
      /* No redirection, continue parsing command. */
      // Place your code here.
      if (buf[i] == ' ') {
        if (!we) {
          buf[i] = 0;
          we = 1;
          ws = 1;
        }
        continue;
      }
      if (ws) {
        arguments[numargs++] = &buf[i];
        ws = 0;
        we = 0;
      }
    } else {
      /* Redirection command. Capture the file names. */
      // ##### Place your code here.
      if (buf[i] != ' ' && buf[i] != '\t') {
        if (redirection_left && file_name_l == 0) {
          file_name_l = &buf[i];
        } else if (redirection_right && file_name_r == 0) {
          file_name_r = &buf[i];
        }

        if (buf[i] == ' ' || buf[i] == '\t') {
          buf[i] = 0;
          ws = 1;
          we = 1;
        }
      }
    }
  }
  /*
    Sequence command. Continue this command in a new process.
    Wait for it to complete and execute the command following ';'.
  */
  if (sequence_cmd) {
    sequence_cmd = 0;
    if (fork() != 0) {
      wait(0);
      // ##### Place your code here.
      // Call run_command recursively
      char *next_cmd = strchr(buf, ';');
      if (next_cmd) {
        *next_cmd = 0;
        if (fork() == 0) {
          run_command(buf, nbuf, pcp);
        }
        wait(0);
        run_command(next_cmd + 1, nbuf, pcp);
      }
    }
  }

  /*
    If this is a redirection command,
    tie the specified files to std in/out.
  */
  if (redirection_left) {
    // ##### Place your code here.
    int fd0 = open(file_name_l, O_RDONLY);
    if (fd0 < 0) {
      fprintf(2, "Cannot open %s for reading\n", file_name_l);
      exit(1);
    }
    close(0);
    dup(fd0);
    close(fd0);
  }
  if (redirection_right) {
    // ##### Place your code here.
    int fd1 = open(file_name_r, O_WRONLY | O_CREATE | O_TRUNC);
    if (fd1 < 0) {
      fprintf(2, "Cannot open %s for writing\n", file_name_r);
      exit(1);
    }
    close(1);
    dup(fd1);
    close(fd1);
  }

  /* Parsing done. Execute the command. */

  /*
    If this command is a CD command, write the arguments to the pcp pipe
    and exit with '2' to tell the parent process about this.
  */
  if (strcmp(arguments[0], "cd") == 0) {
    if (arguments[1] == 0) {
      fprintf(2, "cd has a missing argument\n");
      exit(0);
    } else {
      write(pcp[1], arguments[1], strlen(arguments[1]));
      exit(2); // parent handles cd
    }
  } else {
    /*
      Pipe command: fork twice. Execute the left hand side directly.
      Call run_command recursion for the right side of the pipe.
    */
    if (pipe_cmd) {
      // ##### Place your code here.
      pipe(p);
      if (fork() == 0) { // left side of pipe
        close(1);
        dup(p[1]);
        close(p[0]);
        close(p[1]);
        exec(arguments[0], arguments);
        fprintf(2, "Exec failed for %s\n", arguments[0]);
        exit(1);
      }
      if (fork() == 0) { // right side of pipe
        close(0);
        dup(p[0]);
        close(p[0]);
        close(p[1]);
        run_command(right_cmd, nbuf, pcp);
      }
      close(p[0]);
      close(p[1]);
      wait(0);
      wait(0);
      exit(0);
    } else {
      // ##### Place your code here.
      // Simple command; call exec()
      exec(arguments[0], arguments);
      fprintf(2, "Exec %s failed\n", arguments[0]);
      exit(1);
    }
  }
  exit(0);
}

int main(void) {

  static char buf[100];

  int pcp[2];
  pipe(pcp);

  /* Read and run input commands. */
  while(getcmd(buf, sizeof(buf)) >= 0){
    if (buf[0] == 0) {
      continue;
    }

    // to check for cd input (need to edit later to allow execution of some type)
    // if (strncmp(buf, "cd", 2) == 0 && (buf[2] == ' ' || buf[2] == 0)) {
      // char *path = buf + 2;
      // while (*path == ' ') path++;
      // if (chdir(path) < 0) {
        // fprintf(2, "cd cannot cd%s\n", path);
      // continue;
      // }
    // }
    
    if(fork() == 0)
      run_command(buf, 100, pcp);

    /*
      Check if run_command found this is
      a CD command and run it if required.
    */

    int child_status;
    // ##### Place your code here
    wait(&child_status);

    if (child_status == 2) {
      char path[100];
      int n = read(pcp[0], path, sizeof(path));
      if (n > 0) {
        chdir(path);
      }
    }
  }
  exit(0);
}
