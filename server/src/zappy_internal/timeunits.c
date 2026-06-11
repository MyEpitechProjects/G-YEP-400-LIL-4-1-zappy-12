/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy_internal.h"


time_t get_time_unit(connection_t *_server, unit_type_t type)
{
    for (uint8_t i = 0; unit_values[i].type != UNIT_NONE; i++){
        if (unit_values[i].type == type)
            return (time_t) (unit_values[i].value /
                _server->arguments->freq);
    }
    return (time_t) (0);
}

static unit_type_t get_command_unit_type(char *command)
{
    for (uint8_t i = 0; commands[i].func != UNK_INVOCATION; i++){
        if (strcmp(command, commands[i].name) != 0)
            continue;
        return commands[i].type;
    }
    return UNIT_NONE;
}

time_t get_command_time(connection_t *_server, char *command_name)
{
    unit_type_t type = get_command_unit_type(command_name);

    for (uint8_t i = 0; unit_values[i].type != UNIT_NONE; i++){
        if (type != unit_values[i].type)
            continue;
        return get_time_unit(_server, unit_values[i].type);
    }
    return UNIT_NONE;
}
