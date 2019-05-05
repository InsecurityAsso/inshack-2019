weak: main.c
	gcc -o weak main.c -Wl,-z,norelro -fno-stack-protector -z execstack -w

clean:
	rm -rf weak nostackprotector canary hardened exec_gcc
