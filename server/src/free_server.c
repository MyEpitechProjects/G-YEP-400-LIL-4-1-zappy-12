/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "myserver.h"
#include "zappy/zappy_internal.h"
#include "clock.h"

void *myfree(void *ptr)
{
    free(ptr);
    return NULL;
}

static void *free_fds(connection_t *_server)
{
    if (!_server->_fds)
        return NULL;
    for (uint32_t i = 0; i < _server->nfds; i++){
        close(_server->_fds[i].fd);
    }
    free(_server->_fds);
    return NULL;
}

static void *free_fdsm(connection_t *_server)
{
    free(_server->_fdsm);
    return NULL;
}

static void *free_socket(connection_t *_server)
{
    if (!_server->_sockets)
        return NULL;
    for (uint32_t i = 0; i < _server->nsock; i++){
        close(_server->_sockets[i]._fd);
    }
    free(_server->_sockets);
    return NULL;
}

void destroy_clock(myclock_t *_clock)
{
    if (!_clock)
        return;
    free(_clock);
}

bool free_array(void **arr)
{
    for (uint64_t i = 0; arr[i] != NULL; i++){
        free(arr[i]);
    }
    free(arr);
    return true;
}

int free_server(connection_t *_server, bool code)
{
    free_fds(_server);
    free_fdsm(_server);
    free_socket(_server);
    free_zappy(_server, &_server->zappy);
    _server->nfds = 0;
    _server->nfds = 0;
    return code;
}
