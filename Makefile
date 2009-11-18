# Special thanks to bd_ in oftc#linode who helped me with the
#   solaris target
CC = gcc
CFLAGS = -D _REENTRANT
LDFLAGS = -lpthread
ofiles = proj2.o store.o tractor.o clerk.o
SOLARIS = -D _SOLARIS

proj2: $(ofiles)

solaris:
	make clean
	make sol

sol: $(ofiles:%.o=%.sol.o)
	gcc $(LDFLAGS) -o proj2 $^

%.o: %.c
	gcc -c $(CFLAGS) -o $@ $<

%.sol.o: %.c
	gcc -c $(CFLAGS) $(SOLARIS) -o $@ $<

clean:
	rm -f *.o proj2
