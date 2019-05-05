#ifndef __DDFS_H__
#define __DDFS_H__

#include "log.h"
#include "config.h"
#include "record.h"
#include <dirent.h>

/*
 *
 */
typedef struct ddfs_data_s {
    char *rootdir;      /*  */
    char *mountdir;     /*  */
    user_t user;        /*  */
} ddfs_data_t;
/*
 * DDFS Private Data
 */
#define debug_ddfs_data_t(ddfs_data) \
    { \
        log_debug("ddfs_data_t at %p {", (ddfs_data)); \
        log_debug("    .rootdir=%s", (ddfs_data)->rootdir); \
        log_debug("    .mountdir=%s", (ddfs_data)->mountdir); \
        log_debug("    .user="); \
        debug_user_t(&((ddfs_data)->user)); \
        log_debug("}"); \
    }
/*
 * FUSE context accessor as a macro
 */
#define DDFS_DATA() ((ddfs_data_t*)fuse_get_context()->private_data)
/*
 * DDFS File Handle
 */
typedef struct ddfs_fh_s {
    int fd;                 /* file descriptor */
    DIR *dp;                /* directory pointer */
    bool_t append;          /* append flag */
    bool_t rd_only;         /* read only flag */
    uint_t flen;            /* real file length */
    uchar_t *fbuf;          /* file buffer */
    uchar_t iv[REC_IV_SZ];  /* initialization vector */
} ddfs_fh_t;
#define init_ddfs_fh(ddfs_fh) \
    (ddfs_fh)->fd=-1; \
    (ddfs_fh)->dp=NULL; \
    (ddfs_fh)->append=NOK; \
    (ddfs_fh)->rd_only=NOK; \
    (ddfs_fh)->flen=-1; \
    (ddfs_fh)->fbuf=NULL
/*
 *
 */
#define debug_ddfs_fh_t(ddfs_fh) \
    { \
        log_debug("ddfs_fh_t at %p {", (ddfs_fh)); \
        log_debug("    .fd=%d", (ddfs_fh)->fd); \
        log_debug("    .dp=%p", (ddfs_fh)->dp); \
        log_debug("    .append=%s", bool2str((ddfs_fh)->append)); \
        log_debug("    .rd_only=%s", bool2str((ddfs_fh)->rd_only)); \
        log_debug("    .flen=%d", (ddfs_fh)->flen); \
        log_debug("    .fbuf=%p", (ddfs_fh)->fbuf); \
        log_debug("    .iv="); \
        log_debug_hd((ddfs_fh)->iv, REC_IV_SZ); \
        log_debug("}"); \
    }
/*
 * FUSE file handle getter/setter as a macro
 */
#define DDFS_SET_FH(fi, ddfs_fh) (fi->fh=(uint64_t)(ddfs_fh))
#define DDFS_GET_FH(fi) ((ddfs_fh_t*)(fi->fh))
/*
 * Get file attributes.
 *
 * Similar to stat().  The 'st_dev' and 'st_blksize' fields are
 * ignored.  The 'st_ino' field is ignored except if the 'use_ino'
 * mount option is given.
 */
int ddfs_getattr(const char *path,
                 struct stat *stbuf);
/*
 * Check file access permissions
 *
 * This will be called for the access() system call.  If the
 * 'default_permissions' mount option is given, this method is not
 * called.
 *
 * This method is not called under Linux kernel versions 2.4.x
 *
 * Introduced in version 2.5
 */
int ddfs_access(const char *path,
                int mask);
/*
 * Create a file node
 *
 * This is called for creation of all non-directory, non-symlink
 * nodes.  If the filesystem defines a create() method, then for
 * regular files that will be called instead.
 */
int ddfs_mknod(const char *path,
               mode_t mode,
               dev_t rdev);
/*
 * Create a directory
 *
 * Note that the mode argument may not have the type specification
 * bits set, i.e. S_ISDIR(mode) can be false.  To obtain the
 * correct directory type bits use  mode|S_IFDIR
 */
int ddfs_mkdir(const char *path,
               mode_t mode);
/*
 * Remove a file
 */
int ddfs_unlink(const char *path);
/*
 * Remove an empty directory
 */
int ddfs_rmdir(const char *path);
/*
 * Rename a file
 */
int ddfs_rename(const char *from,
                const char *to);
/*
 * Change the permission bits of a file
 */
int ddfs_chmod(const char *path,
               mode_t mode);
/*
 * Change the owner and group of a file
 */
int ddfs_chown(const char *path,
               uid_t uid,
               gid_t gid);
/*
 * Set time-related metadata of a file
 */
int ddfs_utime(const char *path,
               struct utimbuf *times);

