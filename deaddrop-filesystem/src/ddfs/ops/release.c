#include "config.h"
#include "macro.h"
#include "index.h"
#include "ddfs.h"
#include "path.h"
#include "log.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

/*
 * Release an open file
 *
 * Release is called when there are no more references to an open
 * file: all file descriptors are closed and all memory mappings
 * are unmapped.
 *
 * For every open() call there will be exactly one release() call
 * with the same flags and file descriptor.  It is possible to
 * have a file opened more than once, in which case only the last
 * release will mean, that no more reads/writes will happen on the
 * file.  The return value of release is ignored.
 *
 * Changed in version 2.2
 */
int ddfs_release(const char *path,
                 struct fuse_file_info *fi)
{
    log_in("path=%s,fi=%p", path, fi);
    int err=OK;
    ddfs_fh_t *ddfs_fh;
    ddfs_data_t *ddfs_data;
    /**/
    ddfs_fh=DDFS_GET_FH(fi);
    ddfs_data=DDFS_DATA();
    IF_NOK(index_close_file(ddfs_fh, &(ddfs_data->user))) {
        log_warn("failed to close file properly.");
        err=NOK;
    }
    FREE(ddfs_fh);
    if(err) {
        err=-errno;
    }
    log_out_fuse();
    return err;
}
