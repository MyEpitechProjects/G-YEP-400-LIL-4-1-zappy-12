/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"


bool intern_protocol_player_pos(connection_t *_server, uint32_t i,
    player_t *player)
{
    char *string = string_create("ppo");

    string = string_append(string, "#", false);
    string = string_append_uint(string, player->index, true);
    string = string_append_uint(string, player->pos.x, true);
    string = string_append_uint(string, player->pos.y, true);
    string = string_append_uint(string, player->o, true);
    string = send_string(_server->_fds[i].fd, string);
    return true;
}

bool protocol_player_pos(connection_t *_server, uint32_t i, char **command)
{
    int64_t index = 0;
    char *end = NULL;
    player_t *player = NULL;

    if (list_len(command) != 2)
        return protocol_graphic_bad_param(_server, i, NULL);
    index = extract_num(command[1], &end, 10, true);
    if (*end != '\0')
        return protocol_graphic_bad_param(_server, i, NULL);
    player = get_player(_server, index);
    if (!player)
        return protocol_graphic_bad_param(_server, i, NULL);
    return intern_protocol_player_pos(_server, i, player);
}
