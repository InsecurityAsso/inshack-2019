# Tower of Babel (intended) Write-up

## Finding whats wrong about this memory dump

Scanning the memory dump with volatility should be enough to find hints that a unknown kernel module has been inserted.

You may also do using `strings mem.dmp | grep -Ei 'tower\s*of\s*babel'` and realize that this challenge as something to do with a character device.

## Extracting the kernel module

The tricky part here is finding a way to extract it from the memory dump. 

As soon as you've identified that strange kernel module, there are several words you can use to start 'grep-ing' in memory.

You can identify a some functions names in symbol tab that match `_hhhhhhhh`, `h` being part of the hexadecimal alphabet.

If you look for one of these names in the memory dump, you'll find it ~4 times.

At this point keep in mind that what you're looking for is a kernel module which is nothing else than an ELF binary.

You will find that (at least) one of the four locations is a valid ELF binary (`binwalk` should be able to give you the offset of this binary).

Then you perform some `dd` magic and get something to reverse... or give to a reverser because you hate assembly (two brains are often better than one).

## Reversing the kernel module

You can perform this step using Ghidra or IDA. 

If done carefully you will find that a lot of bytearrays are stored in `.rodata` section and 
used within the program with a `xor` operation. One of the bytearray is a key and all others are strings xor-ed with this key.

If you decode these bytearrays, you'll get your flag.
