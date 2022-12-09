#ifndef DRONE_SENDER_H
#define DRONE_SENDER_H

#include "parse_routing_table.h"
#include "shared_structs.h"

int sockfd;
struct sockaddr_in servaddr;
socklen_t addrlen;

// socket related functions
int connect_to_host(char *hostname, int portno);
void *run_sender(void *vargp);

// packet by packet sending of a file
int send_file();

// get the image name from the list of images in the imgs directory
void get_img_name(char *img_name);

// avoid the . and .. directories in the image search
int filter_dir(const struct dirent *e);

// check if there are any images that need to be sent
int get_num_available_files();

#endif