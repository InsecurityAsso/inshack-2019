all: hash

hash: hash.c
	gcc -O0 -o ../public-files/hash hash.c -s
	strip -s ../public-files/hash

clean:
	rm hash