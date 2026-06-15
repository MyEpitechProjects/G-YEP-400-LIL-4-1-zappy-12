/*
** EPITECH PROJECT, 2026
** myteams common protocol
** File description:
** Shared structs and macros between client and server
*/

#ifndef __LOOK__
    #define __LOOK__
    #include "zappy/player.h"
    #include "zappy/broadcast.h"
    #include <stdint.h>
    #include <stdbool.h>

typedef struct look_args_t {
    player_t *player;
    broadcast_pos_t fwd;
    broadcast_pos_t right;
    int64_t k;
    int64_t col;
    bool first_tile;
} look_args_t;

#endif
