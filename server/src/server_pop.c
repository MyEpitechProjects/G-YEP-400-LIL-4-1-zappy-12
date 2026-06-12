/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "myserver.h"

static bool pop_poller_cpy(connection_t *_server, uint32_t i,
    struct pollfd *tmp_fds, pollfd_meta_t *tmp_fdsm)
{
    memcpy(tmp_fds, _server->_fds, sizeof(struct pollfd) * i);
    memcpy(tmp_fdsm, _server->_fdsm, sizeof(pollfd_meta_t) * i);
    memcpy(tmp_fds + i, _server->_fds + (i + 1),
        sizeof(struct pollfd) * (_server->nfds - (i + 1)));
    memcpy(tmp_fdsm + i, _server->_fdsm + (i + 1),
        sizeof(pollfd_meta_t) * (_server->nfds - (i + 1)));
    close(_server->_fds[i].fd);
    _server->_fds = myfree(_server->_fds);
    _server->_fdsm = myfree(_server->_fdsm);
    return true;
}

bool pop_poller(connection_t *_server, uint32_t *i)
{
    struct pollfd *tmp_fds = malloc(sizeof(struct pollfd) *
        (_server->nfds - 1));
    pollfd_meta_t *tmp_fdsm = malloc(sizeof(pollfd_meta_t) *
        (_server->nfds - 1));

    _server->_fdsm[*i].buffer = myfree(_server->_fdsm[*i].buffer);
    if (!tmp_fds || !tmp_fdsm) {
        tmp_fds = myfree(tmp_fds);
        tmp_fdsm = myfree(tmp_fdsm);
        return false;
    }
    pop_poller_cpy(_server, *i, tmp_fds, tmp_fdsm);
    _server->_fds = tmp_fds;
    _server->_fdsm = tmp_fdsm;
    _server->nfds--;
    (*i)--;
    return true;
}
