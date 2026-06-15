/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"

bool trigger_graphic_player_login(connection_t *_server, player_t *player,
    egg_t *egg)
{
    for (uint64_t i = 0; i < _server->nfds; i++){
        if (_server->_fdsm[i].client_type != CLIENT_GUI)
            continue;
        intern_protocol_egg_take(_server, i, egg);
        intern_protocol_player_connected(_server, i, player);
        intern_protocol_player_inventory(_server, i, player);
        intern_protocol_player_level(_server, i, player);
    }
    return true;
}
