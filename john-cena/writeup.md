An input is expected from the given server. If we type a string, it prints it back to us and ask if we want to continue.

If we enter "%x %x", we notice that we are printing values from stack. Let use this format string to dump the binary. If the binary has been compiled with no pie, it should be loaded at 0x40000. Just leak this address and check if we obtain the magic bytes for ELF with are "\x7fELF". That's the case we are lucky. So dump the memory until you can open it in a disassembler.

By reversing the binary, we notice that if we enter "Hack_the_system" when prompt for a string, we activate a hidden part of the binary that do the following :

```
int* dst = user_input1;
*dst = user_input2;```

We can now write what we want everywhere. Why not overwriting the GOT address of `printf` with the one of `system` ?

For that we need to :
1) leak the current address of `printf` by using the same format string vulnerabilty.
2) Find which libc is used, using this [tool](https://github.com/niklasb/libc-database).
3) Compute the current libc base address with the computation : `PRINTF_GOT - PRINTF_LIBC`.
4) Compute the current address of system with the computation: `system_addr = PRINTF_GOT - PRINTF_LIBC + SYSTEM_LIBC`
5) Then, write using the hidden part of the binary (or also with the format string if you are a real hacker) `system_addr` to `PRINTF_GOT`.
6) Then, the next time you hit a `printf`, it will trigger `system` instead of `printf`.

Let have a look at the python exploit to have more details. I am waiting for your write up ! :)