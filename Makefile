# Tokens
CC = gcc
CFLAGS = -Wall -O2
ARCHIVE = ar rcs
# Dir
BUILD = ./build/
BIN = ./bin/
LIB = ./lib/
INCLUDE = ./include/
SRC = ./src/

all: library
	$(BIN)main

clean:
	rm -f $(LIB)*.a $(BIN)* $(BUILD)*.o ./main

main.o: $(SRC)main.c
	$(CC) $(CFLAGS) -c $< -o $(BUILD)main.o

mymemory.o: $(SRC)mymemory.c
	$(CC) $(CFLAGS) -c $< -o $(BUILD)mymemory.o

lib_mymemory.a: mymemory.o
	$(ARCHIVE) $(LIB)lib_mymemory.a $(BUILD)mymemory.o

library: lib_mymemory.a
	$(CC) -o $(BIN)main $(SRC)main.c -L. -l:$(LIB)lib_mymemory.a