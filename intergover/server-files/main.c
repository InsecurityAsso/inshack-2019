#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ./integer 242
void gimmeFlagPliz() {
    system("cat flag.txt");
}

int main(int argc, char* argv[]) {
    printf("Give me one param: ");
    fflush(NULL);
    
    int nb;
    int res = scanf("%d", &nb);
    if (res != 1) {
        printf("I expect a number.\n");
        fflush(NULL);
    }

    char a = 0;

    for (int i = 0; i < nb; i++) {
        a++;
    }

    if (a == -14) {
        gimmeFlagPliz();
    } else {
        printf("No, I can't give you the flag: %d\n", a);
        fflush(NULL);
    }

    return 0;
}