import os
import uuid
import zlib
import struct
import random
import pathlib
import argparse
import cryptography.hazmat.backends as backends
import cryptography.hazmat.primitives.ciphers as ciphers

#
# Format  C Type             Python type     Standard size   Notes
# x       pad byte           no value
# c       char               bytes of length 1   1
# b       signed char        integer     1   (1),(3)
# B       unsigned char      integer     1   (3)
# ?       _Bool              bool    1   (1)
# h       short              integer     2   (3)
# H       unsigned short     integer     2   (3)
# i       int                integer     4   (3)
# I       unsigned int       integer     4   (3)
# l       long               integer     4   (3)
# L       unsigned long      integer     4   (3)
# q       long long          integer     8   (2), (3)
# Q       unsigned long long integer     8   (2), (3)
# n       ssize_t            integer         (4)
# N       size_t             integer         (4)
# e       (7)                float   2   (5)
# f       float              float   4   (5)
# d       double             float   8   (5)
# s       char[]             bytes
# p       char[]             bytes
# P       void *             integer         (6)
#

NULL = b'\x00'
IV_SZ = 16
KEY_SZ = 16
BACKEND = backends.default_backend()

class Cipher:

    def __init__(self):
        self._iv = os.urandom(IV_SZ)
        self._key = os.urandom(KEY_SZ)
        self._cipher = ciphers.Cipher(ciphers.algorithms.AES(self._key),
                                      ciphers.modes.CBC(self._iv),
                                      backend=BACKEND)

    def __str__(self):
        return f"[{self._iv.hex()}|{self._key.hex()}]"

    def encrypt(self, data: 'bytes'):
        encryptor = self._cipher.encryptor()
        return encryptor.update(data) + encryptor.finalize()

    def serialized(self):
        data = list(self._iv + self._key)
        data.reverse()
        return bytes(data)

def pad(data: 'bytes', bs: 'int'):
    sz = bs - (len(data) % bs)
    return data + bytes([sz]*sz)

class Shuffler:
    '''Shuffles a file
    '''
    BS = 128
    CRYPT_BS = 16

    def __init__(self, entry: 'ArchiveEntry'):
        self._entry = entry
        self._cipher = Cipher()
        self._shuffle_map = {}

    def __str__(self):
        return f"[{self._entry.uid}|{self._cipher}]"

    def serialized_metadata(self):
        if not self._shuffle_map:
            raise RuntimeError("Serializing key before data...")
        data = self._entry.uid.bytes
        data += self._cipher.serialized()
        data += struct.pack('I', len(self._shuffle_map))
        for src_idx, dst_idx in self._shuffle_map.items():
            data += struct.pack('2I', src_idx, dst_idx)
        return data

    def serialized_data(self):
        with self._entry.path.open('rb') as fp:
            plaindata = fp.read()
        # compress data + pad
        compressed_data = pad(zlib.compress(plaindata), Shuffler.CRYPT_BS)
        # encrypt data + pad
        encrypted_data = pad(self._cipher.encrypt(compressed_data), Shuffler.BS)
        # intialize shuffle map
        bcnt = len(encrypted_data) // Shuffler.BS
        indices = list(range(bcnt))
        random.shuffle(indices)
        for src_idx, dst_idx in zip(list(range(bcnt)), indices):
            self._shuffle_map[src_idx] = dst_idx
        # shuffle blocks
        blocks = [encrypted_data[k*Shuffler.BS:(k+1)*Shuffler.BS] for k in range(bcnt)]
        data = b''
        for k in range(bcnt):
            data += blocks[self._shuffle_map[k]]
        return data

class ArchiveEntry:
    '''[summary]
    '''
    def __init__(self, file: 'pathlib.Path'):
        self._file = file
        self._uid = uuid.uuid4()
        self._stat = file.stat()

    def __str__(self):
        return f"[{self._uid}|{self._file}]"

    @property
    def uid(self):
        return self._uid

    @property
    def path(self):
        return self._file

    @property
    def name(self):
        return str(self._file)

    @property
    def size(self):
        return self._stat.st_size

    @property
    def mode(self):
        return self._stat.st_mode

    @property
    def atime(self):
        return self._stat.st_atime

    @property
    def mtime(self):
        return self._stat.st_mtime

    @property
    def ctime(self):
        return self._stat.st_ctime

    def serialized(self):
        name = self.name.encode() + NULL
        data = struct.pack('I', len(name))
        data += name
        data += self.uid.bytes
        data += struct.pack('2I', self.size, self.mode)
        data += struct.pack('3d', self.atime, self.mtime, self.ctime)
        return data

class KeyStore:
    '''Represents the keyfile
    '''
    MAGIC = b'L0LKSTR\x00'

    def __init__(self):
        self._shufflers = []

    def add_entry(self, entry: 'ArchiveEntry'):
        shuffler = Shuffler(entry)
        print(shuffler)
        self._shufflers.append(shuffler)
        return shuffler

    def save(self, output_dir: 'pathlib.Path'):
        data = KeyStore.MAGIC
        data += struct.pack('I', len(self._shufflers))
        for shuffler in self._shufflers:
            data += shuffler.serialized_metadata()
        output_dir.joinpath('keystore').write_bytes(data)

class Archive:
    '''Represents an archive
    '''
    MAGIC = b'L0LARCH\x00'
    SIZE_LIMIT = 1024*1024*1

    def __init__(self):
        self._entries = []
        self._keystore = KeyStore()

    def add_entry(self, file: 'pathlib.Path'):
        entry = ArchiveEntry(file)
        print(entry)
        if entry.size > Archive.SIZE_LIMIT:
            raise RuntimeError(f"{entry.path} size is above the limit ({Archive.SIZE_LIMIT})!")
        shuffler = self._keystore.add_entry(entry)
        self._entries.append((entry, shuffler))

    def save(self, output_dir: 'pathlib.Path'):
        # saving archive
        data = Archive.MAGIC
        data += struct.pack('I', len(self._entries))
        for entry, shuffler in self._entries:
            print(f"adding {entry.path}...")
            data += entry.serialized()
            shuffled = shuffler.serialized_data()
            data += struct.pack('I', len(shuffled))
            data += shuffled
        output_dir.joinpath('archive').write_bytes(data)
        # saving keystore
        # WARNING: keystore must be saved when archive serialization
        #          process is over
        self._keystore.save(output_dir)

class Directory:
    '''Represents a directory
    '''
    def __init__(self, path: 'pathlib.Path'):
        self._path = pathlib.Path(path)

    def files(self, recurse: 'bool'):
        files = list(self._path.glob('*'))
        if recurse:
            files = list(self._path.rglob('*'))

        return list(filter(lambda p: p.is_file(), files))

def parse_args():
    '''[summary]
    '''
    p = argparse.ArgumentParser(description="")
    p.add_argument('d')
    p.add_argument('o')
    return p.parse_args()

def app():
    '''[summary]
    '''
    args = parse_args()
    archive = Archive()
    directory = Directory(args.d)
    filepath_out = pathlib.Path(args.o)
    for file in directory.files(True):
        archive.add_entry(file)
    filepath_out.mkdir(parents=True, exist_ok=True)
    archive.save(filepath_out)

if __name__ == "__main__":
    app()
