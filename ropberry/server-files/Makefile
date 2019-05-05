all: ropberry

ropberry: main.c
	clang -m32 -o ropberry -static main.c -fno-stack-protector -w

clean:
	rm -rf weak nostackprotector canary hardened exec_gcc
