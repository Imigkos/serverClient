CC ?= cc
CFLAGS ?= -std=gnu11 -Wall -Wextra -g

.PHONY: all clean
all: build/server build/client

build/server: server.c server_functions.c server.h
	mkdir -p build
	$(CC) $(CFLAGS) server.c -o $@ -pthread

build/client: client.c client_functions.c client.h
	mkdir -p build
	$(CC) $(CFLAGS) client.c -o $@

clean:
	rm -rf build