#ifdef HAVE_UTIMENSAT
/*
 * Change the access and modification times of a file with
 * nanosecond resolution
 *
 * This supersedes the old utime() interface.  New applications
 * should use this.
 *
 * See the utimensat(2) man page for details.
 *
 * Introduced in version 2.6
 */
int ddfs_utimens(const char *path,
                 const struct timespec ts[2],
                 struct fuse_file_info *fi);
#endif /* HAVE_UTIMENSAT */

/*
 * Create and open a file
 *
 * If the file does not exist, first create it with the specified
 * mode, and then open it.
 *
 * If this method is not implemented or under Linux kernel
 * versions earlier than 2.6.15, the mknod() and open() methods
 * will be called instead.
 *
 * Introduced in version 2.5
 */
int ddfs_create(const char *path,
                mode_t mode,
                struct fuse_file_info *fi);
/*
 * File open operation
 *
 * No creation (O_CREAT, O_EXCL) and by default also no
 * truncation (O_TRUNC) flags will be passed to open(). If an
 * application specifies O_TRUNC, fuse first calls truncate()
 * and then open(). Only if 'atomic_o_trunc' has been
 * specified and kernel version is 2.6.24 or later, O_TRUNC is
 * passed on to open.
 *
 * Unless the 'default_permissions' mount option is given,
 * open should check if the operation is permitted for the
 * given flags. Optionally open may also return an arbitrary
 * filehandle in the fuse_file_info structure, which will be
 * passed to all file operations.
 *
 * Changed in version 2.2
 */
int ddfs_open(const char *path,
              struct fuse_file_info *fi);
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
              struct fuse_file_info *fi);
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
                  struct fuse_file_info *fi);
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
               struct fuse_file_info *fi);
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
                   struct fuse_file_info *fi);
/*
 * Get file system statistics
 *
 * The 'f_frsize', 'f_favail', 'f_fsid' and 'f_flag' fields are ignored
 *
 * Replaced 'struct statfs' parameter with 'struct statvfs' in
 * version 2.5
 */
int ddfs_statfs(const char *path,
                struct statvfs *stbuf);
/*
 * Release an open file
 *
 * Release is called when there are no more references to an open
 * file: all file descriptors are closed and all memory mappings
 * are unmapped.
 *
 * For every open() call there will be exactly one release() call
 * with the same flags and file descriptor.  It is possible to
 * have a file opened more than once, in which case only the last
 * release will mean, that no more reads/writes will happen on the
 * file.  The return value of release is ignored.
 *
 * Changed in version 2.2
 */
int ddfs_release(const char *path,
                 struct fuse_file_info *fi);
/*
 * Synchronize file contents
 *
 * If the datasync parameter is non-zero, then only the user data
 * should be flushed, not the meta data.
 *
 * Changed in version 2.2
 */
int ddfs_fsync(const char *path,
               int isdatasync,
               struct fuse_file_info *fi);
/*
 * Get attributes from an open file
 *
 * This method is called instead of the getattr() method if the
 * file information is available.
 *
 * Currently this is only called after the create() method if that
 * is implemented (see above).  Later it may be called for
 * invocations of fstat() too.
 *
 * Introduced in version 2.5
 */
int ddfs_fgetattr(const char *path,
                  struct stat *st,
                  struct fuse_file_info *fi);
/*
 * Open directory
 *
 * Unless the 'default_permissions' mount option is given,
 * this method should check if opendir is permitted for this
 * directory. Optionally opendir may also return an arbitrary
 * filehandle in the fuse_file_info structure, which will be
 * passed to readdir, closedir and fsyncdir.
 *
 * Introduced in version 2.3
 */
int ddfs_opendir(const char *path,
                 struct fuse_file_info *fi);
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
                 struct fuse_file_info *fi);
/*
 * Synchronize directory contents
 *
 * If the datasync parameter is non-zero, then only the user data
 * should be flushed, not the meta data
 *
 * Introduced in version 2.3
 */
int ddfs_fsyncdir(const char *path,
                  int isdatasync,
                  struct fuse_file_info *fi);
/*
 * Release directory
 *
 * Introduced in version 2.3
 */
int ddfs_releasedir(const char *path,
                    struct fuse_file_info *fi);
/*
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
                   struct fuse_file_info *fi);
#ifdef HAVE_SETXATTR
/*
 * Set extended attributes
 */
int ddfs_setxattr(const char *path,
                  const char *name,
                  const char *value,
                  size_t size,
                  int flags);
/*
 * Get extended attributes
 */
int ddfs_getxattr(const char *path,
                  const char *name,
                  char *value,
                  size_t size);
/*
 * List extended attributes
 */
int ddfs_listxattr(const char *path,
                   char *list,
                   size_t size);
/*
 * Remove extended attributes
 */
int ddfs_removexattr(const char *path,
                     const char *name);
#endif /* HAVE_SETXATTR */

#endif /* __DDFS_H__ */
