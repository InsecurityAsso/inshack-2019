import sys

if len(sys.argv) != 2:
    print("python troll.py <exec>")
    sys.exit(1)

byte_one = []
with open(sys.argv[1], "rb") as f:
    idx = 0
    byte = f.read(1)
    byte_one.append(byte)
    while byte != b"":
        # Do stuff with byte.
        byte = f.read(1)
        byte_one.append(byte)

zob = byte_one
print("#define NB_TROLL " + str(len(zob)))
print("char vm_troll[" + str(len(zob)) + "] = {")
for c in zob:
    print(hex(int.from_bytes(c, "little")) + ",")
print("};")
