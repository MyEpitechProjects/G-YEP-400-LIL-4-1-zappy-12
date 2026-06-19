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

egg_t *get_last_egg(connection_t *_server)
{
    egg_t *egg = NULL;

    for (uint64_t i = 0; i < _server->zappy.nb_eggs; i++){
        egg = &_server->zappy.eggs[i];
    }
    return egg;
}

pos_t *set_pos_xy(pos_t *pos, uint64_t x, uint64_t y)
{
    pos->x = x;
    pos->y = y;
    return pos;
}

static bool push_egg(connection_t *_server,
    player_t *player)
{
    bool status = false;
    uint64_t nbeggs = _server->zappy.nb_eggs;
    egg_t *me = NULL;

    _server->zappy.eggs = list_push(_server->zappy.eggs,
        &_server->zappy.nb_eggs, sizeof(egg_t), &status);
    if (status){
        me = get_last_egg(_server);
        me->team = player->team;
        me->creatorIndex = player->index;
        me->index = nbeggs;
        set_pos_xy(&me->pos, randint(_server->arguments->width),
            randint(_server->arguments->height));
        me->o = randint(ORIENTATION_WEST + 1);
        return trigger_graphic_egg_layed(_server,
            me);
    }
    return false;
}

bool protocol_ia_fork(connection_t *_server, uint32_t i,
    char **command)
{
    player_t *me = get_player_by_id(_server, _server->_fdsm[i].id);

    (void)command;
    trigger_graphic_egg_laying(_server, me);
    push_egg(_server, me);
    return protocol_ok(_server, i, command);
}
