

parasite.so:  parasite.o
	gcc -g -fPIC -shared -ldl -lpthread parasite.o -o parasite.so

parasite.o: CFLAGS=-fpic
