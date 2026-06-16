/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"


bool protocol_ia_rotate_right(connection_t *_server, uint32_t i,
    char **command)
{
    player_t *player = get_player_by_id(_server, _server->_fdsm[i].id);

    switch (player->o){
        case ORIENTATION_NORTH:
            player->o = ORIENTATION_EST;
            break;
        case ORIENTATION_EST:
            player->o = ORIENTATION_SOUTH;
            break;
        case ORIENTATION_SOUTH:
            player->o = ORIENTATION_WEST;
            break;
        case ORIENTATION_WEST:
            player->o = ORIENTATION_NORTH;
            break;
    }
    trigger_graphic_player_move(_server, player);
    return protocol_ok(_server, i, command);
}

bool protocol_ia_rotate_left(connection_t *_server, uint32_t i,
    char **command)
{
    player_t *player = get_player_by_id(_server, _server->_fdsm[i].id);

    switch (player->o){
        case ORIENTATION_NORTH:
            player->o = ORIENTATION_WEST;
            break;
        case ORIENTATION_EST:
            player->o = ORIENTATION_NORTH;
            break;
        case ORIENTATION_SOUTH:
            player->o = ORIENTATION_EST;
            break;
        case ORIENTATION_WEST:
            player->o = ORIENTATION_SOUTH;
            break;
    }
    trigger_graphic_player_move(_server, player);
    return protocol_ok(_server, i, command);
}
