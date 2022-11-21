CFLAGS  = -g -Wall -pthread
CC = gcc

OBJFILES = src/cJSON.o src/parse_virtual_switch.o src/parse_routing_table.o src/sender.o src/receiver.o src/b_server.o src/b_client.o src/broadcast.o src/create_routing_table.o src/main.o 
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES)

clean:
	rm -f $(OBJFILES) $(TARGET) *~
