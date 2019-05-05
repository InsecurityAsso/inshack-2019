#ifndef __CRYPTO_H__
#define __CRYPTO_H__

#include "record.h"

#define CRYPT_BLK_SZ    16

/*
 * Initialize underlying crypto library
 */
bool_t crypto_init(user_t *user);
/*
 * Produce a hash of given name
 */
bool_t crypto_hash_name(const char *name,
                        uchar_t hash[REC_HASH_SZ]);
/*
 * Initialize 'hash' field of the given record structure
 */
bool_t crypto_init_hash(const char *name,
                        record_t *record);
/*
 * Initialize 'iv' field of the given record structure
 */
bool_t crypto_init_iv(record_t *record);
/*
 * Initialize 'seal' field of the given record structure
 */
bool_t crypto_init_seal(const user_t *user,
                        record_t *record);
/*
 * Check if record has been sealed with given user
 */
bool_t crypto_check_seal(const user_t *user,
                         const record_t *record);
/*
 * Encrypt 'name' field of the given record structure
 */
bool_t crypto_encrypt_name(const user_t *user,
                           record_t *record);
/*
 * Decrypt 'name' field of the given record structure
 */
bool_t crypto_decrypt_name(const user_t *user,
                           record_t *record);
/*
 * In-place encryption of given buffer
 */
bool_t crypto_encrypt_buffer(const uchar_t *k,
                             int klen,
                             const uchar_t *iv,
                             int ivlen,
                             uchar_t *buffer,
                             int buflen);
/*
 * In-place decryption of given buffer
 */
bool_t crypto_decrypt_buffer(const uchar_t *k,
                             int klen,
                             const uchar_t *iv,
                             int ivlen,
                             uchar_t *buffer,
                             int buflen);
/*
 * Return the smallest count of bocks needed to store nbytes
 */
int crypto_round_blk_sz(int nbytes);

#endif /* __CRYPTO_H__ */

