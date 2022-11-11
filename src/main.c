#include "parse_neighbor_list.h"
#include "receiver.h"
#include "sender.h"
#include "shared_structs.h"

thread_data_t thread_data_;

routing_entry_t create_sample_neighbor() {
  routing_entry_t entry;
  entry.destination_ip = "192.168.50.132";
  entry.cost = 0;
  entry.next_hop_ip = " ";
  return entry;
}

image_data_t create_sample_image() {
  image_data_t img;
  img.source_ip = "192.168.50.139";
  img.image_name = "sample.jpg";
  return img;
}

void populate_shared_resources() {
  routing_entry_t entry = create_sample_neighbor();
  image_data_t img = create_sample_image();
  thread_data_.neighbors[0] = entry;
  thread_data_.images[0] = img;
}

int main(int argc, char** argv) {
  printf("Initializing Drone Program!\n");
  populate_shared_resources();
  printf("Finished Initializing Drone Program!\n");

  char* file_name = "neighbor_list.json";
  struct stat st;
  stat(file_name, &st);
  size_t file_size = st.st_size;
  printf("file_size %ld\n", file_size);

  int file_desc = open(file_name, O_RDONLY, S_IRUSR);
  if (file_desc == -1) {
    printf("error opening file\n");
  }

  char data_buffer[MAXBUF];
  size_t frame_size = read(file_desc, data_buffer, MAXBUF);
  printf("strlen(data_buffer) %ld\n", strlen(data_buffer));

  char destination_ip[16];
  get_destination_ip(data_buffer, "192.168.50.1", destination_ip);
  return 0;

  pthread_t thread1;
  pthread_t thread2;
  int iret1, iret2;

  iret1 = pthread_create(&thread1, NULL, run_sender, &thread_data_);
  iret2 = pthread_create(&thread2, NULL, run_receiver, NULL);

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  printf("Thread 1 returns: %d\n", iret1);
  printf("Thread 2 returns: %d\n", iret2);

  return 0;
}