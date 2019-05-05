#include "config.h"
#include "macro.h"
#include "ddfs.h"
#include "path.h"
#include "log.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

/*
 * Change the owner and group of a file
 */
int ddfs_chown(const char *path,
               uid_t uid,
               gid_t gid)
{
    log_in("path=%s,uid=%d,gid=%d", path, uid, gid);
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
    log_debug("chown(epath=%s,uid=%d,gid=%d)", epath, uid, gid);
    err=chown(epath, uid, gid);
end:
    FREE(epath);
    if(err) {
        err=-errno;
    }
    log_out_fuse();
    return err;
}
