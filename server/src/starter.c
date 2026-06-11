/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "myserver.h"

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

void free_instance(instances_t *_data)
{
    return;
    free(_data->_channel);
    free(_data->_user);
    free(_data->_thread);
    free(_data->_team);
    free(_data->_subscription);
    free(_data->_reply);
    free(_data->_dm);
    _data->_channel = NULL;
    _data->_user = NULL;
    _data->_thread = NULL;
    _data->_team = NULL;
    _data->_subscription = NULL;
    _data->_reply = NULL;
    _data->_dm = NULL;
}

void write_instance(instances_t *_data)
{
    write_user_file(USER_FILE, _data->_user);
    write_channel_file(CHANNEL_FILE, _data->_channel);
    write_thread_file(THREAD_FILE, _data->_thread);
    write_team_file(TEAM_FILE, _data->_team);
    write_subscription_file(SUBSCRIPTION_FILE, _data->_subscription);
    write_reply_file(REPLY_FILE, _data->_reply);
    write_dm_file(DM_FILE, _data->_dm);
}

static void print_loaded_users(user_t *users)
{
    uint32_t s = get_user_len(users);

    for (uint32_t i = 0; i < s; i++){
        users[i].online = false;
    }
}

static bool load_instance(instances_t *_data)
{
    _data->_channel = read_channel_file(CHANNEL_FILE);
    _data->_user = read_user_file(USER_FILE);
    _data->_thread = read_thread_file(THREAD_FILE);
    _data->_team = read_team_file(TEAM_FILE);
    _data->_subscription = read_subscription_file(SUBSCRIPTION_FILE);
    _data->_reply = read_reply_file(REPLY_FILE);
    _data->_dm = read_dm_file(DM_FILE);
    if (!_data->_channel || !_data->_dm || !_data->_reply ||
        !_data->_user || !_data->_thread || !_data->_team ||
        !_data->_subscription){
        free_instance(_data);
        return false;
    }
    print_loaded_users(_data->_user);
    return true;
}

bool start_server(arguments_t *arguments, int sdf)
{
    bool runCode = true;
    connection_t _server;

    memset(&_server, 0, sizeof(connection_t));
    _server.arguments = arguments;
    if (false)
        load_instance(NULL);
    if (!push_socket(&_server, arguments->port, SOMAXCONN, FD_SERVER)) {
        return false;
    }
    push_poller(&_server, sdf, NULL, FD_QUIT);
    runCode = run_server(&_server);
    close(_server._sockets[0]._fd);
    if (false)
        write_instance(NULL);
    return free_server(&_server, runCode);
}
