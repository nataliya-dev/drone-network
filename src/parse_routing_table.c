#include "parse_routing_table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"
#include "shared_structs.h"

int get_next_hop_id(int destination_id) {
  pthread_mutex_lock(&routing_table_mtx);

  char *rt_filename = "routing_table.json";
  const cJSON *drone = NULL;
  const cJSON *drones = NULL;
  int next_hop = -1;

  // struct stat st;
  // stat(rt_filename, &st);
  // size_t file_size = st.st_size;
  // printf("%s file_size %ld\n", rt_filename, file_size);

  int file_desc = open(rt_filename, O_RDONLY, S_IRUSR);
  if (file_desc == -1) {
    printf("error opening file %s\n", rt_filename);
    goto end;
  }

  char data_buffer[MAXBUF];
  size_t frame_size = read(file_desc, data_buffer, MAXBUF);
  if (frame_size <= 0) {
    printf("unable to read file %s\n", rt_filename);
    goto end;
  }

  printf("get_next_hop_id: destination_id; %d\n", destination_id);

  // see if file exists and can be opened etc.
  cJSON *table_json = cJSON_Parse(data_buffer);
  if (table_json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      fprintf(stderr, "Error before: %s\n", error_ptr);
    }
    goto end;
  }

  drones = cJSON_GetObjectItemCaseSensitive(table_json, "drones");
  cJSON_ArrayForEach(drone, drones) {
    cJSON *destination = cJSON_GetObjectItemCaseSensitive(drone, "drone");
    cJSON *hop = cJSON_GetObjectItemCaseSensitive(drone, "next-hop");

    int drone_destination = destination->valueint;
    int drone_hop = hop->valueint;

    if (destination_id == drone_destination) {
      next_hop = drone_hop;
      printf("next_hop %d\n", next_hop);
      goto end;
    }
  }

end:
  cJSON_Delete(table_json);
  pthread_mutex_unlock(&routing_table_mtx);
  return next_hop;
}

int update_drone_ip_file(int drone_id, char *drone_ip) {
  char *rt_filename = "drone_ip.json";
  const cJSON *drone = NULL;
  cJSON *drones = NULL;
  int status = -1;

  /*to acquire mutex to avoid corruption of drone ip json*/
  pthread_mutex_lock(&drone_ip_table_mutex);

  int file_desc = open(rt_filename, O_RDONLY, S_IRUSR);
  if (file_desc == -1) {
    printf("error opening file %s\n", rt_filename);
    goto end;
  }

  char data_buffer[MAXBUF];
  size_t frame_size = read(file_desc, data_buffer, MAXBUF);
  if (frame_size <= 0) {
    printf("unable to read file %s\n", rt_filename);
    goto end;
  }

  cJSON *table_json = cJSON_Parse(data_buffer);
  if (table_json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      fprintf(stderr, "Error before: %s\n", error_ptr);
    }
    goto end;
  }

  drones = cJSON_GetObjectItemCaseSensitive(table_json, "drones");

  cJSON_ArrayForEach(drone, drones) {
    cJSON *number = cJSON_GetObjectItemCaseSensitive(drone, "number");
    cJSON *ip = cJSON_GetObjectItemCaseSensitive(drone, "ip");

    if (!cJSON_IsString(ip)) {
      goto end;
    }

    int cur_drone_num = number->valueint;

    if (drone_id == cur_drone_num) {
      ip->valuestring = drone_ip;

      char *updatedJsonString = cJSON_Print(table_json);

      file_desc = open(rt_filename, O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
      write(file_desc, updatedJsonString, strlen(updatedJsonString));

      status = 1;
      goto end;
    }
  }

  /*if the drone number doesnot exist we need to add it*/
  if (status != 1) {
    printf("inside adding new node to json file \n");

    cJSON *drone = NULL;
    cJSON *json_drone_id = NULL;
    cJSON *json_drone_ip = NULL;

    drone = cJSON_CreateObject();

    json_drone_id = cJSON_CreateNumber(drone_id);
    cJSON_AddItemToObject(drone, "number", json_drone_id);

    json_drone_ip = cJSON_CreateString(drone_ip);
    cJSON_AddItemToObject(drone, "ip", json_drone_ip);

    cJSON_AddItemToArray(drones, drone);

    char *updatedJsonString = cJSON_Print(table_json);

    file_desc = open(rt_filename, O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    write(file_desc, updatedJsonString, strlen(updatedJsonString));
  }

end:

  // printf("json: %s\n", cJSON_Print(table_json));

  pthread_mutex_unlock(&drone_ip_table_mutex);

  return status;
}

int convert_to_drone_ip(int drone_id, char *drone_ip) {
  char *rt_filename = "drone_ip.json";
  const cJSON *drone = NULL;
  const cJSON *drones = NULL;
  int status = -1;

  /*to acquire mutex to avoid corruption of drone ip json*/
  pthread_mutex_lock(&drone_ip_table_mutex);

  int file_desc = open(rt_filename, O_RDONLY, S_IRUSR);
  if (file_desc == -1) {
    printf("error opening file %s\n", rt_filename);
    goto end;
  }

  char data_buffer[MAXBUF];
  size_t frame_size = read(file_desc, data_buffer, MAXBUF);
  if (frame_size <= 0) {
    printf("unable to read file %s\n", rt_filename);
    goto end;
  }

  cJSON *table_json = cJSON_Parse(data_buffer);
  if (table_json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      fprintf(stderr, "Error before: %s\n", error_ptr);
    }
    goto end;
  }

  drones = cJSON_GetObjectItemCaseSensitive(table_json, "drones");

  cJSON_ArrayForEach(drone, drones) {
    cJSON *number = cJSON_GetObjectItemCaseSensitive(drone, "number");
    cJSON *ip = cJSON_GetObjectItemCaseSensitive(drone, "ip");

    if (!cJSON_IsString(ip)) {
      goto end;
    }

    int cur_drone_num = number->valueint;
    char *cur_drone_ip = ip->valuestring;

    if (drone_id == cur_drone_num) {
      strcpy(drone_ip, cur_drone_ip);
      // printf("drone_ip; %s\n", drone_ip);

      status = 1;
      goto end;
    }
  }

end:
  cJSON_Delete(table_json);

  pthread_mutex_unlock(&drone_ip_table_mutex);

  return status;
}