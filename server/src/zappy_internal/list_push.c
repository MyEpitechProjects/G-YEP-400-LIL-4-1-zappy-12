/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"
#include "sys.h"

void *list_push(void *original, uint64_t *size, uint64_t itemSize, bool *status)
{
    void *new = NULL;

    if (!original){
        *(status) = true;
        *(size) = 1;
        new = calloc(1, itemSize);
        if (!new){
            *(status) = false;
        }
        return new;
    }
    new = realloc(original, (*size + 1) * itemSize);
    if (!new){
        *(status) = false;
        return original;
    }
    my_memset(new + (*size * itemSize), 0, itemSize);
    *(size) = (*size + 1);
    *(status) = true;
    return new;
}
