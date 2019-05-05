#include "log.h"
#include "macro.h"
#include "crypto.h"
#include "record.h"

#include <stdlib.h>
#include <string.h>

/*
 * Initializes record fields
 */
int record_init(record_t *record,
                const user_t *user,
                const char *basename,
                int32_t type)
{
    log_in("record=%p,user=%p,basename=%s,type=%d", record, user, basename, type);
    int err, len;
    /**/
    len=strlen(basename);
    if(len==0||len>REC_NAME_SZ) {
        log_error("record name length must be comprised in ]0,192]!");
        NOK_GOTO(end);
    }
    memset(record, 0, sizeof(record_t));
    IF_NOK(crypto_init_hash(basename, record)) {
        log_error("failed to init hash.");
        NOK_GOTO(end);
    }
    record->type=type;
    memcpy(record->name, basename, REC_NAME_SZ);
    IF_NOK(crypto_init_iv(record)) {
        log_error("failed to init IV.");
        NOK_GOTO(end);
    }
    memcpy(record->seal, REC_SEAL, REC_SEAL_SZ);
    IF_NOK(crypto_init_seal(user, record)) {
        log_error("failed to encrypt record seal!");
        NOK_GOTO(end);
    }
    REC_RST_FLAG(record->flags);
    REC_SET_FLAG(record->flags, REC_FLAG_USED);
    err=OK;
end:
    log_out_err();
    return err;
}
/*
 * Encrypt record fields (in place)
 */
int record_encrypt(record_t *record,
                   const user_t *user)
{
    log_in("record=%p,user=%p", record, user);
    int err;
    /**/
    IF_OK(REC_IS_ENCRYPTED(record)) {
        log_error("this record is already encrypted.");
        NOK_GOTO(end);
    }
    IF_NOK(crypto_check_seal(user, record)) {
        log_error("this record does not belong to you: %s", user->username);
        NOK_GOTO(end);
    }
    crypto_encrypt_name(user, record);
    REC_SET_ENCRYPTED(record);
    err=OK;
end:
    log_out_err();
    return err;
}
/*
 * Decrypt record fields (in place)
 */
int record_decrypt(record_t *record,
                   const user_t *user)
{
    log_in("record=%p,user=%p", record, user);
    int err;
    /**/
    IF_NOK(REC_IS_ENCRYPTED(record)) {
        log_error("this record is already decrypted.");
        NOK_GOTO(end);
    }
    IF_NOK(crypto_check_seal(user, record)) {
        log_error("this record does not belong to you: %s", user->username);
        NOK_GOTO(end);
    }
    crypto_decrypt_name(user, record);
    REC_REM_ENCRYPTED(record);
    err=OK;
end:
    log_out_err();
    return err;
}
