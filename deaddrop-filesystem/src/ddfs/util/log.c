#include "log.h"
#include "macro.h"

#include <libgen.h>
#include <stdlib.h>
#include <string.h>
/*
 *
 */
static void _log_print_start(const char *file,
                             int line,
                             const char *func,
                             const char *color,
                             const char *lvl)
{
    char *lfile=NULL, *bname;
#ifdef COLORIZE
    fprintf(stderr, "\x1b[%s;1m", color);
#endif
    lfile=strdup(file);
    bname=basename(lfile);
    fprintf(stderr, "[%s](%04d|%s|%s): ", lvl, line, bname, func);
    free(lfile);
}
/*
 *
 */
static void _log_print_end(void)
{
#ifdef COLORIZE
    fprintf(stderr, "\x1b[0m");
#endif
    fprintf(stderr, "\n");
}
/*
 *
 */
void log_print(const char *file,
               int line,
               const char *func,
               const char *color,
               const char *lvl,
               const char *fmt,
               ...)
{
    va_list args;
    /**/
    va_start(args, fmt);
    _log_print_start(file, line, func, color, lvl);
    vfprintf(stderr, fmt, args);
    _log_print_end();
    va_end(args);
}

static void _log_print_prepare_hd_line(const unsigned char *buf,
                                       size_t remaining,
                                       char *lbuf)
{
    char current;
    int rsz, k, hex_oft, asc_oft;
    /**/
    memset(lbuf, ' ', HD_LSZ);
    rsz=MIN(remaining, HD_BCNT);
    /* fill line with bytes and chars */
    for(k=0;k<rsz;k++) {
        current=buf[k];
        hex_oft=(2*k)+(k/HD_WIDTH);
        asc_oft=(HD_LSZ-HD_BCNT-1)+k;
        sprintf(lbuf+hex_oft, "%02x", current&0xff);
        sprintf(lbuf+asc_oft, "%c", PRINTABLE(current));
    }
    /* replace all null bytes with blankspace except the last one */
    for(k=0;k<HD_LSZ-1;k++) {
        if(!lbuf[k]) {
            lbuf[k]=' ';
        }
    }
    /* put pipes around ascii representation */
    lbuf[HD_LSZ-2]='|';
    lbuf[HD_LSZ-HD_BCNT-2]='|';
    /* ensure trailing null byte exists */
    lbuf[HD_LSZ-1]=0;
}
/*
 *
 */
void log_print_hd(const char *file,
                  int line,
                  const char *func,
                  const char *color,
                  const char *lvl,
                  const unsigned char *buf,
                  int size)
{
    int lcnt, oft, k;
    char lbuf[HD_LSZ];
    /**/
    lcnt=size/HD_BCNT+((size%HD_BCNT)>0?1:0);
    for(k=0;k<lcnt;k++) {
        oft=k*HD_BCNT;
        _log_print_start(file, line, func, color, lvl);
        fprintf(stderr, "%#010x: ", oft);
        _log_print_prepare_hd_line(buf+oft, size-oft, lbuf);
        fprintf(stderr, "%s", lbuf);
        _log_print_end();
    }
}
