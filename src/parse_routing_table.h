#ifndef DRONE_PARSE_ROUTING_TABLE_H
#define DRONE_PARSE_ROUTING_TABLE_H

// check what is the next hop for given drone id
int get_next_hop_id(int destination_id);

// convert the drone id to its ip given the drop ip json file
int convert_to_drone_ip(int drone_id, char* drone_ip);

// add a value to the drone ip json file given an incoming connection from given
// drone
int update_drone_ip_file(int drone_id, char* drone_ip);
#endif