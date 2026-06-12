/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"


time_t get_time_unit(connection_t *_server, unit_type_t type)
{
    for (uint8_t i = 0; unit_values[i].type != UNIT_NONE; i++){
        if (unit_values[i].type == type)
            return (time_t) (unit_values[i].value /
                _server->arguments->freq);
    }
    return (time_t) (0);
}

time_t get_command_time(connection_t *_server, unit_type_t type)
{
    for (uint8_t i = 0; unit_values[i].type != UNIT_NONE; i++){
        if (type != unit_values[i].type)
            continue;
        return get_time_unit(_server, unit_values[i].type);
    }
    return 0;
}
