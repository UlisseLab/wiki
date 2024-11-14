#!/usr/bin/env python3

from pwn import *

exe = ELF("./dim")

context.binary = exe

def main():
    r = process("./dim")
    r.interactive()


if __name__ == "__main__":
    main()
