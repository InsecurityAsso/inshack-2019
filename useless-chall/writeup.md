The binary of this challenge looks big and messy but in fact, a lot of code is just noise.

First of all, in the `main` function, a call to `len_check` function is made. This is a very basic function which return `0` if the typed password has 41 characters.

Then, we enter to the heart of the program: 3 nested if. Have a look at the condition :
1) The first one is `a^2 - 9a + 56 > -`. This condition is always true, so we don't have to reverse the function that compute `a`.
2) The second condition is something like `hash(pass) == expected_hass`. We will handle this condition later.
3) The second condition is `(c^2 + 1) % 7 != 0`. This condition is also always true. So we don't need to reverse the function that compute `c`.

The second condition is the only one that is important. Have a look at it. By opening the binary in your favorite disassembler, you can notice that the function is pretty simple :

```
int hash(char* pass) {
    int size = strlen(pass);
    int hashed = 0;
    for (int i = 0; i < size; i++) {
        hashed += (pass[i] << i);
    }

    return hashed;
}```

We want the result of this function to be equal to `1012589865`. Use your brain or your favorite constraint solver to find the flag, knowing all the condition (flag must be of the form `INSA{....}` with only letters, numbers and symbols).

This kind of obfuscation (adding useless if condition) is called `opaque predicate`. The goal is to add useless branching, that is always evaluate to true/false, to the source code to increase the code size and slow down the reverse process. 