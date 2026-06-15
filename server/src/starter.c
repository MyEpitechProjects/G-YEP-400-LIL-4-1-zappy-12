/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "myserver.h"
#include "zappy/zappy_internal.h"
#include "sys.h"

void get_sock_port(socket_t *_socket)
{
    int port = 0;

    getsockname(_socket->_fd,
        (struct sockaddr *)&_socket->_address, &_socket->_addrlen);
    port = ntohs(_socket->_address.sin_port);
    _socket->_port._p1 = port / 256;
    _socket->_port._p2 = port % 256;
}

static bool is_buffer_empty(connection_t *_server)
{
    char *buffer = NULL;

    for (uint32_t i = 0; i < _server->nfds; i++){
        buffer = get_buffer(_server->_fdsm[i].buffer);
        if (buffer != NULL)
            return false;
    }
    return true;
}

static uint64_t get_eat_timedout(connection_t *_server, uint64_t time)
{
    uint64_t eatUnit = 0;
    uint64_t calculated_time = 0;

    for (uint32_t i = 0; i < _server->zappy.nb_players; i++){
        if (_server->zappy.players[i].dead)
            continue;
        eatUnit = calculate_time_freq(_server, EAT_UNIT);
        calculated_time = (eatUnit -
            get_elapsed(&_server->zappy.players[i].eatClock));
        time = min(time, calculated_time);
    }
    return time;
}

static uint64_t get_timedout(connection_t *_server)
{
    uint64_t foodUnit = calculate_time_freq(_server, FOOD_SPAWN_UNIT);
    uint64_t timed = 0;
    uint64_t elapsed = get_elapsed(&_server->zappy.foodClock);

    if (foodUnit >= elapsed)
        timed = foodUnit - elapsed;
    else
        timed = 0;
    timed = get_eat_timedout(_server, timed);
    return max(0, timed) * 1000;
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
        trigger_graphic_player_death(_server, me);
        return pop_poller(_server, i);
    }
    me->inventory.food--;
    trigger_graphic_player_eat(_server, me);
    return false;
}

static bool execute_client(connection_t *_server, uint32_t *i, bool ret)
{
    bool down = false;

    if (_server->_fdsm[*i].client_type == CLIENT_IA)
        down = check_eat(_server, i);
    if (!down){
        ret = requese_handler(_server, i, ret);
        execute_buffer(_server, i, ret);
    }
    return ret;
}

static bool run_server(connection_t *_server)
{
    bool ret = true;
    uint64_t timedout = 0;

    while (ret) {
        if (is_buffer_empty(_server))
            timedout = get_timedout(_server);
        else
            timedout = 0;
        spawn_food(_server, &_server->zappy);
        poll(_server->_fds, _server->nfds, timedout);
        for (uint32_t i = 0; i < _server->nfds;){
            ret = execute_client(_server, &i, ret);
            i++;
        }
    }
    return true;
}

bool start_server(arguments_t *arguments, int sdf)
{
    bool runCode = true;
    connection_t _server;

    memset(&_server, 0, sizeof(connection_t));
    _server.arguments = arguments;
    zappy(&_server, &_server.zappy);
    if (!push_socket(&_server, arguments->port, SOMAXCONN, FD_SERVER)) {
        return false;
    }
    push_poller(&_server, sdf, NULL, FD_QUIT);
    runCode = run_server(&_server);
    close(_server._sockets[0]._fd);
    return free_server(&_server, runCode);
}
