CFLAGS ?= -pthread

all: example

example: example.o asyncaddrinfo.o
	$(CC) $(CFLAGS) -o example example.o asyncaddrinfo.o

clean:
	rm -f *.o example

%.o: %.c *.h
	$(CC) -c $(CFLAGS) $< -o $@
