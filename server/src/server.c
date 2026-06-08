/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "myserver.h"

bool recieve_request(connection_t *_server)
{
    bool good = push_poller(_server, accept(_server->_sockets[0]._fd,
            NULL, NULL), NULL, FD_CONTROL);
    message_t resp;

    if (!good)
        return false;
    memset(&resp, 0, sizeof(message_t));
    set_message(&resp, RESPONSE, READY, 0);
    return send_response(_server->_fds[_server->nfds - 1].fd, &resp, NULL);
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

static status_t check_functinons_args(message_t *req,
    req_body_t *body)
{
    status_t status = COMMAND_OK;

    for (int j = 0; commands[j].func != UNK_INVOCATION; j++) {
        if (req->_func != commands[j].func)
            continue;
        status = body_check(&commands[j].check, body);
        break;
    }
    return status;
}

static bool final_handle_request(connection_t *_server, uint32_t i,
    message_t *req, req_body_t *body)
{
    message_t resp;

    memset(&resp, 0, sizeof(message_t));
    for (int j = 0; commands[j].func != UNK_INVOCATION; j++) {
        if (req->_func != commands[j].func)
            continue;
        return commands[j].handler(_server, i, body);
    }
    set_message(&resp, FAILED, INTERNAL, 0);
    return send_response(_server->_fds[i].fd, &resp, NULL);
}

bool handle_request(connection_t *_server, uint32_t i,
    char **command)
{
    message_t resp;
    status_t status = COMMAND_OK;

    (void)command;
    return true;
    memset(&resp, 0, sizeof(message_t));
    status = check_functinons_args(NULL, NULL);
    if (status != COMMAND_OK){
        set_message(&resp, FAILED, status, 0);
        return send_response(_server->_fds[i].fd, &resp, NULL);
    }
    status = check_request(_server, &_server->_fdsm[i], NULL, NULL);
    if (status != COMMAND_OK){
        set_message(&resp, FAILED, status, 0);
        return err_response(_server->_fds[i].fd, &resp, NULL);
    }
    if (false)
        return final_handle_request(_server, i, NULL, NULL);
    set_message(&resp, FAILED, INTERNAL, 0);
    return send_response(_server->_fds[i].fd, &resp, NULL);
}

bool requese_handler(connection_t *_server, uint32_t *i, bool ret)
{
    if (!ret)
        return false;
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
