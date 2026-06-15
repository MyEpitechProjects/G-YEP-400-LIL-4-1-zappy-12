/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"

static char *append_item(char *result, char *name, uint64_t value, bool last)
{
    result = string_append(result, name, true);
    result = string_append_uint(result, value, false);
    if (!last)
        result = string_append(result, ",", true);
    else
        result = string_append(result, " ]", true);
    return result;
}

bool protocol_ia_inventory(connection_t *_server, uint32_t i,
    char **command)
{
    char *result = string_create("[");
    inventory_t *inv = &get_player_by_id(_server,
        _server->_fdsm[i].id)->inventory;
    char *res_names[] = {
        "food", "linemate", "deraumere",
        "sibur", "mendiane", "phiras", "thystame", NULL
    };
    uint64_t inv_value[] = {
        inv->food, inv->linemate, inv->deraumere, inv->sibur,
        inv->mendiane, inv->phiras, inv->thystame
    };

    (void)command;
    for (uint64_t x = 0; res_names[x] != NULL; x++){
        result = append_item(result, res_names[x], inv_value[x],
            res_names[x + 1] == NULL);
    }
    result = send_string(_server->_fds[i].fd, result);
    return true;
}
