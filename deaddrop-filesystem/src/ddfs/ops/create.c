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
 * Create and open a file
 *
 * If the file does not exist, first create it with the specified
 * mode, and then open it.
 *
 * If this method is not implemented or under Linux kernel
 * versions earlier than 2.6.15, the mknod() and open() methods
 * will be called instead.
 *
 * Introduced in version 2.5
 */
int ddfs_create(const char *path,
                mode_t mode,
                struct fuse_file_info *fi)
{
    log_in("path=%s,mode=%d,fi=%p", path, mode, fi);
    int err;
    char *lpath=NULL,
         *lepath=NULL,
         *bname,
         *edname;
    const char *epath=NULL;
    ddfs_fh_t *ddfs_fh;
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
    lepath=strdup(epath);
    bname=path_basename(lpath);
    edname=path_dirname(lepath);
    IF_NOK(index_create_file(edname, bname, REC_TYPE_REGULAR,
                             &(ddfs_data->user))) {
        log_error("failed to create file record in index.");
        NOK_GOTO(end);
    }
    ddfs_fh=ALLOC(ddfs_fh_t, 1);
    if(ddfs_fh==NULL) {
        log_errno("failed to allocate memory for ddfs_fh.");
        NOK_GOTO(end);
    }
    init_ddfs_fh(ddfs_fh);
    IF_NOK(index_open_file(epath, bname, fi->flags, mode,
                           &(ddfs_data->user), ddfs_fh)) {
        log_error("failed to open file.");
        FREE(ddfs_fh);
        NOK_GOTO(end);
    }
    DDFS_SET_FH(fi, ddfs_fh);
    err=OK;
end:
    FREE(lpath);
    FREE(lepath);
    FREE(epath);
    if(err) {
        err=-errno;
    }
    log_out_fuse();
    return err;
}
