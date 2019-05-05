#include "log.h"
#include "path.h"
#include "macro.h"
#include "record.h"
#include "crypto.h"
#include "convert.h"

#include <linux/limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libgen.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define PATH_SEP_CHR '/'
#define PATH_SEP_STR "/"

/*
 * See man 3 basename
 */
char *path_basename(char *path)
{
    log_in("path=%s", path);
    char *bname;
    /**/
    bname=basename(path);
    log_out("bname=%s", bname);
    return bname;
}
/*
 * See man 3 basename
 */
char *path_dirname(char *path)
{
    log_in("path=%s", path);
    char *dname;
    /**/
    dname=dirname(path);
    log_out("dname=%s", dname);
    return dname;
}
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
                        const char **relpath)
{
    log_in("prefix=%s,path=%s,relpath=%p", prefix, path, relpath);
    bool_t err;
    int prelen, plen;
    /**/
    plen=strlen(path);
    prelen=strlen(prefix);
    if(prelen>=plen) {
        log_error("no relative part if prefix len greater or equal to path len.");
        NOK_GOTO(end);
    }
    IF_NOK(memcmp(prefix, path, prelen)) {
        log_error("path does not contain given prefix.");
        NOK_GOTO(end);
    }
    (*relpath)=path+(path[prelen]==PATH_SEP_CHR?prelen+1:prelen);
    err=OK;
end:
    log_out_err();
    return err;
}
/*
 * Count path components following two rules :
 *     1. trailing '/' is ignored,
 *     2. begining '/' counts as one component
 *
 * /!\ path SHALL BE A NULL-TERMINATED STRING /!\
 */
int path_components_count(const char *path)
{
    log_in("path=%s", path);
    char c;
    int count=0, k=0;
    /**/
    if(strlen(path)==0) {
        goto end;
    }
    count=1;
    do {
        c=path[k];
        if(c==PATH_SEP_CHR&&path[k+1]!='\0') {
            count++;
        }
        k++;
    } while(c!='\0');
end:
    log_out("count=%d", count);
    return count;
}
/*
 * Split path into an array of parts, count will contain the number of parts encoutered
 *
 * /!\ path SHALL BE A NULL-TERMINATED STRING /!\
 * /!\ path SHALL NOT BE FREED WHILE parts IS BEING USED /!\
 * /!\ ONLY parts (array) SHALL BE FREED BY CALLER, DO NOT FREE parts[N] INDIVIDUALLY /!\
 *
 * Example:
 *     in: path=/home/user/example/ or path=/home/user/example
 *     out: parts=["","home","user","example"] and count=4
 */
bool_t path_components(const char *path,
                       path_t *path_s)
{
    log_in("path=%s,path_s=%p", path, path_s);
    bool_t err;
    int plen, sep_cnt, k, p;
    char *_d=NULL, **components=NULL;
    /**/
    plen=strlen(path);
    sep_cnt=path_components_count(path);
    if(sep_cnt==0) {
        _d=NULL;
        components=NULL;
        goto success;
    }
    _d=ALLOC(char, plen+1);
    components=ALLOC(char*, sep_cnt);
    if(_d==NULL||components==NULL) {
        log_errno("failed to allocate memory!");
        NOK_GOTO(cleanup);
    }
    memcpy(_d, path, plen+1);
    components[0]=_d;
    for(p=1,k=0;k<plen;k++) {
        if(_d[k]!=PATH_SEP_CHR) {
            continue;
        }
        _d[k]='\0';
        if(_d[k+1]!='\0') {
            components[p]=_d+(k+1);
            p++;
        }
    }
success:
    path_s->_d=_d;
    path_s->count=sep_cnt;
    path_s->components=components;
    OK_GOTO(end);
cleanup:
    FREE(_d);
    FREE(components);
end:
    log_out_err();
    return err;
}
/*
 * Properly frees path_t structure
 */
void path_free_components(path_t *path_s)
{
    log_in("path_s=%p", path_s);
    FREE(path_s->_d);
    FREE(path_s->components);
    path_s->count=0;
    log_out_void();
}
/*
 * Return 0 if given path exists else 1
 *
 * /!\ path SHALL BE A NULL-TERMINATED STRING /!\
 */
bool_t path_exists(const char *path)
{
    log_in("path=%s", path);
    bool_t err;
    struct stat st;
    /**/
    IF_NOK(stat(path, &st)) {
        if(errno!=ENOENT) {
            log_errno("failed to stat path: %s", path);
        }
        NOK_GOTO(end);
    }
    err=OK;
end:
    log_out_err();
    return err;
}
/*
 * Return 0 if given path is absolute else 1
 *
 * /!\ path SHALL BE A NULL-TERMINATED STRING /!\
 */
