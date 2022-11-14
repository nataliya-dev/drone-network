#ifndef DRONE_PARSE_NEIGHBOR_H
#define DRONE_PARSE_NEIGHBOR_H

int get_destination_ip(const char *const monitor, const char *source_ip,
                       char *destination_ip);
int get_source_ip(const char *const monitor, char *source_ip);

#endif