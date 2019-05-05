#include <stdio.h>
#include <sys/ptrace.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h> // mkostemp
#include <sys/stat.h> // O_WRONLY
#include <unistd.h> // read
#include "vm_troll.h"

#include "vm_dir/core_vm.h"

void memexec(void * exe, size_t exe_size)
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

    char* newenviron[] = { NULL };

    char* argv[]={name, NULL};
    fexecve(fd_ro, argv, newenviron);
    perror("ERROR");
}

int main() {
    memexec(vm_troll, NB_TROLL);
    exit(1);
}

void banner() {
    printf("%s\n", "##################################################################################");
    printf("%s\n", "# ooooooooo.             ooooooooo.             oooooo     oooo ooo        ooooo #");
    printf("%s\n", "# `888   `Y88.           `888   `Y88.            `888.     .8'  `88.       .888' #");
    printf("%s\n", "#  888   .d88'  .oooo.    888   .d88'  .oooo.     `888.   .8'    888b     d'888  #");
    printf("%s\n", "#  888ooo88P'  `P  )88b   888ooo88P'  `P  )88b     `888. .8'     8 Y88. .P  888  #");
    printf("%s\n", "#  888          .oP8888   888          .oP8888      `888.8'      8  `888'   888  #");
    printf("%s\n", "#  888         d8(  888   888         d8(  888       `888'       8    Y     888  #");
    printf("%s\n", "# o888o        `Y888888o o888o        `Y888888o       `8'       o8o        o888o #");
    printf("%s\n", "# NO BRUTEFORCE. FLAG IS PRINTABLE. Flag only contains letters, numbers, symbols.#");
    printf("%s\n", "##################################################################################");
    printf("\n");
}

void usage() {
    printf("Usage: \n");
    printf("    help: The help.\n");
    printf("    launch: Launch the service.\n");
    printf("    quit: because you are weak.\n");
}

void help() {
    printf("You really need help ?\n");
}

int justforfun_real_main() {
    system("killall -q python");
    banner();
    while (1) {
        printf("> ");                                                   
        char user_input[255];
        memset(user_input, 255, 0x0);
        fgets(user_input, 244, stdin);

        if (strncmp(user_input, "help", 4) == 0) {
            help();
        } else if (strncmp(user_input, "launch", 6) == 0) { 
            launch_vm();
        } else if (strncmp(user_input, "quit", 4) == 0) {
            exit(0);
        } else {
            usage();
        }
    }
    exit(0);
}