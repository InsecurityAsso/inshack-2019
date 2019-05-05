#include "log.h"
#include "macro.h"
#include "crypto.h"

#include <string.h>
#include <gcrypt.h>

static bool_t ginitialized=NOK;

static void log_gcry_error(gcry_error_t err) {
    log_error("libgcrypt error: %s/%s", gcry_strerror(err), gcry_strsource(err));
}

bool_t crypto_init(user_t *user)
{
    log_in("user=%p", user);
    bool_t err;
    gcry_error_t nok;
    /**/
    IF_OK(ginitialized) {
        log_error("double initialization attempt caught.");
        NOK_GOTO(end);
    }
    /*
     * Version check should be the very first call because it
     * makes sure that important subsystems are initialized
     */
    if (!gcry_check_version(GCRYPT_VERSION)) {
        log_error("libgcrypt version mismatch!");
        NOK_GOTO(end);
    }
    /* Disable secure memory /!\ keys will be stored in insecure memory /!\ */
    gcry_control(GCRYCTL_DISABLE_SECMEM, 0);
    /* Tell Libgcrypt that initialization has completed */
    gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);
    /*
     * Perform key derivation
     * /!\ WARNING:
     *     - predictable salt used
     *     - SHA1 is weak
     *     - minimum iteration count recomended in NIST Special Publication 800-132
     *     - encryption key will be stored in insecure memory
     */
    IF_NOK_SET(gcry_kdf_derive(user->password,
                               user->password_len,
                               GCRY_KDF_PBKDF2, /* /!\ */
                               GCRY_MD_SHA1,
                               user->username, /* /!\ */
                               user->username_len,
                               1000, /* /!\ */
                               USR_ENC_KEY_SZ,
                               user->enc_key)) { /* /!\ */
        log_error("failed to derive encryption key from password.");
        log_gcry_error(nok);
        NOK_GOTO(end);
    }
    ginitialized=OK;
    err=OK;
end:
    log_out_err();
    return err;
}
/*
 *
 */
bool_t crypto_hash_name(const char *name,
                        uchar_t hash[REC_HASH_SZ])
{
    log_in("name=%s,hash=%p", name, hash);
    bool_t err;
    /**/
    IF_NOK(ginitialized) {
        log_error("initialize crypto first.");
        NOK_GOTO(end);
    }
    gcry_md_hash_buffer(GCRY_MD_SHA256, hash, name, strlen(name));
    err=OK;
end:
    log_out_err();
    return err;
}
/*
 *
 */
bool_t crypto_init_hash(const char *name,
                        record_t *record)
{
    log_in("name=%s,record=%p", name, record);
    bool_t err;
    /**/
    IF_NOK(ginitialized) {
        log_error("initialize crypto first.");
        NOK_GOTO(end);
    }
    err=crypto_hash_name(name, record->hash);
end:
    log_out_err();
    return err;
}
/*
 *
 */
bool_t crypto_init_iv(record_t *record)
{
    log_in("record=%p", record);
    bool_t err;
    /**/
    IF_NOK(ginitialized) {
        log_error("initialize crypto first.");
        NOK_GOTO(end);
    }
    gcry_create_nonce(record->iv, REC_IV_SZ);
    err=OK;
end:
    log_out_err();
    return err;
}
/*
 *
 */
bool_t crypto_init_seal(const user_t *user,
                        record_t *record)
{
    log_in("user=%p,record=%p", user, record);
    bool_t err;
    /**/
    IF_NOK(ginitialized) {
        log_error("initialize crypto first.");
        NOK_GOTO(end);
    }
    IF_NOK(crypto_encrypt_buffer(user->enc_key,
                                      USR_ENC_KEY_SZ,
                                      record->iv,
                                      REC_IV_SZ,
                                      record->seal,
                                      REC_SEAL_SZ)) {
        log_error("failed to encrypt seal!");
        NOK_GOTO(end);
    }
    err=OK;
end:
    log_out_err();
    return err;
}
/*
 *
 */
