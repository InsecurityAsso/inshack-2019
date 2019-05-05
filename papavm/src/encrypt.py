secret = "Coucou je suis un gros faisan et j'aime bien les groooosses poules !!"
bytes_secret = [ord(c) for c in secret]
key = "IAMTHEMASTEROFYOURSOUL"

def encrypt_file():
    file_hex = []
    with open("vm", "rb") as f:
        byte = f.read(1)
        file_hex.append(byte)
        while byte != b"":
            # Do stuff with byte.
            byte = f.read(1)
            file_hex.append(byte)

    global bytes_secret
    bytes_secret = file_hex
    return encrypt()

def encrypt():
    output = []
    idx_key = 0
    for c in bytes_secret:
        key_i_hex = ord(key[idx_key])
        output.append(int.from_bytes(c, "little") ^ key_i_hex)

        idx_key += 1
        if idx_key == len(key):
            idx_key = 0
    return output

def decrypt(hashed):
    output = []
    idx_key = 0
    for c in hashed:
        key_i_hex = ord(key[idx_key])
        output.append(c ^ key_i_hex)

        idx_key += 1
        if idx_key == len(key):
            idx_key = 0
    return output

# print("".join([chr(c) for c in decrypt(encrypt())]))
zob = encrypt_file()
print("#define NB_INSTR " + str(len(zob)))
print("char vm_instrs[" + str(len(zob)) + "] = {")
for c in zob:
    print(hex(c) + ",")
print("};")