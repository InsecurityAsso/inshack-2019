# New Authent

This chall is divided in 2 steps. First you have to find the seed or the totp generator. It's possible because you have 2 samples and their timestamps. A mode is associated in hashcat, you can find more info [here](https://www.unix-ninja.com/p/attacking_google_authenticator). Don't forget to adapt the hashcat mask as you know it's 8 digits.

Then you have to mimic the printer behavior. Each printer leave a specific code when printing, the Document Colour Tracking Dots. Using [deda](https://github.com/dfd-tud/deda) you can find this code (`deda_parse_print`) and generate it yourself with `deda_create_dots --serial 000324`.