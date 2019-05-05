#include "config.h"
#include "macro.h"
#include "ddfs.h"
#include "path.h"
#include "log.h"

#include <unistd.h>
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
int ddfs_fsyncdir(const char *path,
                  int isdatasync,
                  struct fuse_file_info *fi)
{
    log_in("path=%s,fi=%p", path, fi);
    int err=OK;
    ddfs_fh_t *ddfs_fh=NULL;
    /**/
    ddfs_fh=DDFS_GET_FH(fi);
    if(isdatasync) {
        log_debug("fdatasync(fd=%d)", ddfs_fh->fd);
        err=fdatasync(ddfs_fh->fd);
    } else {
        log_debug("fsync(fd=%d)", ddfs_fh->fd);
        err=fsync(ddfs_fh->fd);
    }
    if(err) {
        err=-errno;
    }
    log_out_fuse();
    return err;
}
