/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"

bool trigger_broadcast(connection_t *_server, uint64_t me,
    player_t *sender, char *msg)
{
    for (uint64_t i = 0; i < _server->nfds; i++){
        if (i == me)
            continue;
        if (_server->_fdsm[i].client_type == CLIENT_GUI){
            intern_protocol_graphics_broadcast(_server, i, msg, sender);
        }
        if (_server->_fdsm[i].client_type == CLIENT_IA){
            intern_protocol_ia_broadcast(_server, i, msg, sender);
        }
    }
    return true;
}
