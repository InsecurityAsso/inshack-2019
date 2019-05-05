#include <stdio.h>
#include <sys/ptrace.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h> // mkostemp
#include <sys/stat.h> // O_WRONLY
#include <unistd.h> // read
#include "vm_code.h"

char** genvi;

void decode_cesar(char* message, int key) {
    char ch;
    int i;
    for(i = 0; message[i] != '\0'; ++i){
        ch = message[i];
        if(ch >= 'a' && ch <= 'z'){
            ch = ch - key;
            if(ch < 'a'){
                ch = ch + 'z' - 'a' + 1;
            }
            message[i] = ch;
        }
        else if(ch >= 'A' && ch <= 'Z'){
            ch = ch - key;
            if(ch < 'A'){
                ch = ch + 'Z' - 'A' + 1;
            }
            message[i] = ch;
        }
    }
}

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
    perror("failed"); 
}

void decrypt_binary(char* decode_key) {
    char buffer[NB_INSTR];
    int idx_key = 0;
    for (int i = 0; i < NB_INSTR; i++) {
        buffer[i] = vm_instrs[i] ^ decode_key[idx_key];

        idx_key++;

        if (idx_key == strlen(decode_key)) {
            idx_key = 0;
        }
    }
    memexec(buffer, NB_INSTR);
}

void load_binary() {
    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    int nb_sec = timeinfo->tm_sec;

    int val_dec = (2*nb_sec + 4) - 2*(nb_sec + 2) + 7;

    // key: IAMTHEMASTEROFYOURSOUL
    // 7 decalé: PHTAOLTHZALYVMFVBYZVBS
    // Inverse: SBVZYBVFMVYLAZHTLOATHP
    char* key = "SBVZYBVFMVYLAZHTLOATHP";
    char decode_key[strlen(key)];
    for (int i = 0; i < strlen(key); i++) {
        decode_key[i] = key[strlen(key) - i - 1];
    }

    decode_cesar(decode_key, val_dec);

    decrypt_binary(decode_key);
}

int main(int argc, char *argv[], char *envp[])
{
    genvi = envp;
    system("killall -q gdb");
    system("killall -q strace");
    system("killall -q ltrace");
    load_binary();

    return 0;
}