bool_t path_is_absolute(const char *path)
{
    log_in("path=%s", path);
    bool_t err;
    /**/
    err=(path[0]==PATH_SEP_CHR?OK:NOK);
    log_out_err();
    return err;
}
/*
 * Join allocates path and concatenate prefix and suffix in it.
 *
 * Expects prefix and suffix to be null-terminated strings
 *
 * /!\ path SHALL BE FREED BY CALLER /!\
 * /!\ prefix AND suffix SHALL BE NULL-TERMINATED STRINGS /!\
 *
 * Return 0 on success, 1 on failure and path is set to NULL
 */
bool_t path_join(const char *prefix,
                 const char *suffix,
                 char **path)
{
    log_in("prefix=%s,suffix=%s,path=%p", prefix, suffix, path);
    bool_t err, sep_req;
    int psz, ssz;
    char *lpath;
    /**/
    psz=strlen(prefix);
    ssz=strlen(suffix);
    if(psz==0||ssz==0) {
        log_error("prefix/suffix is an empty string!");
        NOK_GOTO(end);
    }
    sep_req=((prefix[psz-1]!=PATH_SEP_CHR&&suffix[0]!=PATH_SEP_CHR)?OK:NOK);
    lpath=ALLOC(char, psz+ssz+1+(sep_req==OK?1:0));
    if(lpath==NULL) {
        log_errno("failed to allocate memory to store path!");
        NOK_GOTO(end);
    }
    strcat(lpath, prefix);
    IF_OK(sep_req) {
        strcat(lpath, PATH_SEP_STR);
    }
    strcat(lpath, suffix);
    (*path)=lpath;
    err=OK;
end:
    log_out_err();
    return err;
}
/*
 *  Translate a mountdir path to a real encrypted path
 */
bool_t path_translate(const char *rootdir,
                      const char *path,
                      const char **epath)
{
    log_in("rootdir=%s,path=%s,epath=%p",
           rootdir, path, epath);
    bool_t err;
    int epathlen, k;
    char *lepath=NULL, hexstr[2*REC_HASH_SZ+1];
    uchar_t hash[REC_HASH_SZ];
    path_t path_s;
    /**/
    IF_NOK(path_components(path, &path_s)) {
        log_error("failed to resolve relative path.");
        NOK_GOTO(end);
    }
    epathlen=strlen(rootdir)+path_s.count*(2*REC_HASH_SZ+1)+1;
    if(epathlen>PATH_MAX) {
        log_error("epath length is greater than PATH_MAX limit.");
        NOK_GOTO(cleanup);
    }
    lepath=ALLOC(char, epathlen);
    if(lepath==NULL) {
        log_errno("failed to allocate memory!");
        NOK_GOTO(cleanup);
    }
    strncat(lepath, rootdir, epathlen);
    for(k=0;k<path_s.count;k++) {
        if(strlen(path_s.components[k])==0) {
            log_debug("skipping empty component.");
            continue;
        }
        strncat(lepath, PATH_SEP_STR, 1);
        IF_NOK(crypto_hash_name(path_s.components[k], hash)) {
            log_error("failed to hash name.");
            NOK_GOTO(cleanup);
        }
        uchar2hexstr(hash, REC_HASH_SZ, hexstr);
        strcat(lepath, hexstr);
    }
    (*epath)=lepath;
    OK_GOTO(end);
cleanup:
    FREE(lepath);
end:
    path_free_components(&path_s);
    log_out_err();
    return err;
}
/*
 * Check if path points to a directory or not and return respectively OK or KO
 */
bool_t path_isdir(const char *path)
{
    log_in("path=%s", path);
    bool_t err;
    struct stat st;
    /**/
    IF_NOK(stat(path, &st)) {
        log_errno("failed to stat path: %s", path);
        NOK_GOTO(end);
    }
    if((st.st_mode&S_IFMT)!=S_IFDIR) {
        log_debug("not a directory: %s", path);
        NOK_GOTO(end);
    }
    err=OK;
end:
    log_out_err();
    return err;
}
/*
 * Check if path points to a regular file or not and return respectively OK or KO
 */
bool_t path_isreg(const char *path)
{
    log_in("path=%s", path);
    bool_t err;
    struct stat st;
    /**/
    IF_NOK(stat(path, &st)) {
        log_errno("failed to stat path: %s", path);
        NOK_GOTO(end);
    }
    if((st.st_mode&S_IFMT)!=S_IFREG) {
        log_debug("not a regular file: %s", path);
        NOK_GOTO(end);
    }
    err=OK;
end:
    log_out_err();
    return err;
}
