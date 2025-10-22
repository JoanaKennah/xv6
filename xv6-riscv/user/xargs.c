#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

// the command specified as an argument to xargs is run as many times
// as there are lines in the standard input,
// each time with different data that is contained in a particular line

void execute_line(char *line, char *args[], int skip) {
    int f = fork();
    if (f < 0) {
        fprintf(2, "xargs: fork failed\n");
        exit(1);
    } else if(f == 0) { // child
        // int ws = 1;
        char **a, *l;

        // Args contains an array of pointers to the strings
        // given as input arguments to args (the command).
        // We go through a line of standard input and append to
        // args pointers to separate words in the line.
        a = &args[skip];
        l = line;
        while (*l != '\0') {
            // PLACE YOUR CODE HERE
            while (*l == ' ' || *l == '\t') { // skip spaces
                l++;
            }
            if (*l == '\0') {
                break;
            }
            *a = l; // start of the word read
            a++;
            while (*l != '\0' && *l != ' ' && *l != '\t') { // move along until space/end
                l++;
            }
            if (*l != '\0') { // terminate the word
                *l = '\0';
                l++;
            }
        }
        *a = 0; // terminate the args array for later

        // Call exec() by passing the modified
        // args array to it.
        // PLACE YOUR CODE HERE
        exec(args[0], args);
        fprintf(2, "xargs: exec failed\n"); // only if exec() fails
        exit(1);
    } else { // parent
        wait((int *) 0);
    }
}

int main(int argc, char *argv[]) {
    char *args[MAXARG] = { 0 };
    char buf[512], *b;
    b = buf;
    for (int i = 1; i < argc; i++) {
        args[i - 1] = argv[i];
    }

    // Read each line of stdin into the buffer and execute
    // the command given as args. This routine replaces
    // newlines with nulls to terminate the string.
    while (read(0, b, 1) > 0) {
        if (*b == '\n') {
            // Terminate the string with ’\0’ and call
            // execute_line.
            // PLACE YOUR CODE HERE
            *b = '\0'; // terminates line
            execute_line(buf, args, argc - 1); // executes line
            b = buf; // reset the buffer for next line in read
        } else {
            b++;
        }
    }
    exit(0);
}