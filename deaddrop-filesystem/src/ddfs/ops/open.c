#include "config.h"
#include "macro.h"
#include "index.h"
#include "ddfs.h"
#include "path.h"
#include "log.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

/*
 * File open operation
 *
 * No creation (O_CREAT, O_EXCL) and by default also no
 * truncation (O_TRUNC) flags will be passed to open(). If an
 * application specifies O_TRUNC, fuse first calls truncate()
 * and then open(). Only if 'atomic_o_trunc' has been
 * specified and kernel version is 2.6.24 or later, O_TRUNC is
 * passed on to open.
 *
 * Unless the 'default_permissions' mount option is given,
 * open should check if the operation is permitted for the
 * given flags. Optionally open may also return an arbitrary
 * filehandle in the fuse_file_info structure, which will be
 * passed to all file operations.
 *
 * Changed in version 2.2
 */
int ddfs_open(const char *path,
              struct fuse_file_info *fi)
{
    log_in("path=%s,fi=%p", path, fi);
    int err;
    char *lpath=NULL, *bname;
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
    lpath=strdup(path);
    bname=path_basename(lpath);
    ddfs_fh=ALLOC(ddfs_fh_t, 1);
    if(ddfs_fh==NULL) {
        log_errno("failed to allocate memory for ddfs_fh.");
        NOK_GOTO(end);
    }
    init_ddfs_fh(ddfs_fh);
    IF_NOK(index_open_file(epath, bname, fi->flags, 0,
                           &(ddfs_data->user), ddfs_fh)) {
        log_error("failed to open file.");
        FREE(ddfs_fh);
        NOK_GOTO(cleanup);
    }
    DDFS_SET_FH(fi, ddfs_fh);
    OK_GOTO(end);
cleanup:
    FREE(ddfs_fh);
end:
    FREE(lpath);
    FREE(epath);
    if(err) {
        FREE(ddfs_fh);
        err=-errno;
    }
    log_out_fuse();
    return err;
}
