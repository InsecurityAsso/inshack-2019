#include <stdio.h>
#include <sys/ptrace.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h> // mkostemp
#include <sys/stat.h> // O_WRONLY
#include <unistd.h> // read
#include "errno.h"
#include "signal.h"
#include "vm_split.h"

char** genvi;

int memexec(void * exe, size_t exe_size)
{
    /* random temporary file name in /tmp */
    char name[15] = "/tmp/fooXXXXXX"; 
    /* creates temporary file, returns writeable file descriptor */
    int fd_wr = mkostemp(name,  O_WRONLY);
    /* makes file executable and readonly */
    chmod(name, S_IRUSR | S_IXUSR);
    /* creates read-only file descriptor before deleting the file */
    int fd_ro = open(name, O_RDONLY);
    /* removes file from file system, kernel buffers content in memory until all fd closed */
    unlink(name);
    /* writes executable to file */
    write(fd_wr, exe, exe_size);
    /* fexecve will not work as long as there in a open writeable file descriptor */
    close(fd_wr);
    /* -fpermissive */

    char* argv[]={name, NULL};
    fexecve(fd_ro, argv, genvi);
    perror("ERROR");
}

void load_binary() {
    char buffer[NB_SPLIT1+NB_SPLIT2];
    int ko = 0;
    int ko2 = 0;

    int idx = 0;
    int idx_buf = 0;
    while(!((ko == 1) && (ko2 == 1))) {
        if (idx < NB_SPLIT1) {
            buffer[idx_buf] = vm_split1[idx];
            idx_buf++;
        } else {
            ko = 1;
        }

        if (idx < NB_SPLIT1) {
            buffer[idx_buf] = vm_split2[idx];
            idx_buf++;
        } else {
            ko2 = 1;
        }

        idx++;
    }

    memexec(buffer, NB_SPLIT1+NB_SPLIT2);
}

void real_main(int signo) {
    load_binary();
}

int main(int argc, char *argv[], char *envp[])
{
    genvi = envp;
    signal(SIGTRAP, real_main);
    __asm__("int3");
    return 1;
    return 0;
}