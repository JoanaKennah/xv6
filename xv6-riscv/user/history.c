#include "kernel/types.h"
#include "user/user.h"

#define HISTORY_FILE ".history"
#define MAX_LINE 128
#define MAX_HISTORY 500
#define O_RDONLY  0x000

int main(int argc, char *argv[]) {
    int fd, n, count = 0;
    char buf[4096];
    char *lines[MAX_HISTORY];
    char *p, *line;
    int num_to_print = -1;

    if (argc  == 2) {
        num_to_print = atoi(argv[1]);
    }
    fd = open(HISTORY_FILE, O_RDONLY);
    if (fd < 0) {
        fprintf(2, "history: cannot open %s\n", HISTORY_FILE);
        exit(1);
    }

    n = read(fd, buf, sizeof(buf)-1);
    close(fd);
    if (n <= 0) {
        printf("No history available\n");
        exit(0);
    }
    buf[n] = '\0';
    p = buf;
    line = p;

    while (*p) {
        if (*p == '\n') {
            *p = '\0';
            if (count < MAX_HISTORY) {
                lines[count++] = line;
            }
            line = p + 1;
        }
        p++;
    }

    int start = 0;
    if (num_to_print > 0 && num_to_print < count) {
        start = count - num_to_print;
    }
    for (int i = start; i < count; i++) {
        printf("%d: %s\n", i+1, lines[i]);
    }
    exit(0);
}