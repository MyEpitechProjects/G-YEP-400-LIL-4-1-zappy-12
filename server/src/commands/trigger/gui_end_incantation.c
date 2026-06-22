/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"

bool trigger_gui_end_incantation(connection_t *_server,
    zappy_t *zappy, uint64_t incidx, bool status)
{
    for (uint64_t i = 0; i < _server->nfds; i++){
        if (_server->_fdsm[i].client_type != CLIENT_GUI)
            continue;
        intern_protocol_graphic_end_incantation(_server, i,
            status, &zappy->incantation[incidx]);
    }
    return true;
}
