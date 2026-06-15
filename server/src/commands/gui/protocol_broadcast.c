/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"

bool intern_protocol_graphics_broadcast(connection_t *_server, uint32_t i,
    char *msg, player_t *sender)
{
    char *string = string_create("pbc");

    string = string_append(string, "#", false);
    string = string_append_uint(string, sender->index, true);
    string = string_append(string, "\"", false);
    string = string_append(string, msg, false);
    string = string_append(string, "\"", true);
    string = send_string(_server->_fds[i].fd, string);
    return true;
}
