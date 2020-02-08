.PHONY  :  all client server
CC      := gcc -std=c11
CFLAGS  := -Wextra -Wall -pedantic -Wno-sign-compare -Wno-long-long -g3 \
           -D_POSIX_SOURCE=600 -D_XOPEN_SOURCE=600 -Wconversion -Wsign-conversion -Wunreachable-code -fstack-protector-all
LDFLAGS := -lzmq -lpthread -lm -lrt
HDR     := $(wildcard ../*.h)

all: ../client ../server

clean:
	rm -f *.o ../client ../server

%.o: ../%.c $(HDR)
	$(CC) $(CFLAGS) -c $<

../client: client.o game.o
	$(CC) $(LDFLAGS) $^ -o $@

../server: server.o game.o
	$(CC) $(LDFLAGS) $^ -o $@


