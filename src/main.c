#include "broadcast.h"
#include "create_routing_table.h"
#include "receiver.h"
#include "sender.h"
#include "shared_structs.h"

int drone_number;

int main(int argc, char** argv) {
  printf("Initializing Drone Program!\n");
  if (argc == 1) {
    printf("Input the Drone Number as an argument\n");
    return 0;
  }
  printf("Number Of Arguments Passed: %d \n", argc);
  printf("Drone Number: %s\n", argv[1]);
  drone_number = atoi(argv[1]);

  clear_routing_table();
  clear_drone_ip_table();

  pthread_t thread1;
  pthread_t thread2;
  pthread_t thread3;

  int iret1, iret2, iret3;

  iret1 = pthread_create(&thread1, NULL, run_sender, NULL);
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