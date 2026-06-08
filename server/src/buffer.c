/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "myserver.h"

static bool new_buffer(connection_t *_server, uint32_t i, char *req)
{
    _server->_fdsm[i].buffer = calloc(strlen(req) + 1, sizeof(char));
    if (!_server->_fdsm)
        return true;
    sprintf(_server->_fdsm[i].buffer, "%s", req);
    return true;
}

static bool append_buffer(connection_t *_server, uint32_t i, char *req)
{
    char *backup = _server->_fdsm[i].buffer;

    _server->_fdsm[i].buffer = realloc(_server->_fdsm[i].buffer,
        strlen(req) + 1);
    if (!_server->_fdsm[i].buffer){
        _server->_fdsm[i].buffer = backup;
    }
    sprintf(_server->_fdsm[i].buffer, "%s%s", _server->_fdsm[i].buffer, req);
    return true;
}

bool complete_buffer(connection_t *_server, uint32_t *i)
{
    char req[1024];

    memset(req, 0, 1024);
    if (read(_server->_fds[*i].fd, req, 1024) == 0)
        return true;
    if (!_server->_fdsm[*i].buffer){
        new_buffer(_server, *i, req);
    } else {
        append_buffer(_server, *i, req);
    }
    return true;
}

static uint64_t get_buffer_lenght(char *input)
{
    uint64_t size = 1;

    if (!input)
        return 0;
    for (uint64_t i = 0; input[i] != '\n' && input[i] != '\0'; i++)
        size++;
    if (input[size] != '\n')
        size = 0;
    return size;
}

static bool pop_buffer(connection_t *_server, uint32_t i, char *input)
{
    uint64_t w = get_buffer_lenght(input);

    if (w == 0)
        return true;
    _server->_fdsm[i].buffer = malloc((strlen(input) - w) + 1);
    if (!_server->_fdsm[i].buffer){
        _server->_fdsm[i].buffer = input;
        return true;
    }
    strcpy(_server->_fdsm[i].buffer, input + w);
    free(input);
    return true;
}

static char *get_buffer(char *input)
{
    uint64_t w = get_buffer_lenght(input);
    char *buffer = NULL;

    if (w == 0)
        return NULL;
    buffer = malloc(sizeof(w));
    if (!buffer)
        return NULL;
    strncpy(buffer, input, w);
    buffer[w - 1] = '\0';
    return buffer;
}

bool execute_buffer(connection_t *_server, uint32_t *i, bool ret)
{
    char *command = NULL;
    char **command_build = NULL;

    if (ret == false)
        return false;
    command = get_buffer(_server->_fdsm[*i].buffer);
    if (!command)
        return true;
    command_build = str_to_word_array(command);
    if (!command_build){
        free(command);
        return true;
    }
    handle_request(_server, *i, command_build);
    pop_buffer(_server, *i, _server->_fdsm[*i].buffer);
    free_array((void **)command_build);
    free(command);
    return true;
}
