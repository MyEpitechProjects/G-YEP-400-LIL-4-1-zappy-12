/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"
#include "zappy/broadcast.h"
#include "zappy/look.h"

static void dx_set(broadcast_pos_t *value, int64_t x, int64_t y)
{
    value->dx = x;
    value->dy = y;
}

static void get_orientation_deltas(orientation_t o,
    broadcast_pos_t *fwd, broadcast_pos_t *right)
{
    switch (o) {
        case ORIENTATION_NORTH:
            dx_set(fwd, 0, -1);
            dx_set(right, 1, 0);
            break;
        case ORIENTATION_EST:
            dx_set(fwd, 1, 0);
            dx_set(right, 0, 1);
            break;
        case ORIENTATION_SOUTH:
            dx_set(fwd, 0, 1);
            dx_set(right, -1, 0);
            break;
        case ORIENTATION_WEST:
            dx_set(fwd, -1, 0);
            dx_set(right, 0, -1);
            break;
    }
}

static void append_resource(char **result, char *name,
    uint32_t count)
{
    for (uint32_t q = 0; q < count; q++) {
        *result = string_append(*result, name, false);
    }
}

static void append_all_resources(char **result, tile_t *tile)
{
    append_resource(result, " food", tile->inventory.food);
    append_resource(result, " linemate", tile->inventory.linemate);
    append_resource(result, " deraumere", tile->inventory.deraumere);
    append_resource(result, " sibur", tile->inventory.sibur);
    append_resource(result, " mendiane", tile->inventory.mendiane);
    append_resource(result, " phiras", tile->inventory.phiras);
    append_resource(result, " thystame", tile->inventory.thystame);
}

static char *tile_to_string(connection_t *_server,
    uint64_t x, uint64_t y)
{
    zappy_t *z = &_server->zappy;
    tile_t *tile = &z->tiles[tile_pos_to_index(_server, x, y)];
    char *result = string_create(NULL);

    for (uint32_t p = 0; p < z->nb_players; p++) {
        if (z->players[p].dead)
            continue;
        if (z->players[p].pos.x != x || z->players[p].pos.y != y)
            continue;
        result = string_append(result, " player", false);
    }
    append_all_resources(&result, tile);
    return result;
}

static char *look_tile(connection_t *_server, look_args_t *args, char *result)
{
    broadcast_pos_t txy = {0};
    char *tile_str = NULL;

    txy.dx = (int64_t)args->player->pos.x
        + args->k * args->fwd.dx + args->col * args->right.dx;
    txy.dy = (int64_t)args->player->pos.y
        + args->k * args->fwd.dy + args->col * args->right.dy;
    txy.dx = ((txy.dx % (int64_t)_server->arguments->width) +
        (int64_t)_server->arguments->width)
        % (int64_t)_server->arguments->width;
    txy.dy = ((txy.dy % (int64_t)_server->arguments->height) +
        (int64_t)_server->arguments->height)
        % (int64_t)_server->arguments->height;
    if (!args->first_tile)
        result = string_append(result, ",", false);
    args->first_tile = false;
    tile_str = tile_to_string(_server, (uint64_t)txy.dx, (uint64_t)txy.dy);
    result = string_append(result, tile_str, false);
    free(tile_str);
    return result;
}

bool protocol_ia_look(connection_t *_server, uint32_t i,
    char **command)
{
    char *result = string_create(NULL);
    look_args_t look;

    (void)command;
    result = string_append(result, "[", false);
    look.player = get_player_by_id(_server, _server->_fdsm[i].id);
    get_orientation_deltas(look.player->o, &look.fwd, &look.right);
    look.col = 0;
    look.k = 0;
    look.first_tile = true;
    for (look.k = 0; look.k <= (int64_t)look.player->level; look.k++) {
        for (look.col = -look.k; look.col <= look.k; look.col++) {
            result = look_tile(_server, &look, result);
        }
    }
    result = string_append(result, " ]", true);
    result = send_string(_server->_fds[i].fd, result);
    return true;
}
