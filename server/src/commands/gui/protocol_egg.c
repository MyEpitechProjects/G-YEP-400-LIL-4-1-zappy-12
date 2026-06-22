/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"

bool intern_protocol_graphics_egg_layed(connection_t *_server, uint32_t i,
    egg_t *egg)
{
    char *string = string_create("enw");

    string = string_append(string, "#", false);
    string = string_append_uint(string, egg->index, true);
    string = string_append(string, "#", false);
    string = string_append_int(string, egg->creatorIndex, true);
    string = string_append_uint(string, egg->pos.x, true);
    string = string_append_uint(string, egg->pos.y, true);
    string = send_string(_server->_fds[i].fd, string);
    return true;
}

bool intern_protocol_graphics_egg_laying(connection_t *_server, uint32_t i,
    player_t *player)
{
    char *string = string_create("pfk");

    string = string_append(string, "#", false);
    string = string_append_uint(string, player->index, true);
    string = send_string(_server->_fds[i].fd, string);
    return true;
}
