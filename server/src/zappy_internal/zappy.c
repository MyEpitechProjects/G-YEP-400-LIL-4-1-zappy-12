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

    if (!zappy->tiles || !need_food_respawn(_server, zappy))
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

bool check_eggs_hatch(connection_t *_server, zappy_t *zappy)
{
    uint64_t hatchUnit = calculate_time_freq(_server, HATCH_UNIT);

    for (uint64_t i = 0; i < zappy->nb_eggs; i++){
        if (zappy->eggs[i].hatched || zappy->eggs[i].taken)
            continue;
        if (get_elapsed(&zappy->eggs[i].hatchClock) >= hatchUnit)
            zappy->eggs[i].hatched = true;
    }
    return true;
}

static bool set_eggs(connection_t *_server, zappy_t *zappy)
{
    uint64_t count = 0;
    uint64_t count2 = 0;

    for (uint64_t i = 0; i < zappy->nb_eggs; i++){
        zappy->eggs[i].index = i;
        zappy->eggs[i].creatorIndex = -1;
        zappy->eggs[i].pos.x = randint(_server->arguments->width);
        zappy->eggs[i].pos.y = randint(_server->arguments->height);
        zappy->eggs[i].team = _server->arguments->teams_names[count];
        zappy->eggs[i].o = randint(ORIENTATION_WEST + 1);
        zappy->eggs[i].hatched = true;
        if ((count2 + 1) == _server->arguments->client_number){
            count++;
            count2 = 0;
        } else
            count2++;
    }
    return true;
}

static bool set_tile_pos(connection_t *_server, zappy_t *zappy)
{
    uint64_t w = _server->arguments->width;
    uint64_t h = _server->arguments->height;
    tile_t *tile = NULL;

    for (uint64_t y = 0; y < h; y++){
        for (uint64_t x = 0; x < w; x++){
            tile = &zappy->tiles[tile_pos_to_index(_server,
                    x, y)];
            set_pos_xy(&tile->pos, x, y);
        }
    }
    return true;
}

bool init_zappy(connection_t *_server, zappy_t *zappy)
{
    srand(time(NULL));
    zappy->tiles = calloc((_server->arguments->width *
            _server->arguments->height), sizeof(tile_t));
    if (zappy->tiles)
        set_tile_pos(_server, zappy);
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
