#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include <sys/types.h> 
#include <unistd.h>

void get_flag() {
    printf("Really ?\n");
}

void bof() {
    puts("So you want this option ? ");
    fflush(NULL);

    char canary[512];
    fgets(canary, 512, stdin);
    printf(canary); // <--- Format String.
    fflush(NULL);

    if (strncmp(canary, "Hack_the_system", 15) == 0) {
        int64_t dst;
        scanf(" %ld", &dst);
        printf("DST: %ld\n", dst);
        fflush(NULL);

        int64_t src;
        scanf(" %ld", &src);
        printf("SRC: %ld\n", src);
        fflush(NULL);

        int64_t* ptr = dst;
        *ptr = src;

        char buf[10];
        fgets(buf, 10, stdin);
    }
}

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        printf("Hi everyone %s!\n", "all");
        fflush(NULL);

        while (1) {
            bof();

            puts("Do you want to relaunch the program? [y/n] ");
            fflush(NULL);
            
            char tmp[10];
            fgets(tmp, 8, stdin);

            if (tmp[0] == 'n') {
                break;
            }
        }
    } else {
        waitpid(0,0,0);
    }

}