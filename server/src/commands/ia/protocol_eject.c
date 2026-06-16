/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"

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
    orientation_t orientation)
{
    bool ret = false;

    for (uint64_t i = 0; i < _server->zappy.nb_players; i++){
        if (_server->zappy.players[i].dead ||
            _server->zappy.players[i].pos.x != pos->x ||
            _server->zappy.players[i].pos.y != pos->y)
            continue;
        ret = true;
        player_move(_server, &_server->zappy.players[i], orientation);
        trigger_graphic_ejected(_server, &_server->zappy.players[i]);
    }
    return ret;
}

bool protocol_ia_eject(connection_t *_server, uint32_t i,
    char **command)
{
    player_t *player = get_player_by_id(_server, _server->_fdsm[i].id);
    tile_t *tile = &_server->zappy.tiles[tile_pos_to_index(_server,
            player->pos.x, player->pos.y)];
    bool ejectedPlayer = eject_players(_server, &tile->pos, player->o);
    bool killedEggs = kill_eggs(_server, &tile->pos);

    if (!ejectedPlayer && !killedEggs)
        return protocol_ko(_server, i, command);
    return protocol_ok(_server, i, command);
}
