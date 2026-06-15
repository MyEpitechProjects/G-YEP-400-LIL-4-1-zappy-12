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

static uint64_t derive_food_number(connection_t *_server, uint64_t density)
{
    uint64_t v = (_server->arguments->width *
        _server->arguments->height * density) / 100;

    if (v == 0)
        return 1;
    return v;
}

static void add_resource_to_tile(tile_t *tile, resource_type_t type)
{
    switch (type) {
        case RES_LINEMATE:
            tile->inventory.linemate++;
            break;
        case RES_DERAUMERE:
            tile->inventory.deraumere++;
            break;
        case RES_SIBUR:
            tile->inventory.sibur++;
            break;
        case RES_MENDIANE:
            tile->inventory.mendiane++;
            break;
        case RES_PHIRAS:
            tile->inventory.phiras++;
            break;
        case RES_THYSTAME:
            tile->inventory.thystame++;
            break;
    }
}

static void dispatch_resource(connection_t *_server, zappy_t *zappy,
    uint32_t qty, resource_type_t type)
{
    uint64_t map_size = _server->arguments->width * _server->arguments->height;

    for (uint32_t i = 0; i < qty; i++){
        if (type == 0){
            zappy->tiles[randint(map_size)].inventory.food++;
        } else
            add_resource_to_tile(&zappy->tiles[randint(map_size)], type);
    }
}

static void dispatch_inventory(connection_t *_server, zappy_t *zappy,
    inventory_t *inv)
{
    dispatch_resource(_server, zappy, inv->food, 0);
    dispatch_resource(_server, zappy, inv->linemate, RES_LINEMATE);
    dispatch_resource(_server, zappy, inv->deraumere, RES_DERAUMERE);
    dispatch_resource(_server, zappy, inv->sibur, RES_SIBUR);
    dispatch_resource(_server, zappy, inv->mendiane, RES_MENDIANE);
    dispatch_resource(_server, zappy, inv->phiras, RES_PHIRAS);
    dispatch_resource(_server, zappy, inv->thystame, RES_THYSTAME);
}

static inventory_t *get_density_inv(connection_t *_server, zappy_t *zappy,
    inventory_t *inv)
{
    (void)zappy;
    inv->food = derive_food_number(_server, DENSITY_FOOD);
    inv->linemate = derive_food_number(_server, DENSITY_LINEMATE);
    inv->deraumere = derive_food_number(_server, DENSITY_DERAUMERE);
    inv->sibur = derive_food_number(_server, DENSITY_SIBUR);
    inv->mendiane = derive_food_number(_server, DENSITY_MENDIANE);
    inv->phiras = derive_food_number(_server, DENSITY_PHIRAS);
    inv->thystame = derive_food_number(_server, DENSITY_THYSTAME);
    return inv;
}

static inventory_t *get_tile_food(connection_t *_server, zappy_t *zappy,
    inventory_t *inv)
{
    for (uint64_t i = 0; i < (_server->arguments->width *
            _server->arguments->height); i++){
        inv->food += zappy->tiles[i].inventory.food;
        inv->linemate += zappy->tiles[i].inventory.linemate;
        inv->deraumere += zappy->tiles[i].inventory.deraumere;
        inv->sibur += zappy->tiles[i].inventory.sibur;
        inv->mendiane += zappy->tiles[i].inventory.mendiane;
        inv->phiras += zappy->tiles[i].inventory.phiras;
        inv->thystame += zappy->tiles[i].inventory.thystame;
    }
    return inv;
}

static inventory_t *food_soustraction(inventory_t *inv, inventory_t *inv2)
{
    inv->food = subsctract(inv->food, inv2->food);
    inv->linemate = subsctract(inv->linemate, inv2->linemate);
    inv->deraumere = subsctract(inv->deraumere, inv2->deraumere);
    inv->sibur = subsctract(inv->sibur, inv2->sibur);
    inv->mendiane = subsctract(inv->mendiane, inv2->mendiane);
    inv->phiras = subsctract(inv->phiras, inv2->phiras);
    inv->thystame = subsctract(inv->thystame, inv2->thystame);
    return inv;
}

bool add_food_tile(connection_t *_server, zappy_t *zappy)
{
    inventory_t inv = {0};
    inventory_t inv2 = {0};

    get_density_inv(_server, zappy, &inv);
    get_tile_food(_server, zappy, &inv2);
    food_soustraction(&inv, &inv2);
    dispatch_inventory(_server, zappy, &inv);
    return true;
}

bool need_food_respawn(connection_t *_server, zappy_t *zappy)
{
    inventory_t inv = {0};
    inventory_t inv2 = {0};

    get_density_inv(_server, zappy, &inv);
    get_tile_food(_server, zappy, &inv2);
    if (inv.food > inv2.food || inv.deraumere > inv2.deraumere ||
        inv.linemate > inv2.linemate || inv.mendiane > inv2.mendiane ||
        inv.phiras > inv2.phiras || inv.sibur > inv2.sibur ||
        inv.thystame > inv2.thystame)
        return true;
    return false;
}
