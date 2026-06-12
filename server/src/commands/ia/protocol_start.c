/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"

static uint64_t get_available_eggs(connection_t *_server, char *team)
{
    uint64_t res = 0;

    for (uint64_t i = 0; i < _server->zappy.nb_eggs; i++){
        if (strcmp(_server->zappy.eggs[i].team, team) == 0 &&
            !_server->zappy.eggs[i].taken)
            res++;
    }
    return res;
}

egg_t *get_egg_by_team(connection_t *_server, char *team)
{
    for (uint64_t i = 0; i < _server->zappy.nb_eggs; i++){
        if (strcmp(_server->zappy.eggs[i].team, team) == 0 &&
            !_server->zappy.eggs[i].taken)
            return &_server->zappy.eggs[i];
    }
    return NULL;
}

static bool push_player(connection_t *_server, uint32_t i, char *team,
    egg_t *egg)
{
    bool status = false;
    uint64_t nbplayer = _server->zappy.nb_players;

    _server->zappy.players = list_push(_server->zappy.players,
        &_server->zappy.nb_players, sizeof(player_t), &status);
    if (status){
        _server->zappy.players[nbplayer].team = team;
        _server->zappy.players[nbplayer].index = nbplayer;
        _server->zappy.players[nbplayer].pos.x = egg->pos.x;
        _server->zappy.players[nbplayer].pos.x = egg->pos.y;
        _server->zappy.players[nbplayer].inventory.food = BASE_FOOD - 1;
        start_clock(&_server->zappy.players[nbplayer].eatClock);
        _server->_fdsm[i].id = _server->zappy.players[nbplayer].index;
        return trigger_graphic_player_login(_server,
            &_server->zappy.players[nbplayer], egg);
    }
    return false;
}

bool protocol_ia_start(connection_t *_server, uint32_t i,
    char **command, char *team)
{
    egg_t *egg = get_egg_by_team(_server, team);
    uint64_t available = get_available_eggs(_server, team);
    char *string = NULL;

    (void)command;
    if (!egg || !push_player(_server, i, team, egg))
        return protocol_ia_bad_param(_server, i, NULL);
    egg->taken = true;
    string = string_create(NULL);
    string = string_append_uint(string, available - 1, true);
    string = send_string(_server->_fds[i].fd, string);
    string = string_create(NULL);
    string = string_append_uint(string, _server->arguments->width - 1, true);
    string = string_append_uint(string, _server->arguments->height - 1, true);
    string = send_string(_server->_fds[i].fd, string);
    _server->_fdsm[i].client_type = CLIENT_IA;
    return true;
}
