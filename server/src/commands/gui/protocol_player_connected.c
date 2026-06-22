/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"

bool intern_protocol_player_connected(connection_t *_server, uint32_t i,
    player_t *player)
{
    char *string = string_create("pnw");

    string = string_append(string, "#", false);
    string = string_append_uint(string, player->index, true);
    string = string_append_uint(string, player->pos.x, true);
    string = string_append_uint(string, player->pos.y, true);
    string = string_append_uint(string, player->o + 1, true);
    string = string_append_uint(string, player->level, true);
    string = string_append(string, player->team, true);
    string = send_string(_server->_fds[i].fd, string);
    return true;
}

bool intern_protocol_egg_take(connection_t *_server, uint32_t i,
    egg_t *egg)
{
    char *string = string_create("ebo");

    string = string_append(string, "#", false);
    string = string_append_uint(string, egg->index, true);
    string = send_string(_server->_fds[i].fd, string);
    return true;
}
