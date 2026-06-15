/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"

static bool player_move(uint64_t *p, int64_t n, uint64_t c)
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

static bool send_move_response(connection_t *_server, uint32_t i, char *result,
    player_t *player)
{
    trigger_graphic_player_move(_server, player);
    result = send_string(_server->_fds[i].fd, result);
    return true;
}

bool protocol_ia_move(connection_t *_server, uint32_t i,
    char **command)
{
    char *string = string_create("ok");
    player_t *player = get_player_by_id(_server, _server->_fdsm[i].id);

    (void)command;
    switch (player->o){
        case ORIENTATION_NORTH:
            player_move(&player->pos.y, -1, _server->arguments->height);
            break;
        case ORIENTATION_EST:
            player_move(&player->pos.x, 1, _server->arguments->width);
            break;
        case ORIENTATION_SOUTH:
            player_move(&player->pos.y, 1, _server->arguments->height);
            break;
        case ORIENTATION_WEST:
            player_move(&player->pos.x, -1, _server->arguments->width);
            break;
    }
    return send_move_response(_server, i, string, player);
}
