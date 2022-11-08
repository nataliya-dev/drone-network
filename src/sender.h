#ifndef DRONE_SENDER_H
#define DRONE_SENDER_H

#include "shared_structs.h"

int sockfd;
struct sockaddr_in servaddr;
socklen_t addrlen;

int connect_to_host(char *hostname, int portno);
int send_file(image_data_t img);
void run_sender(routing_entry_t neighbors[], image_data_t images[]);

#endif