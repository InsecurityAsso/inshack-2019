#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include <stdarg.h>

#define LOG_LVL_DBG "DBG"
#define LOG_LVL_INF "INF"
#define LOG_LVL_WRN "WRN"
#define LOG_LVL_ERR "ERR"
#define LOG_LVL_FAT "FAT"

#define LOG_CLR_DBG "32"
#define LOG_CLR_INF "34"
#define LOG_CLR_WRN "33"
#define LOG_CLR_ERR "31"
#define LOG_CLR_FAT "35"

#define HD_WIDTH    2
#define HD_COUNT    4
#define HD_BCNT     (HD_COUNT*HD_WIDTH)
#define HD_LSZ      (HD_COUNT*(3*HD_WIDTH+1)+3)
/*
 * Print a log message
 */
void log_print(const char *file,
               int line,
               const char *func,
               const char *color,
               const char *lvl,
               const char *fmt,
               ...);
/*
 * Print a log message with a hexdump of buf
 */
void log_print_hd(const char *file,
                  int line,
                  const char *func,
                  const char *color,
                  const char *lvl,
                  const unsigned char *buf,
                  int size);

#define bool2str(b)     ((b)==OK?"OK":"NOK")

#ifdef COLORIZE
#   define log(color, lvl, ...) \
    log_print(__FILE__, __LINE__, __func__, color, lvl, __VA_ARGS__)
#   define log_hd(color, lvl, buf, size) \
    log_print_hd(__FILE__, __LINE__, __func__, color, lvl, buf, size)
#else /* COLORIZE */
#   define log(color, lvl, ...) \
    log_print(__FILE__, __LINE__, __func__, NULL, lvl, __VA_ARGS__)
#   define log_hd(color, lvl, buf, size) \
    log_print_hd(__FILE__, __LINE__, __func__, NULL, lvl, buf, size)
#endif /* COLORIZE */

#ifdef DEBUG
#   define log_debug(...)           log(LOG_CLR_DBG, LOG_LVL_DBG, __VA_ARGS__)
#   define log_debug_hd(buf, size)  log_hd(LOG_CLR_DBG, LOG_LVL_DBG, buf, size)
#   define log_info(...)           log(LOG_CLR_INF, LOG_LVL_INF, __VA_ARGS__)
#   define log_info_hd(buf, size)  log_hd(LOG_CLR_INF, LOG_LVL_INF, buf, size)
#   define log_warn(...)           log(LOG_CLR_WRN, LOG_LVL_WRN, __VA_ARGS__)
#   define log_warn_hd(buf, size)  log_hd(LOG_CLR_WRN, LOG_LVL_WRN, buf, size)
#   define log_error(...)          log(LOG_CLR_ERR, LOG_LVL_ERR, __VA_ARGS__)
#   define log_error_hd(buf, size) log_hd(LOG_CLR_ERR, LOG_LVL_ERR, buf, size)
#   define log_fatal(...)          log(LOG_CLR_FAT, LOG_LVL_FAT, __VA_ARGS__)
#   define log_fatal_hd(buf, size) log_hd(LOG_CLR_FAT, LOG_LVL_FAT, buf, size)
#else /* DEBUG */
#   define log_debug(...)           /* skip useless lines of code in release mode  */
#   define log_debug_hd()           /* skip useless lines of code in release mode  */
#   define log_info(...)            /* skip useless lines of code in release mode  */
#   define log_info_hd(buf, size)   /* skip useless lines of code in release mode  */
#   define log_warn(...)            /* skip useless lines of code in release mode  */
#   define log_warn_hd(buf, size)   /* skip useless lines of code in release mode  */
#   define log_error(...)           /* skip useless lines of code in release mode  */
#   define log_error_hd(buf, size)  /* skip useless lines of code in release mode  */
#   define log_fatal(...)           /* skip useless lines of code in release mode  */
#   define log_fatal_hd(buf, size)  /* skip useless lines of code in release mode  */
#endif /* DEBUG */

#define log_errno(...) \
    log_error(__VA_ARGS__); \
    log_error("errno: %s", strerror(errno))

#define log_in(args_fmt, ...)   log_debug("IN ("args_fmt")", __VA_ARGS__)
#define log_out(ret_fmt, ret)   log_debug("OUT -> "ret_fmt, ret)
#define log_out_err()           log_debug("OUT -> %s", bool2str(err))
#define log_in_void()           log_debug("IN")
#define log_out_void()          log_debug("OUT")
#define log_out_fuse()          log_debug("OUT -> %s", strerror(-err))

#endif /* __LOG_H__ */
