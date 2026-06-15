/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"

static bool send_move_response(connection_t *_server, uint32_t i, char *result,
    player_t *player)
{
    trigger_graphic_player_move(_server, player);
    result = send_string(_server->_fds[i].fd, result);
    return true;
}

bool protocol_ia_rotate_right(connection_t *_server, uint32_t i,
    char **command)
{
    char *string = string_create("ok");
    player_t *player = get_player_by_id(_server, _server->_fdsm[i].id);

    (void)command;
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
    return send_move_response(_server, i, string, player);
}

bool protocol_ia_rotate_left(connection_t *_server, uint32_t i,
    char **command)
{
    char *string = string_create("ok");
    player_t *player = get_player_by_id(_server, _server->_fdsm[i].id);

    (void)command;
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
    return send_move_response(_server, i, string, player);
}
