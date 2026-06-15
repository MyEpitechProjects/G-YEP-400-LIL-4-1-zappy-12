/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"
#include "sys.h"

static bool check_zappy(zappy_t *zappy)
{
    return zappy->tiles != NULL && zappy->eggs != NULL;
}

bool spawn_food(connection_t *_server, zappy_t *zappy)
{
    uint64_t unit = calculate_time_freq(_server, FOOD_SPAWN_UNIT);
    bool bypass = false;

    if (!zappy->tiles)
        return true;
    if (zappy->foodClock.base == 0){
        start_clock(&zappy->foodClock);
        bypass = true;
    }
    if (get_elapsed(&zappy->foodClock) < unit && !bypass)
        return true;
    reset_clock(&zappy->foodClock);
    start_clock(&zappy->foodClock);
    add_food_tile(_server, zappy);
    return true;
}

static bool set_eggs(connection_t *_server, zappy_t *zappy)
{
    uint64_t count = 0;

    for (uint64_t i = 0; i < zappy->nb_eggs; i++){
        zappy->eggs[i].index = i;
        zappy->eggs[i].creatorIndex = -1;
        zappy->eggs[i].pos.x = randint(_server->arguments->width);
        zappy->eggs[i].pos.y = randint(_server->arguments->height);
        zappy->eggs[i].team = _server->arguments->teams_names[count];
        zappy->eggs[i].o = randint(ORIENTATION_WEST + 1);
        if ((i + 1) == _server->arguments->client_number)
            count++;
    }
    return true;
}

bool zappy(connection_t *_server, zappy_t *zappy)
{
    srand(time(NULL));
    zappy->tiles = calloc((_server->arguments->width *
            _server->arguments->height), sizeof(tile_t));
    zappy->nb_eggs = _server->arguments->team_number *
        _server->arguments->client_number;
    zappy->eggs = calloc(zappy->nb_eggs, sizeof(egg_t));
    if (zappy->eggs)
        set_eggs(_server, zappy);
    spawn_food(_server, zappy);
    return check_zappy(zappy);
}

uint64_t tile_pos_to_index(connection_t *_server, uint64_t x, uint64_t y)
{
    return ((y * _server->arguments->width) + x);
}
