/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"

bool trigger_graphic_ejected(connection_t *_server,
    player_t *player)
{
    for (uint64_t i = 0; i < _server->nfds; i++){
        if (_server->_fdsm[i].client_type != CLIENT_GUI)
            continue;
        intern_protocol_ejected(_server, i, player);
    }
    return true;
}
