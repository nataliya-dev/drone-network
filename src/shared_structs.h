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
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define MAXBUF 8192
#define MAXLINE 1024

#define LISTENQ 50
#define PORTNO 8080

typedef struct broadcast_reply_s {
  int drone_id;
  char routing_table[MAXBUF];
} broadcast_reply_t;

typedef struct thread_data_s {
} thread_data_t;

#endif