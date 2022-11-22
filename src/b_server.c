#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
//#include <linux/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/socket.h>

#include "b_server.h"

#define IP_FOUND "IP_FOUND"
#define IP_FOUND_ACK "IP_FOUND_ACK"
#define PORT 9999

extern int drone_number;

broadcast_reply_t create_broadcast_reply() {
  broadcast_reply_t reply;
  reply.drone_id = drone_number;  // fill in with global id value
  char filename[MAXLINE] = "routing_table.json";

  // printf("Opening file: %s\n", filename);
  int file_desc = open(filename, O_RDONLY, S_IRUSR);
  if (file_desc == -1) {
    printf("Error opening file\n");
  }

  size_t frame_size = read(file_desc, reply.routing_table, MAXBUF);
  // printf("frame_size: %ld\n", frame_size);
  if (frame_size == -1) {
    printf("File read error\n");
  }

  return reply;
}

void *broadcast_server(void *arg) {
  int sock;
  // int yes = 1;
  struct sockaddr_in client_addr;
  struct sockaddr_in server_addr;
  socklen_t addr_len;
  int ret;
  fd_set readfd;
  char buffer[MAXLINE];

  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    perror("sock error\n");
    // return -1;
  }

  addr_len = sizeof(struct sockaddr_in);

  memset((void *)&server_addr, 0, addr_len);
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htons(INADDR_ANY);
  server_addr.sin_port = htons(PORT);

  ret = bind(sock, (struct sockaddr *)&server_addr, addr_len);
  if (ret < 0) {
    perror("bind error\n");
    // return -1;
  }
  while (1) {
    FD_ZERO(&readfd);
    FD_SET(sock, &readfd);

    ret = select(sock + 1, &readfd, NULL, NULL, 0);
    if (ret > 0) {
      if (FD_ISSET(sock, &readfd)) {
        recvfrom(sock, buffer, MAXLINE, 0, (struct sockaddr *)&client_addr,
                 &addr_len);
        if (strstr(buffer, IP_FOUND)) {
          printf(
              "b_server:Client connection information:\n\t IP: %s, Port: %d\n",
              inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

          // memcpy(buffer, IP_FOUND_ACK, strlen(IP_FOUND_ACK) + 1);
          broadcast_reply_t reply = create_broadcast_reply();
          sendto(sock, &reply, sizeof(reply), 0,
                 (struct sockaddr *)&client_addr, addr_len);
        }
      }
    }
  }
}
