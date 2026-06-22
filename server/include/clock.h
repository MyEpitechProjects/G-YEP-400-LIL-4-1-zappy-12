/*
** EPITECH PROJECT, 2026
** myteams common protocol
** File description:
** Shared structs and macros between client and server
*/

#ifndef __CLOCK__
    #define __CLOCK__
    #include <stdint.h>
    #include <stdbool.h>
    #include <time.h>
    #include <sys/time.h>


typedef struct myclock_t {
    uint64_t base;
    uint64_t offset;
    bool active;
} myclock_t;

myclock_t *reset_clock(myclock_t *_clock);

myclock_t *start_clock(myclock_t *_clock);

myclock_t *stop_clock(myclock_t *_clock);

uint64_t get_elapsed(myclock_t *_clock);

#endif
