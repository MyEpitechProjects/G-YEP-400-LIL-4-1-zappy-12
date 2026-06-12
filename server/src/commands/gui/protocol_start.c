/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"

uint64_t extract_num(char *ptr, char **endptr, int base, bool ok)
{
    if (ptr){
        if (ptr[0] == '#' && ok)
            ptr++;
        if (ptr[0] == '-')
            ptr[0] = 'a';
    }
    return strtoul(ptr, endptr, base);
}

bool protocol_graphic_start(connection_t *_server, uint32_t i, char **command)
{
    protocol_map_size(_server, i, command);
    protocol_time_unit(_server, i, command);
    protocol_map_tiles(_server, i, command);
    protocol_teams(_server, i, command);
    for (uint64_t w = 0; w < _server->zappy.nb_players; w++)
        intern_protocol_player_connected(_server, i,
            &_server->zappy.players[w]);
    return true;
}
