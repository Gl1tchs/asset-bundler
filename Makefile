CC=clang++
CFLAGS=-std=c++17 -O3 -Wall -Wextra -pedantic -Werror -Iinclude/
TARGET=bin/asset-bundler

all: $(TARGET)

$(TARGET): src/main.cpp
	@mkdir -p bin
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(TARGET) data/example.apkg.bin 
	@rmdir bin

.PHONY: all clean
