# Exploring the Universe Write-Up

Here no need for guessing, everything is explained in the description. 
It might be unclear (hey, it's a CTF) but it's there.

First, you need to find out about the `nano/vim` clue combined with `server.py`. 

Both `nano` and `vim` create swap files, there are different namings depending on versions but you can find them on the Internet or by experimenting a bit.

Then you can find a file `https://exploring-the-universe.ctf.insecurity-insa.fr/.server.py.swp` on the server.

By reading at the code you should find a **limited** path traversal vulnerability in this application.

You can just go to the parent folder and its subdirectories but nowhere else.

Then you use the second clue, you are asked to find the `flag` in the `universe/`. 

Ohoh `universe/` looks like a folder why not concatenating both, you obtain `universe/flag`.

But wait, I found a vulnerability, lets combine it with the previous finding.

`https://exploring-the-universe.ctf.insecurity-insa.fr/%2e%2e/universe/flag`

You get the flag.
