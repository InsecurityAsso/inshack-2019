This challenge has multiple anti-debugging techniques.

First, the binary seems to be packed with a strange packer. It looks like UPX, but some recognizable strings that are present in UPX packed binary are not here (`strings final | grep UPX`). Just compile a hello world with UPX, compare the missing string and replace it in the `final` binary. And voila ! You can now unpack the patched final binary with UPX. That was the first step. Easy.

By reversing the unpacked binary, we found that there is another packer in place. This is very simple packer that load the binary from two source, byte per byte, one source after one other. Write a small python script that do it for you :). Second step, quit fun.

Now that you have another binary, fire your favorite disassembler and reverse. Another packer. Cool ! By reversing this binary, we notice that the binary is packed as follow :
1) A key is shift using a ceaser cipher, shift of +7.
2) This key is reversed.
3) The packed binary is xored byte per byte with the reverse key.
Write a small python script that do it for you, third step. I love packer.

Now open your disassembler, reverse main. You find that the program only print `Flag is: INSA{{I_r34lLy_L0v3_tR0ll1nG !}}`. After all this step, you may think that this is the flag. But launch the program and you will quickly notice that, I am just a troll. In fact the `Flag is: INSA{{I_r34lLy_L0v3_tR0ll1nG !}}` is just a `println` done by an embed Rust program in the binary. But, the function called `main` is not the function launch at startup. The "real main" is another function. Which execute a VM :). Reverse this VM and you will find a very basic algorithm:

```
int64_t hash(char* pass) {
    int64_t hash = 0;
    for (int i = 0; i < PASS_LEN; i++) {
        int64_t p_64 = pass[i];
        int64_t t_64 = tab[i];
        hash += p_64 * t_64;
    }
    return hash;
}
```
Notice that this hash function looks like the Hash function of Java.String :)

We want the result of this function to be equal to `-237549964763`. Use your brain or your favorite constraint solver to find the flag, knowing all the condition (flag must be of the form `INSA{....}` with only letters, numbers and symbols).
