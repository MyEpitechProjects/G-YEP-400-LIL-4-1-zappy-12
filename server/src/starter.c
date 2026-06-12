/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "myserver.h"
#include "zappy/zappy_internal.h"

void get_sock_port(socket_t *_socket)
{
    int port = 0;

    getsockname(_socket->_fd,
        (struct sockaddr *)&_socket->_address, &_socket->_addrlen);
    port = ntohs(_socket->_address.sin_port);
    _socket->_port._p1 = port / 256;
    _socket->_port._p2 = port % 256;
}

static bool run_server(connection_t *_server)
{
    bool ret = true;

    while (ret) {
        poll(_server->_fds, _server->nfds, -1);
        for (uint32_t i = 0; i < _server->nfds;){
            ret = requese_handler(_server, &i, ret);
            execute_buffer(_server, &i, ret);
            i++;
        }
    }
    return true;
}

bool start_server(arguments_t *arguments, int sdf)
{
    bool runCode = true;
    connection_t _server;

    memset(&_server, 0, sizeof(connection_t));
    _server.arguments = arguments;
    zappy(&_server, &_server.zappy);
    if (!push_socket(&_server, arguments->port, SOMAXCONN, FD_SERVER)) {
        return false;
    }
    push_poller(&_server, sdf, NULL, FD_QUIT);
    runCode = run_server(&_server);
    close(_server._sockets[0]._fd);
    return free_server(&_server, runCode);
}
