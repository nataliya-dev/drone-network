#include "broadcast.h"
#include "parse_neighbor_list.h"
#include "receiver.h"
#include "sender.h"
#include "shared_structs.h"
#include "create_routing_table.h"

thread_data_t thread_data_;
int drone_number;

void init_thread_data() {
  printf("Initializing thread data.\n");
  return;
}

int main(int argc, char** argv) {
  printf("Initializing Drone Program!\n");
  if(argc==1){
    printf("Input the Drone Number as an argument\n");
    return 0;
  }
  printf("Number Of Arguments Passed: %d \n",argc);
  printf("Drone Number: %s\n",argv[1]);
  drone_number = atoi(argv[1]);


  init_thread_data();

  //printf("%s\n", create_monitor());

  /*to create the routing table and write it into a json file*/
  int fd = open("routing_table_1.json", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

  char *retStr = create_routing_table();
  char buff[10000];
  int i = 0, size = 0;

  /*to convert a string to character buffer*/
  while(retStr[i] != '\0')
  {
    buff[i] = retStr[i];
    i++;
  }
  size = i;

  /*writing to the json file*/
  write(fd, buff, size);

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