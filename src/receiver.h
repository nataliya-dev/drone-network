#ifndef DRONE_RECEIVER_H
#define DRONE_RECEIVER_H
#include "shared_structs.h"

// socket related functions
int open_listenfd(int port);
void *thread(void *vargp);

// move the final received image to imgs directory where the sender can retrieve
// them
void move_recvd_file();

// packet by packet receipt of image
void receive_file(int connfd);

// spins a thread when a new connection is a estbalished
void *run_receiver(void *);

#endif