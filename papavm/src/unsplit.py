import sys

if len(sys.argv) != 4:
    print("python unsplit.py <exec> <part1> <part2>")
    sys.exit(1)

byte_one = []
with open(sys.argv[2], "rb") as f:
    byte = f.read(1)
    byte_one.append(byte)
    while byte != b"":
        byte = f.read(1)
        byte_one.append(byte)

byte_two = []
with open(sys.argv[3], "rb") as f:
    byte = f.read(1)
    byte_two.append(byte)
    while byte != b"":
        byte = f.read(1)
        byte_two.append(byte)

with open(sys.argv[1], "wb") as f:
    idx = 0
    ko = False
    ko2 = False
    while not (ko and ko2):
        if idx < len(byte_one):
            f.write(byte_one[idx])
        else:
            ko = True
        if idx < len(byte_two):
            f.write(byte_two[idx])
        else:
            ko2 = True
        idx += 1