#include "parse_virtual_switch.h"

#include <stdio.h>
#include <string.h>

#include "cJSON.h"

int is_connection_allowed(const char *const monitor, int source_id,
                          int destination_id) {
  const cJSON *drone = NULL;
  const cJSON *drones = NULL;
  int status = 0;

  char source_str[10];
  char dest_str[10];

  sprintf(source_str, "%d", source_id);
  sprintf(dest_str, "%d", destination_id);

  printf("is_connection_allowed: source_id; %s\n", source_str);
  printf("is_connection_allowed: dest_str; %s\n", dest_str);

  // see if file exists and can be opened etc.
  cJSON *monitor_json = cJSON_Parse(monitor);
  if (monitor_json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      fprintf(stderr, "Error before: %s\n", error_ptr);
    }
    goto end;
  }

  drones = cJSON_GetObjectItemCaseSensitive(monitor_json, "drones");
  cJSON_ArrayForEach(drone, drones) {
    cJSON *source = cJSON_GetObjectItemCaseSensitive(drone, "source");
    cJSON *destination = cJSON_GetObjectItemCaseSensitive(drone, "destination");

    if (!cJSON_IsString(source) || !cJSON_IsString(destination)) {
      goto end;
    }

    char *drone_source_id = source->valuestring;
    char *drone_dest_id = destination->valuestring;

    if (strcmp(source_str, drone_source_id) == 0 &&
        strstr(drone_dest_id, dest_str) != NULL) {
      status = 1;
      goto end;
    }
  }

end:
  cJSON_Delete(monitor_json);
  return status;
}
