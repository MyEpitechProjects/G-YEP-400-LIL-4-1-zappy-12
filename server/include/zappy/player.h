/*
** EPITECH PROJECT, 2026
** myteams common protocol
** File description:
** Shared structs and macros between client and server
*/

#ifndef __PLAYER__
    #define __PLAYER__
    #include <stdbool.h>
    #include "inventory.h"
    #include "orientation.h"
    #include "position.h"

typedef struct player_t {
    pos_t pos;
    orientation_t o;
    inventory_t inventory;
} player_t;

#endif
