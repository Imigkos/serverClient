# Hotel Booking Client–Server

A C socket-programming project with a terminal client and a multithreaded hotel-search server.

## Features

- Search rooms by location, bed count or price range.
- Submit booking date ranges and check room availability.
- Load hotel and room data from the included `hotels.csv` fixture.
- Handle client connections with POSIX threads and protect booking updates with a mutex.

## Build

Requires a POSIX environment, a C compiler, Make and POSIX threads.

```sh
make
```

The entry points include their respective `*_functions.c` files directly, so the Makefile compiles each entry point once rather than linking those files twice.

## Run

From the repository root, start the server:

```sh
./build/server 8080
```

In a second terminal:

```sh
./build/client 8080
```

The client connects to `127.0.0.1`. The server reads `hotels.csv` relative to its working directory. Follow the numbered client menu; booking ranges use `DD/MM-DD/MM`, for example `01/06-05/06`.

## Limitations

This is an educational implementation intended for local experimentation. The server binds to all interfaces. It has no authentication or encryption and should not be exposed as a public booking service. Its native C-struct/long wire format assumes matching client/server architecture. TCP message framing, disconnected-client handling and thread-slot reuse need further work before production use. Booking state is held in memory.

## Checks

`make` builds both programs with compiler warnings enabled. `make clean` removes generated executables. Manually exercise searching and booking with both programs when changing the protocol; a successful build alone does not validate concurrency or networking behavior.
