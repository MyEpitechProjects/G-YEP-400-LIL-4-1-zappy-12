/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"

static bool player_pos_move(uint64_t *p, int64_t n, uint64_t c)
{
    if (n < 0 && *p == 0){
        *(p) = (c - 1);
        return true;
    } else if (n > 0 && *p == (c - 1)){
        *(p) = 0;
        return true;
    }
    *(p) = (*p + n);
    return true;
}

bool player_move(connection_t *_server, player_t *player,
    orientation_t orientation)
{
    switch (orientation){
        case ORIENTATION_NORTH:
            player_pos_move(&player->pos.y, -1, _server->arguments->height);
            break;
        case ORIENTATION_EST:
            player_pos_move(&player->pos.x, 1, _server->arguments->width);
            break;
        case ORIENTATION_SOUTH:
            player_pos_move(&player->pos.y, 1, _server->arguments->height);
            break;
        case ORIENTATION_WEST:
            player_pos_move(&player->pos.x, -1, _server->arguments->width);
            break;
    }
    return trigger_graphic_player_move(_server, player);
}

bool protocol_ia_move(connection_t *_server, uint32_t i,
    char **command)
{
    player_t *player = get_player_by_id(_server, _server->_fdsm[i].id);

    player_move(_server, player, player->o);
    return protocol_ok(_server, i, command);
}
