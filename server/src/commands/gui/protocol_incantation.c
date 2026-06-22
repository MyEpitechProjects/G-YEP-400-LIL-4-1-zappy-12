/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"

bool intern_protocol_graphic_start_incantation(connection_t *_server,
    uint64_t i, uint64_t level, pos_t *pos)
{
    player_t *me = NULL;
    char *string = string_create("pic");

    string = string_append_uint(string, pos->x, true);
    string = string_append_uint(string, pos->y, true);
    string = string_append_uint(string, level, true);
    for (uint64_t x = 0; x < _server->zappy.nb_players; x++){
        me = &_server->zappy.players[x];
        if (!me || me->dead || me->level != level ||
            me->pos.x != pos->x || me->pos.y != pos->y ||
            !me->incanting)
            continue;
        string = string_append(string, "#", false);
        string = string_append_uint(string, me->index, true);
    }
    string = send_string(_server->_fds[i].fd, string);
    return true;
}

bool intern_protocol_graphic_end_incantation(connection_t *_server,
    uint64_t i, bool status, incantation_t *inc)
{
    char *string = string_create("pie");

    string = string_append_uint(string, inc->pos.x, true);
    string = string_append_uint(string, inc->pos.y, true);
    string = string_append_uint(string, (uint64_t)status, true);
    string = send_string(_server->_fds[i].fd, string);
    return true;
}
