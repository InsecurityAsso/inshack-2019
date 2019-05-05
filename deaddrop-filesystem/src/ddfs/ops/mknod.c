#include "config.h"
#include "macro.h"
#include "index.h"
#include "ddfs.h"
#include "path.h"
#include "log.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

/*
 * Create a file node
 *
 * This is called for creation of all non-directory, non-symlink
 * nodes.  If the filesystem defines a create() method, then for
 * regular files that will be called instead.
 */
int ddfs_mknod(const char *path,
               mode_t mode,
               dev_t rdev)
{
    log_in("path=%s,mode=%d,rdev=%d", path, mode, rdev);
    int err;
    char *lpath=NULL,
         *lepath=NULL,
         *bname,
         *edname;
    const char *epath=NULL;
    ddfs_data_t *ddfs_data;
    /**/
    ddfs_data=DDFS_DATA();
    if((mode&S_IFREG)!=S_IFREG) {
        log_error("can only mknod regular files.");
        NOK_GOTO(end);
    }
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
    log_debug("mknod(epath=%s,mode=%d,dev=%d)", epath, mode, rdev);
    if(mknod(epath, mode, rdev)) {
        log_errno("failed to mknod.");
        NOK_GOTO(end);
    }
    err=OK;
end:
    FREE(lpath);
    FREE(epath);
    FREE(lepath);
    if(err) {
        err=-errno;
    }
    log_out_fuse();
    return err;
}
