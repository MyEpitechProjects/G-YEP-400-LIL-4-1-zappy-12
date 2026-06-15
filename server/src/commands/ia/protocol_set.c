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

static bool set_item(connection_t *_server, uint32_t i, uint64_t *me_inv,
    uint64_t *tile_inv)
{
    char *string = string_create("ok");

    if (*me_inv == 0){
        protocol_ia_bad_param(_server, i, string);
        return false;
    }
    *(me_inv) = (*me_inv - 1);
    *(tile_inv) = (*tile_inv + 1);
    string = send_string(_server->_fds[i].fd, string);
    return true;
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
        if (!set_item(_server, i, inv_me[x], inv_tile[x]))
            return true;
        trigger_graphic_drop(_server, me, x);
        trigger_graphic_player_eat(_server, me);
        return trigger_graphic_tile_change(_server, tile);
    }
    return protocol_ia_bad_param(_server, i, NULL);
}
