#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("Error, no parameters provided");
    }
    int number = atoi(argv[2]);
    pause(number);
    exit(0);
}
