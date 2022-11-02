CFLAGS  = -g -Wall -pthread
CC = gcc

TARGETS = sender_program receiver_program

all: $(TARGETS)

sender_program: src/main.c
	gcc $(CFLAGS) src/main.c -o main
	
receiver_program: src/receiver.c
	gcc $(CFLAGS) src/receiver.c -o receiver

clean:
	rm main
