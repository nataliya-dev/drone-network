CFLAGS  = -g -Wall -pthread
CC = gcc

TARGETS = main_program

all: $(TARGETS)

main_program: src/main.c
	gcc $(CFLAGS) src/main.c -o main

clean:
	rm main