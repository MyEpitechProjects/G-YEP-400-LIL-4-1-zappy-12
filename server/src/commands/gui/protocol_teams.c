/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"

bool protocol_teams(connection_t *_server, uint32_t i, char **command)
{
    char *string = NULL;

    (void)command;
    for (uint64_t c = 0; c < _server->arguments->team_number; c++){
        string = string_create("tna");
        string = string_append(string, _server->arguments->teams_names[c]);
        string = send_string(_server->_fds[i].fd, string);
    }
    return true;
}
