#include "config.h"
#include "crypto.h"
#include "macro.h"
#include "ddfs.h"
#include "path.h"
#include "log.h"

#define __USE_GNU
#include <sys/mman.h>
#undef __USE_GNU
#include <unistd.h>
#include <string.h>
#include <errno.h>

/**
 * Change the size of an open file
 *
 * This method is called instead of the truncate() method if the
 * truncation was invoked from an ftruncate() system call.
 *
 * If this method is not implemented or under Linux kernel
 * versions earlier than 2.6.15, the truncate() method will be
 * called instead.
 *
 * Introduced in version 2.5
 */
int ddfs_ftruncate(const char *path,
                   off_t length,
                   struct fuse_file_info *fi)
{
    log_in("path=%s,length=%d,fi=%p", path, length, fi);
    int err;
    void *addr;
    ddfs_fh_t *ddfs_fh;
    /**/
    ddfs_fh=DDFS_GET_FH(fi);
    if(length==ddfs_fh->flen) {
        log_debug("truncate file to its actual size. Skipped.");
        OK_GOTO(end);
    }
    if(length==0) {
        /* special case: ftruncate passthrough */
        log_debug("ftruncate(fd=%d,0)", ddfs_fh->fd);
        IF_NOK(ftruncate(ddfs_fh->fd, 0)) {
            log_errno("ftruncate failed!");
            NOK_GOTO(end);
        }
        log_debug("munmap(fbuf=%p,flen=%d)", ddfs_fh->fbuf, ddfs_fh->flen);
        IF_NOK(munmap(ddfs_fh->fbuf, ddfs_fh->flen)) {
            log_errno("failed to unmap memory after 0-length truncation.");
            NOK_GOTO(end);
        }
        addr=NULL;
    } else {
        log_debug("mremap(fbuf=%p,flen=%d,length=%d,MREMAP_MAYMOVE)",
                  ddfs_fh->fbuf, ddfs_fh->flen, length);
        addr=mremap(ddfs_fh->fbuf, ddfs_fh->flen, length, MREMAP_MAYMOVE);
        if(addr==MAP_FAILED) {
            log_errno("failed to remap file in memory.");
            NOK_GOTO(end);
        }
    }
    /* success */
    ddfs_fh->flen=length;
    ddfs_fh->fbuf=(uchar_t*)addr;
    err=OK;
end:
    if(err) {
        err=-errno;
    }
    log_out_fuse();
    return err;
}

