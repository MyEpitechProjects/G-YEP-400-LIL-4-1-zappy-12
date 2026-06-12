/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"

bool protocol_map_size(connection_t *_server, uint32_t i, char **command)
{
    char *string = string_create("msz");

    (void)command;
    string = string_append_uint(string, _server->arguments->width);
    string = string_append_uint(string, _server->arguments->height);
    string = send_string(_server->_fds[i].fd, string);
    return true;
}
