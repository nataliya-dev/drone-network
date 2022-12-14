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

#define DESTINATION_DRONE_ID 3  // the drone that will receive all the images
#define THREAD_SLEEP_TIME_S \
  1  // frequency of sender, receiver, and broadcaster when they scan for
     // neighbors
#define MAX_TIME_NEIGHBOR_SILENT \
  20  // the maximum amount of time that the drone can be absent from sendin
      // broadcasts before the current drone will remove it from its routing
      // table

pthread_mutex_t routing_table_mtx;     // protects routing table json
pthread_mutex_t drone_ip_table_mutex;  // protexts drone ip json

// the content of the broadcast server reply to the hellow message
typedef struct broadcast_reply_s {
  int drone_id;
  char routing_table[MAXBUF];
} broadcast_reply_t;

#endif