bool_t crypto_check_seal(const user_t *user,
                         const record_t *record)
{
    log_in("user=%p,record=%p", user, record);
    bool_t err;
    uchar_t seal[REC_SEAL_SZ];
    /**/
    IF_NOK(ginitialized) {
        log_error("initialize crypto first.");
        NOK_GOTO(end);
    }
    memcpy(seal, record->seal, REC_SEAL_SZ);
    IF_NOK(crypto_decrypt_buffer(user->enc_key,
                                  USR_ENC_KEY_SZ,
                                  record->iv,
                                  REC_IV_SZ,
                                  seal,
                                  REC_SEAL_SZ)) {
        log_error("failed to decrypt seal!");
        NOK_GOTO(end);
    }
    IF_NOK(memcmp(seal, REC_SEAL, REC_SEAL_SZ)) {
        log_error("seal check failed!");
        NOK_GOTO(end);
    }
    err=OK;
end:
    log_out_err();
    return err;
}
/*
 *
 */
bool_t crypto_encrypt_name(const user_t *user,
                           record_t *record)
{
    log_in("user=%p,record=%p", user, record);
    bool_t err;
    /**/
    IF_NOK(ginitialized) {
        log_error("initialize crypto first.");
        NOK_GOTO(end);
    }
    IF_NOK(crypto_encrypt_buffer(user->enc_key,
                                  USR_ENC_KEY_SZ,
                                  record->iv,
                                  REC_IV_SZ,
                                  record->name,
                                  REC_NAME_SZ)) {
        log_error("failed to encrypt seal!");
        NOK_GOTO(end);
    }
    err=OK;
end:
    log_out_err();
    return err;
}
/*
 *
 */
bool_t crypto_decrypt_name(const user_t *user,
                           record_t *record)
{
    log_in("user=%p,record=%p", user, record);
    bool_t err;
    /**/
    IF_NOK(ginitialized) {
        log_error("initialize crypto first.");
        NOK_GOTO(end);
    }
    IF_NOK(crypto_decrypt_buffer(user->enc_key,
                                  USR_ENC_KEY_SZ,
                                  record->iv,
                                  REC_IV_SZ,
                                  record->name,
                                  REC_NAME_SZ)) {
        log_error("failed to encrypt seal!");
        NOK_GOTO(end);
    }
    err=OK;
end:
    log_out_err();
    return err;
}
/*
 * 'in' is XORed in-place with 'xbuf'. 'xbuf' length must be greater or equal
 * to 'len', 'len' being the exact size of 'in' buffer.
 */
void _xor(uchar_t *in, int len, const uchar_t *xbuf)
{
    log_in("in=%p,len=%d,xbuf=%p", in, len, xbuf);
    int i;
    /**/
    for(i=0;i<len;i++) {
        in[i]^=xbuf[i];
    }
    log_out_void();
}
/*
 *
 */
