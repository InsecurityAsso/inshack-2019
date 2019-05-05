import sys

if len(sys.argv) != 4:
    print("python split.py <exec> <part1> <part2>")
    sys.exit(1)

byte_one = []
byte_two = []
with open(sys.argv[1], "rb") as f:
    with open(sys.argv[2], "wb") as o1:
        with open(sys.argv[3], "wb") as o2:
            idx = 0
            byte = f.read(1)
            byte_one.append(byte)
            idx += 1
            while byte != b"":
                # Do stuff with byte.
                byte = f.read(1)
                if idx % 2 == 0:
                    byte_one.append(byte)
                else:
                    byte_two.append(byte)
                idx += 1

zob = byte_one
print("#define NB_SPLIT1 " + str(len(zob)))
print("char vm_split1[" + str(len(zob)) + "] = {")
for c in zob:
    print(hex(int.from_bytes(c, "little")) + ",")
print("};")

zob = byte_two
print("#define NB_SPLIT2 " + str(len(zob)))
print("char vm_split2[" + str(len(zob)) + "] = {")
for c in zob:
    print(hex(int.from_bytes(c, "little")) + ",")
print("};")