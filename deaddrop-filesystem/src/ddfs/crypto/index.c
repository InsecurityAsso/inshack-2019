#include "log.h"
#include "path.h"
#include "index.h"
#include "record.h"
#include "crypto.h"
#include "convert.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

/*
 * Create ddfs.idx file in specified directory
 *
 * /!\ Expects OBFUSCATED dirpath /!\
 */
bool_t index_create(const char *dirpath)
{
    log_in("dirpath=%s", dirpath);
    bool_t err;
    int fd=-1;
    char *idx_path=NULL;
    index_t index;
    /**/
    IF_NOK(path_exists(dirpath)) {
        log_error("non existant dirpath: %s", dirpath);
        NOK_GOTO(end);
    }
    IF_NOK(path_join(dirpath, IDX_FILENAME, &idx_path)) {
        log_error("failed to allocate index file path!");
        NOK_GOTO(end);
    }
    IF_OK(path_exists(idx_path)) {
        log_info("an index file already exists in dirpath: %s", dirpath);
        OK_GOTO(end);
    }
    log_debug("creat(idx_path=%s,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)",
              idx_path);
    fd=creat(idx_path, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
    if(fd<0) {
        log_errno("failed to create index file!");
        NOK_GOTO(end);
    }
    memset(&index, 0, sizeof(index_t));
    init_index_hdr(&(index.header));
    if(write(fd, &index, sizeof(index_t))!=sizeof(index_t))  {
        log_errno("failed to write index file!");
        NOK_GOTO(end);
    }
    err=OK;
end:
    if(fd>=0) {
        IF_NOK(close(fd)) {
            log_errno("failed to close index file: fd=%d", fd);
        }
    }
    FREE(idx_path);
    log_out_err();
    return err;
}
/*
 * Test if ddfs.idx file exists in specified directory
 *
 * /!\ Expects OBFUSCATED dirpath /!\
 */
bool_t index_exists(const char *dirpath)
{
    log_in("dirpath=%s", dirpath);
    bool_t err;
    char *idx_path=NULL;
    /**/
    IF_NOK(path_exists(dirpath)) {
        NOK_GOTO(end);
    }
    IF_NOK(path_join(dirpath, IDX_FILENAME, &idx_path)) {
        log_error("failed to allocate index file path!");
        NOK_GOTO(end);
    }
    IF_NOK(path_exists(idx_path)) {
        NOK_GOTO(end);
    }
    err=OK;
end:
    FREE(idx_path);
    log_out_err();
    return err;
}
/*
 * Obtaining ddfs.idx file descriptor and map the file in memory for future use
 *
 * /!\ Expects OBFUSCATED dirpath /!\
 *
 * Always call index_close() on an index_hd_t
 */
bool_t index_open(const char *dirpath,
                  index_hd_t *idx_hd)
{
    log_in("dirpath=%p,idx_hd=%p", dirpath, idx_hd);
    bool_t err;
    int fd=-1;
    char *idx_path=NULL;
    void *addr=NULL;
    /**/
    IF_NOK(path_join(dirpath, IDX_FILENAME, &idx_path)) {
        log_error("failed to allocate index file path!");
        NOK_GOTO(end);
    }
    log_debug("open(idx_path=%s, O_RDWR)", idx_path);
    fd=open(idx_path, O_RDWR);
    if(fd<0) {
        log_errno("failed to open index file!");
        NOK_GOTO(end);
    }
    log_debug("mmap(NULL,%d,PROT_READ|PROT_WRITE,MAP_PRIVATE,%d,0)",
              sizeof(index_t), fd);
    addr=mmap(NULL, sizeof(index_t), PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
    if(addr==MAP_FAILED) {
        log_errno("failed to map index file!");
        NOK_GOTO(end);
    }
    idx_hd->fd=fd;
    idx_hd->index=(index_t*)addr;
    err=OK;
end:
    if(err) {
        if(fd>=0) {
            IF_NOK(close(fd)) {
                log_errno("failed to close index file: fd=%d", fd);
            }
        }
        if(addr!=NULL) {
            IF_NOK(munmap(addr, sizeof(index_t))) {
                log_errno("failed to unmap file from memory.");
            }
        }
    }
    FREE(idx_path);
    log_out_err();
    return err;
}
/*
 * Unmap ddfs.idx file from memory and close underlying file descriptor
 *
 * /!\ idx_hd SHALL NOT BE USED AFTER A CALL TO THIS FUNCTION /!\
 */
bool_t index_close(index_hd_t *idx_hd)
{
    log_in("idx_hd=%p", idx_hd);
    bool_t err=OK;
    /**/
    if(idx_hd->fd<0||idx_hd->index==NULL) {
        log_debug("called on an invalid index handle.");
        OK_GOTO(end);
    }
    log_debug("write(fd=%d,index=%p,size=%d)",
              idx_hd->fd, idx_hd->index, sizeof(index_t));
    if(write(idx_hd->fd, idx_hd->index, sizeof(index_t))!=sizeof(index_t)) {
        log_errno("failed to write index back to file.");
        err=NOK;
    }
    log_debug("munmap(index=%p,length=%d)", idx_hd->index, sizeof(index_t));
    IF_NOK(munmap((void *)idx_hd->index, sizeof(index_t))) {
        log_errno("failed to unmap index file! Closing underlying file anyway.");
        err=NOK;
    }
    log_debug("close(fd=%d)", idx_hd->fd);
    IF_NOK(close(idx_hd->fd)) {
        log_errno("failed to close index file: fd=%d", idx_hd->fd);
        err=NOK;
    }
    idx_hd->index=NULL;
    idx_hd->fd=-1;
end:
    log_out_err();
    return err;
}
/*
 * Unlink ddfs.idx file from specified directory
 *
 * /!\ Expects OBFUSCATED dirpath /!\
 */
bool_t index_unlink(const char *dirpath)
{
    log_in("dirpath=%s", dirpath);
    bool_t err;
    char *idx_path=NULL;
    /**/
    IF_NOK(path_join(dirpath, IDX_FILENAME, &idx_path)) {
        log_error("failed to allocate index file path!");
        NOK_GOTO(end);
    }
    log_debug("unlinking %s", idx_path);
    IF_NOK(unlink(idx_path)) {
        log_errno("failed to remove index file from dirpath: %s", dirpath);
        NOK_GOTO(end);
    }
    err=OK;
end:
    FREE(idx_path);
    log_out_err();
    return err;
}
/*
 * Find an available record slot within given index
 */
bool_t index_alloc_record(const index_hd_t *idx_hd,
                          record_t **record)
{
    log_in("idx_hd=%p,record=%p", idx_hd, record);
    bool_t err;
    int k;
    record_t *lrecord;
    /**/
    for(k=0;k<IDX_MAX_RECORD;k++) {
        lrecord=&(idx_hd->index->records[k]);
        IF_NOK(REC_IS_USED(lrecord)) {
            REC_SET_USED(lrecord);
            (*record)=lrecord;
            err=OK;
            goto end;
        }
    }
    log_error("failed to find a free record slot in given index.");
    err=NOK;
end:
    log_out_err();
    return err;
}
/*
 * Retrieve an existing record by name
 *
 * /!\ Expects CLEAR basename /!\
 */
bool_t index_get_record_by_name(const index_hd_t *idx_hd,
                                const char *basename,
                                record_t **record)
{
    log_in("idx_hd=%p,basename=%s,record=%p", idx_hd, basename, record);
    bool_t err;
    uchar_t hash[REC_HASH_SZ];
    /**/
    IF_NOK(crypto_hash_name(basename, hash)) {
        log_error("failed to hash name.");
        NOK_GOTO(end);
    }
    IF_NOK(index_get_record_by_hash(idx_hd, hash, record)) {
        log_error("record not found using hash.");
        NOK_GOTO(end);
    }
    err=OK;
end:
    log_out_err();
    return err;
}
/*
 * Retrieve an existing record by hash
 *
 * /!\ Expects CLEAR basename /!\
 */
bool_t index_get_record_by_hash(const index_hd_t *idx_hd,
                                const uchar_t hash[REC_HASH_SZ],
                                record_t **record)
{
    log_in("idx_hd=%p,hash=%p,record=%p", idx_hd, hash, record);
    bool_t err;
    int k;
    record_t *lrecord;
    /**/
    for(k=0;k<IDX_MAX_RECORD;k++) {
        lrecord=&(idx_hd->index->records[k]);
        IF_NOK(REC_IS_USED(lrecord)) {
            continue;
        }
        IF_OK(memcmp(hash, lrecord->hash, REC_HASH_SZ)) {
            log_debug("found record: %p", lrecord);
            (*record)=lrecord;
            OK_GOTO(end);
        }
    }
    log_error("failed to find record for given path.");
    err=NOK;
end:
    log_out_err();
    return err;
}
/*
 * 'Delete' an existing record
 *
 * /!\ Expects CLEAR basename /!\
 */
bool_t index_del_record(const index_hd_t *idx_hd,
                     const char *basename)
{
    log_in("idx_hd=%p,basename=%s", idx_hd, basename);
    bool_t err;
    int k;
    record_t *lrecord;
    uchar_t hash[REC_HASH_SZ];
    /**/
    IF_NOK(crypto_hash_name(basename, hash)) {
        log_error("failed to hash name.");
        NOK_GOTO(end);
    }
    for(k=0;k<IDX_MAX_RECORD;k++) {
        lrecord=&(idx_hd->index->records[k]);
        IF_NOK(REC_IS_USED(lrecord)) {
            continue;
        }
        IF_OK(memcmp(hash, lrecord->hash, REC_HASH_SZ)) {
            REC_REM_USED(lrecord);
            err=OK;
            goto end;
        }
    }
    log_error("failed to find record for given path.");
    err=NOK;
end:
    log_out_err();
    return err;
}
/*
 * Rename an existing record
 *
 * /!\ Expects OBFUSCATED from_dir and to_dir /!\
 * /!\ Expects CLEAR from_name and to_name /!\
 */
bool_t index_rename_record(const char *from_dir,
                           const char *to_dir,
                           const char *from_name,
                           const char *to_name,
                           const user_t *user)
{
    log_in("from_dir=%s,to_dir=%s,from_name=%s,to_name=%s,user=%p",
           from_dir, to_dir, from_name, to_name, user);
    bool_t err;
    int to_name_len;
    record_t *from_rec, *to_rec;
    index_hd_t idx_from_hd, idx_to_hd;
    /**/
    init_index_hd(&idx_from_hd);
    init_index_hd(&idx_to_hd);
    to_name_len=strlen(to_name);
    if(to_name_len>REC_NAME_SZ) {
        log_error("index record rename failed: destination name is too long.");
        NOK_GOTO(end);
    }
    IF_NOK(index_open(from_dir, &idx_from_hd)) {
        log_error("index record rename failed: failed to open from_dir index.");
        NOK_GOTO(end);
    }
    /* rename record in the same directory  */
    IF_NOK(index_get_record_by_name(&idx_from_hd, from_name, &from_rec)) {
        log_error("index record rename failed: failed to get record from index.");
        NOK_GOTO(cleanup);
    }
    IF_NOK(record_decrypt(from_rec, user)) {
        log_error("index record rename failed: failed to decrypt record.");
        NOK_GOTO(cleanup);
    }
    memset(from_rec->name, 0, REC_NAME_SZ);
    memcpy(from_rec->name, to_name, MIN(REC_NAME_SZ, to_name_len));
    IF_NOK(record_encrypt(from_rec, user)) {
        log_error("index record rename failed: failed to encrypt record.");
        NOK_GOTO(cleanup);
    }
    if(strcmp(from_dir, to_dir)!=0) {
        /* move renamed record to another directory  */
        IF_NOK(index_open(to_dir, &idx_to_hd)) {
            log_error("index record rename failed: failed to open to_dir index.");
            NOK_GOTO(cleanup);
        }
        IF_NOK(index_alloc_record(&idx_to_hd, &to_rec)) {
            log_error("index record rename failed: failed to allocate record.");
            NOK_GOTO(cleanup);
        }
        memcpy(to_rec, from_rec, sizeof(record_t));
    }
    err=OK;
cleanup:
    IF_NOK(index_close(&idx_from_hd)) {
        log_warn("index record rename failed: failed to close from_dir index.");
    }
    IF_NOK(index_close(&idx_to_hd)) {
        log_warn("index record rename failed: failed to close to_dir index.");
    }
end:
    log_out_err();
    return err;
}
/*
 * Create a record in the index located in edirname
 */
bool_t index_create_file(const char *edirname,
                         const char *basename,
                         int32_t rec_type,
                         const user_t *user)
{
    log_in("edirname=%s,basename=%s,rec_type=%d,user=%p",
           edirname, basename, rec_type, user);
    bool_t err;
    record_t *record;
    index_hd_t idx_hd;
    /**/
    init_index_hd(&idx_hd);
    IF_NOK(index_open(edirname, &idx_hd)) {
        log_error("failed to open index.");
        NOK_GOTO(end);
    }
    IF_NOK(index_alloc_record(&idx_hd, &record)) {
        log_error("failed to allocate new record.");
        NOK_GOTO(end);
    }
    IF_NOK(record_init(record, user, basename, rec_type)) {
        log_error("failed to initialize record.");
        NOK_GOTO(end);
    }
    IF_NOK(record_encrypt(record, user)) {
        log_error("failed to encrypt record.");
        NOK_GOTO(end);
    }
    err=OK;
end:
    IF_NOK(index_close(&idx_hd)) {
        log_warn("failed to open index.");
    }
    log_out_err();
    return err;
}
/*
 * Open the file, decrypt it in memory and allocate and fill given ddfs_fh
 */
bool_t index_open_file(const char *epath,
                       const char *basename,
                       int flags,
                       mode_t mode,
                       const user_t *user,
                       ddfs_fh_t *ddfs_fh)
{
    log_in("epath=%s,basename=%s,flags=%#x,mode=%d,user=%p,ddfs_fh=%p",
           epath, basename, flags, mode, user, ddfs_fh);
    bool_t err, mapped=NOK, opened=NOK, append, rd_only;
    int fd;
    uint_t flen;
    void *addr;
    char *lepath, *edname;
    uchar_t *fbuf;
    record_t *record;
    index_hd_t idx_hd;
    struct stat st;
    /**/
    log_debug("initial open flags were: %#x", flags);
    append=((flags&O_APPEND)==O_APPEND?OK:NOK);
    rd_only=((flags&0x3)==O_RDONLY?OK:NOK);
    if((flags&O_CREAT)==O_CREAT) {
        log_debug("creation flag set.");
        if((fd=open(epath, O_CREAT|O_WRONLY, mode))<0) {
            log_errno("file creation failed.");
            NOK_GOTO(end);
        }
        close(fd);
    }
    flags&=0x3;
    if(flags==O_WRONLY) {
        log_debug("overriding write only with read/write flag.");
        flags=O_RDWR;
    }
    log_debug("overrided open flags are: %#x", flags);
    init_index_hd(&idx_hd);
    /* open file & get its size */
    log_debug("open(epath=%s,flags=%#x,mode=%d)", epath, flags, mode);
    if((fd=open(epath, flags, mode))<0) {
        log_errno("failed to create file: %s", epath);
        NOK_GOTO(end);
    }
    opened=OK;
    log_debug("fstat(fd=%d,st=%p)", fd, &st);
    IF_NOK(fstat(fd, &st)) {
        log_errno("failed to get file statistics.");
        NOK_GOTO(end);
    }
    flen=st.st_size;
    /* open index */
    lepath=strdup(epath);
    edname=path_dirname(lepath);
    IF_NOK(index_open(edname, &idx_hd)) {
        log_error("failed to open index.");
        NOK_GOTO(end);
    }
    /* find record inside index */
    IF_NOK(index_get_record_by_name(&idx_hd, basename, &record)) {
        log_error("failed to find record by name.");
        NOK_GOTO(end);
    }
    if(flen>0) {
        /* map file in private memory */
        log_debug("mmap(NULL,flen=%d,PROT_READ|PROT_WRITE,MAP_PRIVATE,fd=%d,0)", flen, fd);
        addr=mmap(NULL, flen, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
        if(addr==MAP_FAILED) {
            log_errno("failed to map file in memory.");
            NOK_GOTO(end);
        }
        mapped=OK;
        fbuf=(uchar_t*)addr;
        /* decrypt file in memory */
        IF_NOK(crypto_decrypt_buffer(user->enc_key, USR_ENC_KEY_SZ,
                                     record->iv, REC_IV_SZ,
                                     fbuf, flen)) {
            log_error("failed to decrypt file in memory.");
            NOK_GOTO(end);
        }
    } else {
        /* it is meaningless to map an empty file */
        fbuf=NULL;
    }
    /* success */
    ddfs_fh->fd=fd;
    ddfs_fh->flen=flen;
    ddfs_fh->fbuf=fbuf;
    ddfs_fh->append=append;
    ddfs_fh->rd_only=rd_only;
    memcpy(ddfs_fh->iv, record->iv, REC_IV_SZ);
    err=OK;
end:
    if(err) {
        IF_OK(mapped) {
            log_debug("munmap(addr=%p,flen=%d)", addr, flen);
            IF_NOK(munmap(addr, flen)) {
                log_warn("failed to unmap file from memory.");
            }
        }
        IF_OK(opened) {
            log_debug("close(fd=%d)", fd);
            IF_NOK(close(fd)) {
                log_warn("failed to close underlying file.");
            }
        }
    }
    log_out_err();
    return err;
}
/*
 * Release all resources of given ddfs_fh, invalidating it.
 */
bool_t index_close_file(ddfs_fh_t *ddfs_fh,
                        const user_t *user)
{
    log_in("ddfs_fh=%p,user=%p", ddfs_fh, user);
    bool_t err=OK;
    /**/
    if(ddfs_fh->fd<0) {
        log_debug("called on an invalid ddfs file handle.");
        OK_GOTO(end);
    }
    if(ddfs_fh->flen==0) {
        log_debug("skipping write operations, closing directly.");
        goto skip_unmap;
    }
    IF_OK(ddfs_fh->rd_only) {
        log_debug("read only file: skip writing buffer back to file.");
        goto skip_write;
    }
    /* encrypt file in memory */
    IF_NOK(crypto_encrypt_buffer(user->enc_key, USR_ENC_KEY_SZ,
                                 ddfs_fh->iv, REC_IV_SZ,
                                 ddfs_fh->fbuf, ddfs_fh->flen)) {
        log_error("failed to encrypt file in memory. Index updates will be lost.");
        err=NOK;
        goto skip_write; /* do not write if encryption failed */
    }
    /* write encrypted data to file */
    log_debug("write(fd=%d,fbuf=%p,flen=%d)", ddfs_fh->fd, ddfs_fh->fbuf, ddfs_fh->flen);
    if(write(ddfs_fh->fd, ddfs_fh->fbuf, ddfs_fh->flen)!=ddfs_fh->flen) {
        log_errno("failed to write encrypted data to file.");
        err=NOK;
    }
skip_write:
    /* unmap file from memory */
    log_debug("munmap(fbuf=%p,flen=%d)", ddfs_fh->fbuf, ddfs_fh->flen);
    IF_NOK(munmap(ddfs_fh->fbuf, ddfs_fh->flen)) {
        log_errno("failed to unmap file! Closing underlying file anyway.");
        err=NOK;
    }
skip_unmap:
    /* close file descriptor */
    log_debug("close(fd=%d)", ddfs_fh->fd);
    IF_NOK(close(ddfs_fh->fd)) {
        log_errno("failed to close index file: fd=%d", ddfs_fh->fd);
        err=NOK;
    }
end:
    log_out_err();
    return err;
}
