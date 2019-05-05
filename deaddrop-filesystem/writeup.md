# Deaddrop Filesystem Write Up

After downloading and extracting the archive, you find two elements:

 - an ext4 filesystem,
 - and a static library.

Your can start by exploring the filesystem, it is full of unreadable files and strange index files...

The only way to solve this challenge then is to take some time to reverse binary objects embedded into the static
library and understand how it works. You can use IDA or Ghidra to perform this analysis.

When this is done, you should have enough elements to build a decryption tool to recover files and get your flag !

Note: see the `exploit/` folder for a working decryption tool.
