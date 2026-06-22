/*
** EPITECH PROJECT, 2026
** myteams common protocol
** File description:
** Shared structs and macros between client and server
*/

#ifndef __POSITION__
    #define __POSITION__
    #include <stdbool.h>
    #include <stdint.h>

typedef struct pos_t {
    uint64_t x;
    uint64_t y;
} pos_t;

#endif
