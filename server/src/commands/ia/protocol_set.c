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

uint64_t **set_resources_array(uint64_t **src, inventory_t *inv)
{
    src[0] = &inv->food;
    src[1] = &inv->linemate;
    src[2] = &inv->deraumere;
    src[3] = &inv->sibur;
    src[4] = &inv->mendiane;
    src[5] = &inv->phiras;
    src[6] = &inv->thystame;
    src[7] = NULL;
    return src;
}

bool switch_item(connection_t *_server, uint32_t i, uint64_t *inv1,
    uint64_t *inv2)
{
    if (*inv1 == 0){
        protocol_ia_bad_param(_server, i, NULL);
        return false;
    }
    *(inv1) = (*inv1 - 1);
    *(inv2) = (*inv2 + 1);
    return protocol_ok(_server, i, NULL);
}

bool protocol_ia_set(connection_t *_server, uint32_t i,
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
        if (!switch_item(_server, i, inv_me[x], inv_tile[x]))
            return true;
        trigger_graphic_drop(_server, me, x);
        trigger_graphic_player_inv(_server, me);
        return trigger_graphic_tile_change(_server, tile);
    }
    return protocol_ia_bad_param(_server, i, NULL);
}
