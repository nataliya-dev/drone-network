#include "sender.h"

int connect_to_host(char *hostname, int portno) {
  printf("Hostname: %s\n", hostname);
  printf("Port: %d\n", portno);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    printf("Unable to create socket\n");
    return -1;
  }

  addrlen = sizeof(struct sockaddr_in);
  memset(&servaddr, 0, addrlen);
  servaddr.sin_family = AF_INET;            // IPv4 Internet protocols
  servaddr.sin_port = htons(portno);        // convert to network byte order
  inet_aton(hostname, &servaddr.sin_addr);  // from ipv4 to byte form

  int status = connect(sockfd, (struct sockaddr *)&servaddr, addrlen);
  if (status == -1) {
    printf("Unable to connect to host\n");
    return -1;
  }

  printf("Connected to host!\n");

  return 1;
}

int send_file(image_data_t img) {
  printf("Opening file: %s\n", img.image_name);
  int file_desc = open(img.image_name, O_RDONLY, S_IRUSR);
  if (file_desc == -1) {
    printf("Error opening file\n");
    return -1;
  }

  char databuf[MAXBUF];
  size_t frame_size;

  do {
    bzero(databuf, MAXBUF);
    frame_size = read(file_desc, databuf, MAXBUF);
    printf("frame_size: %ld\n", frame_size);
    if (frame_size == -1) {
      printf("File read error\n");
      break;
    }
    size_t bytes_written = write(sockfd, databuf, strlen(databuf));
    printf("img bytes_written: %ld\n", bytes_written);

  } while (frame_size > 0);

  return 1;
}

void *run_sender(void *vargp) {
  printf("Start run_sender\n");

  thread_data_t thread_data = *((thread_data_t *)vargp);
  routing_entry_t neighbor = thread_data.neighbors[0];
  image_data_t img = thread_data.images[0];

  char *my_ip = " ";

  while (1) {
    if (img.source_ip != my_ip) {
      sleep(2);
      continue;
    }

    int status = connect_to_host(neighbor.destination_ip, PORTNO);
    if (status == -1) {
      return NULL;
    }

    status = send_file(img);
    if (status == -1) {
      return NULL;
    }
  }

  printf("Image sent!\n");
}