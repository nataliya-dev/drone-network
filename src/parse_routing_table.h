#ifndef DRONE_PARSE_ROUTING_TABLE_H
#define DRONE_PARSE_ROUTING_TABLE_H

int get_next_hop_id(int destination_id);
int convert_to_drone_ip(int drone_id, char* drone_ip);
#endif