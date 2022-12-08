// create a monitor with a list of supported resolutions
// NOTE: Returns a heap allocated string, you are required to free it after use.
#include "create_routing_table.h"

#include <stdio.h>
#include <time.h>

cJSON *get_my_routing_table_json() {
  // printf("get_my_routing_table_json\n");
  cJSON *my_table = NULL;
  char *rt_filename = "routing_table.json";

  // struct stat st;
  // stat(rt_filename, &st);
  // size_t file_size = st.st_size;
  // printf("%s file_size %ld\n", rt_filename, file_size);

  int file_desc = open(rt_filename, O_RDONLY, S_IRUSR);
  if (file_desc == -1) {
    printf("error opening file %s\n", rt_filename);
    return my_table;
  }

  char data_buffer[MAXBUF];
  size_t frame_size = read(file_desc, data_buffer, MAXBUF);
  if (frame_size <= 0) {
    printf("unable to read file %s\n", rt_filename);
    return my_table;
  }

  // see if file exists and can be opened etc.
  my_table = cJSON_Parse(data_buffer);
  if (my_table == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      fprintf(stderr, "Error before: %s\n", error_ptr);
    }
    return my_table;
  }

  // char *string = cJSON_Print(my_table);
  // printf("my_routing_table \n%s\n", string);

  return my_table;
}

cJSON *entry_exists(cJSON *my_routing_table, int drone_destination) {
  cJSON *entry = NULL;
  // cJSON *drone = NULL;
  cJSON *drones = cJSON_GetObjectItemCaseSensitive(my_routing_table, "drones");
  cJSON_ArrayForEach(entry, drones) {
    cJSON *destination = cJSON_GetObjectItemCaseSensitive(entry, "drone");
    if (destination->valueint == drone_destination) {
      return entry;
    }
  }
  return entry;
}

cJSON *neighbor_entry_exists(cJSON *my_routing_table, int drone_destination) {
  cJSON *entry = NULL;
  // cJSON *drone = NULL;
  cJSON *drones = cJSON_GetObjectItemCaseSensitive(my_routing_table, "drones");
  cJSON_ArrayForEach(entry, drones) {
    cJSON *destination = cJSON_GetObjectItemCaseSensitive(entry, "drone");
    cJSON *next_hop = cJSON_GetObjectItemCaseSensitive(entry, "next-hop");
    cJSON *cost = cJSON_GetObjectItemCaseSensitive(entry, "cost");

    if (destination->valueint == drone_destination) {
      cJSON_SetNumberValue(next_hop, drone_destination);
      cJSON_SetNumberValue(cost, 1);
      return entry;
    }
  }
  return entry;
}

cJSON *add_to_table(cJSON *my_routing_table, int destination, int nexthop,
                    int cost) {
  cJSON *drones = cJSON_GetObjectItemCaseSensitive(my_routing_table, "drones");
  cJSON *new_destination = cJSON_CreateObject();
  cJSON *item = cJSON_CreateNumber(destination);
  cJSON_AddItemToObject(new_destination, "drone", item);
  item = cJSON_CreateNumber(nexthop);
  cJSON_AddItemToObject(new_destination, "next-hop", item);
  item = cJSON_CreateNumber(cost);
  cJSON_AddItemToObject(new_destination, "cost", item);
  cJSON_AddItemToArray(drones, new_destination);
  return my_routing_table;
}

cJSON *remove_from_table(cJSON *my_routing_table, int drone_number) {
  cJSON *drones = cJSON_GetObjectItemCaseSensitive(my_routing_table, "drones");
  cJSON *drone = NULL;

  const int max_num_drones = 10;
  int to_remove[max_num_drones];
  for (size_t i = 0; i < max_num_drones; i++) {
    to_remove[i] = -1;
  }

  int i = 0;
  cJSON_ArrayForEach(drone, drones) {
    cJSON *next_hop = cJSON_GetObjectItemCaseSensitive(drone, "next-hop");
    if (next_hop->valueint == drone_number) {
      printf("deleting from array %d\n", i);
      printf("going to remove drone_number %u\n", drone_number);
      to_remove[i] = 1;
      break;
    }
    i++;
  }

  for (size_t i = 0; i < max_num_drones; i++) {
    int status = to_remove[i];
    // printf("status %ld: %d\n", i, status);
    if (status == 1) {
      printf("deleting from array %ld\n", i);
      cJSON_DeleteItemFromArray(drones, i);
      break;
    }
  }

  return my_routing_table;
}

