#ifndef DRONE_SENDER_H
#define DRONE_SENDER_H

#include "parse_routing_table.h"
#include "shared_structs.h"

int sockfd;
struct sockaddr_in servaddr;
socklen_t addrlen;

int connect_to_host(char *hostname, int portno);
int send_file();
void *run_sender(void *vargp);
void get_img_name(char *img_name);
int filter_dir(const struct dirent *e);
int get_num_available_files();

#endif