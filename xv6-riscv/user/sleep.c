#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("Error, no parameters provided");
    }
    atoi(argv[1]);
    sys_pause(argv[1]);
    exit(0);
}
