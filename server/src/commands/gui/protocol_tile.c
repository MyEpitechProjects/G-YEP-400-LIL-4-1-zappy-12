/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"


static char *add_inventory(tile_t *tile,
    char *string)
{
    string = string_append_uint(string,
        tile->inventory.food, true);
    string = string_append_uint(string,
        tile->inventory.linemate, true);
    string = string_append_uint(string,
        tile->inventory.deraumere, true);
    string = string_append_uint(string,
        tile->inventory.sibur, true);
    string = string_append_uint(string,
        tile->inventory.mendiane, true);
    string = string_append_uint(string,
        tile->inventory.phiras, true);
    string = string_append_uint(string,
        tile->inventory.thystame, true);
    return string;
}

bool intern_protocol_map_tile(connection_t *_server, uint32_t i, tile_t *tile)
{
    char *string = string_create("bct");

    string = string_append_uint(string, tile->pos.x, true);
    string = string_append_uint(string, tile->pos.y, true);
    string = add_inventory(tile, string);
    send_string(_server->_fds[i].fd, string);
    return true;
}

bool protocol_tile(connection_t *_server, uint32_t i, char **command)
{
    uint64_t x = 0;
    uint64_t y = 0;
    char *end1 = NULL;
    char *end2 = NULL;
    tile_t *tile = NULL;

    if (list_len(command) != 3)
        return protocol_graphic_bad_param(_server, i, NULL);
    x = strtol(command[1], &end1, 10);
    y = strtol(command[2], &end2, 10);
    if (*end1 != '\0' || *end2 != '\0' || x < 0 || y < 0)
        return protocol_graphic_bad_param(_server, i, NULL);
    tile = &_server->zappy.tiles[tile_pos_to_index(_server, x, y)];
    return intern_protocol_map_tile(_server, i, tile);
}

bool protocol_map_tiles(connection_t *_server, uint32_t i, char **command)
{
    tile_t *tile = NULL;

    (void)command;
    for (uint64_t y = 0; y < _server->arguments->height; y++){
        for (uint64_t x = 0; x < _server->arguments->width; x++){
            tile = &_server->zappy.tiles[tile_pos_to_index(
                    _server, x, y)];
            intern_protocol_map_tile(_server, i, tile);
        }
    }
    return true;
}
