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

typedef struct myclock_t {
    time_t base;
    time_t offset;
    bool active;
} myclock_t;

myclock_t *reset_clock(myclock_t *_clock);

myclock_t *create_clock(void);

myclock_t *start_clock(myclock_t *_clock);

myclock_t *stop_clock(myclock_t *_clock);

time_t get_elapsed(myclock_t *_clock);

#endif
