#include "convert.h"
#include "macro.h"
#include "log.h"

#include <stdio.h>
#include <string.h>

/*
 * Fills hexstr with hexadecimal representation of buffer bytes
 *
 * /!\ hexstr SHALL BE ALLOCATED BY CALLER /!\
 * /!\ hexstr SHALL HAVE A SIZE OF AT LEAST 2*len+1 /!\
 *
 * Return OK on success, NOK otherwise
 */
void uchar2hexstr(const uchar_t *buffer, int len, char *hexstr)
{
    log_in("buffer=%p,len=%d,hexstr=%p", buffer, len, hexstr);
    int k;
    /**/
    memset(hexstr, 0, 2*len+1);
    for(k=0;k<len;k++) {
        snprintf(hexstr+(2*k), 3, "%02x", buffer[k]);
    }
    log_out_void();
}
/*
 * Convert a hexstring to bytes
 *
 * /!\ buffer SHALL BE ALLOCATED BY CALLER /!\
 * /!\ buffer SHALL HAVE A SIZE OF AT LEAST strlen(hexstr)/2 /!\
 *
 * Return OK on success, NOK otherwise
 */
bool_t hexstr2uchar(const char *hexstr, uchar_t *buffer)
{
    log_in("hexstr=%s,buffer=%p", hexstr, buffer);
    bool_t err;
    int k, len;
    uint_t value;
    /**/
    len=strlen(hexstr);
    if(len%2!=0) {
        NOK_GOTO(end);
    }
    for(k=0;k<len/2;k++) {
        sscanf(hexstr+(k*2), "%02x", &value);
        buffer[k]=(uchar_t)(value&0xff);
    }
    err=OK;
end:
    log_out_err();
    return err;
}
