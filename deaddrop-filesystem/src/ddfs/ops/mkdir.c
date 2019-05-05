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

/*
 * Create a directory
 *
 * Note that the mode argument may not have the type specification
 * bits set, i.e. S_ISDIR(mode) can be false.  To obtain the
 * correct directory type bits use  mode|S_IFDIR
 */
int ddfs_mkdir(const char *path,
               mode_t mode)
{
    log_in("path=%s,mode=%d", path, mode);
    int err;
    char *lpath=NULL,
         *lepath=NULL,
         *bname,
         *edname;
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
    lepath=strdup(epath);
    edname=path_dirname(lepath);
    IF_NOK(index_create_file(edname, bname, REC_TYPE_DIRECTORY,
                             &(ddfs_data->user))) {
        log_error("failed to create file record in index.");
        NOK_GOTO(end);
    }
    log_debug("mkdir(epath=%s,mode=%d)", epath, mode);
    if(mkdir(epath, mode)) {
        log_errno("failed to make directory.");
        NOK_GOTO(end);
    }
    IF_NOK(index_create(epath)) {
        log_error("failed to create index within new directory.");
        NOK_GOTO(end);
    }
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
