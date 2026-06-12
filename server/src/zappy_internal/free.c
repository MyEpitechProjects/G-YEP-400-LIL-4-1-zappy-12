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


bool free_zappy(connection_t *_server, zappy_t *zappy)
{
    (void)_server;
    zappy->tiles = myfree(zappy->tiles);
    zappy->eggs = myfree(zappy->eggs);
    zappy->players = myfree(zappy->players);
    return true;
}
