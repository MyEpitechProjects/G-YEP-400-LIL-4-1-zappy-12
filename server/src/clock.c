/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "sys.h"
#include <stdlib.h>

static uint64_t now_ms(void)
{
    struct timeval tv = {0};

    if (gettimeofday(&tv, NULL) == -1)
        return 0;
    return (uint64_t)(tv.tv_sec * 1000ULL + tv.tv_usec / 1000ULL);
}

myclock_t *reset_clock(myclock_t *_clock)
{
    if (!_clock)
        return NULL;
    _clock->base = 0;
    _clock->offset = 0;
    _clock->active = false;
    return _clock;
}

myclock_t *start_clock(myclock_t *_clock)
{
    if (!_clock)
        return NULL;
    if (_clock->base == 0){
        _clock->base = now_ms();
    } else if (_clock->offset != 0) {
        _clock->base += (now_ms() - _clock->offset);
        _clock->offset = 0;
    }
    _clock->active = true;
    return _clock;
}

myclock_t *stop_clock(myclock_t *_clock)
{
    if (!_clock)
        return NULL;
    if (_clock->base == 0)
        _clock->base = now_ms();
    else if (_clock->offset != 0) {
        _clock->base += (now_ms() - _clock->offset);
        _clock->offset = 0;
    }
    _clock->active = false;
    return _clock;
}

uint64_t get_elapsed(myclock_t *_clock)
{
    if (!_clock || _clock->base == 0)
        return 0;
    if (_clock->active)
        return (max(0, now_ms() - _clock->base));
    return (max(0, _clock->offset - _clock->base));
}
