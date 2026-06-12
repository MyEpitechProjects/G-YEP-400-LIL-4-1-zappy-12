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

typedef struct inventory_t {
    uint32_t food;
    uint32_t linemate;
    uint32_t deraumere;
    uint32_t sibur;
    uint32_t mendiane;
    uint32_t phiras;
    uint32_t thystame;
} inventory_t;

#endif