cJSON *update_last_seen(cJSON *my_routing_table, int drone_number) {
  cJSON *drones =
      cJSON_GetObjectItemCaseSensitive(my_routing_table, "neighbors");

  int is_added = -1;
  const cJSON *neighbor = NULL;
  cJSON *drone = NULL;
  cJSON *last_seen = NULL;

  cJSON_ArrayForEach(neighbor, drones) {
    drone = cJSON_GetObjectItemCaseSensitive(neighbor, "drone");
    last_seen = cJSON_GetObjectItemCaseSensitive(neighbor, "last-seen");
    if (drone->valueint == drone_number) {
      printf("updating last seen value for drone num %u\n", drone_number);
      printf("last_seen->valueint %u\n", last_seen->valueint);
      int number = time(NULL);
      cJSON_SetNumberValue(last_seen, number);
      printf("last_seen->valueint %u\n", last_seen->valueint);
      is_added = 1;
    }
  }

  if (is_added == -1) {
    printf("adding new last seen value for drone num %u\n", drone_number);
    drones = cJSON_GetObjectItemCaseSensitive(my_routing_table, "neighbors");
    cJSON *new_neighbor = cJSON_CreateObject();
    cJSON *item = cJSON_CreateNumber(drone_number);
    cJSON_AddItemToObject(new_neighbor, "drone", item);
    int number = time(NULL);
    item = cJSON_CreateNumber(number);
    cJSON_AddItemToObject(new_neighbor, "last-seen", item);
    cJSON_AddItemToArray(drones, new_neighbor);
  }

  return my_routing_table;
}

void remove_inactive(void) {
  // for all items in neighbors array
  // get drone number
  // get timeout value
  // check if current time - last seen time is greater than theshold
  // if yes, then remove_from_table()
  // if no, then continue

  char *rt_filename = "routing_table.json";
  const cJSON *drone = NULL;
  cJSON *neighbors = NULL;

  pthread_mutex_lock(&routing_table_mtx);

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

  neighbors = cJSON_GetObjectItemCaseSensitive(table_json, "neighbors");

  cJSON_ArrayForEach(drone, neighbors) {
    cJSON *drone_ptr = cJSON_GetObjectItemCaseSensitive(drone, "drone");

    cJSON *lastSeenTime_ptr =
        cJSON_GetObjectItemCaseSensitive(drone, "last-seen");

    int cur_drone_num = drone_ptr->valueint;
    int last_seen_time = lastSeenTime_ptr->valueint;

    int current_time = time(NULL);

    printf("cur_drone_num = %d\n", cur_drone_num);
    printf("lastSeenTIme = %d\n", last_seen_time);
    printf("(current_time - last_seen_time) = %d\n",
           (current_time - last_seen_time));

    if ((current_time - last_seen_time) > MAX_TIME_NEIGHBOR_SILENT) {
      remove_from_table(table_json, cur_drone_num);
    }
  }

end:;
  clear_routing_table();
  char *table_str;
  table_str = cJSON_Print(table_json);
  // printf("===== table_json \n%s\n", table_str);
  int status = write_table_to_file(table_str);
  if (status == -1) {
    printf("Writing the data to file has failed\n");
  }

  pthread_mutex_unlock(&routing_table_mtx);
}

