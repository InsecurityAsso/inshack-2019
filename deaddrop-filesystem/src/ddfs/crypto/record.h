#ifndef __RECORD_H__
#define __RECORD_H__

#include "types.h"
#include "macro.h"
#include <stdint.h>

#define USR_ENC_KEY_SZ  16

#define REC_SZ          256                 /*  */
#define REC_HASH_SZ     32                  /*  */
#define REC_NAME_SZ     176                 /*  */
#define REC_IV_SZ       16                  /*  */
#define REC_SEAL        "-!-RECORDSEAL-!-"  /*  */
#define REC_SEAL_SZ     16                  /*  */
/* record types */
#define REC_TYPE_REGULAR    0x00000001   /* regular file */
#define REC_TYPE_DIRECTORY  0x00000002   /* directory */
/* record flags */
#define REC_FLAG_NONE       0x00000000
#define REC_FLAG_ENCRYPTED  ((1<<0)&0xffffffff)
#define REC_FLAG_USED       ((1<<1)&0xffffffff)

#define REC_RST_FLAG(flags)         ((flags)=REC_FLAG_NONE)
#define REC_SET_FLAG(flags, flag)   ((flags)|=(flag))
#define REC_REM_FLAG(flags, flag)   ((flags)&=~(flag))
#define REC_HAS_FLAG(flags, flag)   ((((flags)&(flag))==(flag))?OK:NOK)

#define REC_IS_USED(record)         (REC_HAS_FLAG((record)->flags, REC_FLAG_USED))
#define REC_IS_ENCRYPTED(record)    (REC_HAS_FLAG((record)->flags, REC_FLAG_ENCRYPTED))

#define REC_SET_USED(record)        (REC_SET_FLAG((record)->flags, REC_FLAG_USED))
#define REC_SET_ENCRYPTED(record)   (REC_SET_FLAG((record)->flags, REC_FLAG_ENCRYPTED))

#define REC_REM_USED(record)        (REC_REM_FLAG((record)->flags, REC_FLAG_USED))
#define REC_REM_ENCRYPTED(record)   (REC_REM_FLAG((record)->flags, REC_FLAG_ENCRYPTED))

/*
 * User representation
 */
typedef struct user_s {
    char *username;                     /*  */
    int32_t username_len;               /*  */
    char *password;                     /*  */
    int32_t password_len;               /*  */
    uchar_t enc_key[USR_ENC_KEY_SZ];    /*  */
} user_t;
/*
 * Print user structure fields for debug purpose
 */
#define debug_user_t(user) \
    { \
        log_debug("user_t at %p {", (user)); \
        log_debug("    .username=%s", (user)->username); \
        log_debug("    .password=*******"); \
        log_debug("}"); \
    }
/*
 * Record representation
 */
typedef struct record_s {
    uchar_t hash[REC_HASH_SZ];  /*  */
    uchar_t name[REC_NAME_SZ];  /*  */
    uchar_t iv[REC_IV_SZ];      /*  */
    uchar_t seal[REC_SEAL_SZ];  /*  */
    int32_t type;               /*  */
    int32_t flags;              /*  */
    int32_t reserved[2];        /*  */
} record_t;
ASSERT(sizeof(record_t)==REC_SZ, record_h)
/*
 * Print record structure fields for debug purpose
 */
#define debug_record_t(record) \
    { \
        log_debug("record_t at %p {", (record)); \
        log_debug("    .hash="); \
        log_debug_hd((record)->hash, REC_HASH_SZ); \
        log_debug("    .type=%d", (record)->type); \
        log_debug("    .name="); \
        log_debug_hd((record)->name, REC_NAME_SZ); \
        log_debug("    .iv="); \
        log_debug_hd((record)->iv, REC_IV_SZ); \
        log_debug("    .seal="); \
        log_debug_hd((record)->seal, REC_SEAL_SZ); \
        log_debug("    .type=%#010x", (record)->type); \
        log_debug("    .flags=%#010x", (record)->flags); \
        log_debug("}"); \
    }
/*
 * Initializes record fields
 */
int record_init(record_t *record,
                const user_t *user,
                const char *basename,
                int32_t type);
/*
 * Encrypt record fields (in place)
 */
int record_encrypt(record_t *record,
                   const user_t *user);
/*
 * Decrypt record fields (in place)
 */
int record_decrypt(record_t *record,
                   const user_t *user);

#endif /* __RECORD_H__ */
