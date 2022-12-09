#ifndef CREATE_ROUTING_TABLE_H
#define CREATE_ROUTING_TABLE_H
#include "cJSON.h"
#include "shared_structs.h"

// given the neighbor routing table, its id, and current drone ip, use
// link-state OSPF to update the routing table
int update_my_routing_table(char *neighbor_table, int neighbor_id, int my_id);

// check if the destination drone already exists in routing table
cJSON *entry_exists(cJSON *my_routing_table, int drone_destination);

// read the routing table into cJSON format
cJSON *get_my_routing_table_json();

// add the following entry to the table
cJSON *add_to_table(cJSON *my_routing_table, int destination, int nexthop,
                    int cost);

// given the routing table in string, write it to a json file in memory
int write_table_to_file(char *string);

// clear any entries in these two tables
void clear_routing_table();
void clear_drone_ip_table();

// remove any entry that has the given drone number as an entry
// this means we haven't heard from this drone in a while
cJSON *remove_from_table(cJSON *my_routing_table, int drone_number);

// update the system time for the given drone
// this means we recently received a broadcast from this drone
cJSON *update_last_seen(cJSON *my_routing_table, int drone_number);

// remove any drone from the routing table who have a last-seen time to be a
// long time ago
void remove_inactive(void);

// check if this neighbor exists in the routing table
cJSON *neighbor_entry_exists(cJSON *my_routing_table, int drone_destination);
#endif