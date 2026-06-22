/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"

bool protocol_time_unit(connection_t *_server, uint32_t i, char **command)
{
    char *string = string_create("sgt");

    (void)command;
    string = string_append_uint(string, _server->arguments->freq, true);
    string = send_string(_server->_fds[i].fd, string);
    return true;
}

bool protocol_time_unit_edit(connection_t *_server, uint32_t i,
    char **command)
{
    char *string = string_create("sst");
    uint64_t new_value = 0;
    char *end = NULL;

    if (list_len(command) != 2)
        return protocol_graphic_bad_param(_server, i, string);
    new_value = extract_num(command[1], &end, 10, false);
    if (*end != '\0' || new_value == 0)
        return protocol_graphic_bad_param(_server, i, NULL);
    _server->arguments->freq = new_value;
    string = string_append_uint(string, _server->arguments->freq, true);
    string = send_string(_server->_fds[i].fd, string);
    return true;
}
