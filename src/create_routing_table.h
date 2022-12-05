#ifndef CREATE_ROUTING_TABLE_H
#define CREATE_ROUTING_TABLE_H
#include "cJSON.h"
#include "shared_structs.h"

char *create_routing_table(void);
int update_my_routing_table(char *neighbor_table, int neighbor_id, int my_id);
cJSON *entry_exists(cJSON *my_routing_table, int drone_destination);
cJSON *get_my_routing_table_json();
cJSON *add_to_table(cJSON *my_routing_table, int destination, int nexthop,
                    int cost);
int write_table_to_file(char *string);
void clear_routing_table();
void clear_drone_ip_table();

cJSON *remove_from_table(cJSON *my_routing_table, int drone_number);
cJSON *remove_inactive(cJSON *my_routing_table);
cJSON *update_last_seen(cJSON *my_routing_table, int drone_number);

#endif