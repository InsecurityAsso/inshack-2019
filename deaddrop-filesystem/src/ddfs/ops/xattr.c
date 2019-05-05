#include "config.h"
#include "crypto.h"
#include "macro.h"
#include "ddfs.h"
#include "path.h"
#include "log.h"

#include <sys/types.h>
#include <attr/xattr.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef HAVE_SETXATTR
/*
 * Set extended attributes
 */
int ddfs_setxattr(const char *path,
                  const char *name,
                  const char *value,
                  size_t size,
                  int flags)
{
    log_in("path=%s,name=%s,value=%s,size=%d,flags=%d",
           path, name, value, size, flags);
    int err;
    const char *epath=NULL;
    ddfs_data_t *ddfs_data;
    /**/
    ddfs_data=DDFS_DATA();
    IF_NOK(path_translate(ddfs_data->rootdir,
                          path,
                          &epath)) {
        log_error("failed to translate path.");
        NOK_GOTO(end);
    }
    log_debug("setxattr(epath=%s,name=%s,value=%s,size=%d,flags=%d)",
              epath, name, value, size, flags);
    err=setxattr(epath, name, value, size, flags);
end:
    FREE(epath);
    if(err) {
        err=-errno;
    }
    log_out_fuse();
    return err;
}
/*
 * Get extended attributes
 */
int ddfs_getxattr(const char *path,
                  const char *name,
                  char *value,
                  size_t size)
{
    log_in("path=%s,name=%s,value=%p,size=%d", path, name, value, size);
    int err;
    const char *epath=NULL;
    ddfs_data_t *ddfs_data;
    /**/
    ddfs_data=DDFS_DATA();
    IF_NOK(path_translate(ddfs_data->rootdir,
                          path,
                          &epath)) {
        log_error("failed to translate path.");
        NOK_GOTO(end);
    }
    log_debug("getxattr(epath=%s,name=%s,value=%p,size=%d)",
              epath, name, value, size);
    err=getxattr(epath, name, value, size);
end:
    FREE(epath);
    if(err) {
        err=-errno;
    }
    log_out_fuse();
    return err;
}
/*
 * List extended attributes
 */
int ddfs_listxattr(const char *path,
                   char *list,
                   size_t size)
{
    log_in("path=%s,list=%p,size=%d", path, list, size);
    int err;
    const char *epath=NULL;
    ddfs_data_t *ddfs_data;
    /**/
    ddfs_data=DDFS_DATA();
    IF_NOK(path_translate(ddfs_data->rootdir,
                          path,
                          &epath)) {
        log_error("failed to translate path.");
        NOK_GOTO(end);
    }
    log_debug("listxattr(epath=%s,list=%p,size=%d)", epath, list, size);
    err=listxattr(epath, list, size);
end:
    FREE(epath);
    if(err) {
        err=-errno;
    }
    log_out_fuse();
    return err;
}
/*
 * Remove extended attributes
 */
int ddfs_removexattr(const char *path,
                     const char *name)
{
    log_in("path=%s,name=%s", path, name);
    int err;
    const char *epath=NULL;
    ddfs_data_t *ddfs_data;
    /**/
    ddfs_data=DDFS_DATA();
    IF_NOK(path_translate(ddfs_data->rootdir,
                          path,
                          &epath)) {
        log_error("failed to translate path.");
        NOK_GOTO(end);
    }
    log_debug("removexattr(epath=%s,name=%s)", epath, name);
    err=removexattr(epath, name);
end:
    FREE(epath);
    if(err) {
        err=-errno;
    }
    log_out_fuse();
    return err;
}
#endif /* HAVE_SETXATTR */
