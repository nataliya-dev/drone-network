#include "sender.h"

#include <dirent.h>
#include <errno.h>
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
    printf("Unable to connect to host %d\n", errno);
    return -1;
  }

  printf("Connected to host!\n");

  return 1;
}

int filter_dir(const struct dirent *e) {
  struct stat st;
  stat(e->d_name, &st);
  return !(st.st_mode & S_IFDIR);
}

int get_num_available_files() {
  struct dirent **namelist;
  int file_num = scandir("./imgs", &namelist, NULL, alphasort);
  file_num = file_num - 2;
  printf("Num files: %d\n", file_num);
  free(namelist);
  return file_num;
}

void get_img_name(char *img_name) {
  struct dirent **namelist;

  /* Obtain a list of all files in the directory. Use the filter_dir
   * function to not include other directories like . and ..*/
  int file_num = scandir("./imgs", &namelist, NULL, alphasort);
  // printf("Num files: %d\n", file_num);
  if (file_num <= 0) {
    return;
  } else {
    while (file_num--) {
      // store each filename in the buffer
      printf("namelist[file_num]->d_name %s\n", namelist[file_num]->d_name);
      if (strstr(namelist[file_num]->d_name, ".png") != NULL ||
          strstr(namelist[file_num]->d_name, ".jpg") != NULL) {
        strcpy(img_name, namelist[file_num]->d_name);
        free(namelist[file_num]);
      } else {
        free(namelist[file_num]);
        continue;
      }
    }
  }
  // free memory
  free(namelist);
}

int send_file() {
  char img_name[25];
  get_img_name(img_name);
  if (strlen(img_name) == 0) {
    printf("No images found to be sent.\n");
    return -1;
  }
  printf("img_name: %s\n", img_name);

  char file_path[50];
  char *folder_name = "./imgs";
  sprintf(file_path, "%s/%s", folder_name, img_name);
  size_t header_total = strlen(file_path);
  printf("header_total: %ld\n", header_total);

  printf("Opening file: %s\n", file_path);
  int file_desc = open(file_path, O_RDONLY, S_IRUSR);
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
    size_t bytes_written = write(sockfd, databuf, frame_size);
    printf("img bytes_written: %ld\n", bytes_written);

  } while (frame_size > 0);

  int is_rem = remove(file_path);
  if (is_rem == 0) {
    printf("File deleted successfully.\n");
  } else {
    perror("Delete file error.\n");
  }

  return 1;
}

void *run_sender(void *vargp) {
  printf("Start run_sender\n");
  while (1) {
    int num_files = get_num_available_files();
    if (num_files <= 0) {
      printf("No available files for sending\n");
      goto end;
    }

    int status;

    int next_hop = get_next_hop_id(DESTINATION_DRONE_ID);
    if (next_hop == -1) {
      printf("error parsing json file for destination drone %d\n",
             DESTINATION_DRONE_ID);
      goto end;
    }

    char destination_ip[16];
    status = convert_to_drone_ip(next_hop, destination_ip);
    if (status == -1) {
      printf("error converting this drone id to ip %d\n", next_hop);
      goto end;
    }

    status = connect_to_host(destination_ip, PORTNO);
    if (status == -1) {
      goto end;
    }

    status = send_file();
    if (status == -1) {
      goto end;
    }
    printf("Image sent!\n");

  end:
    sleep(THREAD_SLEEP_TIME_S);
    continue;
  }
}