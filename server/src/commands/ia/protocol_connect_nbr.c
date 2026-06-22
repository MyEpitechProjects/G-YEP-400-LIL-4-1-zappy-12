/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"


bool protocol_ia_connect_nbr(connection_t *_server, uint32_t i,
    char **command)
{
    char *string = string_create(NULL);
    player_t *me = get_player_by_id(_server, _server->_fdsm[i].id);
    uint64_t remain = get_available_eggs(_server, me->team);

    (void)command;
    string = string_append_uint(string,
        remain, true);
    string = send_string(_server->_fds[i].fd, string);
    return true;
}
