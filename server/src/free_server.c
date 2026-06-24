/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "myserver.h"
#include "zappy/zappy_internal.h"
#include "clock.h"

void my_close(int fd)
{
    if (fd < 0)
        return;
    close(fd);
}

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
        my_close(_server->_fds[i].fd);
    }
    return myfree(_server->_fds);
}

static void *free_fdsm(connection_t *_server)
{
    return myfree(_server->_fdsm);
}

static void *free_socket(connection_t *_server)
{
    if (!_server->_sockets)
        return NULL;
    for (uint32_t i = 0; i < _server->nsock; i++){
        my_close(_server->_sockets[i]._fd);
    }
    return myfree(_server->_sockets);
}

void destroy_clock(myclock_t *_clock)
{
    if (!_clock)
        return;
    myfree(_clock);
}

bool free_array(void **arr)
{
    for (uint64_t i = 0; arr[i] != NULL; i++){
        myfree(arr[i]);
    }
    myfree(arr);
    return true;
}

int free_server(connection_t *_server, bool code)
{
    free_fds(_server);
    for (uint32_t i = 0; i < _server->nfds; i++){
        if (_server->_fdsm[i].buffer)
            _server->_fdsm[i].buffer = myfree(_server->_fdsm[i].buffer);
    }
    _server->_fdsm = free_fdsm(_server);
    _server->_sockets = free_socket(_server);
    free_zappy(&_server->zappy);
    _server->nfds = 0;
    _server->nsock = 0;
    return code;
}
