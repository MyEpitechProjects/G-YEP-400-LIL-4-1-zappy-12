/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "myserver.h"
#include "zappy/zappy_internal.h"
#include "sys.h"

static bool is_buffer_empty(connection_t *_server)
{
    uint64_t buffer_size = 0;
    uint64_t targetElapsed = 0;
    uint64_t elapsed = 0;

    for (uint32_t i = 0; i < _server->nfds; i++){
        buffer_size = get_buffer_length(_server->_fdsm[i].buffer);
        targetElapsed = _server->_fdsm[i].targetElapsed;
        elapsed = 0;
        if (_server->_fdsm[i].clock.active)
            elapsed = get_elapsed(&_server->_fdsm[i].clock);
        if (buffer_size != 0 && (targetElapsed == 0 ||
                elapsed >= targetElapsed))
            return false;
    }
    return true;
}

static bool check_eat(connection_t *_server, uint32_t *i)
{
    uint64_t eatUnit = calculate_time_freq(_server, EAT_UNIT);
    player_t *me = get_player_by_id(_server, _server->_fdsm[*i].id);

    if (get_elapsed(
            &me->eatClock) < eatUnit)
        return false;
    reset_clock(&me->eatClock);
    start_clock(&me->eatClock);
    if (me->inventory.food == 0){
        me->dead = true;
        protocol_ia_death(_server, *i, NULL);
        return client_disconnect(_server, i);
    }
    me->inventory.food--;
    trigger_graphic_player_inv(_server, me);
    return false;
}

static bool execute_client(connection_t *_server, uint32_t *i, bool ret)
{
    bool down = false;
    bool end = false;
    player_t *player = NULL;

    if (!ret)
        return ret;
    if (_server->_fdsm[*i].client_type == CLIENT_IA){
        player = get_player_by_id(_server, _server->_fdsm[*i].id);
        down = check_eat(_server, i);
        if (!down)
            end = check_incantation_finish(_server, &_server->zappy);
    }
    if (!down && !end){
        ret = requese_handler(_server, i, ret);
        if (!player || !player->incanting)
            execute_buffer(_server, i, ret);
    }
    if (end)
        return false;
    return ret;
}

static bool run_server(connection_t *_server)
{
    bool ret = true;
    int64_t timedout = -1;

    while (ret) {
        timedout = -1;
        if (is_buffer_empty(_server))
            timedout = (int64_t)get_timedout(_server);
        if (!is_buffer_empty(_server))
            timedout = 0;
        spawn_food(_server, &_server->zappy);
        if (poll(_server->_fds, _server->nfds, timedout) < 0)
            continue;
        for (uint32_t i = 0; i < _server->nfds;){
            ret = execute_client(_server, &i, ret);
            i++;
        }
    }
    return true;
}

bool client_disconnect(connection_t *_server, uint32_t *i)
{
    player_t *player = NULL;

    if (_server->_fdsm[*i].client_type == CLIENT_IA) {
        player = get_player_by_id(_server, _server->_fdsm[*i].id);
        trigger_graphic_player_death(_server, player);
        player->dead = true;
    }
    pop_poller(_server, i);
    return true;
}

static void print_config(arguments_t *args)
{
    printf("Port: %u\n", args->port);
    printf("Width: %lu\n", args->width);
    printf("Height: %lu\n", args->height);
    printf("Frequency: %lu\n", args->freq);
    printf("Max clients per team: %lu\n", args->client_number);
    printf("Teams (%lu):\n", args->team_number);
    for (uint64_t i = 0; i < args->team_number; i++) {
        printf("  - %s\n", args->teams_names[i]);
    }
}

bool start_server(arguments_t *arguments, int sdf)
{
    bool runCode = true;
    connection_t _server = {0};

    _server.arguments = arguments;
    if (!push_socket(&_server, arguments->port, SOMAXCONN, FD_SERVER) ||
        !init_zappy(&_server, &_server.zappy)) {
        return free_server(&_server, false);
    }
    push_poller(&_server, sdf, NULL, FD_QUIT);
    print_config(arguments);
    start_clock(&_server.zappy.gameClock);
    runCode = run_server(&_server);
    my_close(_server._sockets[0]._fd);
    return free_server(&_server, runCode);
}
