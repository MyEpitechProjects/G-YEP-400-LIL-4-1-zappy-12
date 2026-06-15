/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"

bool intern_protocol_graphic_drop(connection_t *_server, uint32_t i,
    player_t *player, uint8_t resource_index)
{
    char *string = string_create("pdr");

    string = string_append(string, "#", false);
    string = string_append_uint(string, player->index, true);
    string = string_append_uint(string, resource_index, true);
    string = send_string(_server->_fds[i].fd, string);
    return true;
}
