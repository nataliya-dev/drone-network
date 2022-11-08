#ifndef DRONE_RECEIVER_H
#define DRONE_RECEIVER_H
#include "shared_structs.h"

int open_listenfd(int port);
void *thread(void *vargp);
void handleMessages(int connfd);
void *run_receiver(void *);

#endif