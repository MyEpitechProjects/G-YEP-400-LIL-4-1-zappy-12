/*
** EPITECH PROJECT, 2026
** myteams common protocol
** File description:
** Shared structs and macros between client and server
*/

#ifndef __TILE__
    #define __TILE__
    #include <stdbool.h>
    #include "inventory.h"
    #include "position.h"

typedef struct tile_t {
    bool isTile;
    inventory_t inventory;
} tile_t;

#endif
