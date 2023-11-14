# Comandos
CC = gcc
CFLAGS = -Wall -O2
ARCHIVE = ar rcs
# Diretórios
BUILD = ./build/
BIN = ./bin/
LIB = ./lib/
INCLUDE = ./include/
SRC = ./src/

clean:
	rm -f $(LIB)*.a $(BIN)* $(BUILD)*.o ./main

main.o: $(SRC)main.c
	$(CC) $(CFLAGS) -c $(SRC)main.c -o $(BUILD)main.o

main: main.o
	$(CC) $(CFLAGS) $(BUILD)main.o -o $(BIN)main

all: main
	$(BIN)main