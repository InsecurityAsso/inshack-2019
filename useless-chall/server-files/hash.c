#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int hash(char* pass) {
    int size = strlen(pass);
    int hashed = 0;
    for (int i = 0; i < size; i++) {
        hashed += (pass[i] << i);
    }

    return hashed;
}

int len_check(char* pass) {
    if (strlen(pass) == 41) {
        return 0;
    }

    return 1;
}

int useless(char* a, char* b) {
    int lena = strlen(a);
    int lenb = strlen(b);
    int k = 150;
    if (lena ^ lenb) {
        for(int i = 0; i < lena; i++) {
label1:
            for (int j = i; j < lenb; j++) {
                if (i == lena + 10) {
                    goto label1;
                }

                k ^= (lena * a[i]) & b[j];
                if (k + i == k + *b) {
                    goto label2;
                }
            }
        }
    }
label2:
    return k;
}

int useless2(char* pass, int v) {
    int sum = 0;
    int lenpass = strlen(pass);
    for (int i = 0; i < lenpass; i++) {
        sum += pass[i];
    }

    return sum * v;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Far Far Far Far Away.\n");
        return 1;
    }

    char* pass = argv[1];

    if (len_check(pass) != 0) {
        printf("Far Far Far Away.\n");
        return 1;
    }

    int expected_hash = 1012589865;
    int check = useless(pass, "What a wonderful world ...");
    if (check*check - 9*check + 56 > -4) {
        if (hash(pass) == expected_hash) {
            int64_t check2 = useless2(pass, check);
            if (((check2*check2 + 1) % 7) != 0) {
                printf("Good job ! Validate with this flag.\n");
                return 0;
            } else {
                printf("Love this chall ?\n");
                return 1;
            }
        } else {
            printf("Far Away, but close.\n");
            return 1;
        }
    } else {
        printf("Far Far Away..\n");
        return 1;
    }

    return 1;
}