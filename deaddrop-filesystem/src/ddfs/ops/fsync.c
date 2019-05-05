#include "config.h"
#include "macro.h"
#include "ddfs.h"
#include "path.h"
#include "log.h"

#include <unistd.h>
#include <string.h>
#include <errno.h>

/*
 * Synchronize file contents
 *
 * If the datasync parameter is non-zero, then only the user data
 * should be flushed, not the meta data.
 *
 * Changed in version 2.2
 */
int ddfs_fsync(const char *path,
               int isdatasync,
               struct fuse_file_info *fi)
{
    log_in("path=%s,isdatasync=%d,fi=%p", path, isdatasync, fi);
    int err;
    ddfs_fh_t *ddfs_fh;
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
