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


static void get_broadcast_rotation(player_t *receiver,
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

static uint64_t get_broadcast_mapping(int64_t local_x, int64_t local_y)
{
    if (local_y > 0 && local_x == 0)
        return 1;
    if (local_y > 0 && local_x > 0)
        return 2;
    if (local_y == 0 && local_x > 0)
        return 3;
    if (local_y < 0 && local_x > 0)
        return 4;
    if (local_y < 0 && local_x == 0)
        return 5;
    if (local_y < 0 && local_x < 0)
        return 6;
    if (local_y == 0 && local_x < 0)
        return 7;
    if (local_y > 0 && local_x < 0)
        return 8;
    return 0;
}

static uint64_t get_broadcast_direction(player_t *receiver, player_t *sender,
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
    get_broadcast_rotation(receiver, &dv, &local_x, &local_y);
    return get_broadcast_mapping(local_x, local_y);
}

bool intern_protocol_ia_broadcast(connection_t *_server, uint32_t i,
    char *msg, player_t *sender)
{
    char *string = string_create("message");
    player_t *me = get_player_by_id(_server, _server->_fdsm[i].id);
    uint64_t dir = get_broadcast_direction(me, sender,
        _server->arguments->width, _server->arguments->height);

    string = string_append_uint(string, dir, false);
    string = string_append(string, ",", true);
    string = string_append(string, msg, true);
    string = send_string(_server->_fds[i].fd, string);
    return true;
}

bool protocol_ia_broadcast(connection_t *_server, uint32_t i,
    char **command)
{
    player_t *me = get_player_by_id(_server, _server->_fdsm[i].id);

    trigger_broadcast(_server, i, me, command[1]);
    return protocol_ok(_server, i, command);
}
