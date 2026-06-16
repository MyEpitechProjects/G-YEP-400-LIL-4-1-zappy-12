/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"

bool trigger_graphic_start_incantation(connection_t *_server,
    uint64_t level, pos_t *pos)
{
    for (uint64_t i = 0; i < _server->nfds; i++){
        if (_server->_fdsm[i].client_type != CLIENT_GUI)
            continue;
        intern_protocol_graphic_start_incantation(_server, i,
            level, pos);
    }
    return true;
}
