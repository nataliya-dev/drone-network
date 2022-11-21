#include "broadcast.h"

#include <stdio.h>

#include "b_client.h"
#include "b_server.h"
#include "shared_structs.h"

void* run_broadcast(void* arg) {
  pthread_t thread1;
  pthread_t thread2;

  int iret1, iret2;

  iret1 = pthread_create(&thread1, NULL, broadcast_server, NULL);
  sleep(1);
  iret2 = pthread_create(&thread2, NULL, broadcast_client, NULL);

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  printf("Thread 1 returns: %d\n", iret1);
  printf("Thread 2 returns: %d\n", iret2);
  return NULL;
}