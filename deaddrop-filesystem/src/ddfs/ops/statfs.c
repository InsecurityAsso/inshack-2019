#include "config.h"
#include "macro.h"
#include "ddfs.h"
#include "path.h"
#include "log.h"

#include <sys/statvfs.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*
 * Get file system statistics
 *
 * The 'f_frsize', 'f_favail', 'f_fsid' and 'f_flag' fields are ignored
 *
 * Replaced 'struct statfs' parameter with 'struct statvfs' in
 * version 2.5
 */
int ddfs_statfs(const char *path,
                struct statvfs *stbuf)
{
    log_in("path=%s,stbuf=%p", path, stbuf);
    int err;
    const char *epath=NULL;
    ddfs_data_t *ddfs_data;
    ddfs_data=DDFS_DATA();
    IF_NOK(path_translate(ddfs_data->rootdir,
                          path,
                          &epath)) {
        log_error("failed to translate path.");
        NOK_GOTO(end);
    }
    log_debug("statvfs(epath=%s,stbuf=%p)", epath, stbuf);
    err=statvfs(epath, stbuf);
end:
    FREE(epath);
    if(err) {
        err=-errno;
    }
    log_out_fuse();
    return err;
}
