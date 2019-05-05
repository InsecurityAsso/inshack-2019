Meet in the middle attack.

```python
from Crypto.Cipher import AES

PEARSON_TABLE = [199, 229, 151, 178, 53, 6, 131, 42, 248, 110, 39, 28, 51, 216, 32, 14, 77, 34, 166, 213, 157, 150, 115, 197, 228, 221, 254, 172, 84, 27, 36, 156, 69, 96, 12, 220, 225, 137, 246, 141, 44, 208, 191, 109, 163, 21, 173, 250, 98, 227, 203, 162, 188, 3, 105, 171, 215, 15, 207, 218, 234, 56, 136, 235, 97, 79, 189, 102, 134, 11, 224, 117, 177, 222, 100, 129, 78, 18, 130, 187, 9, 184, 99, 108, 202, 13, 238, 17, 94, 70, 180, 144, 185, 168, 123, 71, 176, 91, 4, 153, 103, 242, 80, 127, 198, 82, 169, 148, 48, 120, 59, 55, 230, 209, 50, 73, 31, 49, 142, 149, 167, 249, 116, 1, 7, 86, 143, 101, 29, 52, 114, 154, 160, 128, 19, 170, 46, 214, 38, 67, 186, 252, 181, 145, 212, 183, 22, 231, 107, 43, 47, 122, 251, 217, 5, 62, 88, 244, 200, 93, 240, 219, 124, 58, 161, 89, 211, 158, 247, 60, 236, 65, 106, 113, 66, 81, 165, 194, 223, 40, 233, 126, 139, 72, 132, 61, 135, 57, 87, 182, 164, 35, 159, 118, 8, 83, 210, 243, 104, 76, 75, 119, 90, 138, 20, 206, 95, 16, 74, 33, 245, 237, 111, 64, 253, 125, 23, 232, 193, 37, 175, 92, 30, 241, 255, 133, 0, 140, 2, 155, 85, 10, 146, 179, 25, 26, 226, 201, 195, 121, 190, 63, 68, 152, 45, 147, 41, 204, 192, 205, 196, 54, 174, 239, 112, 24]

def get_pearson_hash(passphrase):
    key, iv = '', ''
    for i in range(32):
        h = (i + ord(passphrase[0])) % 256
        for c in passphrase[1:]:
            h = PEARSON_TABLE[h ^ ord(c)]
        if i < 16:
            key += chr(h)
        else:
            iv += chr(h)
    return key, iv

BLOCK = open('admin.dla', 'rb').read()[:16]
wordlist = open('wordlist.txt', 'rb').read().strip().split('\n')
enc, dec = {}, {}

# Runs in ~ 2 minutes
for i,w in enumerate(wordlist):
    if not i%10000: print i,'/',len(wordlist)
    key, iv = get_pearson_hash(w)
    enc[AES.new(key, AES.MODE_CBC, iv).encrypt('SQLite format 3\x00')] = w
    dec[AES.new(key, AES.MODE_CBC, iv).decrypt(BLOCK)] = w

for mid in set(enc.keys()).intersection(set(dec.keys())):
    print enc[mid], dec[mid]
```