/*
** EPITECH PROJECT, 2026
** myteams common protocol
** File description:
** Shared structs and macros between client and server
*/

#ifndef __ORIENTATION__
    #define __ORIENTATION__
    #include <stdbool.h>
    #include "inventory.h"

typedef enum orientation_t {
    ORIENTATION_NONE,
    ORIENTATION_NORTH,
    ORIENTATION_EST,
    ORIENTATION_SOUTH,
    ORIENTATION_WEST,
} orientation_t;

#endif
