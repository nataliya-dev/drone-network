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
    sleep(THREAD_SLEEP_TIME_S);
  }
}

/* thread routine */
void *thread(void *vargp) {
  int connfd = *((int *)vargp);
  pthread_detach(pthread_self());
  printf(" thread id  = %ld \n", pthread_self());
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

void move_recvd_file(char *file_name) {
  printf("Moving received file\n");

  // char old_name[] = "rcvd.jpg";
  // char folder_path[10] = "./imgs";
  // char image_name[10] = "img_";
  // char new_name[40];
  // sprintf(new_name, "%s/%s%d", folder_path, image_name, rand() % 50);
  // printf("New image name %s\n", new_name);

  // int ret = rename(old_name, new_name);
  // printf("rename ret %d\n", ret);

  char command[100];
  sprintf(command, "mv %s imgs/%s", file_name, file_name);
  // strcpy(command, "cp file_name imgs/file_name");
  system(command);

  // if (ret == 0) {
  //   printf("File renamed successfully\n");
  // } else {
  //   printf("Error: unable to rename the file\n");
  // }
}

void handleMessages(int connfd) {
  size_t readRetVal;
  size_t writeRetVal;
  int image_count = 0;
  char buffer[MAXBUF];

  char file_name[50];
  sprintf(file_name, "%d_rcvd_%ld.jpg", image_count, pthread_self());
  printf("File name is: %s.", file_name);
  image_count++;

  int fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND,
                S_IRUSR | S_IWUSR);

  struct timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 0;
  setsockopt(connfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv);

  do {
    printf("waiting read\n");
    readRetVal = recv(connfd, buffer, MAXBUF, 0);

    writeRetVal = write(fd, buffer, readRetVal);

    printf("readRetVal = %ld\n", readRetVal);
    printf("writeRetVal = %ld\n", writeRetVal);

  } while (readRetVal != -1);

  move_recvd_file(file_name);
}
