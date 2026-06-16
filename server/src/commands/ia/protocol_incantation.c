/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"
#include "sys.h"

incantation_t *get_last_incantation(connection_t *_server)
{
    incantation_t *incantation = NULL;

    for (uint64_t i = 0; i < _server->zappy.nb_incantation; i++){
        incantation = &_server->zappy.incantation[i];
    }
    return incantation;
}

static bool set_incantation_entry(connection_t *_server,
    player_t *player, incantation_t *me,
    const incantation_type_t *type)
{
    me->active = true;
    me->initiatorIndex = player->index;
    me->type = type;
    set_pos_xy(&me->pos, player->pos.x,
        player->pos.y);
    trigger_ia_start_incantation(_server, player->level, &player->pos,
        me->index);
    trigger_graphic_start_incantation(_server,
        player->level, &player->pos);
    reset_clock(&me->clock);
    start_clock(&me->clock);
    return true;
}

static bool push_incantation(connection_t *_server,
    player_t *player, const incantation_type_t *type)
{
    bool status = false;
    uint64_t nbinc = _server->zappy.nb_incantation;
    incantation_t *me = NULL;

    _server->zappy.incantation = list_push(
        _server->zappy.incantation,
        &_server->zappy.nb_incantation,
        sizeof(incantation_t), &status);
    if (status){
        me = get_last_incantation(_server);
        me->index = nbinc;
        return set_incantation_entry(_server, player, me,
            type);
    }
    return false;
}

const incantation_type_t *get_incantation_type(uint64_t level)
{
    for (uint64_t i = 0; incantation_types[i].level != 0; i++){
        if (incantation_types[i].level != level)
            continue;
        return &incantation_types[i];
    }
    return NULL;
}

bool protocol_ia_incantation(connection_t *_server, uint32_t i,
    char **command)
{
    player_t *me = get_player_by_id(_server, _server->_fdsm[i].id);
    const incantation_type_t *type = get_incantation_type(me->level);

    if (!check_incantation(_server, type, &me->pos, true) ||
        !push_incantation(_server, me, type))
        return protocol_ko(_server, i, command);
    return true;
}

bool protocol_ia_incantation_start(connection_t *_server,
    uint32_t i)
{
    char *string = string_create("Elevation underway");

    string = send_string(_server->_fds[i].fd, string);
    return true;
}

bool protocol_ia_incantation_end(connection_t *_server,
    uint32_t i, player_t *me)
{
    char *string = string_create("Current level:");

    string = string_append_uint(string, me->level, true);
    string = send_string(_server->_fds[i].fd, string);
    return true;
}
