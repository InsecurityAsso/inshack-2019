#include "config.h"
#include "macro.h"
#include "ddfs.h"
#include "path.h"
#include "log.h"

#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#ifdef HAVE_UTIMENSAT
/*
 * Change the access and modification times of a file with
 * nanosecond resolution
 *
 * This supersedes the old utime() interface.  New applications
 * should use this.
 *
 * See the utimensat(2) man page for details.
 *
 * Introduced in version 2.6
 */
int ddfs_utimens(const char *path,
                 const struct timespec ts[2],
                 struct fuse_file_info *fi)
{
    log_in("path=%s,ts=%p,fi=%p", path, ts, fi);
    int err;
    ddfs_fh_t *ddfs_fh;
    /**/
    ddfs_fh=DDFS_GET_FH(fi);
    log_debug("futimens(fd=%d,ts=%p)", ddfs_fh->fd, ts);
    if(err=futimens(ddfs_fh->fd, ts)) {
        err=-errno;
    }
    log_out_fuse();
    return err;
}
#endif /* HAVE_UTIMENSAT */
