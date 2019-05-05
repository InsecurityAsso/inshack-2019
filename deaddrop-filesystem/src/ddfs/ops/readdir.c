#include "convert.h"
#include "config.h"
#include "macro.h"
#include "index.h"
#include "ddfs.h"
#include "path.h"
#include "log.h"

#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define SPECIAL_DIRS_CNT 2
static char *gspecial_dirs[SPECIAL_DIRS_CNT]={
    ".",
    ".."
};

/*
 * Read directory
 *
 * This supersedes the old getdir() interface.  New applications
 * should use this.
 *
 * The filesystem may choose between two modes of operation:
 *
 * 1) The readdir implementation ignores the offset parameter, and
 * passes zero to the filler function's offset.  The filler
 * function will not return '1' (unless an error happens), so the
 * whole directory is read in a single readdir operation.  This
 * works just like the old getdir() method.
 *
 * 2) The readdir implementation keeps track of the offsets of the
 * directory entries.  It uses the offset parameter and always
 * passes non-zero offset to the filler function.  When the buffer
 * is full (or an error happens) the filler function will return
 * '1'.
 *
 * Introduced in version 2.3
 */
int ddfs_readdir(const char *path,
                 void *buf,
                 fuse_fill_dir_t filler,
                 off_t offset,
                 struct fuse_file_info *fi)
{
    log_in("path=%s,buf=%p,filler,offset=%d,fi=%p", path, buf, offset, fi);
    int i, err;
    struct stat st;
    record_t *record;
    ddfs_fh_t *ddfs_fh=NULL;
    index_hd_t idx_hd;
    struct dirent *de;
    const char *epath=NULL;
    ddfs_data_t *ddfs_data;
    char name[REC_NAME_SZ+1];
    uchar_t hash[REC_HASH_SZ];
    /**/
    init_index_hd(&idx_hd);
    ddfs_fh=DDFS_GET_FH(fi);
    ddfs_data=DDFS_DATA();
    IF_NOK(path_translate(ddfs_data->rootdir,
                          path,
                          &epath)) {
        log_error("failed to translate path.");
        NOK_GOTO(end);
    }
    IF_NOK(index_open(epath, &idx_hd)) {
        log_error("failed to open index.");
        NOK_GOTO(end);
    }
    /* ensure directory is at begining */
    rewinddir(ddfs_fh->dp);
    while((de=readdir(ddfs_fh->dp))!=NULL) {
        /* SKIP index file */
        IF_OK(strcmp(de->d_name, IDX_FILENAME)) {
            log_debug("Skipping index file.");
            continue;
        }
        /* clear name buffer */
        memset(name, 0, REC_NAME_SZ+1);
        /* handle special directories */
        for(i=0;i<SPECIAL_DIRS_CNT;i++) {
            IF_OK(strcmp(de->d_name, gspecial_dirs[i])) {
                log_debug("Handling '%s' special directory.", gspecial_dirs[i]);
                strcat(name, gspecial_dirs[i]);
                goto skip_index_lookup;
            }
        }
        /* find record */
        IF_NOK(hexstr2uchar(de->d_name, hash)) {
            log_error("failed to convert hexstr to hash.");
            NOK_GOTO(end);
        }
        IF_NOK(index_get_record_by_hash(&idx_hd, hash, &record)) {
            log_error("failed to find record using hash.");
            NOK_GOTO(end);
        }
        IF_NOK(record_decrypt(record, &(ddfs_data->user))) {
            log_debug("failed to decrypt record. Skipping.");
            continue;
        }
        /* copy decrypted record name */
        memcpy(name, record->name, REC_NAME_SZ);
        /* encrypt record */
        IF_NOK(record_encrypt(record, &(ddfs_data->user))) {
            log_error("failed to encrypt record.");
            NOK_GOTO(end);
        }
skip_index_lookup:
        /* fill stat structure */
        memset(&st, 0, sizeof(struct stat));
        st.st_ino=de->d_ino;
        st.st_mode=de->d_type<<12;
        /* pass to filler */
        IF_NOK(filler(buf, name, &st, 0)) {
            break;
        }
    }
    err=OK;
end:
    FREE(epath);
    IF_NOK(index_close(&idx_hd)) {
        log_warn("failed to close index.");
    }
    if(err) {
        err=-errno;
    }
    log_out_fuse();
    return err;
}
