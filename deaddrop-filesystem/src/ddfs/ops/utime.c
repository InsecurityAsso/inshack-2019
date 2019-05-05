#include "config.h"
#include "macro.h"
#include "ddfs.h"
#include "path.h"
#include "log.h"

#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <utime.h>

int ddfs_utime(const char *path,
               struct utimbuf *times)
{
    log_in("path=%s,times=%p", path, times);
    int err;
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
    log_debug("utime(epath=%s,times=%p)", epath, times);
    err=utime(epath, times);
end:
    FREE(epath);
    if(err) {
        err=-errno;
    }
    log_out_fuse();
    return err;
}
