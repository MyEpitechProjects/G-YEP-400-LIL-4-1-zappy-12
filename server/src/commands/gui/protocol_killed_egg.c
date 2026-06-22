/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"

bool intern_protocol_killed_egg(connection_t *_server, uint32_t i,
    egg_t *egg)
{
    char *string = string_create("edi");

    string = string_append(string, "#", false);
    string = string_append_uint(string, egg->index, true);
    string = send_string(_server->_fds[i].fd, string);
    return true;
}
