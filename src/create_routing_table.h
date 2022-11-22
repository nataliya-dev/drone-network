#ifndef CREATE_ROUTING_TABLE_H
#define CREATE_ROUTING_TABLE_H
#include "cJSON.h"
#include "shared_structs.h"

char *create_routing_table(void);
int update_my_routing_table(char *neighbor_table, int neighbor_id);
cJSON *entry_exists(cJSON *my_routing_table, int drone_destination);
cJSON *get_my_routing_table_json();
cJSON *add_to_table(cJSON *my_routing_table, int destination, int nexthop,
                    int cost);
int write_table_to_file(char *string);
#endif