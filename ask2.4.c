#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char **argv) {

    pid_t p;

    p= fork();

    if (p < 0) {
        perror("fork child failed\n");
        return 1;
    }

    else if (p == 0) {

        char *new_argv[] = {"./mya1.1", argv[1], argv[2], argv[3], NULL};

        // Replace child process with a1 program
        if (execv("./mya1.1", new_argv) == -1) {
            perror("execv failed");
            exit(1);
        }
    }

    else {
         // Parent process
        // Wait for the child to terminate
        wait(NULL);
        printf("Child has finished\n");
     }

    return 0;
}
