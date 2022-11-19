#include "receiver.h"

void *run_receiver(void *arg) {
  printf("Start run_receiver\n");

  int listenfd, *connfdp;
  socklen_t clientlen = sizeof(struct sockaddr_in);
  struct sockaddr_in clientaddr;
  pthread_t tid;

  listenfd = open_listenfd(PORTNO);

  while (1) {
    connfdp = malloc(4 * sizeof(int));

    *connfdp = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);
    if (*connfdp < 1) {
      continue;
    }
    printf("connected to server \n");
    pthread_create(&tid, NULL, thread, connfdp);
  }
}

/* thread routine */
void *thread(void *vargp) {
  int connfd = *((int *)vargp);
  pthread_detach(pthread_self());
  free(vargp);

  handleMessages(connfd);

  printf("connection closed \n");
  close(connfd);
  return NULL;
}

/*
 * open_listenfd - open and return a listening socket on port
 * Returns -1 in case of failure
 */
int open_listenfd(int port) {
  int listenfd;
  // int optval = 1;
  struct sockaddr_in serveraddr;
  socklen_t addrlen;

  /* Create a socket descriptor */
  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    return -1;
  }
  /* Eliminates "Address already in use" error from bind. */
  if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int))) {
    return -1;
  }

  /* listenfd will be an endpoint for all requests to port
     on any IP address for this host */
  bzero((char *)&serveraddr, sizeof(serveraddr));
  addrlen = sizeof(struct sockaddr_in);
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((unsigned short)port);
  if (bind(listenfd, (struct sockaddr *)&serveraddr, addrlen) < 0) return -1;

  /* Make it a listening socket ready to accept connection requests */
  if (listen(listenfd, LISTENQ) < 0) return -1;

  return listenfd;
} /* end open_listenfd */

void move_recvd_file() {
  printf("Moving received file\n");
  char old_name[] = "rcvd.jpg";
  char folder_path[10] = "./imgs";
  char image_name[10] = "img_";
  char new_name[40];
  sprintf(new_name, "%s/%s%d", folder_path, image_name, rand() % 50);
  printf("New image name %s\n", new_name);

  int ret = rename(old_name, new_name);
  printf("rename ret %d\n", ret);

  if (ret == 0) {
    printf("File renamed successfully\n");
  } else {
    printf("Error: unable to rename the file\n");
  }
}

void handleMessages(int connfd) {
  size_t readRetVal;
  size_t writeRetVal;
  char buffer[MAXBUF];

  int fd = open("rcvd.jpg", O_WRONLY | O_CREAT | O_TRUNC | O_APPEND,
                S_IRUSR | S_IWUSR);

  do {
    readRetVal = read(connfd, buffer, MAXBUF);

    writeRetVal = write(fd, buffer, readRetVal);

    // printf("readRetVal = %ld\n", readRetVal);
    // printf("writeRetVal = %ld\n", writeRetVal);

  } while (readRetVal > 0);

  // move_recvd_file();
}
