#include "broadcast.h"
#include "parse_neighbor_list.h"
#include "receiver.h"
#include "sender.h"
#include "shared_structs.h"

thread_data_t thread_data_;

void init_thread_data() {
  printf("Initializing thread data.\n");
  return;
}

int main(int argc, char** argv) {
  printf("Initializing Drone Program!\n");

  init_thread_data();

  pthread_t thread1;
  pthread_t thread2;
  pthread_t thread3;

  int iret1, iret2, iret3;

  iret1 = pthread_create(&thread1, NULL, run_sender, &thread_data_);
  iret2 = pthread_create(&thread2, NULL, run_receiver, NULL);
  iret3 = pthread_create(&thread3, NULL, run_broadcast, NULL);

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  pthread_join(thread3, NULL);

  printf("Thread 1 returns: %d\n", iret1);
  printf("Thread 2 returns: %d\n", iret2);
  printf("Thread 3 returns: %d\n", iret3);

  return 0;
}