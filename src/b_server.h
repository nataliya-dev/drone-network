#ifndef B_SERVER_H
#define B_SERVER_H

#include "shared_structs.h"

void* broadcast_server(void* arg);
broadcast_reply_t create_broadcast_reply();

#endif