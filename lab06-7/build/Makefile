.PHONY  :  all client server
CC      := gcc
CFLAGS  := -Wextra -Wall -pedantic -Wno-sign-compare -Wno-long-long -g3 \
           -D_POSIX_SOURCE=600 -D_XOPEN_SOURCE=600 -Wconversion -Wsign-conversion -Wunreachable-code -fstack-protector-all
LDFLAGS := -lzmq -lpthread
HDR     := $(wildcard ../*.h)

all: ../client ../server

clean:
	rm -f *.o ../client ../server

../client: client.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

../server: bank.o server.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

%.o: ../%.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

