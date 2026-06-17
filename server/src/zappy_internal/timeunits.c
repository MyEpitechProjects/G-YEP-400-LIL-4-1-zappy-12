/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"

uint64_t calculate_time_freq(connection_t *_server, uint64_t value)
{
    return (uint64_t)((value * 1000ULL) / _server->arguments->freq);
}

uint64_t get_time_unit(connection_t *_server, unit_type_t type)
{
    for (uint8_t i = 0; unit_values[i].type != UNIT_NONE; i++){
        if (unit_values[i].type == type)
            return calculate_time_freq(_server, unit_values[i].value);
    }
    return (uint64_t) (0);
}

uint64_t get_command_time(connection_t *_server, unit_type_t type)
{
    for (uint8_t i = 0; unit_values[i].type != UNIT_NONE; i++){
        if (type != unit_values[i].type)
            continue;
        return get_time_unit(_server, unit_values[i].type);
    }
    return 0;
}
