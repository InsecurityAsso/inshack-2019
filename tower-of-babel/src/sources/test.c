#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef struct buf {
    int size;
    char *data;
} buf_t;

static const int gsesame_sz=10;
static const buf_t gsesame[10]={
    {.size=25, .data="i would like the flag...\n"},
    {.size=16, .data="onegaishimasu ?\n"},
    {.size=10, .data="olu'olu ?\n"},
    {.size=12, .data="por favor ?\n"},
    {.size=14, .data="alsjeblieft ?\n"},
    {.size=14, .data="per piecere ?\n"},
    {.size=18, .data="s'il vous plait ?\n"},
    {.size=8, .data="bitte ?\n"},
    {.size=12, .data="vengligst ?\n"},
    {.size=15, .data="please H4x0r !\n"}
};

#define BUF_SZ 1024

int main(int argc, char *argv[])
{
    int i, fd;
    char buffer[BUF_SZ]={0};
    if((fd=open("/dev/tob", O_RDWR))<0) {
        printf("failed to open 'tob' device.\n");
        return 1;
    }
    for(i=0;i<gsesame_sz;i++) {
        printf("writing: %s\n", gsesame[i].data);
        write(fd, gsesame[i].data, gsesame[i].size);
    }
    read(fd, buffer, BUF_SZ-1);
    printf("read from device: %s\n", buffer);
    return 0;
}
