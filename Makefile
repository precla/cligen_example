CC = gcc
CFLAGS = -std=c99 -D_DEFAULT_SOURCE
LDFLAGS = -lcligen

# Default target
all: scli scli_emb

scli: simple_cli_cligen.c
	$(CC) $(CFLAGS) -o scli simple_cli_cligen.c $(LDFLAGS)

scli_emb: simple_cli_cligen_embedded.c
	$(CC) $(CFLAGS) -o scli_emb simple_cli_cligen_embedded.c $(LDFLAGS)

clean:
	rm -f scli scli_emb *.o

run: scli
	./scli

run-embedded: scli_emb
	./scli_emb

.PHONY: all clean run run-embedded
