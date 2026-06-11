/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include <stdlib.h>


myclock_t *reset_clock(myclock_t *_clock)
{
    if (!_clock)
        return NULL;
    _clock->base = 0;
    _clock->offset = 0;
    _clock->active = false;
    return _clock;
}

myclock_t *create_clock(void)
{
    myclock_t *_clock = malloc(sizeof(myclock_t));

    if (!_clock)
        return NULL;
    return reset_clock(_clock);
}

myclock_t *start_clock(myclock_t *_clock)
{
    if (!_clock)
        return NULL;
    if (_clock->base == 0){
        _clock->base = time(NULL);
    } else if (_clock->offset != 0) {
        _clock->base += (time(NULL) - _clock->offset);
        _clock->offset = 0;
    }
    _clock->active = false;
    return _clock;
}

myclock_t *stop_clock(myclock_t *_clock)
{
    if (!_clock)
        return NULL;
    if (_clock->base == 0)
        _clock->base = time(NULL);
    else if (_clock->offset != 0) {
        _clock->base += (time(NULL) - _clock->offset);
        _clock->offset = 0;
    }
    _clock->active = false;
    return _clock;
}

time_t get_elapsed(myclock_t *_clock)
{
    if (!_clock || _clock->base == 0)
        return 0;
    if (_clock->active)
        return (time(NULL) - _clock->base);
    return (_clock->offset - _clock->base);
}
