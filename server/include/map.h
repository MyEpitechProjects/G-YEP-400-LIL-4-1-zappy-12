/*
** EPITECH PROJECT, 2026
** myteams common protocol
** File description:
** Shared structs and macros between client and server
*/

#ifndef __MAP__
    #define __MAP__
    #include <stdint.h>
    #include "tiles.h"

typedef struct map_t {
    uint64_t w;
    uint64_t h;
    tile_t *tiles;
} map_t;

#endif
