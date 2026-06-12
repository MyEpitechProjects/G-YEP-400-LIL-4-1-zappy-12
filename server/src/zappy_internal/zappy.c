/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"

static bool check_zappy(zappy_t *zappy)
{
    return zappy->tiles != NULL && zappy->eggs != NULL;
}

bool zappy(connection_t *_server, zappy_t *zappy)
{
    zappy->tiles = calloc((_server->arguments->width *
            _server->arguments->height), sizeof(tile_t));
    zappy->nb_eggs = _server->arguments->team_number *
        _server->arguments->client_number;
    zappy->eggs = calloc(zappy->nb_eggs, sizeof(egg_t));
    return check_zappy(zappy);
}

uint64_t tile_pos_to_index(connection_t *_server, uint64_t x, uint64_t y)
{
    return ((y * _server->arguments->width) + x);
}
