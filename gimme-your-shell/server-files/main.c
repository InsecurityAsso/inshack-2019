#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define ERROR 1
#define SUCCESS 0

void vuln() {
    char buffer[4];
    printf("Ok, now give me the name of our president.\n");
    fflush(NULL);
    gets(buffer);
    printf("Oh I remember !\n");
    fflush(NULL);
}

int main(int argc, char* argv[]) {
    vuln();
    printf("Thanks ! Good Bye :)\n");
    fflush(NULL);
    return 0;
}
