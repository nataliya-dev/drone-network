CFLAGS  = -g -Wall -pthread
CC = gcc
OBJFILES = src/sender.o src/receiver.o src/main.o
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES)

clean:
	rm -f $(OBJFILES) $(TARGET) *~
