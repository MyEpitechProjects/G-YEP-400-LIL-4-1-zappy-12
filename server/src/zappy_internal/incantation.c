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

static bool check_incantation_items(const inventory_t *inv_tile,
    const inventory_t *target_inv)
{
    return (inv_tile->deraumere >= target_inv->deraumere &&
        inv_tile->linemate >= target_inv->linemate &&
        inv_tile->mendiane >= target_inv->mendiane &&
        inv_tile->phiras >= target_inv->phiras &&
        inv_tile->sibur >= target_inv->sibur &&
        inv_tile->thystame >= target_inv->thystame);
}

static bool check_incantation_player(zappy_t *zappy,
    const incantation_type_t *target, pos_t *targetPos, bool startCheck)
{
    uint64_t count = 0;

    for (uint64_t i = 0; i < zappy->nb_players; i++){
        if (zappy->players[i].incanting && startCheck &&
            zappy->players[i].pos.x == targetPos->x &&
            zappy->players[i].pos.y == targetPos->y)
            return 0;
        if (zappy->players[i].dead ||
            zappy->players[i].level != target->level ||
            zappy->players[i].pos.x != targetPos->x ||
            zappy->players[i].pos.y != targetPos->y)
            continue;
        count++;
    }
    return count >= target->nbPlayer;
}

bool check_incantation(connection_t *_server, const incantation_type_t *target,
    pos_t *incantationPos, bool startCheck)
{
    tile_t *tile = &_server->zappy.tiles[tile_pos_to_index(_server,
            incantationPos->x, incantationPos->y)];

    if (!target)
        return false;
    return (check_incantation_player(&_server->zappy,
            target, incantationPos, startCheck) &&
        check_incantation_items(&tile->inventory, &target->inv));
}

static uint64_t get_level8_palyers_by_team(connection_t *_server,
    const char *team)
{
    uint64_t count = 0;

    for (uint64_t i = 0; i < _server->zappy.nb_players; i++){
        if (_server->zappy.players[i].dead ||
            _server->zappy.players[i].incanting ||
            strcmp(_server->zappy.players[i].team, team) != 0 ||
            _server->zappy.players[i].level < WIN_LEVEL)
            continue;
        count++;
    }
    return count;
}

static bool check_end(connection_t *_server)
{
    uint64_t count = 0;
    uint64_t ms = 0;

    for (uint64_t i = 0; i < _server->arguments->team_number; i++){
        count = get_level8_palyers_by_team(_server,
            _server->arguments->teams_names[i]);
        if (count >= WIN_PLAYER_COUNT){
            trigger_graphic_game_end(_server,
                _server->arguments->teams_names[i]);
            ms = get_elapsed(&_server->zappy.gameClock);
            printf("Team \"%s\" wins in %lu seconds and %lu milliseconds\n",
                _server->arguments->teams_names[i], ms / 1000, ms % 1000);
            return true;
        }
    }
    return false;
}

static bool remove_stones_incantation(connection_t *_server,
    incantation_t *incantation)
{
    pos_t *pos = &incantation->pos;
    const inventory_t *to_remove = &incantation->type->inv;
    tile_t *tile = &_server->zappy.tiles[tile_pos_to_index(_server,
            pos->x, pos->y)];

    tile->inventory.deraumere -= to_remove->deraumere;
    tile->inventory.linemate -= to_remove->linemate;
    tile->inventory.mendiane -= to_remove->mendiane;
    tile->inventory.phiras -= to_remove->phiras;
    tile->inventory.sibur -= to_remove->sibur;
    tile->inventory.thystame -= to_remove->thystame;
    return trigger_graphic_tile_change(_server, tile);
}

bool check_incantation_finish(connection_t *_server, zappy_t *zappy)
{
    uint64_t unit = calculate_time_freq(_server, INCANTATION_UNIT);
    uint64_t inc_index = 0;

    for (uint64_t i = 0; i < zappy->nb_incantation; i++){
        if (!zappy->incantation[i].active ||
            get_elapsed(&zappy->incantation[i].clock) < unit)
            continue;
        inc_index = zappy->incantation[i].index;
        if (check_incantation(_server, zappy->incantation[i].type,
                &zappy->incantation[i].pos, false)){
            trigger_gui_end_incantation(_server, zappy, i, true);
            trigger_ia_end_incantation_ok(_server, inc_index);
            remove_stones_incantation(_server, &_server->zappy.incantation[i]);
        } else {
            trigger_gui_end_incantation(_server, zappy, i, false);
            trigger_ia_end_incantation_ko(_server, inc_index);
        }
        zappy->incantation[i].active = false;
    }
    return check_end(_server);
}
