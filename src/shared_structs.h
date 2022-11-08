#ifndef DRONE_SHARED_STRUCTS_H
#define DRONE_SHARED_STRUCTS_H

#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define MAXBUF 8192
#define LISTENQ 50
#define MAX_NEIGHBORS 100
#define MAX_IMAGES 100
#define PORTNO 8888

typedef struct routing_entry_s {
  char *source_ip;
  char *destination_ip;
  size_t cost;
  char *next_hop_ip;
} routing_entry_t;

typedef struct image_data_s {
  char *source_ip;
  char *image_name;
} image_data_t;

typedef struct thread_data_s {
  routing_entry_t neighbors[MAX_NEIGHBORS];
  image_data_t images[MAX_IMAGES];
} thread_data_t;

#endif