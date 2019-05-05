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
 * Remove a file
 */
int ddfs_unlink(const char *path)
{
    log_in("path=%s", path);
    int err;
    char *lpath=NULL, *lepath=NULL;
    const char *epath=NULL, *basename, *dirname;
    ddfs_data_t *ddfs_data;
    index_hd_t idx_hd;
    /**/
    init_index_hd(&idx_hd);
    ddfs_data=DDFS_DATA();
    IF_NOK(path_translate(ddfs_data->rootdir,
                          path,
                          &epath)) {
        log_error("failed to translate path.");
        NOK_GOTO(end);
    }
    IF_NOK(path_isreg(epath)) {
        log_error("cannot unlink something else than a file.");
        NOK_GOTO(end);
    }
    lpath=strdup(path);
    lepath=strdup(epath);
    basename=path_basename(lpath);
    dirname=path_dirname(lepath);
    /* regular file, delete record first */
    IF_NOK(index_open(dirname, &idx_hd)) {
        log_error("failed to open index.");
        NOK_GOTO(end);
    }
    IF_NOK(index_del_record(&idx_hd, basename)) {
        log_error("failed to delete record.");
        NOK_GOTO(end);
    }
    /* delete file from underlying filesystem */
    log_debug("unlink(epath=%s)", epath);
    err=unlink(epath);
end:
    IF_NOK(index_close(&idx_hd)) {
        log_warn("failed to close index.");
    }
    FREE(lpath);
    FREE(lepath);
    FREE(epath);
    if(err) {
        err=-errno;
    }
    log_out_fuse();
    return err;
}