bool_t crypto_encrypt_buffer(const uchar_t *k,
                             int klen,
                             const uchar_t *iv,
                             int ivlen,
                             uchar_t *buffer,
                             int buflen)
{
    log_in("k=%p,klen=%d,iv=%p,ivlen=%d,buffer=%p,buflen=%d", k, klen, iv, ivlen, buffer, buflen);
    bool_t err;
    int remaining;
    uchar_t xbuf[REC_IV_SZ];
    gcry_error_t nok;
    gcry_cipher_hd_t hd;
    /**/
    IF_NOK(ginitialized) {
        log_error("initialize crypto first.");
        NOK_GOTO(end);
    }
    remaining=buflen%CRYPT_BLK_SZ;
    if(remaining!=0) {
        buflen-=remaining;
        memcpy(xbuf, iv, REC_IV_SZ);
        IF_NOK(crypto_encrypt_buffer(k, klen, iv, ivlen, xbuf, REC_IV_SZ)) {
            log_error("failed to prepare xbuf.");
            NOK_GOTO(end);
        }
        /* Another vulnerability here... */
        _xor(buffer+buflen, remaining, xbuf);
    }
    if(buflen==0) {
        log_debug("encrypted a buffer smaller than a block.");
        OK_GOTO(end);
    }
    IF_NOK_SET(gcry_cipher_open(&hd, GCRY_CIPHER_AES, GCRY_CIPHER_MODE_CBC, 0)) {
        log_error("failed to open cipher handle!");
        log_gcry_error(nok);
        NOK_GOTO(end);
    }
    IF_NOK_SET(gcry_cipher_setkey(hd, k, klen)) {
        log_error("failed to set cipher key!");
        log_gcry_error(nok);
        NOK_GOTO(cleanup);
    }
    IF_NOK_SET(gcry_cipher_setiv(hd, iv, ivlen)) {
        log_error("failed to set cipher IV!");
        log_gcry_error(nok);
        NOK_GOTO(cleanup);
    }
    IF_NOK_SET(gcry_cipher_encrypt(hd, buffer, buflen, NULL, 0)) {
        log_error("failed to encrypt buffer!");
        log_gcry_error(nok);
        NOK_GOTO(cleanup);
    }
    err=OK;
cleanup:
    gcry_cipher_close(hd);
end:
    log_out_err();
    return err;
}
/*
 *
 */
bool_t crypto_decrypt_buffer(const uchar_t *k,
                             int klen,
                             const uchar_t *iv,
                             int ivlen,
                             uchar_t *buffer,
                             int buflen)
{
    log_in("k=%p,klen=%d,iv=%p,ivlen=%d,buffer=%p,buflen=%d", k, klen, iv, ivlen, buffer, buflen);
    bool_t err;
    int remaining;
    uchar_t xbuf[REC_IV_SZ];
    gcry_error_t nok;
    gcry_cipher_hd_t hd;
    /**/
    IF_NOK(ginitialized) {
        log_error("initialize crypto first.");
        NOK_GOTO(end);
    }
    remaining=buflen%CRYPT_BLK_SZ;
    if(remaining!=0) {
        buflen-=remaining;
        memcpy(xbuf, iv, REC_IV_SZ);
        IF_NOK(crypto_encrypt_buffer(k, klen, iv, ivlen, xbuf, REC_IV_SZ)) {
            log_error("failed to prepare xbuf.");
            NOK_GOTO(end);
        }
        _xor(buffer+buflen, remaining, xbuf);
    }
    if(buflen==0) {
        log_debug("encrypted a buffer smaller than a block.");
        OK_GOTO(end);
    }
    IF_NOK_SET(gcry_cipher_open(&hd, GCRY_CIPHER_AES, GCRY_CIPHER_MODE_CBC, 0)) {
        log_error("failed to open cipher handle!");
        log_gcry_error(nok);
        NOK_GOTO(end);
    }
    IF_NOK_SET(gcry_cipher_setkey(hd, k, klen)) {
        log_error("failed to set cipher key!");
        log_gcry_error(nok);
        NOK_GOTO(cleanup);
    }
    IF_NOK_SET(gcry_cipher_setiv(hd, iv, ivlen)) {
        log_error("failed to set cipher IV!");
        log_gcry_error(nok);
        NOK_GOTO(cleanup);
    }
    IF_NOK_SET(gcry_cipher_decrypt(hd, buffer, buflen, NULL, 0)) {
        log_error("failed to decrypt buffer!");
        log_gcry_error(nok);
        NOK_GOTO(cleanup);
    }
    err=OK;
cleanup:
    gcry_cipher_close(hd);
end:
    log_out_err();
    return err;
}
/*
 * Return the smallest count of bytes needed to store nbytes rounded to
 * internal block size
 */
int crypto_round_blk_sz(int nbytes)
{
    log_in("nbytes=%d", nbytes);
    int size;
    size=(nbytes/CRYPT_BLK_SZ+(nbytes%CRYPT_BLK_SZ==0?0:1))*CRYPT_BLK_SZ;
    log_out("size=%d", size);
    return size;
}
