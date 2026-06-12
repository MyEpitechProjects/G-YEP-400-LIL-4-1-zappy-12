/*
** EPITECH PROJECT, 2026
** myteams common protocol
** File description:
** Shared structs and macros between client and server
*/

#ifndef __EGG__
    #define __EGG__
    #include <stdbool.h>
    #include <stdint.h>
    #include "position.h"

typedef struct egg_t {
    pos_t pos;
    uint64_t index;
    int64_t creatorIndex;
    char *team;
    bool taken;
} egg_t;

#endif
