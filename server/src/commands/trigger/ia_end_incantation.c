/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"

bool trigger_ia_end_incantation_ko(connection_t *_server,
    uint64_t inc_index)
{
    player_t *me = NULL;

    for (uint64_t i = 0; i < _server->nfds; i++){
        if (_server->_fdsm[i].client_type != CLIENT_IA)
            continue;
        me = get_player_by_id(_server, _server->_fdsm[i].id);
        if (!me->incanting || me->incantationIndex != inc_index)
            continue;
        me->incanting = false;
        protocol_ko(_server, i, NULL);
    }
    return true;
}

bool trigger_ia_end_incantation_ok(connection_t *_server,
    uint64_t inc_index)
{
    player_t *me = NULL;

    for (uint64_t i = 0; i < _server->nfds; i++){
        if (_server->_fdsm[i].client_type != CLIENT_IA)
            continue;
        me = get_player_by_id(_server, _server->_fdsm[i].id);
        if (!me->incanting || me->incantationIndex != inc_index)
            continue;
        me->incanting = false;
        me->level++;
        trigger_graphic_player_level(_server, me);
        protocol_ia_incantation_end(_server, i, me);
    }
    return true;
}
