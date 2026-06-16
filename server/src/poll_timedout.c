/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "myserver.h"
#include "zappy/zappy_internal.h"
#include "sys.h"


static int64_t get_eat_timedout(connection_t *_server)
{
    uint64_t eatUnit = 0;
    uint64_t calculated_time = 0;
    int64_t time = -1;
    bool first = true;

    for (uint32_t i = 0; i < _server->zappy.nb_players; i++){
        if (_server->zappy.players[i].dead)
            continue;
        eatUnit = calculate_time_freq(_server, EAT_UNIT);
        calculated_time = (eatUnit -
            get_elapsed(&_server->zappy.players[i].eatClock));
        if (first)
            time = calculated_time;
        else
            time = min(time, calculated_time);
        first = false;
    }
    return time;
}

static int64_t get_incantation_timedout(connection_t *_server)
{
    uint64_t unit = 0;
    uint64_t calculated_time = 0;
    int64_t time = -1;
    bool first = true;

    for (uint32_t i = 0; i < _server->zappy.nb_incantation; i++){
        if (!_server->zappy.incantation[i].active)
            continue;
        unit = calculate_time_freq(_server, INCANTATION_UNIT);
        calculated_time = (unit -
            get_elapsed(&_server->zappy.incantation[i].clock));
        if (first)
            time = calculated_time;
        else
            time = min(time, calculated_time);
        first = false;
    }
    return time;
}

static uint64_t get_command_timedout(connection_t *_server)
{
    uint64_t buffer_size = 0;
    uint64_t targetElapsed = 0;
    uint64_t elapsed = 0;
    int64_t time = -1;
    bool first = true;

    for (uint32_t i = 0; i < _server->nfds; i++){
        buffer_size = get_buffer_length(_server->_fdsm[i].buffer);
        targetElapsed = _server->_fdsm[i].targetElapsed;
        elapsed = 0;
        if (_server->_fdsm[i].clock.active)
            elapsed = get_elapsed(&_server->_fdsm[i].clock);
        if (buffer_size != 0 && (elapsed < targetElapsed) && first){
            time = (targetElapsed - elapsed);
            first = false;
        }
        if (buffer_size != 0 && (elapsed < targetElapsed) && !first)
            time = min(time, targetElapsed - elapsed);
    }
    return time;
}

static int64_t get_respawn_timedout(connection_t *_server)
{
    int64_t time = -1;
    uint64_t foodUnit = calculate_time_freq(_server, FOOD_SPAWN_UNIT);
    uint64_t elapsed = get_elapsed(&_server->zappy.foodClock);

    if (!need_food_respawn(_server, &_server->zappy))
        return -1;
    if (foodUnit >= elapsed)
        time = foodUnit - elapsed;
    return time;
}

static int64_t compare_timedout(int64_t a, int64_t b)
{
    if (a == -1 && b == -1)
        return -1;
    if (a == -1)
        return b;
    if (b == -1)
        return a;
    return min(a, b);
}

int64_t get_timedout(connection_t *_server)
{
    int64_t time = get_respawn_timedout(_server);

    time = compare_timedout(get_eat_timedout(_server), time);
    time = compare_timedout(get_command_timedout(_server), time);
    time = compare_timedout(get_incantation_timedout(_server), time);
    if (time == -1)
        return -1;
    return max(0, (uint64_t)time) * 1000;
}
