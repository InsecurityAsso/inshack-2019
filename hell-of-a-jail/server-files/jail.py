"""Unobfuscated source for Hell of a jaill."""


import os
import sys
import code
import signal


def handler(signum, frame):
    """Handling SIGSTOP"""
    pass

def user_input(arg):
    """Filter input."""
    s = input(">>> ")
    s = s[:14:]
    if "\"" in s:
        s = "print('TROLLED !!!!'*1000)"
    s = s.replace('__', '')
    s = s.replace('.', '')
    return s

def exit(arg):
    """Must invoke with the right arg in order to get the flag."""
    if arg == os.environ['0f4d0db3668dd58cabb9eb409657eaa8']:
        print("Oh no ! You managed to escape\nValidate with the key")
        return sys.exit(0)
    print("Wrong key")

def sandbox():
    """PyJail sandbox."""
    scope = {"__builtins__": {"exit": exit, "getattr":getattr, "print": print}}
    banner = "Oh my jail ! You need to exit() with the correct key.\nIt might make you free (and give you the flag)"
    exit_msg = "No shell through this..."

    signal.signal(signal.SIGTSTP, handler)

    while True:
        try:
            code.interact(banner, user_input, scope, exit_msg)
        except SystemExit:
            sys.exit(0)

sandbox()
