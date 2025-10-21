#include "kernel/types.h"
#include "user/user.h"

int main() {
    //declare two pipes
    int p2c[2];
    int c2p[2];

    // call pipe() system call
    pipe(p2c);
    pipe(c2p);

    //declare one byte for child process
    char our_byte = 'P';

    //fork a child process
    int pid = fork();
    if (pid > 0) {
        // 1. code to send byte to the child proces
        
        close(p2c[0]); // close read end
        write(p2c[1], &our_byte, 1); // load byte into place
        close(p2c[1]); // close write end

        // 5. code to recieve modified byte

        char recieved;
        close(c2p[1]); // close write end
        read(c2p[0], &recieved, 1); // load recieved byte
        close(c2p[0]); // close read end
        printf("\nRecieved ping %c", recieved);

    } else if (pid == 0) {
        // 2. code to read a byte in pipe p2c

        close(p2c[1]); // close write end
        char child_byte; // create child byte
        read(p2c[0], &child_byte, 1); // load new byte
        printf("Recieved ping %c", child_byte);
        close(p2c[0]); // close read end

        // 3. code to modify the byte to letter "R"

        child_byte = 'R'; // define child byte

        // 4. code to send byte back over c2p pipe

        close(c2p[0]); // close read end
        write(c2p[1], &child_byte, 1); // load byte
        close(c2p[1]); // close write end
    }
    exit(0);
}

//use system calls read(), write(), exit(), wait(), close()