#include "config.h"
#include "macro.h"
#include "ddfs.h"
#include "path.h"
#include "log.h"

#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*
 * Change the permission bits of a file
 */
int ddfs_chmod(const char *path,
               mode_t mode)
{
    log_in("path=%s,mode=%d", path, mode);
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
    log_debug("chmod(epath=%s,mode=%d)", epath, mode);
    err=chmod(epath, mode);
end:
    FREE(epath);
    if(err) {
        err=-errno;
    }
    log_out_fuse();
    return err;
}
