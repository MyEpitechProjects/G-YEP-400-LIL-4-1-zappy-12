/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "myserver.h"
#include "sys.h"

static void init_poller(connection_t *_server, int fd, socket_t *sock,
    fd_type_t _type)
{
    my_memset(&_server->_fds[_server->nfds], 0, sizeof(struct pollfd));
    _server->_fds[_server->nfds].fd = fd;
    _server->_fds[_server->nfds].events = POLLIN;
    my_memset(&_server->_fdsm[_server->nfds], 0, sizeof(pollfd_meta_t));
    _server->_fdsm[_server->nfds]._socket = sock;
    _server->_fdsm[_server->nfds]._type = _type;
}

static bool create_socket(socket_t *_socket, uint32_t port,
    uint32_t listenMax)
{
    int opt = 1;

    my_memset(_socket, 0, sizeof(socket_t));
    _socket->_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket->_fd < 0)
        return false;
    if (setsockopt(_socket->_fd, SOL_SOCKET, SO_REUSEADDR,
            &opt, sizeof(opt)) < 0)
        return false;
    _socket->_address.sin_port = htons(port);
    _socket->_address.sin_family = AF_INET;
    _socket->_address.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(_socket->_fd, (struct sockaddr *) &_socket->_address,
            sizeof(_socket->_address)) < 0){
        return false;
    }
    _socket->_addrlen = sizeof(_socket->_address);
    if (listen(_socket->_fd, listenMax) < 0)
        return -1;
    return true;
}

bool push_poller(connection_t *_server, int fd,
    socket_t *sock, fd_type_t _type)
{
    struct pollfd *tmp_fds = realloc(_server->_fds,
        sizeof(struct pollfd) * (_server->nfds + 1));
    pollfd_meta_t *tmp_fdsm = realloc(_server->_fdsm,
        sizeof(pollfd_meta_t) * (_server->nfds + 1));

    if (!tmp_fds || !tmp_fdsm){
        tmp_fds = myfree(tmp_fds);
        tmp_fdsm = myfree(tmp_fdsm);
        return false;
    }
    _server->_fds = tmp_fds;
    _server->_fdsm = tmp_fdsm;
    init_poller(_server, fd, sock, _type);
    _server->nfds++;
    return (fd >= 0);
}

bool push_socket(connection_t *_server, uint32_t port,
    uint32_t listenMax, fd_type_t type)
{
    socket_t *tmp = realloc(_server->_sockets,
        sizeof(socket_t) * (_server->nsock + 1));

    if (!tmp)
        return false;
    _server->_sockets = tmp;
    if (!create_socket(&_server->_sockets[_server->nsock], port, listenMax))
        return false;
    push_poller(_server, _server->_sockets[_server->nsock]._fd,
        &_server->_sockets[_server->nsock], type);
    _server->nsock++;
    return true;
}
