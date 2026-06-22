/*
** EPITECH PROJECT, 2026
** myteams common protocol
** File description:
** Shared structs and macros between client and server
*/

#ifndef __INCANTATION__
    #define __INCANTATION__
    #include <stdint.h>
    #include <stdbool.h>
    #include "inventory.h"
    #include "position.h"

typedef struct incantation_type_t {
    uint64_t level;
    uint64_t nextLevel;
    uint64_t nbPlayer;
    inventory_t inv;
} incantation_type_t;

typedef struct incantation_t {
    uint64_t index;
    uint64_t initiatorIndex;
    pos_t pos;
    const incantation_type_t *type;
    myclock_t clock;
    bool active;
} incantation_t;

static const incantation_type_t incantation_types[] = {
    {1, 2, 1, {0, 1, 0, 0, 0, 0, 0}},
    {2, 3, 2, {0, 1, 1, 1, 0, 0, 0}},
    {3, 4, 2, {0, 2, 0, 1, 0, 2, 0}},
    {4, 5, 4, {0, 1, 1, 2, 0, 1, 0}},
    {5, 6, 4, {0, 1, 2, 1, 3, 0, 0}},
    {6, 7, 6, {0, 1, 2, 3, 0, 1, 0}},
    {7, 8, 6, {0, 2, 2, 2, 2, 2, 0}},
    {0},
};

#endif
