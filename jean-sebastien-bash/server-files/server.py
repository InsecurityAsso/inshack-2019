#!/bin/python3
import os
import random
import string
from Crypto.Cipher import AES
from binascii import hexlify, unhexlify
import signal

random.seed(';384lg;pf')
IV = ''.join(random.choice(string.printable) for _ in range(16)).encode()
key = ''.join(random.choice(string.printable) for _ in range(32)).encode()

def handler(sig, frame):
    pass

signal.signal(signal.SIGTSTP, handler)

def pad(s):
    return s + (16 - len(s) % 16) * bytes([16 - len(s) % 16]) 

def unpad(s): 
    p = ord(s[len(s)-1:])

    if s[-p:] == p * bytes([p]):
        return s[:-ord(s[len(s)-1:])]
    else:
        raise Exception("Bad padding")

def create_aes():
    return AES.new(key, AES.MODE_CBC, IV)

def encrypt(plain):
    return hexlify(create_aes().encrypt(pad(plain))).decode('utf-8')

def decrypt(enc):
    cipher = unhexlify(enc.encode('utf-8'))
    return unpad(create_aes().decrypt(cipher))

def main():
    instr_welcome = """Welcome on my server. /help for help  """

    instr_help = """This is a tool so that only me can execute commands on my server
(without all the GNU/Linux mess around users and rights).

- /help for help
- /exit to quit
- /cmd <encrypted> to execute a command

Notes (TODO REMOVE THAT) ---------------------------
Ex: 
/cmd AES(key, CBC, iv).encrypt(my_command)
/cmd {}
""".format(encrypt(b'ls -l'))

    prompt="\n>"
    
    print(instr_welcome)
    while True:
        try:
            cmd = input(prompt).strip()
        except EOFError:
            exit(1)

        if cmd == '/exit':
            print("Good bye!")
            exit(0)
        elif cmd == '/help':
            print(instr_help)
        elif cmd.startswith('/cmd '):
            try:
                enc = cmd[4:].strip()
                bash_cmd = decrypt(enc)
                print("Running", bash_cmd)
                os.system(bash_cmd)
            except Exception:
                print("What do you mean?!")
        else:
            print("What do you mean?!")


if __name__ == '__main__':
    try:
        main()
    except:
        print("Bye.")
        exit(0)
