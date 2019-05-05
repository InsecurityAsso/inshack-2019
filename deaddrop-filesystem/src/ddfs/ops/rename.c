#include "config.h"
#include "macro.h"
#include "index.h"
#include "ddfs.h"
#include "path.h"
#include "log.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

/*
 * Rename a file
 */
int ddfs_rename(const char *from,
                const char *to)
{
    log_in("from=%s,to=%s", from, to);
    int err;
    char *lefrom=NULL,
         *leto=NULL,
         *lfrom=NULL,
         *lto=NULL;
    const char *efrom=NULL,
               *eto=NULL,
               *efrom_dir,
               *eto_dir,
               *from_name,
               *to_name;
    ddfs_data_t *ddfs_data;
    ddfs_data=DDFS_DATA();
    IF_NOK(path_translate(ddfs_data->rootdir,
                          from,
                          &efrom)) {
        log_error("failed to translate path.");
        NOK_GOTO(end);
    }
    IF_NOK(path_translate(ddfs_data->rootdir,
                          to,
                          &eto)) {
        log_error("failed to translate path.");
        NOK_GOTO(end);
    }
    log_debug("rename(efrom=%s,eto=%s)", efrom, eto);
    IF_NOK(rename(efrom, eto)) {
        log_error("failed to rename in fs.");
        NOK_GOTO(end);
    }
    lfrom=strdup(from);
    from_name=path_basename(lfrom);
    lto=strdup(to);
    to_name=path_basename(lto);
    lefrom=strdup(efrom);
    efrom_dir=path_dirname(lefrom);
    leto=strdup(eto);
    eto_dir=path_dirname(leto);
    IF_NOK(index_rename_record(efrom_dir, eto_dir,
                               from_name, to_name,
                               &(ddfs_data->user))) {
        /* perform renaming rollback if index rename operation failed */
        log_error("failed to rename in index.");
        NOK_GOTO(rollback);
    }
    OK_GOTO(end);
rollback:
    IF_NOK(rename(eto, efrom)) {
      log_warn("failed to rollback.");
    }
end:
    FREE(lfrom);
    FREE(lefrom);
    FREE(efrom);
    FREE(lto);
    FREE(leto);
    FREE(eto);
    if(err) {
        err=-errno;
    }
    log_out_fuse();
    return err;
}
