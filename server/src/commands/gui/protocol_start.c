/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"

bool protocol_graphic_start(connection_t *_server, uint32_t i, char **command)
{
    protocol_map_size(_server, i, command);
    protocol_time_unit(_server, i, command);
    protocol_map_tiles(_server, i, command);
    protocol_teams(_server, i, command);
    return true;
}
