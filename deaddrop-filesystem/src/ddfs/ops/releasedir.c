#include "config.h"
#include "macro.h"
#include "ddfs.h"
#include "path.h"
#include "log.h"

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
int ddfs_releasedir(const char *path,
                    struct fuse_file_info *fi)
{
    log_in("path=%s,fi=%p", path, fi);
    int err=OK;
    ddfs_fh_t *ddfs_fh=NULL;
    /**/
    ddfs_fh=DDFS_GET_FH(fi);
    log_debug("closedir(dp=%p)", ddfs_fh->dp);
    IF_NOK(closedir(ddfs_fh->dp)) {
        log_errno("failed to close directory. Freeing ddfs_fh structure anyway.");
        err=NOK;
    }
    ddfs_fh->dp=NULL;
    ddfs_fh->fd=-1;
    FREE(ddfs_fh);
    if(err) {
        err=-errno;
    }
    log_out_fuse();
    return err;
}
