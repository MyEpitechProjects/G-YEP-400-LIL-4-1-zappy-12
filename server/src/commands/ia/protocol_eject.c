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

static void get_eject_rotation(player_t *receiver,
    broadcast_pos_t *dv, int64_t *local_x, int64_t *local_y)
{
    switch (receiver->o) {
        case ORIENTATION_NORTH:
            *(local_x) = dv->dx;
            *(local_y) = -dv->dy;
            break;
        case ORIENTATION_EST:
            *(local_x) = dv->dy;
            *(local_y) = dv->dx;
            break;
        case ORIENTATION_SOUTH:
            *(local_x) = -dv->dx;
            *(local_y) = dv->dy;
            break;
        case ORIENTATION_WEST:
            *(local_x) = -dv->dy;
            *(local_y) = -dv->dx;
            break;
        default:
            return;
    }
}

static uint64_t get_eject_mapping(int64_t local_x, int64_t local_y)
{
    if (local_y > 0 && local_x == 0)
        return 1;
    if (local_y > 0 && local_x < 0)
        return 2;
    if (local_y == 0 && local_x < 0)
        return 3;
    if (local_y < 0 && local_x < 0)
        return 4;
    if (local_y < 0 && local_x == 0)
        return 5;
    if (local_y < 0 && local_x > 0)
        return 6;
    if (local_y == 0 && local_x > 0)
        return 7;
    if (local_y > 0 && local_x > 0)
        return 8;
    return 0;
}

static uint64_t get_eject_direction(player_t *receiver, player_t *sender,
    uint64_t world_w, uint64_t world_h)
{
    broadcast_pos_t dv = (broadcast_pos_t){
        .dx = (int64_t)sender->pos.x - (int64_t)receiver->pos.x,
        .dy = (int64_t)sender->pos.y - (int64_t)receiver->pos.y,
    };
    int64_t local_x;
    int64_t local_y;

    if (dv.dx > (int64_t)(world_w / 2))
        dv.dx -= (int64_t)world_w;
    if (dv.dx < -(int64_t)(world_w / 2))
        dv.dx += (int64_t)world_w;
    if (dv.dy > (int64_t)(world_h / 2))
        dv.dy -= (int64_t)world_h;
    if (dv.dy < -(int64_t)(world_h / 2))
        dv.dy += (int64_t)world_h;
    if (dv.dx == 0 && dv.dy == 0)
        return 0;
    get_eject_rotation(receiver, &dv, &local_x, &local_y);
    return get_eject_mapping(local_x, local_y);
}

static bool intern_protocol_ia_ejected(connection_t *_server, uint32_t i,
    player_t *sender)
{
    char *string = string_create("eject:");
    player_t *me = get_player_by_id(_server, _server->_fdsm[i].id);
    uint64_t dir = get_eject_direction(me, sender,
        _server->arguments->width, _server->arguments->height);

    string = string_append_uint(string, dir, false);
    string = send_string(_server->_fds[i].fd, string);
    return true;
}

static bool kill_eggs(connection_t *_server, pos_t *pos)
{
    bool ret = false;

    for (uint64_t i = 0; i < _server->zappy.nb_eggs; i++){
        if (_server->zappy.eggs[i].taken ||
            _server->zappy.eggs[i].pos.x != pos->x ||
            _server->zappy.eggs[i].pos.y != pos->y)
            continue;
        ret = true;
        _server->zappy.eggs[i].taken = true;
        trigger_graphic_killed_eggs(_server, &_server->zappy.eggs[i]);
    }
    return ret;
}

static bool eject_players(connection_t *_server, pos_t *pos,
    player_t *sender)
{
    player_t *player = NULL;

    for (uint64_t i = 0; i < _server->nfds; i++){
        if (_server->_fdsm[i].client_type != CLIENT_IA)
            continue;
        player = get_player_by_id(_server, _server->_fdsm[i].id);
        if (player->index == sender->index ||
            player->dead ||
            player->pos.x != pos->x ||
            player->pos.y != pos->y)
            continue;
        player_move(_server, player, player->o);
        intern_protocol_ia_ejected(_server, i, player);
    }
    return true;
}

bool protocol_ia_eject(connection_t *_server, uint32_t i,
    char **command)
{
    player_t *player = get_player_by_id(_server, _server->_fdsm[i].id);
    tile_t *tile = &_server->zappy.tiles[tile_pos_to_index(_server,
            player->pos.x, player->pos.y)];
    bool ejectedPlayer = eject_players(_server, &tile->pos, player);
    bool killedEggs = kill_eggs(_server, &tile->pos);

    if (!ejectedPlayer && !killedEggs)
        return protocol_ko(_server, i, command);
    trigger_graphic_ejected(_server, player);
    return protocol_ok(_server, i, command);
}
