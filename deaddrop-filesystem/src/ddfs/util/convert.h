#ifndef __CONVERT_H__
#define __CONVERT_H__

#include "types.h"
/*
 * Fills hexstr with hexadecimal representation of buffer bytes
 *
 * /!\ hexstr SHALL BE ALLOCATED BY CALLER /!\
 * /!\ hexstr SHALL HAVE A SIZE OF AT LEAST 2*len+1 /!\
 *
 */
void uchar2hexstr(const uchar_t *buffer, int len, char *hexstr);
/*
 * Convert a hexstring to bytes
 *
 * /!\ buffer SHALL BE ALLOCATED BY CALLER /!\
 * /!\ buffer SHALL HAVE A SIZE OF AT LEAST strlen(hexstr)/2 /!\
 *
 * Return OK on success, NOK otherwise
 */
bool_t hexstr2uchar(const char *hexstr, uchar_t *buffer);

#endif /* __CONVERT_H__ */
