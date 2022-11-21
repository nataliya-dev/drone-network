//create a monitor with a list of supported resolutions
//NOTE: Returns a heap allocated string, you are required to free it after use.
#include <stdio.h>
#include "create_routing_table.h"
#include "cJSON.h"

char *create_routing_table(void)
{
    const unsigned int routing_parameter_values[2][3] = {
        {3, 2, 2},
        {2, 2, 1},
    };
    char *string = NULL;
    //cJSON *name = NULL;
    cJSON *drones = NULL;
    cJSON *drone = NULL;
    cJSON *destination = NULL;
    cJSON *next_hop = NULL;
    cJSON *cost = NULL;

    size_t index = 0;


    cJSON *monitor = cJSON_CreateObject();
    if (monitor == NULL)
    {
        goto end;
    }
#if 0
    name = cJSON_CreateString("Awesome 4K");
    if (name == NULL)
    {
        goto end;
    }
    /* after creation was successful, immediately add it to the monitor,
     * thereby transferring ownership of the pointer to it */
    cJSON_AddItemToObject(monitor, "name", name);
#endif

    drones = cJSON_CreateArray();
    if (drones == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(monitor, "drones", drones);

    for (index = 0; index < (sizeof(routing_parameter_values) / (3 * sizeof(int))); ++index)
    {
        drone = cJSON_CreateObject();
        if (drone == NULL)
        {
            goto end;
        }
        cJSON_AddItemToArray(drones, drone);

        destination = cJSON_CreateNumber(routing_parameter_values[index][0]);
        if (destination == NULL)
        {
            goto end;
        }
        cJSON_AddItemToObject(drone, "drone", destination);

        next_hop = cJSON_CreateNumber(routing_parameter_values[index][1]);
        if (next_hop == NULL)
        {
            goto end;
        }
        cJSON_AddItemToObject(drone, "next-hop", next_hop);

        cost = cJSON_CreateNumber(routing_parameter_values[index][2]);
        if(NULL == cost)
        {
            goto end;
        }
        cJSON_AddItemToObject(drone, "cost", cost);
    }

    string = cJSON_Print(monitor);
    if (string == NULL)
    {
        fprintf(stderr, "Failed to print monitor.\n");
    }

end:
    cJSON_Delete(monitor);
    return string;
}