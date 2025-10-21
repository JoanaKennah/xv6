#include "kernel/types.h"
#include "user/user.h"

int main() {
    //declare two pipes
    int p2c[2];
    int c2c[2];

    // call pipe() system call
    pipe(p2c);
    pipe(c2p);

    //declare one byte for child process
    char our_byte = 'P';

    //fork a child process
    int pid = fork();
    if (pid > 0) {
        // 1. code to send byte to the child proces
        
        // 5. code to recieve modified byte

    } else if (pid == 0) {
        // 2. code to read a byte in pipe p2c

        // 3. code to modify the byte to letter "R"

        // 4. code to send byte back over c2p pipe

    }
    exit(0);
}

//use system calls read(), write(), exit(), wait(), close()