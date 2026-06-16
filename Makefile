CC = gcc
CFLAGS = -Wall -Wextra -Wno-unused-parameter -Wconversion -Wstrict-prototypes -Wuninitialized -Wshadow -Werror=cast-align -Werror=incompatible-pointer-types -pedantic -std=c99 -D_DEFAULT_SOURCE
LDFLAGS = -lcligen

# Default target
all: scli

scli: scli.c
	$(CC) $(CFLAGS) -o scli scli.c $(LDFLAGS)

clean:
	rm -f scli *.o

run: scli
	./scli

.PHONY: all clean run
