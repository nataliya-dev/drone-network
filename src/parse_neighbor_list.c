#include "parse_neighbor_list.h"

#include <stdio.h>
#include <string.h>

#include "cJSON.h"

int get_destination_ip(const char *const monitor, const char *source_ip,
                       char *destination_ip) {
  const cJSON *drone = NULL;
  const cJSON *drones = NULL;
  int status = 0;

  printf("source_ip; %s\n", source_ip);

  // see if file exists and can be opened etc.
  cJSON *monitor_json = cJSON_Parse(monitor);
  if (monitor_json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      fprintf(stderr, "Error before: %s\n", error_ptr);
    }
    status = 0;
    goto end;
  }

  drones = cJSON_GetObjectItemCaseSensitive(monitor_json, "drones");
  cJSON_ArrayForEach(drone, drones) {
    cJSON *source = cJSON_GetObjectItemCaseSensitive(drone, "source");
    cJSON *destination = cJSON_GetObjectItemCaseSensitive(drone, "destination");

    if (!cJSON_IsString(source) || !cJSON_IsString(destination)) {
      status = 0;
      goto end;
    }

    char *drone_source_ip = source->valuestring;
    printf("drone_source_ip; %s\n", drone_source_ip);

    if (strcmp(source_ip, drone_source_ip) == 0) {
      printf("Destination found!\n");
      strcpy(destination_ip, destination->valuestring);
      printf("destination_ip; %s\n", destination_ip);
      status = 1;
    }
  }

end:
  cJSON_Delete(monitor_json);
  printf("destination_ip; %s\n", destination_ip);

  return status;
}