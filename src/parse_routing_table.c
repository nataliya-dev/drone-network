#include "parse_routing_table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"
#include "shared_structs.h"

int get_next_hop_id(int destination_id) {
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
    cJSON *destination = cJSON_GetObjectItemCaseSensitive(drone, "destination");
    cJSON *hop = cJSON_GetObjectItemCaseSensitive(drone, "next-hop");

    if (!cJSON_IsString(hop) || !cJSON_IsString(destination)) {
      goto end;
    }

    char *drone_destination = destination->valuestring;
    char *drone_hop = hop->valuestring;

    char dest_str[10];
    sprintf(dest_str, "%d", destination_id);

    if (strcmp(drone_destination, dest_str) == 0) {
      next_hop = atoi(drone_hop);
      printf("next_hop %d\n", next_hop);
      goto end;
    }
  }

end:
  cJSON_Delete(table_json);
  return next_hop;
}

int convert_to_drone_ip(int drone_id, char *drone_ip) {
  char *rt_filename = "drone_ip.json";
  const cJSON *drone = NULL;
  const cJSON *drones = NULL;
  int status = -1;

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

    if (!cJSON_IsString(number) || !cJSON_IsString(ip)) {
      goto end;
    }

    char *cur_drone_num = number->valuestring;
    char *cur_drone_ip = ip->valuestring;

    char drone_num_str[16];
    sprintf(drone_num_str, "%d", drone_id);

    if (strcmp(drone_num_str, cur_drone_num) == 0) {
      strcpy(drone_ip, cur_drone_ip);
      // printf("drone_ip; %s\n", drone_ip);

      status = 1;
      goto end;
    }
  }

end:
  cJSON_Delete(table_json);
  return status;
}