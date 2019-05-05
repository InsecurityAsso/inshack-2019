# Write-up: Jean-Sébastien Bash (INS'hAck 2019)

_(I don't know about you, but this is the best pun I've ever heard)_
* Category: crypto
* Description: I've found a revolutionary way to securely expose my server!
* Material: A SSH endpoint

## Exploration

You are given an SSH access to a server, welcoming you with a message and a prompt:
```
Welcome on my server. /help for help  

>
```

Let's type `/help`:
```
>/help
This is a tool so that only me can execute commands on my server
(without all the GNU/Linux mess around users and rights).

- /help for help
- /exit to quit
- /cmd <encrypted> to execute a command

Notes (TODO REMOVE THAT) ---------------------------
Ex: 
/cmd AES(key, CBC, iv).encrypt(my_command)
/cmd 7bcfab368dc137d4628dcf45d41f8885
```
It seems this server can accept 3 commands `/help`, `/exit` and `/cmd <encrypted>`.
Of course, it is the last one that is interesting. We can pass some kind of encrypted command to be executed by the server.

The note gives us the encryption algorithm (AES) and its mode (CBC).
It is also kind enough to give us an example, let's try it.
```
>/cmd 7bcfab368dc137d4628dcf45d41f8885
Running b'ls -l'
total 8
-rw-r--r-- 1 root root   21 Apr 25 21:18 flag.txt
-rwxr-xr-x 1 root root 2066 Apr 25 21:50 server.py
```

Okay! That's quite straightforward. 
The command seems to be an encrypted version of `ls -l`, and there is file named `flag.txt`.

We also know from the `b'ls -l'` (bytes string notation) and `server.py` that the server is written in Python 3.
But this is a crypto challenge, so the goal must be to send the encrypted version of `cat flag.txt` (rather than messing with Python vulnerabilities).

Let's try to send other commands to see how the server reacts:
```
>/aorsuhdoarse
What do you mean?!

>/aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
What do you mean?!

>
What do you mean?!

>/
What do you mean?!

>/#
What do you mean?!

>/cmd 
What do you mean?!

>/cmd cat flag.txt
What do you mean?!

>/flag.txt
What do you mean?!

>////////////////
What do you mean?!
```
Always the same error message `What do you mean?!`.
At first, no sign of glaring weaknesses, let's concentrate on the crypto.

## Cryptanalysis

The encrypted command is a hexadecimal string of length 32, so a 16-bytes string.

In crypto, Wikipedia is always a good resource:
- AES: https://en.wikipedia.org/wiki/Advanced_Encryption_Standard
- CBC mode: https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation#Cipher_Block_Chaining_(CBC)

16 bytes is the size of a block in AES.

A cipher text of 16 bytes long will be decrypted without error, but it is not sent to the bash (I changed the last digit):
```
>/cmd 7bcfab368dc137d4628dcf45d41f8883
What do you mean?!
```

Let's test lots of random inputs (with a script, still 16-bytes long). 
Quite soon we get a case giving a different result:
```
>/cmd 18f0a836a592251462260f7394b3f49a
Running b'\\xe5\\xb5.\\n\\x08\\n\\xac\\xef\\x19*\\xd6\\x81\\xee\\xb6\\xff'
sh: 1: \xe5\xb5.: not found
```
The command is complete garbage, but the server tried to execute it nevertheless!

Now we need to know the difference between this case and the others.
In theory, any string with a length equal to a multiple of 16 bytes should be decrypted without error.
But there is a catch. The plain text in AES must also have a length equal to a multiple of 16 bytes.

It must be **padded**, following this pattern : 
- if it misses 1 byte being a multiple of 16 bytes long, add `\x01`
- 2 bytes too short? add `\x02\x02`
- 3 bytes too short? add `\x03\x03\x03`
- ...
- And if it is multiple of 16 bytes long, we need to add a full block of 16 times `\x10`, to avoid ambiguities.

It is the PKCS7 padding: https://en.wikipedia.org/wiki/Padding_(cryptography)#PKCS#5_and_PKCS#7

Thus, a decrypted message not following this pattern (wrong padding) wasn't encrypted with the same key/IV, and should be considered invalid.
This server must do exactly that!

Can we take advantage of this? Yes, we can.

## Padding oracle attack

The padding oracle attack gets possible on a CBC block-cipher,
when we have access to a server responding differently to badly-padded input and
valid string (but potentially invalid encrypted string).
This is the so-called padding oracle.

Wikipedia article: https://en.wikipedia.org/wiki/Padding_oracle_attack

It can be used to decipher an encrypted text from this server, 
and that's the main subject of the Wikipedia article, 
but we can also use it to forge encrypted texts, without knowledge of the key or the IV.

### Theory

I will keep the notation used in Wikipedia:
- `C_n` is the nth block of cipher text.
- `P_n` is the nth block of plain text.
- `IV` is the initialisation vector, but can be considered as `C_0` during decryption.
- `D()` is the function of decryption, on one block.

Those pieces are linked by the equation (`^` is for XOR): `P_n = D(C_n) ^ C_{n-1}`

`IV` and `D` are unknown in our case.
We can produce any `C_n`.
We want a given `P_n`.

The attack will allow us to find `D(C_n)` for a given `C_n`, by playing with `C_{n-1}`.
Then we choose `P_n` by setting `C_{n-1} = P_n ^ D(C_n)`.

Since we can change the `IV` (i.e. `C_0`), we won't be able to choose `P_1`. 
So we will need two blocks, put the bash command in `P_2`, 
prepending it with `;` to separate it from the garbage.
It won't if there is some special character in the garbage, but it is not so often. 
Trying several times will eventually bring a valid command.

### Algorithm

1. Choose a random `C_1` and `C_2`
2. Modify the last byte of `C_1` until the oracle says the padding is right.
   It means that the last byte of `P_2` is `\x01`.
   We can conclude that the last byte of `D(C_2)` is the the last byte of `C_1` XOR `\x01`.
   
   Note: there is some chance that it is `\x02` or more if the preceding bytes are also `\x02` or more, to form a valid padding
   of more than one byte. But it is somewhat rare, and we can choose to ignore it.
   But we must be aware of this possibility because it can cause the rest of the algorithm to fail.
   In this case, just try again with other random `C_1` and `C_2`.

3. Now, we will find the case where the padding is `\x02\x02`.
   Set the last byte of `C_1` to the last byte of `D(C_2)` XOR `\x02`, 
   and iterate on all values of the penultimate byte of `C_1`.
   It will give us the penultimate byte of `D(C_2)`.

4. With the same method, we can find all the bytes of `D(C_2)`.

5. Set `C_1 = D(C_2) ^ P_2` with `P_2` our command: `; cat flag.txt`, rightly padded 
   (in this case `\x02\x02`). 
   
   The result of the command should be output. 
   If some characters from the garbage annoyed bash,
   tweek the spaces of the command to find a working case.

## Code

The code of the exploit can be found in `exploit.py`.

It should take less than 10 seconds to execute.
If it fails, just retry. I found it working most of the cases.
I've put several bash commands to maximize the chances to work.