CC=g++
CFLAGS=-std=c++17 -O3 -Wall -Wextra -pedantic -Werror
TARGET=asset-bundler

all: $(TARGET)

$(TARGET): asset-bundler.cpp
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(TARGET) example.apkg.bin 

.PHONY: all clean
