#ifndef __PATH_H__
#define __PATH_H__

#include "types.h"
/*
 *
 */
typedef struct path_s {
    char *_d;
    int count;
    char **components;
} path_t;
/*
 *
 */
#define debug_path_t(path) \
    { \
        int i; \
        log_debug("path_t at %p {", (path)); \
        log_debug("    ._d=%p", (path)->_d); \
        log_debug("    .count=%d", (path)->count); \
        log_debug("    .components"); \
        for(i=0;i<(path)->count;i++) { \
            log_debug("        [%d]=%s", i, (path)->components[i]); \
        } \
        log_debug("}"); \
    }
/*
 * See man 3 basename
 */
char *path_basename(char *path);
/*
 * See man 3 dirname
 */
char *path_dirname(char *path);
/*
 * Extract part of the path which is relative to prefix
 *
 * /!\ path SHALL BE A NULL-TERMINATED STRING /!\
 * /!\ path SHALL NOT BE FREED WHILE relpath IS USED /!\
 *
 * Return OK on success, NOK on failure
 */
bool_t path_relative_to(const char *prefix,
                        const char *path,
                        const char **relpath);
/*
 * Count path components following two rules :
 *     1. trailing '/' is ignored,
 *     2. begining '/' counts as one component
 *
 * /!\ path SHALL BE A NULL-TERMINATED STRING /!\
 */
int path_components_count(const char *path);
/*
 * Split path into an array of parts, count will contain the number of parts encoutered
 *
 * /!\ path SHALL BE A NULL-TERMINATED STRING /!\
 * /!\ ONLY components SHALL BE FREED BY CALLER USING path_free_components() /!\
 *
 * Example:
 *     in: path=/home/user/example/ or path=/home/user/example
 *     out: parts=["","home","user","example"] and count=4
 */
bool_t path_components(const char *path,
                    path_t *path_s);
/*
 * Properly frees path_t structure
 */
void path_free_components(path_t *path_s);
/*
 * Return OK if given path exists else NOK
 *
 * /!\ path SHALL BE A NULL-TERMINATED STRING /!\
 */
bool_t path_exists(const char *path);
/*
 * Return OK if given path is absolute else NOK
 *
 * /!\ path SHALL BE A NULL-TERMINATED STRING /!\
 */
bool_t path_is_absolute(const char *path);
/*
 * Join allocates path and concatenate prefix and suffix in it.
 *
 * Expects prefix and suffix to be null-terminated strings
 *
 * /!\ path SHALL BE FREED BY CALLER /!\
 * /!\ prefix AND suffix SHALL BE NULL-TERMINATED STRINGS /!\
 *
 * Return OK on success, NOK on failure and path is set to NULL
 */
bool_t path_join(const char *prefix,
                 const char *suffix,
                 char **path);
/*
 *  Translate a mountdir path to a real encrypted path
 */
bool_t path_translate(const char *rootdir,
                      const char *path,
                      const char **epath);
/*
 * Check if path points to a directory or not and return respectively OK or KO
 */
bool_t path_isdir(const char *path);
/*
 * Check if path points to a regular file or not and return respectively OK or KO
 */
bool_t path_isreg(const char *path);

#endif /* __PATH_H__ */
