#ifndef DRONE_PARSE_VIRTUAL_SWITCH_H
#define DRONE_PARSE_VIRTUAL_SWITCH_H

// check if a drone is allowed to connect with another drone
// establishes rules for direct neighbors
int is_connection_allowed(const char *const monitor, int source_id,
                          int destination_id);

#endif