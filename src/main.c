#include "receiver.h"
#include "sender.h"
#include "shared_structs.h"

routing_entry_t neighbors[MAX_NEIGHBORS];
image_data_t images[MAX_IMAGES];

routing_entry_t create_sample_neighbor() {
  routing_entry_t entry;
  entry.destination_ip = "192.168.50.249";
  entry.cost = 0;
  entry.next_hop_ip = "192.168.50.249";
  return entry;
}

image_data_t create_sample_image() {
  image_data_t img;
  img.source_ip = "2.2.2.2";
  img.image_name = "sample.jpg";
  return img;
}

void populate_shared_resources() {
  routing_entry_t entry = create_sample_neighbor();
  image_data_t img = create_sample_image();
  neighbors[0] = entry;
  images[0] = img;
}

int main(int argc, char** argv) {
  printf("Initializing Drone Program!\n");
  populate_shared_resources();

  // pthread_t thread_id;
  // pthread_create(&thread_id, NULL, run_sender, neighbors, images);

  run_sender(neighbors, images);

  return 0;
}