/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"

bool intern_protocol_game_end(connection_t *_server, uint32_t i,
    const char *team)
{
    char *string = string_create("seg");

    string = string_append(string, team, true);
    string = send_string(_server->_fds[i].fd, string);
    return true;
}
