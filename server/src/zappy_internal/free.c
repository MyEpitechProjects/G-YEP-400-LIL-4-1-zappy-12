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
#include "protocol.h"


void *free_zappy(zappy_t *zappy)
{
    zappy->tiles = myfree(zappy->tiles);
    zappy->eggs = myfree(zappy->eggs);
    zappy->players = myfree(zappy->players);
    zappy->incantation = myfree(zappy->incantation);
    return NULL;
}
