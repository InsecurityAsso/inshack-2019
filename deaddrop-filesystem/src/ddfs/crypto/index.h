#ifndef __INDEX_H__
#define __INDEX_H__

#include "ddfs.h"
#include "macro.h"
#include "record.h"

#define IDX_MAGIC       "DDFS.IDX"     /* header magic value */
#define IDX_MAGIC_SZ    8              /* header magic value size */
#define IDX_MAJOR       1              /* major version */
#define IDX_MINOR       0              /* minor version */
#define IDX_PATCH       0              /* patch version */
#define IDX_VERSION_SZ  3*4            /* size in bytes occupied to version */
#define IDX_FILENAME    "ddfs.idx"     /* index filename */
#define IDX_MAX_RECORD  4095           /* max number of records in an index */
/*
 *
 */
typedef struct index_hdr_s {
    uchar_t magic[IDX_MAGIC_SZ];                            /*  */
    int32_t major;                                          /*  */
    int32_t minor;                                          /*  */
    int32_t patch;                                          /*  */
    uchar_t reserved[REC_SZ-IDX_MAGIC_SZ-IDX_VERSION_SZ];   /*  */
} index_hdr_t;
ASSERT(sizeof(index_hdr_t)==REC_SZ, index_h);
#define init_index_hdr(index_hdr) \
    memcpy((index_hdr)->magic, IDX_MAGIC, IDX_MAGIC_SZ); \
    (index_hdr)->major=IDX_MAJOR; \
    (index_hdr)->minor=IDX_MINOR; \
    (index_hdr)->patch=IDX_PATCH
/*
 *
 */
#define debug_index_hdr_t(idx_hdr) \
    { \
        log_debug("index_hdr_t at %p {", (idx_hdr)); \
        log_debug("    .magic="); \
        log_debug_hd(magic, IDX_MAGIC_SZ); \
        log_debug("}"); \
    }
/*
 *
 */
typedef struct index_s {
    index_hdr_t header;                     /*  */
    record_t records[IDX_MAX_RECORD];       /*  */
} index_t;
ASSERT(sizeof(index_t)==(REC_SZ*(IDX_MAX_RECORD+1)), index_h)
/*
 *
 */
#define debug_index_t(idx) \
    { \
        int i; \
        log_debug("index_t at %p {", (idx)); \
        log_debug("    .header="); \
        debug_index_hdr_t((idx)->header); \
        log_debug("    .records"); \
        for(i=0;i<IDX_MAX_RECORD;i++) { \
            log_debug("        [%d]", i); \
            debug_record_t(&((idx)->records[i])) \
        } \
        log_debug("}"); \
    }
/*
 *
 */
typedef struct index_hd_s {
    int fd;                     /*  */
    index_t *index;             /*  */
} index_hd_t;
#define init_index_hd(idx_hd) \
    (idx_hd)->fd=-1; \
    (idx_hd)->index=NULL
/*
 *
 */
#define debug_index_hd_t(idx_hd) \
    { \
        log_debug("index_hd_t at %p {", (idx_hd)); \
        log_debug("    .fd=%d", (idx_hd)->fd); \
        log_debug("    .index=%p", (idx_hd)->index); \
        log_debug("}"); \
    }
/*
 * Create ddfs.idx file in specified directory
 *
 * /!\ Expects OBFUSCATED dirpath /!\
 */
bool_t index_create(const char *dirpath);
/*
 * Test if ddfs.idx file exists in specified directory
 *
 * /!\ Expects OBFUSCATED dirpath /!\
 */
bool_t index_exists(const char *dirpath);
/*
 * Obtaining ddfs.idx file descriptor and map the file in memory for future use
 *
 * Always call index_close() on an index_hd_t
 *
 * /!\ Expects OBFUSCATED dirpath /!\
 */
bool_t index_open(const char *dirpath,
                  index_hd_t *idx_hd);
/*
 * Unmap ddfs.idx file from memory and close underlying file descriptor
 *
 * /!\ idx_hd SHALL NOT BE USED AFTER A CALL TO THIS FUNCTION /!\
 */
bool_t index_close(index_hd_t *idx_hd);
/*
 * Unlink ddfs.idx file from specified directory
 *
 * /!\ Expects OBFUSCATED dirpath /!\
 */
bool_t index_unlink(const char *dirpath);
/*
 * Find an available record slot within given index
 */
bool_t index_alloc_record(const index_hd_t *idx_hd,
                          record_t **record);
/*
 * Retrieve an existing record by name
 *
 * /!\ Expects CLEAR basename /!\
 */
bool_t index_get_record_by_name(const index_hd_t *idx_hd,
                                const char *basename,
                                record_t **record);
/*
 * Retrieve an existing record by hash
 *
 * /!\ Expects CLEAR basename /!\
 */
bool_t index_get_record_by_hash(const index_hd_t *idx_hd,
                                const uchar_t hash[REC_HASH_SZ],
                                record_t **record);
/*
 * 'Delete' an existing record
 *
 * /!\ Expects CLEAR basename /!\
 */
bool_t index_del_record(const index_hd_t *idx_hd,
                        const char *basename);
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
                           const user_t *user);
/*
 * Create a record in the index located in edirname
 */
bool_t index_create_file(const char *edirname,
                         const char *basename,
                         int32_t rec_type,
                         const user_t *user);
/*
 * Open the file, decrypt it in memory and allocate and fill given ddfs_fh
 */
bool_t index_open_file(const char *epath,
                       const char *basename,
                       int flags,
                       mode_t mode,
                       const user_t *user,
                       ddfs_fh_t *ddfs_fh);
/*
 * Release all resources of given ddfs_fh, invalidating it.
 */
bool_t index_close_file(ddfs_fh_t *ddfs_fh,
                        const user_t *user);

#endif /* __INDEX_H__ */
