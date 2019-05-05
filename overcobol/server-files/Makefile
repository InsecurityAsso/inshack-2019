all: lib exec

exec: 
	 cobc -x Test.CBL -L. -ltest -o overcobol

lib: lib.c
	gcc -fPIC -shared -o libtest.so lib.c

clean:
	rm exec libtest.so -rf