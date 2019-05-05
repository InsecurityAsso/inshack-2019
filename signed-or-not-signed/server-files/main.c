#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void gimmeFlagPliz() {
    system("cat flag.txt");
}

void vuln(unsigned short nb) {
    if (nb == 64870) {
        gimmeFlagPliz();
    } else {
        printf("You are not going to have the flag.\n");
        fflush(NULL);
    }
}

int main(int argc, char* argv[]) {
    int nb;
    printf("Please give me a number:");
    fflush(NULL);
    int res = scanf("%d", &nb);
    if (res != 1) {
        printf("I expect a number.\n");
        fflush(NULL);
        exit(1);
    }

    if (nb > 10) {
        printf("Bro, it's really too big.\n");
        fflush(NULL);
        return 1;
    }

    vuln(nb);
    return 0;
}