#include "config.h"
#include "macro.h"
#include "ddfs.h"
#include "path.h"
#include "log.h"

#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*
 * Open directory
 *
 * Unless the 'default_permissions' mount option is given,
 * this method should check if opendir is permitted for this
 * directory. Optionally opendir may also return an arbitrary
 * filehandle in the fuse_file_info structure, which will be
 * passed to readdir, closedir and fsyncdir.
 *
 * Introduced in version 2.3
 */
int ddfs_opendir(const char *path,
                 struct fuse_file_info *fi)
{
    log_in("path=%s,fi=%p", path, fi);
    int err, dfd;
    DIR *dp;
    ddfs_fh_t *ddfs_fh=NULL;
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
    log_debug("opendir(epath=%s)", epath);
    if((dp=opendir(epath))==NULL) {
        log_errno("failed to open directory.");
        NOK_GOTO(end);
    }
    log_debug("dirfd(dp=%p)", dp);
    if((dfd=dirfd(dp))<0) {
        log_errno("failed to retrieve directory file descriptor.");
        NOK_GOTO(end);
    }
    ddfs_fh=ALLOC(ddfs_fh_t, 1);
    if(ddfs_fh==NULL) {
        log_errno("failed to allocate memory for ddfs_fh.");
        NOK_GOTO(end);
    }
    init_ddfs_fh(ddfs_fh);
    ddfs_fh->dp=dp;
    ddfs_fh->fd=dfd;
    DDFS_SET_FH(fi, ddfs_fh);
    err=OK;
end:
    FREE(epath);
    if(err) {
        err=-errno;
    }
    log_out_fuse();
    return err;
}
