/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "clock.h"
#include "myserver.h"
#include "zappy/zappy_internal.h"

bool protocol_ia_bad_param(connection_t *_server, uint32_t i,
    char *original_resp)
{
    char *string = string_create("ko");

    if (original_resp)
        original_resp = myfree(original_resp);
    string = send_string(_server->_fds[i].fd, string);
    return true;
}