int update_my_routing_table(char *neighbor_table, int neighbor_id, int my_id) {
  pthread_mutex_lock(&routing_table_mtx);
  // printf("update_my_routing_table\n");
  int status = -1;
  const cJSON *drone = NULL;
  const cJSON *drones = NULL;

  cJSON *neighbor_json = cJSON_Parse(neighbor_table);
  if (neighbor_json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      fprintf(stderr, "Error before: %s\n", error_ptr);
    }
    goto end;
  }

  cJSON *my_routing_table = get_my_routing_table_json();
  if (my_routing_table == NULL) {
    printf("error get_my_routing_table_json\n");
    goto end;
  }

  my_routing_table = update_last_seen(my_routing_table, neighbor_id);

  cJSON *my_dest_entry = neighbor_entry_exists(my_routing_table, neighbor_id);
  if (my_dest_entry == NULL) {
    printf("neighbor does not exist, adding to table\n");
    my_routing_table =
        add_to_table(my_routing_table, neighbor_id, neighbor_id, 1);
  } else {
    printf("neighbor exists, no need to add new one\n");
  }

  drones = cJSON_GetObjectItemCaseSensitive(neighbor_json, "drones");

  int neighbor_size = cJSON_GetArraySize(drones);
  printf("num drones in routing array %d\n", neighbor_size);
  if (neighbor_size == 0) {
    goto end;
  }

  cJSON_ArrayForEach(drone, drones) {
    cJSON *destination = cJSON_GetObjectItemCaseSensitive(drone, "drone");
    cJSON *cost = cJSON_GetObjectItemCaseSensitive(drone, "cost");

    int drone_destination = destination->valueint;
    int drone_cost = cost->valueint;

    if (my_id == drone_destination) {
      continue;
    }

    // printf("drone_destination %d\n", drone_destination);
    // printf("drone_cost %d\n", drone_cost);

    cJSON *my_dest_entry = entry_exists(my_routing_table, drone_destination);
    if (my_dest_entry == NULL) {
      printf("entry does not exist, adding to table\n");
      my_routing_table = add_to_table(my_routing_table, drone_destination,
                                      neighbor_id, drone_cost + 1);
      continue;
    } else {
      printf("entry exists, no need to add new one\n");
    }

    cJSON *my_hop = cJSON_GetObjectItemCaseSensitive(my_dest_entry, "next-hop");
    cJSON *my_cost = cJSON_GetObjectItemCaseSensitive(my_dest_entry, "cost");

    // printf("my_hop->valueint %d\n", my_hop->valueint);
    // printf("my_cost->valueint %d\n", my_cost->valueint);

    if (my_hop->valueint == neighbor_id) {
      cJSON_SetNumberValue(my_hop, drone_cost + 1);
      continue;
    }

    if (my_cost->valueint > drone_cost + 1) {
      cJSON_SetNumberValue(my_cost, drone_cost + 1);
      cJSON_SetNumberValue(my_hop, neighbor_id);
      continue;
    }
  }
  status = 1;

end:;
  char *table_str;
  table_str = cJSON_Print(my_routing_table);
  printf("===== my_routing_table \n%s\n", table_str);
  status = write_table_to_file(table_str);
  if (status == -1) {
    printf("Writing the data to file has failed\n");
  }
  cJSON_Delete(neighbor_json);
  pthread_mutex_unlock(&routing_table_mtx);
  return status;
}

int write_table_to_file(char *string) {
  int fd = open("routing_table.json", O_RDWR, S_IRUSR | S_IWUSR);
  return write(fd, string, strlen(string));
}

void clear_routing_table() {
  int fd =
      open("routing_table.json", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  cJSON *table = cJSON_CreateObject();
  cJSON *drones = cJSON_CreateArray();
  cJSON *neighbors = cJSON_CreateArray();
  cJSON_AddItemToObject(table, "drones", drones);
  cJSON_AddItemToObject(table, "neighbors", neighbors);
  char *table_str;
  table_str = cJSON_Print(table);
  write(fd, table_str, strlen(table_str));
}

void clear_drone_ip_table() {
  int fd = open("drone_ip.json", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  cJSON *table = cJSON_CreateObject();
  cJSON *drones = cJSON_CreateArray();
  cJSON_AddItemToObject(table, "drones", drones);
  char *table_str;
  table_str = cJSON_Print(table);
  write(fd, table_str, strlen(table_str));
}