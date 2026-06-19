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
    #include "clock.h"

typedef struct egg_t {
    pos_t pos;
    uint64_t index;
    int64_t creatorIndex;
    orientation_t o;
    char *team;
    bool taken;
    bool hatched;
    myclock_t hatchClock;
} egg_t;

#endif
