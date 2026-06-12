/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"

static char *add_inventory(connection_t *_server, uint64_t x, uint64_t y,
    char *string)
{
    uint64_t index = tile_pos_to_index(_server, x, y);

    string = string_append_uint(string, x);
    string = string_append_uint(string, y);
    string = string_append_uint(string,
        _server->zappy.tiles[index].inventory.food);
    string = string_append_uint(string,
        _server->zappy.tiles[index].inventory.linemate);
    string = string_append_uint(string,
        _server->zappy.tiles[index].inventory.deraumere);
    string = string_append_uint(string,
        _server->zappy.tiles[index].inventory.sibur);
    string = string_append_uint(string,
        _server->zappy.tiles[index].inventory.mendiane);
    string = string_append_uint(string,
        _server->zappy.tiles[index].inventory.phiras);
    string = string_append_uint(string,
        _server->zappy.tiles[index].inventory.thystame);
    return string;
}

bool protocol_tile(connection_t *_server, uint32_t i, char **command)
{
    char *string = string_create("bct");
    uint64_t x = 0;
    uint64_t y = 0;
    char *end1 = NULL;
    char *end2 = NULL;

    if (list_len(command) != 3)
        return protocol_graphic_bad_param(_server, i, string);
    x = strtol(command[1], &end1, 10);
    y = strtol(command[2], &end2, 10);
    if (*end1 != '\0' || *end2 != '\0' || x < 0 || y < 0)
        return protocol_graphic_bad_param(_server, i, string);
    string = add_inventory(_server, x, y, string);
    send_string(_server->_fds[i].fd, string);
    return true;
}

bool protocol_map_tiles(connection_t *_server, uint32_t i, char **command)
{
    char *string = NULL;

    (void)command;
    for (uint64_t y = 0; y < _server->arguments->height; y++){
        for (uint64_t x = 0; x < _server->arguments->width; x++){
            string = string_create("bct");
            string = add_inventory(_server, x, y, string);
            send_string(_server->_fds[i].fd, string);
        }
    }
    return true;
}
