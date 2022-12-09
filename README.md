# DroneNet: Implementing a shortest path multi-hop image transfer architecture based on OSPF 
## Overview of the capabilites
- Ability to control the drone topology using a json file
- Dynamic discovery of neighbors id numbers and their ip addresses
- Dyanamic calculation of next-hop drone that will be the fastest route to destination
- Removal of a neighbor from the routing table if the neighbor has not responded to a broadcast for a period of time
## How to run the code
Clone the directory to at least two machines which share the same network. Then build the code.
```
cd drone-network
make clean
cmake
```

Prior to running the code, the virtual_switch.json needs to be configured. This defined the communication between various drones. If a machine is running as drone number 1, then the virtual switch file needs to contain an entry for with "source":1. If it communicate with drones number 2 and 3 then it needs to have "destination": "2,3" right under the source. This way the drone knows it can only communicate with two other drones. It will ignore all other ones. 

Repeat this process for any other machine running the code. 

Then you can run the following command on each machine. Make sure the drone number is unique to each machine and matches your desired configuration values in virtual_switch.json:
```
./main <drone number>
```

## Dynamic updates during runtime
- routing_table.json : The results of the OSPF algorithm are stored here. The drone keeps track of infromation regarding the drones in the network, how to get to them (next-hop), and the cost to get to them. The values get updates when new drones appear or existing dreones disappear. 
- drone_ip.json : The drone needs ip adresses to send and receive files. However, it is not reader-friendly to deal with ip addresses. Therefore each drone stores a conversion map from drone numbers to their ip adresses in these tables. This table is built dynamically via the broadcast client and server. 
- imgs directory : When you would like to transfer a file from one drone to another, then you need to drop some images in the the imgs directory. The sender thread constantly checks this folder to see if anything comes up. When there is an image, a path to the destination, and a way to connect to the next-hop to the destination, then it sends the image. 

## Basic file overview
- main.c - Spawn the broadcast, receiver, and sender threads.
- receiver.c/h - Receive a file from neighboring drone.
- sender.c/h - Send a file to a neighboring drone. 
- broadcast.c/h - Spawn the b_client/b_server threads. 
- b_client.c/h - Request a routing table from any broadcasters. 
- b_server.c.h - Send routing table to any clients after having broadcasted own ip address and drone number.
- create_routing_table.c/h - The main routing algorithm based on OSPF is located here. The routing table gets updated based on the broadcast client data received. This modfifies the routing_table.json which is then sent by the broadcast server. 
- parse_routing_table.c/h - Used to create and modify the drone_ip.json file, as well as to get the next-hop value for the sender.c to figure out which neighbor to send the image to.
- parse_virtual_switch.c/h - Functions that parse the virtual_switch.json and retreive the allowed neighbors. 
- shared_structs.h - Stores the structs used to communicate between drones, for example to share the routing table, stores mutexes, parameters that can be modified, mutexes and relevant header files. 
- cJSON.c/h - External library used to read and write json file data.

## Parameters that can be modified
- DESTINATION_DRONE_ID : located in shared_structs.h and will define which drone is the destination drone for all the image. 
- THREAD_SLEEP_TIME_S : located in shared_structs.h and will define how much time each thread sleeps within its while loop.  
- MAX_TIME_NEIGHBOR_SILENT : located in shared_structs.h and will define, in seconds, what is the maximum time that one drone can hear nothing from another drone. When this threshold is exceeded then the drone will updated its routing table such that any entries that include the silent drone as next-hope get deleted.
