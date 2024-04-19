CC=g++
CFLAGS=-O -Wall

all:
	$(CC) $(CFLAGS) asset-bundler.cpp -o asset-bundler

clean:
	rm ./asset-bundler

.PHONY: clean
