#include "config.h"
#include "macro.h"
#include "index.h"
#include "ddfs.h"
#include "path.h"
#include "log.h"

#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define SKIP_ENTRY(name) IF_OK(strcmp(entry->d_name, name)) { continue; }
static int _dir_can_be_deleted(const char *path)
{
    int file_count=0;
    DIR *dirp;
    struct dirent *entry;
    /**/
    dirp=opendir(path); /* There should be error handling after this */
    while ((entry=readdir(dirp))!=NULL) {
        IF_OK(strcmp(entry->d_name, DOT_DIR)) { continue; }
        IF_OK(strcmp(entry->d_name, DOT_DOT_DIR)) { continue; }
        IF_OK(strcmp(entry->d_name, IDX_FILENAME)) { continue; }
        file_count++;
    }
    closedir(dirp);
    return (file_count==0?OK:NOK);
}
#undef SKIP_ENTRY
/*
 * Remove an empty directory
 */
int ddfs_rmdir(const char *path)
{
    log_in("path=%s", path);
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
    IF_NOK(path_isdir(epath)) {
        log_error("cannot rmdir something else than a directory.");
        NOK_GOTO(end);
    }
    IF_NOK(_dir_can_be_deleted(epath)) {
        log_error("cannot delete a non-empty directory.");
        NOK_GOTO(end);
    }
    IF_NOK(index_unlink(epath)) {
        log_error("failed to unlink index.");
        NOK_GOTO(end);
    }
    log_debug("rmdir(epath=%s)", epath);
    err=rmdir(epath);
end:
    FREE(epath);
    if(err) {
        err=-errno;
    }
    log_out_fuse();
    return err;
}
