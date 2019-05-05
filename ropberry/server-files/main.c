#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define ERROR 1
#define SUCCESS 0

void vuln() {
    char buffer[4];
    printf("> Ok, now give me the name of our president.\n");
    fflush(NULL);
    gets(buffer);
}

int main(int argc, char* argv[]) {
    vuln();

    return SUCCESS;
}
