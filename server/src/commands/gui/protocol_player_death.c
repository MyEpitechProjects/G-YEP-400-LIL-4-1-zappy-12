/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"

bool intern_protocol_player_death(connection_t *_server, uint32_t i,
    player_t *player)
{
    char *string = string_create("pdi");

    string = string_append(string, "#", false);
    string = string_append_uint(string, player->index, true);
    string = send_string(_server->_fds[i].fd, string);
    return true;
}
