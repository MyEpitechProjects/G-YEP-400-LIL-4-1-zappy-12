/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "myserver.h"
#include "sys.h"

bool recieve_request(connection_t *_server)
{
    char *string = NULL;
    bool good = push_poller(_server, accept(_server->_sockets[0]._fd,
            NULL, NULL), NULL, FD_CONTROL);

    if (!good)
        return false;
    string = string_create("WELCOME");
    string = send_string(_server->_fds[_server->nfds - 1].fd, string);
    return true;
}

bool accept_data(connection_t *_server, uint32_t i)
{
    int data_fd = accept(_server->_fds[i].fd, NULL, NULL);

    if (data_fd < 0)
        return false;
    _server->_fdsm[_server->_fdsm[i]._control_idx]._data_fd = data_fd;
    close(_server->_fds[i].fd);
    _server->nfds--;
    if (i < _server->nfds) {
        _server->_fds[i] = _server->_fds[_server->nfds];
        _server->_fdsm[i] = _server->_fdsm[_server->nfds];
    }
    return true;
}

static bool check_commands_time(connection_t *_server, uint32_t i,
    unit_type_t type)
{
    uint64_t target_time = max(0, get_command_time(_server, type));

    if (type == UNIT_NONE){
        reset_clock(&_server->_fdsm[i].clock);
        _server->_fdsm[i].targetElapsed = 0;
        return true;
    }
    if (_server->_fdsm[i].clock.base == 0){
        start_clock(&_server->_fdsm[i].clock);
        _server->_fdsm[i].targetElapsed = target_time;
    }
    if (get_elapsed(&_server->_fdsm[i].clock) < target_time)
        return false;
    reset_clock(&_server->_fdsm[i].clock);
    _server->_fdsm[i].targetElapsed = 0;
    return true;
}

static bool check_command_args(connection_t *_server, uint32_t i,
    uint8_t nb_args, char **command)
{
    if ((list_len(command) - 1) == nb_args)
        return true;
    if (_server->_fdsm[i].client_type == CLIENT_GUI) {
        protocol_graphic_bad_param(_server, i, NULL);
    } else if (_server->_fdsm[i].client_type == CLIENT_IA) {
        protocol_ia_bad_param(_server, i, NULL);
    }
    pop_buffer(_server, i, _server->_fdsm[i].buffer);
    return false;
}

static bool run_command(connection_t *_server, uint32_t i, char **command,
    const cmd_t *functions)
{
    invocation_t invoked = get_command_invoke(*command, functions);
    unit_type_t type = get_command_unit_type(invoked, functions);
    cmd_handler_t func = get_command_function(invoked, functions);
    uint8_t nb_args = get_command_nb_args(invoked, functions);

    if (!check_commands_time(_server, i, type) ||
        (invoked != UNK_INVOCATION &&
            !check_command_args(_server, i, nb_args, command)))
        return true;
    if (func)
        func(_server, i, command);
    pop_buffer(_server, i, _server->_fdsm[i].buffer);
    return true;
}

static bool register_graphic(connection_t *_server, uint32_t i,
    char **command)
{
    _server->_fdsm[i].client_type = CLIENT_GUI;
    return protocol_graphic_start(_server, i, command);
}

static bool register_ia(connection_t *_server, uint32_t i,
    char **command)
{
    for (uint64_t x = 0; x < _server->arguments->team_number; x++){
        if (strcmp(*command, _server->arguments->teams_names[x]) != 0)
            continue;
        return protocol_ia_start(_server, i, command,
            _server->arguments->teams_names[x]);
    }
    return protocol_ko(_server, i, command);
}

bool handle_request(connection_t *_server, uint32_t i,
    char **command)
{
    if (!command || !command[0])
        return true;
    switch (_server->_fdsm[i].client_type){
        case CLIENT_UNDEFINED:
            pop_buffer(_server, i, _server->_fdsm[i].buffer);
            if (strcmp(command[0], GRAPHICS_CODE) == 0)
                return register_graphic(_server, i, command);
            else
                return register_ia(_server, i, command);
            break;
        case CLIENT_GUI:
            run_command(_server, i, command, gui_commands);
            break;
        case CLIENT_IA:
            run_command(_server, i, command, ia_commands);
            break;
    }
    return true;
}

bool requese_handler(connection_t *_server, uint32_t *i, bool ret)
{
    if (!ret)
        return false;
    if (_server->_fds[*i].revents & (POLLHUP | POLLERR))
        return client_disconnect(_server, i);
    if (!(_server->_fds[*i].revents & POLLIN))
        return true;
    if (_server->_fdsm[*i]._type == FD_SERVER)
        return recieve_request(_server);
    if (_server->_fdsm[*i]._type == FD_DATA)
        return accept_data(_server, *i);
    if (_server->_fdsm[*i]._type == FD_QUIT)
        return false;
    return complete_buffer(_server, i);
}
