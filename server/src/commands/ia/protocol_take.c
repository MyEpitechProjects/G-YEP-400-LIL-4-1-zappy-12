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

static bool take_item(connection_t *_server, uint32_t i, uint64_t *tile_inv,
    uint64_t *me_inv)
{
    char *string = string_create("ok");

    if (*tile_inv == 0){
        protocol_ia_bad_param(_server, i, string);
        return false;
    }
    *(tile_inv) = (*tile_inv - 1);
    *(me_inv) = (*me_inv + 1);
    string = send_string(_server->_fds[i].fd, string);
    return true;
}

bool protocol_ia_take(connection_t *_server, uint32_t i,
    char **command)
{
    player_t *me = get_player_by_id(_server, _server->_fdsm[i].id);
    tile_t *tile = &_server->zappy.tiles[tile_pos_to_index(
            _server, me->pos.x, me->pos.y)];
    uint64_t *inv_tile[NUMBER_RESOURCES + 1];
    uint64_t *inv_me[NUMBER_RESOURCES + 1];

    set_resources_array(inv_tile, &tile->inventory);
    set_resources_array(inv_me, &me->inventory);
    for (uint64_t x = 0; food_names[x] != NULL; x++){
        if (strcmp(food_names[x], command[1]) != 0)
            continue;
        if (!take_item(_server, i, inv_tile[x], inv_me[x]))
            return true;
        trigger_graphic_drop(_server, me, x);
        trigger_graphic_player_eat(_server, me);
        return trigger_graphic_tile_change(_server, tile);
    }
    return protocol_ia_bad_param(_server, i, NULL);
}
