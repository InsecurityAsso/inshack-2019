#include "config.h"
#include "crypto.h"
#include "macro.h"
#include "ddfs.h"
#include "path.h"
#include "log.h"

#define __USE_GNU
#include <sys/mman.h>
#undef __USE_GNU
#include <string.h>
#include <errno.h>

/*
 * Write data to an open file
 *
 * Write should return exactly the number of bytes requested
 * except on error.  An exception to this is when the 'direct_io'
 * mount option is specified (see read operation).
 *
 * Changed in version 2.2
 */
int ddfs_write(const char *path,
               const char *buf,
               size_t size,
               off_t offset,
               struct fuse_file_info *fi)
{
    log_in("path=%s,buf=%p,size=%d,offset=%d,fi=%p", path, buf, size, offset, fi);
    size_t wsz, nlen;
    void *addr;
    ddfs_fh_t *ddfs_fh;
    /**/
    ddfs_fh=DDFS_GET_FH(fi);
    IF_OK(ddfs_fh->append) {
        log_debug("writing to file opened with O_APPEND flag. Setting offset.");
        offset=ddfs_fh->flen;
    }
    if(offset+size>ddfs_fh->flen) {
        nlen=offset+size;
        if(ddfs_fh->flen>0) {
            log_debug("mremap(fbuf=%p,flen=%d,nlen=%d,MREMAP_MAYMOVE)",
                      ddfs_fh->fbuf, ddfs_fh->flen, nlen);
            addr=mremap(ddfs_fh->fbuf, ddfs_fh->flen, nlen, MREMAP_MAYMOVE);
        } else {
            log_debug("mmap(NULL,nlen=%d,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0)", nlen);
            addr=mmap(NULL, nlen, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
        }
        if(addr==MAP_FAILED) {
            log_errno("failed to map/remap file in memory.");
            wsz=-1;
            goto end;
        }
        ddfs_fh->flen=nlen;
        ddfs_fh->fbuf=(uchar_t*)addr;
    }
    log_debug("memcpy(fbuf=%p,offset=%d,buf=%p,size=%d)",
              ddfs_fh->fbuf, offset, buf, size);
    memcpy(ddfs_fh->fbuf+offset, buf, size);
    wsz=size;
end:
    log_out("wsz=%d", wsz);
    return wsz;
}
/*
 * Write contents of buffer to an open file
 *
 * Similar to the write() method, but data is supplied in a
 * generic buffer.  Use fuse_buf_copy() to transfer data to
 * the destination.
 *
 * Introduced in version 2.9
 */
int ddfs_write_buf(const char *path,
                   struct fuse_bufvec *buf,
                   off_t off,
                   struct fuse_file_info *fi)
{
    int wsz;
    log_in("path=%s,buf=%p,off=%d,fi=%p", path, buf, off, fi);
    wsz=-1;
    log_out("wsz=%d", wsz);
    return wsz;
}
