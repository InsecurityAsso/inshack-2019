#include "config.h"
#include "crypto.h"
#include "macro.h"
#include "index.h"
#include "ddfs.h"
#include "path.h"
#include "log.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

/*
 * Read data from an open file
 *
 * Read should return exactly the number of bytes requested except
 * on EOF or error, otherwise the rest of the data will be
 * substituted with zeroes.  An exception to this is when the
 * 'direct_io' mount option is specified, in which case the return
 * value of the read system call will reflect the return value of
 * this operation.
 *
 * Changed in version 2.2
 */
int ddfs_read(const char *path,
              char *buf,
              size_t size,
              off_t offset,
              struct fuse_file_info *fi)
{
    log_in("path=%s,buf=%p,size=%d,offset=%d,fi=%p", path, buf, size, offset, fi);
    size_t rsz;
    ddfs_fh_t *ddfs_fh;
    /**/
    ddfs_fh=DDFS_GET_FH(fi);
    if(ddfs_fh->flen==0) {
        rsz=0;
        goto end;
    }
    if(offset>=ddfs_fh->flen) {
        log_error("reading out of file buffer!");
        errno=EFAULT;
        rsz=-1;
        goto end;
    }
    rsz=MIN(size, ddfs_fh->flen-offset);
    memcpy(buf, ddfs_fh->fbuf+offset, rsz);
end:
    log_out("rsz=%d", rsz);
    return rsz;
}
/*
 * Store data from an open file in a buffer
 *
 * Similar to the read() method, but data is stored and
 * returned in a generic buffer.
 *
 * No actual copying of data has to take place, the source
 * file descriptor may simply be stored in the buffer for
 * later data transfer.
 *
 * The buffer must be allocated dynamically and stored at the
 * location pointed to by bufp.  If the buffer contains memory
 * regions, they too must be allocated using malloc().  The
 * allocated memory will be freed by the caller.
 *
 * Introduced in version 2.9
 */
int ddfs_read_buf(const char *path,
                  struct fuse_bufvec **bufp,
                  size_t size,
                  off_t off,
                  struct fuse_file_info *fi)
{
    int rsz;
    log_in("path=%s,bufp=%p,size=%d,off=%d,fi=%p", path, bufp, size, off, fi);
    rsz=-1;
    log_out("rsz=%d", rsz);
    return rsz;
}

