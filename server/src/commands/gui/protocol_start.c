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

static bool gui_start_egg(connection_t *_server)
{
    for (uint64_t w = 0; w < _server->zappy.nb_eggs; w++){
        if (_server->zappy.eggs[w].taken)
            continue;
        trigger_graphic_egg_layed(_server,
            &_server->zappy.eggs[w]);
    }
    return true;
}

static bool gui_start_player_login(connection_t *_server, uint64_t i)
{
    for (uint64_t w = 0; w < _server->zappy.nb_players; w++){
        if (_server->zappy.players[w].dead)
            continue;
        intern_protocol_player_connected(_server, i,
            &_server->zappy.players[w]);
        intern_protocol_player_inventory(_server, i,
            &_server->zappy.players[w]);
    }
    return gui_start_egg(_server);
}

bool protocol_graphic_start(connection_t *_server, uint32_t i, char **command)
{
    protocol_map_size(_server, i, command);
    protocol_time_unit(_server, i, command);
    protocol_map_tiles(_server, i, command);
    protocol_teams(_server, i, command);
    return gui_start_player_login(_server, i);
}
