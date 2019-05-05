#include "config.h"
#include "macro.h"
#include "ddfs.h"
#include "path.h"
#include "log.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

/*
 * Get attributes from an open file
 *
 * This method is called instead of the getattr() method if the
 * file information is available.
 *
 * Currently this is only called after the create() method if that
 * is implemented (see above).  Later it may be called for
 * invocations of fstat() too.
 *
 * Introduced in version 2.5
 */
int ddfs_fgetattr(const char *path,
                  struct stat *st,
                  struct fuse_file_info *fi)
{
    log_in("path=%s,st=%p,fi=%p", path, st, fi);
    int err;
    ddfs_fh_t *ddfs_fh;
    /**/
    ddfs_fh=DDFS_GET_FH(fi);
    log_debug("fstat(fd=%d,st=%p)", ddfs_fh->fd, st);
    if((err=fstat(ddfs_fh->fd, st))) {
        err=-errno;
    }
    log_out_fuse();
    return err;
}
