/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"

bool trigger_ia_start_incantation(connection_t *_server,
    uint64_t level, pos_t *pos, uint64_t inc_index)
{
    player_t *me = NULL;

    for (uint64_t i = 0; i < _server->nfds; i++){
        if (_server->_fdsm[i].client_type != CLIENT_IA)
            continue;
        me = get_player_by_id(_server, _server->_fdsm[i].id);
        if (!me || me->dead || me->level != level ||
            me->pos.x != pos->x || me->pos.y != pos->y ||
            me->incanting)
            continue;
        me->incanting = true;
        me->incantationIndex = inc_index;
        protocol_ia_incantation_start(_server, i);
    }
    return true;
}
