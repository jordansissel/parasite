

parasite.so: LDFLAGS=-shared -ldl -lpthread -lruby1.8
parasite.so: parasite.o
	$(CC) $(LDFLAGS) parasite.o -o parasite.so

parasite.o: CFLAGS=-fpic -I/usr/lib/ruby/1.8/$(shell uname -m)-linux

clean:
	rm -f *.o *.so
