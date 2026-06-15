/*
** EPITECH PROJECT, 2026
** myteams common protocol
** File description:
** Shared structs and macros between client and server
*/

#ifndef __INVENTORY__
    #define __INVENTORY__
    #include <stdint.h>
    #include <stdbool.h>
    #define NUMBER_RESOURCES 7

typedef struct inventory_t {
    uint64_t food;
    uint64_t linemate;
    uint64_t deraumere;
    uint64_t sibur;
    uint64_t mendiane;
    uint64_t phiras;
    uint64_t thystame;
} inventory_t;

static const char *const food_names[] = {
    "food", "linemate", "deraumere",
    "sibur", "mendiane", "phiras", "thystame", NULL
};

#endif
